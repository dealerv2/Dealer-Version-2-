/* File UserEval_externs.h  -- globals for the various user_eval sub funcs and sub-sub_funcs
 *  Version  Date          Comment
 *    0.5    2022-11-26    First
 */
#ifndef USEREVAL_EXTERNS_H
#define USEREVAL_EXTERNS_H
#include "../include/std_hdrs.h"  /* all the stdio.h stdlib.h string.h assert.h etc. */
#ifndef USEREVAL_TYPES_H
   #include "../include/UserEval_types.h"
#endif
#ifndef MMAP_TEMPLATE_H
   #include "../include/mmap_template.h"
#endif
/*
 * Some Globals
 */
/* semaphore definition */
extern char query_sema[32];  /* these should be filled in from the mmap stuff*/
extern char reply_sema[32];
extern sem_t *p_qmtx, *p_rmtx ;

/* mmap definition */
extern int PageSize;
extern char mmap_fname[32];                   /* no more than 31 chars */
extern char *mm_ptr ;                         /* THE pointer to mmap shared area */

// mmap templates and pointers
extern MMAP_TEMPLATE_k      *p_mm_data ;
extern struct mmap_hdr_st   *p_mm_hdr ;
extern struct query_type_st *p_q_type;
extern struct reply_type_st *p_r_type;
extern DEALDATA_k           *p_dldat ;     /* the deal and handstat area */
extern USER_VALUES_k        *p_nsdat ;     /* results for North, South, and NS side */
extern USER_VALUES_k        *p_ewdat ;     /* results for East, West, and EW side */

extern struct mmap_ptrs_st ptrs ;
extern struct mmap_off_st  offs ;

/* From The file  UserEval_globals.c -- Vars used by most sub functions */


extern enum suit_ek suit ;  // CLUBS, DIAMONDS, HEARTS, SPADES
extern enum metric_ek metric ; // BERG=0,CPU,DKP,GOREN,JGM1,KARPIN,KnR,LAR,MOPC,OPC,PAV,R22,ZAR, Quit=-1
// Global vars used in most of these metric routines; global to this file only, not to other files...
/* results stuff */
extern struct trump_fit_st trump ;
extern struct misfit_st misfit   ;
extern struct UserEvals_st UEv   ;

// Temp vars used by most routines.
extern int h_dummy , h_decl ;
extern int side_nt_pts, seat_nt_pts[2], side_hldf_pts, seat_hdlf_pts[2] ; /* these are the most likely results wanted */
extern int hcp[2], hcp_adj[2], dpts[2], lpts[2], dfit_pts[2], syn_pts[2], hf_pts[2], Fn_pts[2], hand_adj[2], body_pts[2];
extern int freak[2][4], pav_body[2] ;
extern float_t fhcp[2], hcpf_adj[2], suit_qual[2][4]; /* Some calcs will be done in float; cast to int or int*100 at the end*/
extern int suits_by_len[4] ; /* these will be shuffled so that suits_by_len[0] is the shortest e.g. Diam, [3]=longest e.g. hearts */



#endif /* end file guard */



