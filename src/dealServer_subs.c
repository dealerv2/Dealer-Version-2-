/* File: dealServer_subs.c   */
/* Renamed on 2022-12-09  was usereval_subs.c --
 * Subs used by Dealer to setup mmap and semaphores and launch the Server. No metrics calculations here.
 */
#define _GNU_SOURCE
#define JGMDBG 1         /* Define this on the gcc line with -DJGMDBG if needed */
#include <assert.h>         /* dbg  suppress with -DNDEBUG on the gcc line*/
#include <ctype.h>          /* toupper, tolower, isalpha*/
#include <errno.h>          /* errno, ECHILD            */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <math.h>           /* float_t, double_t, plus funcs like sin, cos, sqrt, round  link with -lm */
#include <pthread.h>        /* required for posix semaphore use? */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. Posix version */
#include <signal.h>         /* to Kill server and also report on server exit status ? */
#include <stddef.h>         /* offsetof                 */
#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <string.h>         /* strcpy, strlen, memcpy   */
#include <sys/mman.h>       /* mmap, munmap             */
#include <sys/stat.h>       /* To define stat struct. used to check existence of files */
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <sys/wait.h>       /* Wait status etc. for server */
#include <unistd.h>         /* Routines that should be in stdlib but are not for some reason */
/*
 * other headers that are used elsewhere in dealer
 * stddef.h, time.h, random.h,
 */
#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/dealexterns.h"
#include "../include/dealprotos.h"
#include "../include/dbgprt_macros.h"     /* DBGLOC and DBGPRT */
#include "../include/mmap_template.h"     /* For easy use of offsets and pointers */
#include "../include/dealexterns.h"

/*
 * Globals -- Used throughout the code in this file
 */
/* semaphore definition -- names begin with slash and have no other slashes */
static char query_pfx[] = "/q_sem";
static char reply_pfx[] = "/r_sem";
static char query_sema[32];  /* query_sema becomes /q_sem_nnnnnn where nnnnnn is our pid. */
static char reply_sema[32];
static sem_t *p_qsem, *p_rsem ;

/* mmap definition */
static int   PageSize  = 4096 ;
static char  mmap_fname[32]="/tmp/dealer_mmap_XXXXXX";    /* Template for tempfile name */
static char *mm_ptr ;                             /* __THE__ pointer to mmap shared area */
static int   mm_fd   ;                             /* the fd returned by the mmap open call */
static MMAP_TEMPLATE_k *p_mm_base ;
/* userserver server pathname See dealglobals.c for default value */
char *userserver_path ;

// mmap templates and pointers -- these are globals and presumably exist for the length of the run
MMAP_TEMPLATE_k *p_mm_data;
mmap_hdr_k      *p_mm_hdr ;
query_type_k    *pqt;
reply_type_k    *prt;
DEALDATA_k      *p_dldata;
USER_VALUES_k   *p_mm_nsvals, *p_mm_ewvals;
UEVAL_CACHE_k   *p_mm_cache ;
// local mm_data until the map is assigned.
mmap_hdr_k      mm_hdr_data ;
query_type_k    qt_data;
reply_type_k    rt_data;
struct mmap_ptrs_st mmap_pointers ;
struct mmap_off_st  mmap_offsets  ;

/*
 * Functions prototypes
 */
 // Setup and Teardown Functions
pid_t setup_userserver( char *pathname ) ;
char *create_mmap(int *mm_fd, char *mmap_fname );
void calc_ptrs ( char *mm_ptr, struct mmap_ptrs_st *p_pst, struct mmap_off_st *p_ost ) ;
void calc_mmap_offsets(mmap_hdr_k *mm_hdr) ;   // fill local mm_hdr with offsets

int setup_logfile(char *template) ;
sem_t *open_semaphore(char *sem_pfx, char *sem_name) ;
pid_t create_server(int mm_fd, char *srv_name ) ;
int waitfor_server(pid_t pid )  ;
int cleanup_userserver(pid_t pid) ;
int kill_userserver( pid_t server_pid ) ;


// Query and Reply Functions
int ask_query     (int tag, int side, int coded_qtype ) ;
void true_ask_query(int tag, int side, int coded_qtype, query_type_k *pqt ) ;

 // Error and Debug Functions
void die(char *msg) {   perror(msg);   exit(255); }
void show_mm_header (mmap_hdr_k *phdr) ;        /* debug */
void show_query_type(query_type_k *q_r_type) ;  /* debug */
void show_reply_type(reply_type_k *p_r_type) ;  /* debug */
int  show_mmap(char *mm_ptr, int len ) ;         /* verbose debug */
void show_mmap_sizes( void ) ;
void show_mmap_offs( struct mmap_off_st *offs) ;
void show_mmap_ptrs(char *mm_ptr , struct mmap_ptrs_st *ptrs) ;
// if JGMDBG is defined then DBGLOC is a macro defined in ../include/dbgprt_macros.h

/*
 *  Setup the user server by:
 *    a) creating a shared area (mmap)
 *    b) creating and intializing some semaphores (Posix)
 *    c) Populating the header area of the shared memory with names and pointers.
 *    d) forking, then execve the server binary, which will inherit the fd that implements the mmap
 *    e) Pass the value of the mmap fd as a cmd line argument so the server knows which one it is.
 *       This last step is necessary because Dealer may have opened other files for eXport or CSV report
 *       and the mmap fd may not always be the same number.
 */
pid_t setup_userserver( char *pathname ) {
   userserver_path = pathname ;
   /* Initialize IPC mechanisms */
   mm_ptr = create_mmap(&mm_fd , mmap_fname) ; /* sets (char *)mm_ptr and (int )mm_fd */
   p_mm_base = (void *)mm_ptr ;                /* cast (char *) to (MMAP_TEMPLATE_k *) */
   p_qsem = open_semaphore(query_pfx, query_sema) ; /* create unique semaphores for this process */
   p_rsem = open_semaphore(reply_pfx, reply_sema) ;
   if (jgmDebug >= 3) {
         DBGLOC("Addr of reply mutex=%p Addr of query mutex=%p\n",(void *)p_rsem, (void *)p_qsem  ) ;
         DBGLOC("Create_mmap returns fd=%d, map_ptr=%p \n",mm_fd, mm_ptr ) ;
   }
   // Initialize mmap header portion
   mmap_hdr_k *p_loc_hdr = &mm_hdr_data;
   calc_ptrs(mm_ptr, &mmap_pointers, &mmap_offsets) ; // basic copies all in one place. Not used? Cleanup?
   calc_mmap_offsets(p_loc_hdr ) ;                   // fill local mm_hdr for later memcpy to mmap
   strncpy(p_loc_hdr->map_name,    mmap_fname, 31 ) ;   /* Server proc will access these via mm_fd to establish communication */
   strncpy(p_loc_hdr->q_sema_name, query_sema, 31 ) ;
   strncpy(p_loc_hdr->r_sema_name, reply_sema, 31 ) ;
   #ifdef JGMDBG
      if(jgmDebug >=3 ) {
         DBGLOC("q_off=%#lx, r_off=%#lx,  p_loc_hdr = %p\n",
            p_loc_hdr->q_type_off, p_loc_hdr->r_type_off, (void *)p_loc_hdr );
         show_mmap_ptrs(mm_ptr, &mmap_pointers ) ;
         if(jgmDebug >=8) {
            show_mmap_offs(&mmap_offsets) ;
            show_mmap_sizes( ) ;
            show_mm_header(p_loc_hdr) ;
         }
      }
   #endif
   /* local copy of mm_hdr built. Now copy it to the mmap shared area and flush it, so server can see it. */
   memcpy(mm_ptr , p_loc_hdr, sizeof(mmap_hdr_k) );
   if(jgmDebug >=3 ) {
       DBGLOC("mmap->q_off=%#lx, mmap->r_off=%#lx,  \n",
               p_mm_base->mm_hdr_dat.q_type_off, p_mm_base->mm_hdr_dat.r_type_off  );
   }
   /* now that mmap has the data, make pointers to it so we can write to the shared area */
   p_mm_hdr = (void *) mm_ptr ;
   pqt = (void *)(mm_ptr + p_mm_hdr->q_type_off) ;
   prt = (void *)(mm_ptr + p_mm_hdr->r_type_off) ;
   p_mm_cache = (void *)(mm_ptr + p_mm_hdr->mm_cache_off    ) ;
   p_mm_nsvals= (void *)(mm_ptr + p_mm_hdr->user_nsvals_off ) ;
   p_mm_ewvals= (void *)(mm_ptr + p_mm_hdr->user_ewvals_off ) ;
   p_dldata=    (void *)(mm_ptr + p_mm_hdr->deal_data_off   ) ;

   /*
    * Initialize the Cache so the first query will actually call the server
    */
    p_mm_cache->dealnum[0] = -1 ;         p_mm_cache->dealnum[1] = -1 ;
    p_mm_cache->qtag[0] = -1 ;            p_mm_cache->qtag[1] = -1 ;
    p_mm_cache->state[0] = CACHE_STALE;   p_mm_cache->state[1] = CACHE_STALE;
    if(jgmDebug >=3 ) {
       DBGLOC("Pointers using mmap shared area. q_off=%ld, pqt=%p, r_off=%ld,  prt=%p \n",
            p_mm_hdr->q_type_off, (void *)pqt, p_mm_hdr->r_type_off, (void *)prt );
    }
   msync(mm_ptr, PageSize, MS_SYNC) ;  /* flush the changes to the shared area so they are visible to others. */

   // fork a child; child will execve server; server will then init his own mmap and semaphores.
   pid_t server_pid = create_server(mm_fd,  userserver_path) ;
   /* if we return here we are in Dealer, not in the server */
   if(jgmDebug >=3 ) {  DBGLOC("Create server returns pid= %d \n", server_pid) ; }
   return (server_pid) ;
}  /* end setup userserver -- line 107 */

int cleanup_userserver(pid_t server_pid  ) {
   int exitstatus ;
   exitstatus = kill_userserver( server_pid) ; /* send server a NoMoreQuery request which will cause him to terminate normally */

   sem_close(p_rsem);
   sem_close(p_qsem);
   sem_unlink(query_sema) ;
   sem_unlink(reply_sema) ;
   JGMDPRT(3,"Server Exit OK; Sem Close & Unlink OK; Calling munmap now \n");
   if ( munmap(mm_ptr, PageSize) <    0) { perror("munmap() error"); }
   unlink(mmap_fname) ; /* delete the backing file. A new one with a new name is created on every run. */

   return (exitstatus) ;
} /* end cleanup_userserver */

int kill_userserver( pid_t server_pid ) {
   int exit_status;
   // Tell server no more to do; send him NO_MORE_QUERY request
   pqt->query_tag = -1 ;
   strcpy(pqt->query_descr, "NORMAL EOJ For Server");
   msync(mm_ptr, PageSize, MS_SYNC) ;
   sem_post(p_qsem) ;
   if(jgmDebug >=3 ) {   DBGLOC("Waiting for server pid=%d to exit \n", server_pid); }
   exit_status = waitfor_server(server_pid) ;
   if(jgmDebug >=3 ) {   DBGLOC("server pid=%d exit status = %d \n", server_pid, exit_status); }
  return (exit_status) ;
} /* end kill_userserver */

/*
 * create a backing file in /tmp and extend it to one page.
 */
char *create_mmap(int *fd , char *mmap_fname) {
   mode_t oldmask ;
   size_t bytesWritten ;
   char *mm_ptr ;
   int mm_fd ;
   oldmask = umask(0) ;
   mm_fd = mkostemp(mmap_fname,  (O_CREAT | O_TRUNC | O_RDWR) );
   umask(oldmask);
   if (mm_fd < 0 ) {die("Open mm_fd: "); }
   /*
    * Now extend the file to one PageSize
    */
      char eofmsg[8]="EOF";
      PageSize = (int)sysconf(_SC_PAGESIZE);
      if ( PageSize < 0) { die("Cant sysconf PageSize"); }
      off_t lastoffset = lseek( mm_fd, PageSize-3, SEEK_SET);
      if (lastoffset < 0 ) { die("Cant lseek on mm_fd. "); }
      else {      bytesWritten = write(mm_fd, eofmsg, 3);  } /* grow file1 to 1 page. Dont need the \0 null term here.*/
      if(jgmDebug >=3 ) {
            DBGLOC("Main created mm_fd[%d] and wrote %zd bytes to it\n", mm_fd, bytesWritten );
      }
/*
 * Now mmap the file we created and extended using the mm_fd
 */
      int len;
       off_t offset = 0;      /* start at beginning of file */
       len = PageSize;        /* Map one page */
       mm_ptr = mmap(NULL,                         /* Let Kernel Choose addr in my space */
                      len,                         /* Map one Page */
                      PROT_READ|PROT_WRITE,        /* Allow R/W access to the region; cannot conflict with file perms */
                      MAP_SHARED,                  /* Allow other procs to also map and see our updates. */
                      mm_fd,                       /* The related fd. Will be of no further use in the parent proc */
                      offset );                    /* We want to start at zero */
      if (mm_ptr == MAP_FAILED ) {die(" mmap of mm_fd failed"); }
      if(jgmDebug >=3 ) {
         DBGLOC("\nCREATEMAP_END:: File %s is using fd[%d] and the map_ptr = %p\n",
                  mmap_fname, mm_fd, (void *) mm_ptr );
      }
   *fd = mm_fd ;   /* set the global fd so we can pass to the child */
   return (mm_ptr) ;
} /* end create_mmap line 191*/

void calc_mmap_offsets(mmap_hdr_k *mm_hdr) {  /* store offsets to mmap template sections in the mm_hdr_struct */
   off_t q_off, r_off, dldata_off, user_nsvals_off, user_ewvals_off, cache_off ;

   q_off             = offsetof(MMAP_TEMPLATE_k, mm_qtype_dat      ) ;
   r_off             = offsetof(MMAP_TEMPLATE_k, mm_rtype_dat      ) ;
   dldata_off        = offsetof(MMAP_TEMPLATE_k, mm_deal_data      ) ;
   user_nsvals_off   = offsetof(MMAP_TEMPLATE_k, mm_user_nsvals    ) ;
   user_ewvals_off   = offsetof(MMAP_TEMPLATE_k, mm_user_ewvals    ) ;
   cache_off         = offsetof(MMAP_TEMPLATE_k, mm_cache          ) ;

   mm_hdr->q_type_off = q_off ;
   mm_hdr->r_type_off = r_off ;
   mm_hdr->deal_data_off = dldata_off ;
   mm_hdr->user_nsvals_off = user_nsvals_off ;
   mm_hdr->user_ewvals_off = user_ewvals_off ;
   mm_hdr->mm_cache_off  = cache_off ;
   if(jgmDebug >=3 ) {
      DBGLOC( "mmap_hdr Offsets: Q=%ld, R=%ld, D=%ld, UNS=%ld, UEW=%ld \n",
               q_off, r_off, dldata_off, user_nsvals_off, user_ewvals_off );
   }
   return ;
} /* end calc_mmap_offsets */

void calc_ptrs (char *p_mm, struct mmap_ptrs_st *p_pst, struct mmap_off_st *p_ost ) {
   p_ost->header     = offsetof(MMAP_TEMPLATE_k , mm_hdr_dat      ) ;
   p_ost->query      = offsetof(MMAP_TEMPLATE_k , mm_qtype_dat    ) ;
   p_ost->reply      = offsetof(MMAP_TEMPLATE_k , mm_rtype_dat    ) ;
   p_ost->dldata     = offsetof(MMAP_TEMPLATE_k , mm_deal_data    ) ;
   p_ost->nsres      = offsetof(MMAP_TEMPLATE_k , mm_user_nsvals  ) ;
   p_ost->ewres      = offsetof(MMAP_TEMPLATE_k , mm_user_ewvals  ) ;
   p_ost->cache      = offsetof(MMAP_TEMPLATE_k , mm_cache        ) ;

// I think we need char * here else we bump ptr by the size of obj it points to.
   p_pst->header      = (void *) (p_mm + p_ost->header ) ;
   p_pst->query       = (void *) (p_mm + p_ost->query  ) ;
   p_pst->reply       = (void *) (p_mm + p_ost->reply  ) ;
   p_pst->dldata      = (void *) (p_mm + p_ost->dldata ) ;
   p_pst->nsres       = (void *) (p_mm + p_ost->nsres  ) ;
   p_pst->ewres       = (void *) (p_mm + p_ost->ewres  ) ;
   p_pst->cache       = (void *) (p_mm + p_ost->cache  ) ;
   return ;
}  /* end calc_ptrs */

/* give the semaphore a unique name so that several instances of dealer can be running at the same time */
sem_t *open_semaphore(char *sem_pfx, char *sem_name) {
   pid_t dealer_pid = getpid() ;
   sprintf(sem_name, "%s_%06d", sem_pfx, dealer_pid ) ;
   JGMDPRT(4,"Unique Semaphore Name=%s ",sem_name);
   sem_t *p_mtx ;
   p_mtx = sem_open(sem_name, O_CREAT, 0666, 0);
   if (p_mtx == SEM_FAILED ) die("Main Cannot Open/Create semaphore ");
   JGMDPRT(4,"opened sucessfully !!\n ");
   return (p_mtx) ;
}

/*
 * Create a Server that Maps to our shared mmap.
 */
pid_t create_server(int mm_fd, char *userserver_path) {
   char *args[4] ;  /*array of 4  pointers to char The last entry must be a NULL ptr to terminate arglist */
   char buff[32];
   char dbgbuff[32];
   char logpath[128]="/tmp/DealerServer_XXXXXX.log" ;
   FILE *my_stderr ;

  pid_t server_pid = fork() ;
  if (server_pid == 0 ) { /* we are in child */
      server_pid = getpid() ;
      args[2] = NULL ;  // NULL terminate the argv list to execve -- no   debugging
      args[3] = NULL ;  // NULL terminate the argv list to execve -- with debugging

      /* Some Debugging here */
      if(srvDebug > 0 ) {
         setup_logfile(logpath) ; /* logpath is a template for a temp file name. Will redirect stderr to this path.*/
         printf( "\nServer Logfile Name=%s Check Here for Error Messages and Debugging output\n\n",logpath);
         fsync(1);
         /* if debugging then send Child/UserServer debug output to different location. we can tail -f that file in another terminal */
         DBGLOC("++++ CHILD++ After stderr Reopen. Preparing to execve the server. my_stderr=%p \n",(void *)my_stderr ) ;
         DBGLOC("++++ CHILD::  PID=%d using fd=%d\n",server_pid, mm_fd ) ;
         DBGLOC("++++ CHILD:: Sees mm_ptr=%p, p_mm_hdr=%p\n", (void *)mm_ptr, (void *)p_mm_hdr );
         DBGLOC("++++ CHILD:: Accessing MapName from Child shared area %s \n",p_mm_hdr->map_name ) ;
         fsync(2); /* force DBGLOC printout */
      }
   #ifdef JGMDBG
      snprintf(dbgbuff , 5, "%4d", srvDebug ) ; /* if Debugging; Debug Server also */
      args[2] = dbgbuff ;
   #endif
      /* end Debugging */
      snprintf(buff, 5 , "%4d", mm_fd ) ; /* prepare the argv[1] to pass to child. */
      args[0] = userserver_path ;
      args[1] = buff ;  // pass the mmap fd number as an argument to the server process. fd's are preserved.

      /* pass the mmap fd number to the child process on the cmd line; open fd's are preserved but we need to know which one*/

         JGMDPRT(3,"++++ CHILD :: Argv[0]=%s, Argv[1]=%s \n",args[0],args[1] ) ;
         JGMDPRT(3,"++++ CHILD ::  Child PID=%d calling execve with argv[1]=[%s]\n", server_pid, args[1]  ) ;

      int exe_rc = execve( userserver_path, args, NULL ) ;  /* Start the server */
                           /* Not Reached if execve is successful */
      if ( exe_rc < 0 ) { die("execve of server path FAILED") ; }
      /* NOT REACHED */
    } /* end in child server_pid == 0 */
   /* in parent; server started, resume our own work */
   if(jgmDebug >=3 ) {
      JGMDPRT(3,"After Fork; In Parent Create Child; Server_pid=%d;  Server Log=%s\n",server_pid, logpath);
      fsync(2);
   }
   if (srvDebug > 0 ) { sleep (0) ; } /* give Server time to setup its logfile and issue startup message */
   return (server_pid) ;
} /* end create server line 307 */

int waitfor_server(pid_t pid) {
   int wstatus ;
   int rc ;
   rc = waitpid(-1, &wstatus,  0);  /* the server should have exited by now. If not something wrong */

   if ( 0 == rc ) {  /* No child exited. */
      fprintf(stderr, "UserServer Process did not Exit. To force a stop, Issue  kill -9 %d before killing this process\n",pid);
      rc = waitpid(-1, &wstatus, 0 ) ;
   }


#ifdef JGMDBG
   if(jgmDebug > 0 ) {
      fprintf (stderr,"Server %d Ended with status=%d \n",pid, wstatus ) ;
      if (WIFEXITED(wstatus)) {
            fprintf (stderr,"exited $? status=%d\n", WEXITSTATUS(wstatus));
      }
      else if (WIFSIGNALED(wstatus)) {
            fprintf (stderr,"killed by signal %d %s\n", WTERMSIG(wstatus),
                  WCOREDUMP(wstatus) ? "With core file." : "");
      }
      else if (WIFSTOPPED(wstatus)) {
            fprintf (stderr, "stopped by signal %d\n", WSTOPSIG(wstatus));
      }
      else if (WIFCONTINUED(wstatus)) {
            fprintf (stderr,"continued\n");
      }
   }
#endif
  return (wstatus) ;
} /* end waitfor server */
/* Open a unique filename to log to, so we can run several instances of dealer at once. */
int setup_logfile(char *template) {
   int log_fd ;
   fclose(stderr) ;
   log_fd = mkstemps(template, 4) ;
   if (log_fd < 0 ) {die("Open log_fd for tempfile Failed: "); }
   stderr = fdopen(log_fd, "w+") ;
   return 1 ;
} /* end setup log file */

int cache_upd( UEVAL_CACHE_k *p_cache, int dealnum, int qtag, int side ) {
      p_cache->dealnum[side] = dealnum; /* was ngen but this routine called with ngen */
      p_cache->qtag[side] = qtag ;
      p_cache->state[side] = CACHE_OK ;  /* fresh */
      JGMDPRT(6,"In Dealer. Cache Update Complete. ngen=%d, qtag=%d, side=%d\n",ngen, qtag, side);
      return (CACHE_OK);
}
int cache_chk(UEVAL_CACHE_k *p_cache, int dealnum, int qtag, int side ) {
   if (p_cache->dealnum[side] == dealnum && p_cache->qtag[side] == qtag ) {return( CACHE_OK ); }
   return (CACHE_STALE) ;
}

void true_ask_query(int qtag, int side, int qcoded, query_type_k *pqt ) {
   union qcoded_ut qc ;
   qc.coded_all = qcoded ;  /* allows access to the individual fields: idx, suit, hand, sflag, hflag */
   pqt->query_tag  = qtag;
   pqt->query_side = side;
   pqt->query_hflag = qc.ucbits.hflag;
   pqt->query_hand  = qc.ucbits.hand ;
   pqt->query_sflag = qc.ucbits.sflag;
   pqt->query_suit  = qc.ucbits.suit;
   pqt->query_idx   = qc.ucbits.idx;
   p_mm_cache->state[side] = CACHE_STALE ; /* if we got here the cache must be stale; should be done by ask_query */
   JGMDPRT(6,"True AskQ qtag=%d, side=%d, hflg=%d, hand=%d, sflg=%d, suit=%d, q_idx=%d\n",
         qtag, side, qc.ucbits.hflag, qc.ucbits.hand, qc.ucbits.sflag, qc.ucbits.suit, qc.ucbits.idx) ;
      /*
       * now setup the data from the deal to be used by the user calc routines.
       * running the following set of 3 memcpy's 1 million times takes .12 secs
       * This memcpy will not be done if the results are being retrieved from the cache, nor will it be done
       * on every deal, depending on how the user has written the Dealer Input file.
      */
      memcpy(p_dldata->curdeal, curdeal, sizeof(deal52_k) ) ;
      memcpy(p_dldata->hs, hs, sizeof(HANDSTAT_k)*4 );
      memcpy(p_dldata->ss, ss, sizeof(SIDESTAT_k)*2 );
      p_dldata->curr_gen = ngen ;        /* for debugging */
      p_dldata->curr_prod = nprod ;      /* for debugging */
      msync(mm_ptr, PageSize, MS_SYNC) ;
      sem_post(p_qsem) ;   // ask the server for results
      JGMDPRT(6,"memcpy x3 done. Post Sem Done. Waiting for Server Reply \n");
      sem_wait(p_rsem) ;   // wait till we get them.
      dbg_userserver_extcalls++ ;
      /* server has answered our query .. by filling in the results fields in the mmap*/
      cache_upd(p_mm_cache, ngen, qtag, side ) ;

      /*
       * The server has filled the mmap area with results, as coded by the user.
       * It is up to the user to know which results he wants at this point.
       */
      return ;
} /* end true ask query line 412 */

int ask_query (int qtag, int side, int qcoded) {
   union qcoded_ut qc ;
   int cache_state ;
   USER_VALUES_k   *res_ptr ;
   dbg_userserver_askquery++ ;
   qc.coded_all = qcoded ; /* allows access to the sub fields of qcoded: idx, suit, hand, sflag, hflag */
   JGMDPRT(6,"Ask_Query qtag=%d, side=%d, hex_qcoded=%08x,hex_qc.coded.all=%08x \n", qtag, side, qcoded, qc.coded_all ) ;
   JGMDPRT(6,"Call cache_chk pmm_cache=%p, ngen=%d, qtag=%d, side=%d \n",
                             (void *)p_mm_cache, ngen,  qtag,  side );

   cache_state = cache_chk(p_mm_cache, ngen, qtag, side ) ;
   if (cache_state == CACHE_STALE ) {
      true_ask_query(qtag, side, qcoded, pqt ) ;
   }
   /* The user results area is current; either cached or just updated by true_ask_query*/
    /*
     * The user has four different ways of asking for the usereval results.
     * The simplest is just to specify the side and a result index from 0-63
     * But we also allow him to specify side-suit-index[4], hand-index[8], and hand-suit-index[2] combinations
     * However this is only for his convenience. He can always just use the 64 result array however he likes.
    */
    JGMDPRT(5,"****AskQ, Cache OK:side=%d,  hflg=%d, hand=%d, sflag=%d, suit=%d, qc.idx=%d qc.coded=%08X\n",
            side, qc.ucbits.hflag, qc.ucbits.hand, qc.ucbits.sflag, qc.ucbits.suit, qc.ucbits.idx, qc.coded_all) ;
    if (side == 0 ) { res_ptr = p_mm_nsvals ; }
    else            { res_ptr = p_mm_ewvals ; }
    int hidx = 0 ;
    JGMDPRT(5,"****AskQ:: Side=%d :: Results  [0..5] %d, %d, %d, %d, %d, %d \n", side,
         res_ptr->u.res[0],res_ptr->u.res[1],res_ptr->u.res[2],res_ptr->u.res[3],res_ptr->u.res[4],res_ptr->u.res[5]);
   /*
    * Each of the 4 cases a) to d) can hold 16 results.
    * But the way we have implemented case a here we can use it to access all 64 results
    */
   if ( qc.ucbits.hflag == 0 ) {          // side not hand
      if (qc.ucbits.sflag == 0 )   {      // case a: usereval(side, idx) /* idx in theory 0 .. 15 in practice 0 .. 63 */
            return ( res_ptr->u.res[qc.ucbits.idx] ) ;
      }
      else {                              // case b: usereval(side, suit, idx )
            return (res_ptr->u.dr.side_by_suit[qc.ucbits.suit][qc.ucbits.idx] ) ; /* idx should be 0 .. 3 */
      }
   } /* end side not hand */
   else {                                 // hand not side
      if (qc.ucbits.hand == 2 || qc.ucbits.hand == 3 ) { hidx = 1 ; }
      if (qc.ucbits.sflag == 0 ) {      // case c: usereval(compass, idx)
            return (res_ptr->u.dr.hand_tot[hidx][qc.ucbits.idx ] ) ; /* idx should be 0 .. 7 here */
      }
      else {                            // case d: usereval(compass, suit, idx)
          return (res_ptr->u.dr.hand_suit[hidx][qc.ucbits.suit][qc.ucbits.idx] ); /* idx should be 0 or 1 */
      }
   } /* end hand not side */
} /* end ask query line 390 */

/* these next show_mmap_xxxx routines are for debugging, so use stderr for the output */
void show_mmap_ptrs(char *p_mm , struct mmap_ptrs_st *ptrs) {
   fprintf(stderr," Dealer Base Pointer: %p \n",  (void *)p_mm   ) ;
   fprintf(stderr," \theader_ptr  = %p\n", (void *)ptrs->header  ) ;
   fprintf(stderr," \tquery_ptr   = %p\n", (void *)ptrs->query   ) ;
   fprintf(stderr," \treply_ptr   = %p\n", (void *)ptrs->reply   ) ;
   fprintf(stderr," \tdldata_ptr  = %p\n", (void *)ptrs->dldata  ) ;
   fprintf(stderr," \tnsres_ptr   = %p\n", (void *)ptrs->nsres   ) ;
   fprintf(stderr," \tewres_ptr   = %p\n", (void *)ptrs->ewres   ) ;
   fprintf(stderr," \tcache_ptr   = %p\n", (void *)ptrs->cache  ) ;
}

void show_mmap_offs( struct mmap_off_st *offs) {
   fprintf(stderr," header_off  = %zd\n", offs->header  ) ;
   fprintf(stderr," query_off   = %zd\n", offs->query   ) ;
   fprintf(stderr," reply_off   = %zd\n", offs->reply   ) ;
   fprintf(stderr," dl_data_off = %zd\n", offs->dldata  ) ;
   fprintf(stderr," nsres_off   = %zd\n", offs->nsres   ) ;
   fprintf(stderr," ewres_off   = %zd\n", offs->ewres   ) ;
   fprintf(stderr," cache_off   = %zd\n", offs->cache   ) ;
}

void show_mmap_sizes( void ) {
   fprintf(stderr,"Size of map_template_k %zd\n", sizeof(MMAP_TEMPLATE_k  ));
   fprintf(stderr,"Size of mmap_hdr_k     %zd\n", sizeof(mmap_hdr_k       ));
   fprintf(stderr,"Size of query_data_k   %zd\n", sizeof(query_type_k     ));
   fprintf(stderr,"Size of reply_data_k   %zd\n", sizeof(reply_type_k     ));
   fprintf(stderr,"Size of DEALDATA_k     %zd\n", sizeof(DEALDATA_k       ));
   fprintf(stderr,"Size of USER_VALUES_k  %zd\n", sizeof(USER_VALUES_k    ));
   fprintf(stderr,"Size of UEVAL_CACHE_k  %zd\n", sizeof(UEVAL_CACHE_k    ));
   return ;
}

/* Dump the shared memory area in char and hex fmt. 32 bytes per line */
int show_mmap(char *mm_ptr, int len ) {            /* verbose debug */
   char cbuff[128], xbuff[128];
   char *cptr=cbuff;
   char *xptr=xbuff;
   int i ;

   sprintf(cbuff,"%4x:",0);
   sprintf(xbuff,"    :");
   cptr = cbuff + 5 ;
   xptr = xbuff + 5 ;

   for (i = 1 ; i <= len ; i++, mm_ptr++ ) {
      sprintf(cptr, "%c  ", (*mm_ptr) ? *mm_ptr : '.' ) ; cptr += 3 ;
      sprintf(xptr, "%2x ",  *mm_ptr                  ) ; xptr += 3 ;
      if ( (i%32) == 0 && i > 0 ) {
         *cptr++ = ';'; *cptr++ = '\n'; *cptr = '\0';
         *xptr++ = ';'; *xptr++ = '\n'; *xptr = '\0';
         fprintf(stderr,"%s", cbuff);
         fprintf(stderr,"%s", xbuff);
         cptr = cbuff;
         xptr = xbuff;
         sprintf(cbuff,"%4x:",i);
         sprintf(xbuff,"    :");
         cptr = cbuff + 5 ;
         xptr = xbuff + 5 ;
      }
   } /* end for i <= len */
   --i ;
    if( (i%32) != 0 ) {
         *cptr++='<';  *cptr++='\n'; *cptr='\0';
         *xptr++='^';  *xptr++='\n'; *xptr='\0';
         fprintf(stderr,"%s", cbuff) ;
         fprintf(stderr,"%s", xbuff) ;
   }
      //fprintf(stderr,"\n");
      fsync(2);
  return (i);
} /* end show mmap */

/* A call to this should be inside if(jgmDebug >= ??) so no need to put that here. */
void show_query_type( query_type_k *p_q_type) {
   DBGLOC("In Show QueryType BasePTR[p_q_type]=%p\n", (void *) p_q_type) ;
   DBGLOC("SHOWQuery:: Descr[%s], Tag=[%d], Values=[%d, %d, %d, %d]\n", p_q_type->query_descr, p_q_type->query_tag,
         p_q_type->q_vals[0], p_q_type->q_vals[1], p_q_type->q_vals[2], p_q_type->q_vals[3] );
   return ;
}

void show_reply_type(reply_type_k *p_r_type) {
   DBGLOC("In Show Reply Data in Main; p_r_type=%p\n", (void *) p_r_type) ;
   DBGLOC("MAIN shoReply:: Descr[%s], Tag=[%d], Values=[%d, %d, %d, %d]\n", p_r_type->reply_descr, p_r_type->reply_tag,
         p_r_type->r_vals[0], p_r_type->r_vals[1], p_r_type->r_vals[2], p_r_type->r_vals[3] );
   return ;
}

void show_mm_header (mmap_hdr_k *phdr) {
   DBGLOC("SHOWMMAP_HDR:: \n\tMapName=%s, q_sema_name=%s, r_sema_name=%s\n", phdr->map_name,  phdr->q_sema_name, phdr->r_sema_name);
   DBGLOC("\tq_type_off=%ld, r_type_off=%ld, deal_data_off=%ld, user_nsvals_off=%ld, user_ewvals_off=%ld cache_off=%ld\n",
      phdr->q_type_off, phdr->q_type_off, phdr->deal_data_off, phdr->user_nsvals_off, phdr->user_ewvals_off, phdr->mm_cache_off );
}
 /* end dealServer_subs.c File */




