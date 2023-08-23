/* tst_user_eval.c Scaffolding to test the userEval.c code  */
#define _GNU_SOURCE
// #define JGMDBG 1         /* Define this on the gcc line with -DJGMDBG if needed */
#include <assert.h>         /* dbg  suppress with -DNDEBUG on the gcc line*/
#include <ctype.h>          /* toupper, tolower, isalpha*/
#include <errno.h>          /* errno, ECHILD            */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <pthread.h>        /* required for posix semaphore use? */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. Posix version */
#include <signal.h>         /* to Kill server and also report on server exit status ? */
#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <string.h>         /* strcpy, strlen, memcpy   */
#include <sys/mman.h>       /* mmap                     */
#include <sys/stat.h>       /* To define stat struct. used to check existence of files */
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <sys/wait.h>       /* Wait status etc. for server */
#include <unistd.h>         /* Routines that should be in stdlib but are not for some reason */
/*
 * other headers that are used elsewhere in dealer
 * stddef.h, time.h, random.h,
 */

#include "../include/dbgprt_macros.h"     /* DBGLOC and DBGPRT */
#include "../include/mmap_template.h"     /* For easy use of offsets and pointers */


/*
 * Some Globals -- many with static to limit scope to this file.
 */
/* semaphore definition -- names begin with slash and have no other slashes */
static char query_sema[32]="/q_sem";
static char reply_sema[32]="/r_sem";
static sem_t *p_qmtx, *p_rmtx ;

/* mmap definition */
static int PageSize  = 4096 ;
static char mmap_fname[32]="/tmp/my_mmap";    /* no more than 31 chars */
static char *mm_ptr ;                         /* THE pointer to mmap shared area */
static int  mm_fd   ;                         /* the fd returned by the mmap open call */
/* fork / server definition */
static char server_path[256] = {"UserEval" }; /* ./UserEval does not work. /home/greg19/Programming/Bridge_SW/JGMDealer/deal_v5/src is 56 long */

// mmap templates and pointers
MMAP_TEMPLATE_k *p_mm_data, mm_data;
mmap_hdr_k      *p_mm_hdr ;
query_type_k    *pqt;
reply_type_k    *prt;
DEALDATA_k      *p_dldata;
USER_VALUES_k   *p_user_vals;

/*
 * Functions prototypes
 */
 // Setup and Teardown Functions
char *create_mmap(int *mm_fd, char *mmap_fname );
void calc_mmap_offsets(mmap_hdr_k *mm_hdr) ;
sem_t *open_semaphore(char *sem_name) ;
pid_t create_server(int mm_fd, char *srv_name ) ;
void  waitfor_server(pid_t pid )  ;
void eoj_mutex(  ) ;

// Query and Reply Functions
void fill_query_type( int count, int tag, query_type_k *mm_query_ptr ) ; /* dummy function for some data */

 // Error and Debug Functions
void die(char *msg) {   perror(msg);   exit(255); }
void show_mm_header (mmap_hdr_k *phdr) ;        /* debug */
void show_query_type(query_type_k *q_r_type) ;  /* debug */
void show_reply_type(reply_type_k *p_r_type) ;  /* debug */
int show_mmap(char *mm_ptr, int len ) ;             /* verbose debug */
// DBGLOC is a macro defined if JGMDBG is defined.

int main(int argc, char *argv[] ) {

   /* Initialize IPC mechanisms */
   mm_ptr = create_mmap(&mm_fd , mmap_fname) ; /* sets mm_ptr and mm_fd */
   p_qmtx = open_semaphore(query_sema) ;
   p_rmtx = open_semaphore(reply_sema) ;

   DBGLOC("Addr of reply mutex=%p Addr of query mutex=%p\n",(void *)p_rmtx, (void *)p_qmtx  ) ;
   DBGLOC("Create_mmap returns fd=%d, map_ptr=%p \n",mm_fd, mm_ptr ) ;

   // Initialize mmap data
   mmap_hdr_k *p_loc_hdr = &mm_data.mm_hdr_dat;
   calc_mmap_offsets(p_loc_hdr) ;        /* populate offsets in mm_hdr local storage*/
   DBGLOC("q_off=%#lx, r_off=%#lx,  p_loc_hdr = %p\n", p_loc_hdr->q_type_off, p_loc_hdr->r_type_off, p_loc_hdr );
   strncpy(p_loc_hdr->map_name,    mmap_fname, 31 ) ;   /* Server proc will access these via mm_fd to establish communication */
   strncpy(p_loc_hdr->q_sema_name, query_sema, 31 ) ;
   strncpy(p_loc_hdr->r_sema_name, reply_sema, 31 ) ;
   /* local copy of mm_hdr built. Now copy it to the mmap shared area and flush it, so server can see it. */
   memcpy(mm_ptr , &mm_data, sizeof(MMAP_TEMPLATE_k) ); /* Really only copying hdr since query and reply not setup */

   DBGLOC("q_off=%#lx, r_off=%#lx,  \n", p_loc_hdr->q_type_off, p_loc_hdr->r_type_off );
   /* now that mmap has the data, make pointers to it so we can write to the shared area */
   p_mm_hdr = (void *) mm_ptr ;
   pqt = (void *)(mm_ptr + p_mm_hdr->q_type_off) ;
   prt = (void *)(mm_ptr + p_mm_hdr->r_type_off) ;
   DBGLOC("Main:: Pointers using mmap shared area. q_off=%ld, pqt=%p, r_off=%ld,  prt=%p \n",
            p_mm_hdr->q_type_off, (void *)pqt, p_mm_hdr->r_type_off, (void *)prt );
   msync(mm_ptr, PageSize, MS_SYNC) ;

   // fork a child; child will execve server; server will then init his own mmap and semaphores.
   pid_t server_pid = create_server(mm_fd,  server_path) ;
   DBGLOC("Create server returns pid= %d 1 Sec Nap in Main Loop\n", server_pid) ;
   //sleep(1) ; // give server time to get set up.

   DBGLOC("Starting for loop for queries .. \n");

  // create a series of queries (B, D, G, L, M, P, X)
  //char tst_query[20] = {'B','D', 'G','L','M','P', 'U', 'X' };
    int  tst_query[20] = {10,  20,  30, 40, 50, 60, 70,  NO_MORE_QUERY } ; /* 70 should return the unknown result */
  for (int q = 0 ; tst_query[q] != NO_MORE_QUERY ; q++ ) {
      DBGLOC("Calling Fill query with pqt = %p\n",(void*)pqt );
      fill_query_type((q+1), tst_query[q], pqt ) ;
      show_query_type( pqt) ; /* debug */
      msync(mm_ptr, PageSize, MS_SYNC) ;  /* write the shared area so the server can see it. */
      fsync(2);
      // show_mmap(mm_ptr, 170) ;
      sem_post(p_qmtx) ;      /* wake up server */
      DBGLOC("Posted Query. Waiting for Server to reply \n");
      sem_wait(p_rmtx) ;      /* wait for server to post results */
      DBGLOC("Sem_wait p_rmtx returns.  Calling show reply with prt = %p\n",(void*)prt );
      show_reply_type(prt) ;
   }
   // we are at end of list. Tell server no more to do; we can either kill him, or send him NO_MORE_QUERY
   fill_query_type(NO_MORE_QUERY, NO_MORE_QUERY, pqt) ;
   msync(mm_ptr, PageSize, MS_SYNC) ;
   fsync(2);
   sem_post(p_qmtx) ;
   DBGLOC("Waiting for server to exit \n");
   waitfor_server(server_pid) ;

  if ( munmap(mm_ptr, PageSize) <    0) {
     perror("munmap() error");
  }

   void eoj_mutex(  ) ;
   DBGLOC("Main ending now \n");
   return(0) ;
}/* end main Query */

void fill_query_type(int q_count, int query_tag, query_type_k *pqt ) {
   pqt->query_tag = query_tag ;
   for (int qi = 0 ; qi < 4 ; qi++) { pqt->q_vals[qi] = qi + 10*q_count ; } /* dummy data */
   sprintf(pqt->query_descr, "Query # %d tag=%d",q_count, query_tag );
   return ;
} /* end fill query data */

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
   DBGLOC("\tq_type_off=%ld, r_type_off=%ld, deal_data_off=%ld, user_nsvals_off=%ld, user_ewvals_off=%ld\n",
      phdr->q_type_off, phdr->q_type_off, phdr->deal_data_off, phdr->user_nsvals_off, phdr->user_ewvals_off );
}
/*
 * create a backing file in /tmp and extend it to one page.
 */
char *create_mmap(int *fd , char *mmap_fname) {
   mode_t oldmask ;
   size_t bytesWritten ;
   char *mm_ptr ;
   int mm_fd ;
   oldmask = umask(0) ;
   mm_fd = open(mmap_fname,  (O_CREAT | O_TRUNC | O_RDWR), 0660 );
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

      DBGLOC("Main created mm_fd[%d] and wrote %zd bytes to it\n", mm_fd, bytesWritten );
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

DBGLOC("\nMCREATEMAP_END:: File %s is using fd[%d] and the map_ptr = %p\n", mmap_fname, mm_fd, (void *) mm_ptr );
   *fd = mm_fd ;   /* set the global fd so we can pass to the child */
   return (mm_ptr) ;
} /* end create_mmap */

sem_t *open_semaphore(char *sem_name) {
   sem_t *p_mtx ;
   p_mtx = sem_open(sem_name, O_CREAT, 0666, 0);
   if (p_mtx == SEM_FAILED ) die("Main Cannot Open/Create semaphore ");
   return (p_mtx) ;
}

void eoj_mutex(  ) {
   sem_close(p_rmtx);
   sem_close(p_qmtx);

   sem_unlink(query_sema) ;
   sem_unlink(reply_sema) ;
   return ;
}
/*
 * Create a Server that Maps to our shared mmap.
 */
pid_t create_server(int mm_fd, char *server_path) {
   char *args[3] ;  /*array of 3  pointers to char */
   char buff[32];
  pid_t server_pid = fork() ;
  if (server_pid == 0 ) { /* we are in child */
      server_pid = getpid() ;
      /* Some Debugging here */
      DBGLOC("++++ CHILD::  PID=%d using fd=%d\n",server_pid, mm_fd ) ;
      DBGLOC("++++ CHILD:: Sees mm_ptr=%p, p_mm_hdr=%p\n", (void *)mm_ptr, (void *)p_mm_hdr );
      DBGLOC("++++ CHILD:: Accessing MapName from Child shared area %s \n",p_mm_hdr->map_name ) ;
      strcpy(p_mm_hdr->map_name, mmap_fname ); /* 31 chars max */
      DBGLOC("++++ CHILD:: MapName is now set to: %s\n",p_mm_hdr->map_name ) ; /* will this cause core dump? */
      fsync(2); /* force DBGLOC printout */
      /* end Debugging */
      snprintf(buff, 5 , "%4d", mm_fd ) ; /* prepare the argv[1] to pass to child. */
      args[0] = server_path ;
      args[1] = buff ;  // pass the mmap fd number as an argument to the server process. fd's are preserved.
      args[2] = NULL ;  // NULL terminate the argv list to execve
      /* pass the mmap fd number to the child process on the cmd line; open fd's are preserved but we need to know which one?*/
      DBGLOC("++++ CHILD :: Argv[0]=%s, Argv[1]=%s \n",args[0],args[1] ) ;
      DBGLOC("++++ CHILD ::  Child PID=%d calling execve with argv[1]=[%s]\n", server_pid, args[1]  ) ;
      int exe_rc = execve( server_path, args, NULL ) ;  /* Start the server */
                           /* Not Reached if execve is successful */
      if ( exe_rc < 0 ) { die("execve of server path FAILED") ; }
      /* NOT REACHED */
    } /* end in child */
   /* in parent; server started, resume our own work */
   DBGLOC("After Fork; In Parent Create Child; Server_pid=%d; napping for 1 sec; \n",server_pid);
   fsync(2);
   //sleep(1) ;
   return (server_pid) ;
} /* end create server */

void waitfor_server(pid_t pid) {
   int wstatus ;
   waitpid(-1, &wstatus,  0);
   printf ("Server %d Ended with status=%d \n",pid, wstatus ) ;
                    if (WIFEXITED(wstatus)) {
                       printf("exited $? status=%d\n", WEXITSTATUS(wstatus));
                   } else if (WIFSIGNALED(wstatus)) {
                       printf("killed by signal %d %s\n", WTERMSIG(wstatus),
                           WCOREDUMP(wstatus) ? "With core file." : "");
                   } else if (WIFSTOPPED(wstatus)) {
                       printf("stopped by signal %d\n", WSTOPSIG(wstatus));
                   } else if (WIFCONTINUED(wstatus)) {
                       printf("continued\n");
                   }
  return ;
} /* end waitfor server */

void calc_mmap_offsets(mmap_hdr_k *mm_hdr) {  /* store offsets to mmap template sections in the mm_hdr_struct */
   off_t hdr_off, q_off, r_off, dldata_off, user_nsvals_off, user_ewvals_off ;
   hdr_off = 0 ;
   q_off           = hdr_off         + sizeof(mmap_hdr_k) ;
   r_off           = q_off           + sizeof(query_type_k) ;
   dldata_off      = r_off           + sizeof(reply_type_k) ;
   user_nsvals_off = dldata_off      + sizeof(DEALDATA_k);
   user_ewvals_off = user_nsvals_off + sizeof(USER_VALUES_k);
   mm_hdr->q_type_off = q_off ;
   mm_hdr->r_type_off = r_off ;
   mm_hdr->deal_data_off = dldata_off ;
   mm_hdr->user_nsvals_off = user_nsvals_off ;
   mm_hdr->user_nsvals_off = user_nsvals_off ;
   DBGLOC( "mmap_hdr Offsets: Q=%ld, R=%ld, D=%ld, UNS=%ld, UEW=%ld \n", q_off, r_off, dldata_off, user_nsvals_off, user_ewvals_off );
   return ;
} /* end calc_mmap_offsets */

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
         printf("%s", cbuff);
         printf("%s", xbuff);
         cptr = cbuff;
         xptr = xbuff;
         sprintf(cbuff,"%4x:",i);
         sprintf(xbuff,"    :");
         cptr = cbuff + 5 ;
         xptr = xbuff + 5 ;
      }
   } /* end for */
   --i ;
    if( (i%32) != 0 ) {
         *cptr++='<';  *cptr++='\n'; *cptr='\0';
         *xptr++='^';  *xptr++='\n'; *xptr='\0';
         printf("%s", cbuff) ;
         printf("%s", xbuff) ;
   }
      //printf("\n");
      fsync(1);
  return (i);
}







