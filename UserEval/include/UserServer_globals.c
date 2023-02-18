/* File UserServer_globals.c -- Global Vars -- Stuff used only by the main server not by the calc routines ??
 *  Version  Date          Comment
 *    0.5    2022-12-14    Split out of UserEval_globals.c as a test -- Include this file into UserServer.c ; no separate compile.
 */
#include "../include/std_hdrs_all.h"
#include "../include/UserEval_types.h"
#ifndef MMAP_TEMPLATE_H
   #include "../include/mmap_template.h"        /* Will also include dealtypes.h */
#endif

/* Setup by the Server Infrastructure */

struct         mmap_ptrs_st ptrs ;
struct         mmap_off_st  offs ;

/* semaphore definition */
sem_t *p_qsem, *p_rsem  ;
char query_sema[32] ;      /* copied from the mmap header in shared memory area */
char reply_sema[32] ;

/* mmap definition -- Set by Server Mainline in link_mmap */
size_t PageSize = 4096 ;
char *mmap_ptr ;                    /* same value as mm_base_ptr but different type, for offset calculations */
char mmap_fname[128];               /* Not really needed ; useful for debugging */


MMAP_TEMPLATE_k      *mm_base_ptr ; /* The pointer that is the base for everything else. set by link_map in UserServer.c  -- not used? outside of him?*/
DEALDATA_k           *p_dldat ;     /* the deal and handstat area */
USER_VALUES_k        *p_nsdat ;     /* results for North, South, and NS side */
USER_VALUES_k        *p_ewdat ;     /* results for East, West, and EW side */
HANDSTAT_k           *hs_ptr[4] ;   /* 4 pointers one to each handstat struct */

/* A definition for the Server Only. --- May not be used */
struct userfunc_args_st {  /* needs to be filled out later */
   enum metric_ek mw ;   // metric wanted; tag will fine tune it further or maybe tag IS the metric.
   int tag ;
   int side;         // -1 means not a side
   int seat;         // -1 means not a seat. one or the other of seat or side specified, not both.
   int idx;          // -1 means no specific result returned. Just do all that are relevant for this tag.
   int (*ufunc)(DEALDATA_k *p_dldata, int side, int seat, int idx) ; // the function to be called. NULL nothing specific -- deduce from tag .
} ;

/* end file UserServer_globals  */

