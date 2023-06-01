   /* File {ROOT}/UserEval/src/UserServer.c  -- The Server side  JGM 2022-Nov-15
 *  Version  Date          Comment
 *    0.1    2022-10-20    First Draft
 *    0.5    2022-11-13    Minimal Functionality
 *    0.7    2022-12-02    Was Working. Now moved to its own directory tree.
 *    0.8    2022-12-21    Server setup, sync, and communication with Dealer working well.
 *    0.9    2023-01-01    Bergen, Karpin, Pavlicek metrics coded and working.
 *    0.91   2023-01-13    Larsson metric working; Begin adjHCP redo;
 *    0.9.6  2023-01-13    Refactor Trump Fit into Do_Trump_fit. DKP, jgm1, jgm0, mixed calc, etc. working
 *    0.9.7  2023-01-26    KnR with Fit working.
 *    1.0.0  2023-02-17    Most relevant metrics working. Set88 working. Prep for first github upload
 *    1.0.1  2023-03-31    Set88 modified to return both HLDF and NT pts. More Metrics. Bissell,....
 *    1.0.5  2023-04-10    Coded Sheinwold, Goren; more debugging.
 *    1.5    2023-05-26    Coded all metrics including ZarBasic and ZarFull. Dropped Rule22.
 */
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
/* This is an independent program execve'd by dealer main when the input file calls for usereval(...)
 * This piece looks after the mapping to the shared area, the initializing of the shared semaphores,
 * and the while loop which waits for a semaphore signal from the dealer program.
 *
 * The shared memory area is 4096 bytes. The 'template' provided in the header file is approx 3200 bytes
 * so there is room at the end for further expansion. But that would require mods to the dealer main.
 *
 * The user could also write his external program in another language if such language supports
 * access to a Linux shared memory area.
 *
 */
#define SERVER_NAME "DealerServer"
#define SERVER_VERSION "1.5.0"
#define SERVER_BUILD_DATE "2023/05/26"
#define SERVER_AUTHOR "JGM"
int jgmDebug = 1 ;  /* value will be passed in from Dealer if Debugging wanted */
#include "../include/std_hdrs_all.h"
/* The Interface to Dealer These are symbolic links to the file in the Dealer include directory*/
#include "../include/dealtypes.h"            /* HANDSTAT_k, deal52_k ... */
#include "../include/mmap_template.h"        /* THE key struct and typedefs for IPC between query and reply */
/* The interface to the user supplied calculation routines */
#include "../include/UserEval_types.h"
#include "../include/UserEval_protos.h"
#include "../include/Server_protos.h"
/* UserServer extras */
#include "../include/UserServer_globals.c"  /* no separate comp. no extern needed. file incl into UserServer.c source */
#include "../include/dbgprt_macros.h"


/*
 * Functions prototypes
 */
// Setup  Teardown functions
char *link_mmap(int mm_fd );
sem_t *open_semaphore(char *sem_name) ;
void calc_ptrs ( char *p_mm, struct mmap_ptrs_st *p_pst, struct mmap_off_st *p_ost ) ;
void eoj_mutex(  ) ;
void server_eoj() ;

// Evaluation Reply Functions
/* Replace all these with one function which accepts arguments to fill in the reply block and the user map area */
int user_reply( int tag, int ures[64], struct reply_type_st *prt, struct query_type_st *pqt );
int userfunc( struct query_type_st *p_q_type, struct reply_type_st *p_r_type, DEALDATA_k *p_dldat,
               USER_VALUES_k *p_nsdat,USER_VALUES_k *p_ewdat, MMAP_TEMPLATE_k *mm_ptr) ;
int bergen_reply(   USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int bissell_reply(  USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int dkp_reply(      USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int goren_reply(    USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int hcp_reply(      USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int jgm1_reply(     USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int kaplan_reply(   USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int karpin_reply(   USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int knr_reply(      USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int larsson_reply(  USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int morse_reply(    USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int pav_reply(      USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int sheinw_reply(   USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int zarbasic_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int zarfull_reply(  USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int unk_reply(      USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int mixed_JGM1reply(USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int mixed_LARreply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int test_reply(     USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
int set88_reply(    USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt);
/*
 * Future:
 *  single factors not full metrics; things like QuickTricks, longest/shortes suit, slamControls, Woolsey pts. etc.
 * that are hard to do using std DealerV2 syntax
 * Remote possibility to convert OPC pts to a UserEval metric and so avoid external Perl program.
 */

// Error and Debug functions -- see also Serverdebug_subs.c
static void die(char *msg) {   perror(msg);   exit(255); }
static void show_mmap_ptrs( MMAP_TEMPLATE_k *p_mm, struct mmap_ptrs_st *ptrs, struct mmap_off_st *offs ) ;
void show_hands_pbn( int mask, deal d ) ;
void show_reply_type(struct reply_type_st *prt) ;


// DBGLOC and JGMDPRT are  macros defined if JGMDBG is defined, Do nothing definitions otherwise
extern float DBG_run ;
int setup_logfile(char *template) ;
int main(int argc, char *argv[] ) {
   pid_t my_pid ;
   int urc ;
   int mm_fd ;
   char          *mm_ptr;  /* The base address of the mmap page -- has to be char ptr for offset math to work */
   mmap_hdr_k    *phdr;
   query_type_k  *pqt;
   reply_type_k  *prt;
   DEALDATA_k    *pdldat;
   USER_VALUES_k *p_nsres;
   USER_VALUES_k *p_ewres;
   MMAP_TEMPLATE_k *p_mm_base ;
   deal          *pcurdeal;
    my_pid = getpid() ;
    if (argc < 2 )  {die("DealerServer Missing FD arg. Aborting..."); }
    if (0 == strcmp(argv[1], "-V") ) {
       printf("%s Version %s for DealerV2.  Author=%s Build Date=%s\n",SERVER_NAME, SERVER_VERSION,SERVER_AUTHOR,SERVER_BUILD_DATE ) ;
       printf("Usage: %s <mmap_fd> [dbg_verbosity]  -- note no commas, no minus signs \n", SERVER_NAME);
#ifdef JGMDBG
       JGMDPRT(1, "JGMDBG IS DEFINED= %d in Serverpid=%d; jgmDebug=%d Server_Version = %s DBG_run=%g\n",
               JGMDBG, my_pid, jgmDebug, SERVER_VERSION , DBG_run);
#endif
       return(0);
    }

    if (argc >= 3 ) {
       jgmDebug = atoi( argv[2] ) ;
    }
    JGMDPRT(1, "JGMDPRT DEFINED= %d in Serverpid=%d; jgmDebug=%d Server_Version = %s DBG_run=%g\n",
               JGMDBG, my_pid, jgmDebug,SERVER_VERSION , DBG_run);

    mm_fd = atoi(argv[1] );
   /* Initialize IPC mechanisms */
   mm_ptr = link_mmap( mm_fd ) ; /* sets (char *)mm_ptr */
   p_mm_base = (void *)mm_ptr ;
   JGMDPRT(1,"In %s:: link_mmap returns map_ptr=%p p_mm_tmpl=%p\n",SERVER_NAME,(void *)mm_ptr, (void *)p_mm_base ) ;

   strcpy( (mm_ptr + 4090), "EOF " ); /* so we can see it in a dump of disk file -- Assumes PageSize is 4096*/
   /*
    * Use the data in the mmap_hdr area to find the names and offsets used by the client
    */
    calc_ptrs( mm_ptr, &ptrs, &offs ) ;  // populate the structs using only the mmap_template defn */
    phdr =    ptrs.header;
    pqt =     ptrs.query;
    prt =     ptrs.reply;
    pdldat =  ptrs.dldata;
    pcurdeal= ptrs.p_deal;
    p_nsres = ptrs.nsres ;
    p_ewres = ptrs.ewres ;
    for (int h = 0 ; h < 4 ; h++ ) { hs_ptr[h] = ptrs.phs[h] ; } /* so everything can work off a HANDSTAT typedef */
#ifdef JGMDBG
    if (jgmDebug >= 2 ) { show_mmap_ptrs(p_mm_base, &ptrs, &offs );}
#endif
    strncpy(query_sema, phdr->q_sema_name, 31 ) ;
    strncpy(reply_sema, phdr->r_sema_name, 31 ) ;
    strncpy(mmap_fname, phdr->map_name,   128 ) ;
    JGMDPRT(2, "^^^UserEval Local Copy from shared area: map_name=%s, q_sema=%s, r_sema=%s\n",mmap_fname, query_sema,  reply_sema );

    p_qsem = open_semaphore(query_sema) ;
    p_rsem = open_semaphore(reply_sema) ;
    JGMDPRT(2, "^^^UserEval:: Addr of reply sema=%p Addr of query sema=%p\n",(void *)p_rsem, (void *)p_qsem  ) ;

  /*
    * Server ready to re-act to query posts and issue replies
    */
    //sleep(1) ;
  while( 1 ) {
    sem_wait(p_qsem) ;
    JGMDPRT(4,"in Server:: Woke from p_qsem wait with Query Tag[%i], Descr=[%s], side=%d, idx=%d\n",
               pqt->query_tag, pqt->query_descr,pqt->query_side, pqt->query_idx) ;
    if(pqt->query_tag < 0 ) {
       JGMDPRT(3,"Server pid=%d Got QUIT Query. Descr=%s \n",my_pid, pqt->query_descr) ;
       server_eoj(mm_ptr, PageSize ) ;
       JGMDPRT(1,"%s cleanup done. Exiting from while(1) main loop \n",SERVER_NAME);
       exit(0) ;
       /* NOTREACHED */
    }
    // fprintf(stderr, "***** DealerServer calling sortDeal52 @line %d with dealptr=%p\n", __LINE__, (void *) pcurdeal);
    sortDeal52( (char *)pcurdeal ) ; /* why is not sorted by Dealer already? */

    #ifdef JGMDBG
      if (jgmDebug >= 5) {
         fprintf(stderr, "*------%s.%d Begin Sorted Deal Number %d -------*\n", __FILE__,__LINE__,pdldat->curr_gen );
         show_hands_pbn( 15 , (char *)pcurdeal ) ;
      }
    #endif
    urc = userfunc(pqt, prt, pdldat, p_nsres, p_ewres, p_mm_base) ; /*userfunc uses pqt->query_tag, to call relevant xxxx_reply() */
    if (-1 == urc ) { die("Fatal in User_eval; Aborting. Must Manually stop dealverV2") ; }
/* got OK result. Tell client */
   sem_post(p_rsem) ;
   JGMDPRT(4,"ServerMain:: Dealnum=%d user_eval done for q_tag[%d], r_tag[%d], r_descr=[%s] Waiting for next Query. zzzz \n",
                      pdldat->curr_gen, pqt->query_tag, prt->reply_tag, prt->reply_descr);
  } /* end while(1) */
  return (0) ;  /* NOT REACHED */
}/* end main User_Eval */

/*
 * This next function belongs in a separate file to allow for easy recompilation
 */
int userfunc( struct query_type_st *pqt, struct reply_type_st *prt, DEALDATA_k *p_dldat,
               USER_VALUES_k *p_nsdat,USER_VALUES_k *p_ewdat, MMAP_TEMPLATE_k *mm_ptr)
{
    USER_VALUES_k *res_ptr;
    if (pqt->query_side == 0 ) { res_ptr = p_nsdat ; }
    else                       { res_ptr = p_ewdat ; }
    JGMDPRT(4,"^^^userfunc Switch: for tag=[%d] Descr=[%s] using reply ptr=%p\n",pqt->query_tag, pqt->query_descr, (void *)prt ) ;
    JGMDPRT(4,"Side=%d, Res_ptr=%p \n", pqt->query_side, (void *)res_ptr ) ;

    prt->reply_tag = pqt->query_tag ;  // setup a default. May be over-ridden by code in switch
    strcpy( prt->reply_descr ,  "Normal Server Return" );
/*
 * The Query tags in alpha order: The adj_hcp arrays use these values to lookup adjustments.
 *                     0        1       2     3     4      5      6       7    8    9     10     11       12      13      14        20          21
                    BERGEN=0, BISSEL,  DKP, GOREN, JGM1, KAPLAN, KARPIN, KnR, LAR, MORSE, PAV, SHEINW,  ZARBAS, ZARFULL, metricEND, MixJGM=20, MixMOR,
// possibly add metrics in the 50 - 79 range to implement different hand factors like quicktricks, or quicklosers, or shortest suit etc.
                    SET=88, SYNTST=99, Quit=-1} ;
*/
    switch (pqt->query_tag) {
      case 'B':
      case  BERGEN:  bergen_reply(  res_ptr, prt, p_dldat, pqt);  break ; /* Bergen   */
      case 'b':
      case  BISSEL:  bissell_reply( res_ptr, prt, p_dldat, pqt);  break ; /* Bissel from Bridge Encyclopedia and Pavlicek */
      case 'D':
      case  DKP :    dkp_reply(     res_ptr, prt, p_dldat, pqt); break ; /* D Kleinman LJP from NoTrump Zone */
      case 'G':
      case  GOREN :  goren_reply(   res_ptr, prt, p_dldat, pqt);  break ; /* Goren shortness pts    */
      case 'J':
      case JGM1:     jgm1_reply(    res_ptr, prt, p_dldat, pqt);  break ; /* JGM1  Karpin with BumWrap points  */
      case 'E': /* for Edgar */
      case KAPLAN :  kaplan_reply(  res_ptr, prt, p_dldat, pqt);  break ; /* Kaplan -- length from 1960's book */
      case 'K':
      case KARPIN :  karpin_reply(  res_ptr, prt, p_dldat, pqt);  break ; /* Karpin - length from Pavlicek     */
      case 'k':
      case KnR :     knr_reply(     res_ptr, prt, p_dldat, pqt);  break ; /* KnR 4C's with Dfit  */
      case 'L':
      case LAR :     larsson_reply( res_ptr, prt, p_dldat, pqt);  break ; /* Larsson  */
      case 'M':
      case MORSE:    morse_reply(   res_ptr, prt, p_dldat, pqt);  break ; /* Larsson with BumWrap and Dfit mods   */
      case 'P':
      case PAV :     pav_reply(     res_ptr, prt, p_dldat, pqt);  break ; /* PAV -- from  Website. Like Goren minor mods */
      case 'S':
      case SHEINW :  sheinw_reply(  res_ptr, prt, p_dldat, pqt);  break ; /* Sheinwold from book. Short suits. */
      case 'z':
      case ZARBAS :  zarbasic_reply(res_ptr, prt, p_dldat, pqt);  break ; /* Basic Zar pts from the 2005 PDF download */
      case 'Z':
      case ZARFULL:  zarfull_reply(  res_ptr, prt, p_dldat, pqt);  break ; /* Basic Zar plus HF, FN, HCP in 2/3 suits etc */

      case 20:    mixed_JGM1reply(  res_ptr, prt, p_dldat, pqt);  break ; /* jgm1 and karpin     */
      case 21:    mixed_LARreply(   res_ptr, prt, p_dldat, pqt);  break ; /* morse and larsson   */

      case 88:     set88_reply(     res_ptr, prt, p_dldat, pqt);  break ; /* do all the metrics for which there is code */
      case 99:     test_reply(      res_ptr, prt, p_dldat, pqt);  break ; /* fill the slots with coded values to test syntax  */
      case 'Q':
      case Quit:
            server_eoj(mm_ptr, PageSize) ; /* Dealer is finished. Cleanup our stuff  */
            JGMDPRT(1,"%s cleanup done. Exiting from Quit is mainloop switch statement \n",SERVER_NAME);
            exit (0) ;
            /*NOT REACHED */
            break ;
      default  : unk_reply(    res_ptr, prt, p_dldat, pqt); break ;      /* Unknown      */
   } /* end switch */

   msync((void *)mm_ptr, PageSize, MS_SYNC) ;
   if (jgmDebug >= 5 ) {
         JGMDPRT(5,"Switch Done. Server Msync Done. prt.tag=[%d], prt.descr=[%s] \n", prt->reply_tag, prt->reply_descr );
         show_reply_type( prt) ;
         fsync(2); /* debug */
   }

   return (1) ;
} /* end userfunc */

void calc_ptrs (  char *p_mm, struct mmap_ptrs_st *p_pst, struct mmap_off_st *p_ost ) {
   p_ost->header     = offsetof(MMAP_TEMPLATE_k , mm_hdr_dat      ) ;
   p_ost->query      = offsetof(MMAP_TEMPLATE_k , mm_qtype_dat    ) ;
   p_ost->reply      = offsetof(MMAP_TEMPLATE_k , mm_rtype_dat    ) ;
   p_ost->dldata     = offsetof(MMAP_TEMPLATE_k , mm_deal_data    ) ;
   p_ost->nsres      = offsetof(MMAP_TEMPLATE_k , mm_user_nsvals  ) ;
   p_ost->ewres      = offsetof(MMAP_TEMPLATE_k , mm_user_ewvals  ) ;
   p_ost->cache      = offsetof(MMAP_TEMPLATE_k , mm_cache        ) ;
   p_ost->hs_arr     = offsetof(MMAP_TEMPLATE_k , mm_deal_data.hs ) ;
   p_ost->curdeal    = offsetof(MMAP_TEMPLATE_k , mm_deal_data.curdeal );
   p_pst->header      = (void *) (p_mm + p_ost->header ) ;
   p_pst->query       = (void *) (p_mm + p_ost->query  ) ;
   p_pst->reply       = (void *) (p_mm + p_ost->reply  ) ;
   p_pst->dldata      = (void *) (p_mm + p_ost->dldata ) ;
   p_pst->nsres       = (void *) (p_mm + p_ost->nsres  ) ;
   p_pst->ewres       = (void *) (p_mm + p_ost->ewres  ) ;
   p_pst->cache       = (void *) (p_mm + p_ost->cache  ) ;
   p_pst->p_deal      = (void *) (p_mm + p_ost->curdeal) ;
   for (int h=0 ; h<4 ; h++ ) {
      p_pst->phs[h] = &(p_pst->dldata->hs[h]) ; /* hs[h] is the base address of the handstat struct for hand h; Copy of all handstats in mmap at addr dldatat */
   }
   return ;
}

/*
 * Call with: if(jgmDebug >= n ) { show_mmap_ptrs( ... ) ; }
 */

void show_mmap_ptrs(MMAP_TEMPLATE_k *p_mm, struct mmap_ptrs_st *ptrs, struct mmap_off_st *offs ) {
   fprintf(stderr, "%s.%d show_mmap_ptrs\n",__FILE__,__LINE__ ) ;

   fprintf(stderr," Server Base Pointer: %p \n", (void *)p_mm    ) ;
   fprintf(stderr," \theader_ptr  = %p\n", (void *)ptrs->header  ) ;
   fprintf(stderr," \tquery_ptr   = %p\n", (void *)ptrs->query   ) ;
   fprintf(stderr," \treply_ptr   = %p\n", (void *)ptrs->reply   ) ;
   fprintf(stderr," \tdldata_ptr  = %p\n", (void *)ptrs->dldata  ) ;
   fprintf(stderr," \tnsres_ptr   = %p\n", (void *)ptrs->nsres   ) ;
   fprintf(stderr," \tewres_ptr   = %p\n", (void *)ptrs->ewres   ) ;
   fprintf(stderr," \tcache_ptr   = %p\n", (void *)ptrs->cache   ) ;
   fprintf(stderr," \tptrs.phs[0] = %p\n", (void *)ptrs->phs[0]  ) ;
   fprintf(stderr," \tptrs.phs[1] = %p\n", (void *)ptrs->phs[1]  ) ;
   fprintf(stderr," \tptrs.phs[2] = %p\n", (void *)ptrs->phs[2]  ) ;
   fprintf(stderr," \tptrs.phs[3] = %p\n", (void *)ptrs->phs[3]  ) ;
   fprintf(stderr," \tptrs.p_deal = %p\n", (void *)ptrs->p_deal  ) ;

 if (jgmDebug >= 8) {
   fprintf(stderr," header_off   = %zd\n", offs->header    ) ;
   fprintf(stderr," query_off    = %zd\n", offs->query     ) ;
   fprintf(stderr," reply_off    = %zd\n", offs->reply     ) ;
   fprintf(stderr," dl_data_off  = %zd\n", offs->dldata    ) ;
   fprintf(stderr,"\tHandstat_off= %zd\n", offs->hs_arr  ) ;
   fprintf(stderr,"\tcurdeal_off = %zd\n", offs->curdeal ) ;
   fprintf(stderr," nsres_off    = %zd\n", offs->nsres     ) ;
   fprintf(stderr," ewres_off    = %zd\n", offs->ewres     ) ;
   fprintf(stderr," cache_off    = %zd\n", offs->cache     ) ;

   fprintf(stderr,"Size of map_template_k %zd\n", sizeof(MMAP_TEMPLATE_k  ));
   fprintf(stderr,"Size of mmap_hdr_k     %zd\n", sizeof(mmap_hdr_k       ));
   fprintf(stderr,"Size of query_data_k   %zd\n", sizeof(query_type_k     ));
   fprintf(stderr,"Size of reply_data_k   %zd\n", sizeof(reply_type_k     ));
   fprintf(stderr,"Size of DEALDATA_k     %zd\n", sizeof(DEALDATA_k       ));
   fprintf(stderr,"Size of USER_VALUES_k  %zd\n", sizeof(USER_VALUES_k    ));
   fprintf(stderr,"Size of UEVAL_CACHE_k  %zd\n", sizeof(UEVAL_CACHE_k    ));
   fprintf(stderr,"Size of HANDSTAT_k     %zd, 0x%0lx\n", sizeof(HANDSTAT_k), sizeof(HANDSTAT_k));
 } /* end jgmDebug>=8 */
   return ;
} /* end show_mmap_ptrs */

/*
 * mmap the fd that our parent has passed us.
 */
char *link_mmap(int fd) {
      size_t len;
      char *mm_ptr ;

      PageSize = sysconf(_SC_PAGESIZE);
      if ( PageSize < 0) {die("sysconf() error cant get PAGESIZE"); }
      off_t offset = 0;      /* start at beginning of file */
      len = PageSize;        /* Map one page */
      mm_ptr = mmap(NULL,                          /* Let Kernel Choose addr in my space */
                      len,                         /* Map one Page */
                      PROT_READ|PROT_WRITE,        /* Allow R/W access to the region; cannot conflict with file perms */
                      MAP_SHARED,                  /* Allow other procs to also map and see our updates. */
                      fd,                          /* The related fd. Will be of no further use in the parent proc */
                      offset );                    /* We want to start at zero */
      if (mm_ptr == MAP_FAILED ) {die(" Server:: mmap of mm_fd failed"); }
      JGMDPRT(1,"Server SUCCESS! fd[%d] mapped to child address=%p\n", fd, mm_ptr );
      return (mm_ptr) ;
} /* end link_mmap */

sem_t *open_semaphore(char *sem_name) {
   sem_t *p_mtx ;
   p_mtx = sem_open(sem_name, O_CREAT, 0666, 0);
   if (p_mtx == SEM_FAILED ) die("Main Cannot Open/Create semaphore ");
   return (p_mtx) ;
}
/* Uses file Globals */
void eoj_mutex(  ) {
   sem_close(p_rsem);
   sem_close(p_qsem);
   sem_unlink(query_sema) ;
   sem_unlink(reply_sema) ;
   return ;
}

void server_eoj(char *mm_ptr, size_t PageSize) {
   msync(mm_ptr, PageSize, MS_SYNC) ; // flush any outstanding reply
   JGMDPRT(2,"Calling munmap with mm_ptr=%p, PageSize=%ld \n", (void *)mm_ptr, PageSize ) ;
   if ( munmap(mm_ptr, PageSize ) <    0 )  {
     perror("munmap() error");
  }
   sem_post(p_rsem) ; /* in case the client is still waiting */
   eoj_mutex() ;  /* tell kernel we finished with semaphores */
   return ;
}

int bergen_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   int num_res = 0;
   strcpy(prt->reply_descr, "Bergen" ) ;
   prt->reply_tag = pqt->query_tag ;
   num_res = bergen_calc( pqt->query_side )  ; /* all the xxxx_calc functions call prolog which sets up the ptrs and other stuff */
   JGMDPRT(6,"=====Bergen_calc returns %d fields calculated, res[0]=%d, res[3]=%d\n",num_res, pr->u.res[0],pr->u.res[3]) ;
   return (num_res) ;
}
int bissell_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat, struct query_type_st *pqt) {
   int num_res = 0 ;
   strcpy(prt->reply_descr, "Bissell" ) ;
   prt->reply_tag = pqt->query_tag ;
   num_res = bissell_calc( pqt->query_side )  ; /* all the xxxx_calc functions call prolog which sets up the ptrs and other stuff */
   JGMDPRT(6,"=====Bissell_calc returns %d fields calculated, res[0]=%d, res[3]=%d\n",num_res, pr->u.res[0],pr->u.res[3]) ;
   return (num_res) ;
}
int dkp_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   strcpy(prt->reply_descr, "Kleinman" ) ;
   int num_res = 0;
   prt->reply_tag = pqt->query_tag ;
   num_res = dkp_calc( pqt->query_side )  ; /* all the xxxx_calc functions call prolog which sets up the ptrs and other stuff */
   JGMDPRT(6,"=====DKP_calc returns %d fields calculated, res[0]=%d, res[3]=%d\n",num_res, pr->u.res[0],pr->u.res[3]) ;
   return (num_res) ;
}
int goren_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   strcpy(prt->reply_descr, "Goren" ) ;
   int num_res = 0;
   prt->reply_tag = pqt->query_tag ;
   num_res = goren_calc( pqt->query_side )  ; /* all the xxxx_calc functions call prolog which sets up the ptrs and other stuff */
   JGMDPRT(6,"=====Goren_calc returns %d fields calculated, res[0]=%d, res[3]=%d\n",num_res, pr->u.res[0],pr->u.res[3]) ;
   return (num_res) ;
}
int hcp_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   strcpy(prt->reply_descr, "Work_HCP_only" ) ;
   prt->reply_tag = pqt->query_tag ;
   for (int i = 0 ; i < 4 ; i++ ) {
      pr->u.res[i] = 0 - pdl_dat->hs[0].hs_points[i] ;
   }
   return (0) ;
}
int jgm1_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   int num_res =0 ;
   strcpy(prt->reply_descr, "JGM1" ) ;
    prt->reply_tag = pqt->query_tag ;
    // fprintf(stderr, "**** Calling jgm1_calc with pqt->query_side=%d \n",pqt->query_side );
   num_res = jgm1_calc( pqt->query_side ) ;
   JGMDPRT(6,"jgm1_calc returns %d fields calculated\n",num_res) ;
   return (0) ;
}
int kaplan_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   int num_res = 0 ;
   strcpy(prt->reply_descr, "Kaplan" ) ;
   prt->reply_tag = pqt->query_tag ;
   num_res = kaplan_calc( pqt->query_side) ;
   JGMDPRT(6,"kaplan_calc returns %d fields calculated\n",num_res) ;
   return (0) ;
}
int karpin_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   int num_res = 0 ;
   strcpy(prt->reply_descr, "Karpin" ) ;
   prt->reply_tag = pqt->query_tag ;
   num_res = karpin_calc( pqt->query_side) ;
   JGMDPRT(6,"karpin_calc returns %d fields calculated\n",num_res) ;
   return (0) ;
}
int knr_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   int num_res = 0 ;
   strcpy(prt->reply_descr, "KnR_Four_Cs with Fit Adj" ) ;
   prt->reply_tag = pqt->query_tag ;
   JGMDPRT(3,"KnR_reply Calling knr_calc\n") ;
   num_res = knr_calc( pqt->query_side) ;
   JGMDPRT(6,"knr_calc returns %d fields calculated\n",num_res) ;
   return (num_res) ;
}
int larsson_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   int num_res = 0 ;
   strcpy(prt->reply_descr, "Larsson" ) ;
   prt->reply_tag = pqt->query_tag ;
   num_res = lar_calc( pqt->query_side) ;
   JGMDPRT(6,"lar_calc returns %d fields calculated\n",num_res) ;
   return (num_res) ;
}
int morse_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   int num_res = -1 ;
   strcpy(prt->reply_descr, "Morse" ) ;
    prt->reply_tag = pqt->query_tag ;
   num_res = morse_calc( pqt->query_side) ;
   JGMDPRT(6,"morse_calc returns %d fields calculated\n",num_res) ;
   return (num_res) ;
}
int pav_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   int num_res = -1 ;
   strcpy(prt->reply_descr, "Pavlicek" ) ;
   prt->reply_tag = pqt->query_tag ;
   num_res = pav_calc( pqt->query_side) ;
   JGMDPRT(6,"pav_calc returns %d fields calculated\n",num_res) ;
   return (num_res) ;
} /* end pav_reply */
int sheinw_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   strcpy(prt->reply_descr, "Sheinwold" ) ;
   int num_res = 0;
   prt->reply_tag = pqt->query_tag ;
   num_res = sheinw_calc( pqt->query_side )  ; /* all the xxxx_calc functions call prolog which sets up the ptrs and other stuff */
   JGMDPRT(6,"=====sheinw_calc returns %d fields calculated, res[0]=%d, res[3]=%d\n",num_res, pr->u.res[0],pr->u.res[3]) ;
   return (num_res) ;
} /* end sheinw_reply */
int zarbasic_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   strcpy(prt->reply_descr, "Zar Basic" ) ;
   int num_res = 0;
   prt->reply_tag = pqt->query_tag ;
   num_res = zarbasic_calc( pqt->query_side )  ; /* all the xxxx_calc functions call prolog which sets up the ptrs and other stuff */
   JGMDPRT(6,"=====zarbasic_calc returns %d fields calculated, res[0]=%d, res[3]=%d\n",num_res, pr->u.res[0],pr->u.res[3]) ;
   return (num_res) ;
} /* end zarbasic_reply */
int zarfull_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   strcpy(prt->reply_descr, "Zar Full" ) ;
   int num_res = 0;
   prt->reply_tag = pqt->query_tag ;
   num_res = zarfull_calc( pqt->query_side )  ; /* all the xxxx_calc functions call prolog which sets up the ptrs and other stuff */
   JGMDPRT(6,"=====zarfull_calc returns %d fields calculated, res[0]=%d, res[3]=%d\n",num_res, pr->u.res[0],pr->u.res[3]) ;
   return (num_res) ;
} /* end zarfull_reply */
int set88_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   int num_res = -1 ;
   strcpy(prt->reply_descr, "Set88 Query" ) ;
   prt->reply_tag = pqt->query_tag ;
   num_res = set88_calc( pqt->query_side) ;
   JGMDPRT(6,"set88_calc returns %d fields calculated\n",num_res) ;
   return (num_res) ;
} /* end set88_reply */

int unk_reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   strcpy(prt->reply_descr, "Unknown" ) ;
   prt->reply_tag = pqt->query_tag ;
   for (int i = 0 ; i < 4 ; i++ ) {
      pr->u.res[i] = 0 - pdl_dat->hs[0].hs_points[i] ;
   }
 return (0) ;
}
int mixed_JGM1reply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   int num_res =0 ;
   strcpy(prt->reply_descr, "MIX Test Karpin(Milton) vs JGM(BumWrap)" ) ;
    prt->reply_tag = pqt->query_tag ;
    // fprintf(stderr, "**** Calling mixed_JGM1calc with pqt->query_side=%d \n",pqt->query_side );
   num_res = mixed_JGM1calc( pqt->query_side ) ;
   JGMDPRT(6,"mixed_JGM1calc returns %d fields calculated\n",num_res) ;
 return (0) ;
}
int mixed_LARreply( USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pdl_dat,  struct query_type_st *pqt) {
   int num_res =0 ;
   strcpy(prt->reply_descr, "MIX Test Karpin(Milton) vs JGM(BumWrap)" ) ;
    prt->reply_tag = pqt->query_tag ;
    // fprintf(stderr, "**** Calling mixed_LARcalc with pqt->query_side=%d \n",pqt->query_side );
   num_res = mixed_LARcalc( pqt->query_side ) ;
   JGMDPRT(6,"mixed_LARcalc returns %d fields calculated\n",num_res) ;
 return (0) ;
}
int test_reply(     USER_VALUES_k *pr, struct reply_type_st *prt, DEALDATA_k *pq,  struct query_type_st *pqt) {
   int num_res =0 ;
   strcpy(prt->reply_descr, "Testing Reply" ) ;
    prt->reply_tag = pqt->query_tag ;
   fprintf(stderr, "**** Calling Testing Calc with pqt->query_side=%d \n",pqt->query_side );
   num_res = test_calc( pqt->query_side, pqt ) ;
   JGMDPRT(6,"testing_calc returns %d fields calculated\n",num_res) ;
 return (0) ;
}
void show_reply_type(struct reply_type_st *prt) {
 #ifdef JGMDBG
   DBGLOC( "^^^Server SHOWReply_type:: Descr[%s], Tag=[%d]: Values[", prt->reply_descr, prt->reply_tag);
   for (int v=0 ; v<8; v++) {fprintf(stderr,"%d, ", prt->r_vals[v] );  }
   fprintf(stderr, "]\n");
 #endif
   return ;
}




