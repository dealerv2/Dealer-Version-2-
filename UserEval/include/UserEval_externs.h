/* File UserEval_externs.h  -- globals for the various user_eval sub funcs and sub-sub_funcs
 *  Version  Date          Comment
 *    0.5    2022-11-26    First  -- must keep in sync with UserEval_globals.c
 */
#ifndef USEREVAL_EXTERNS_H
#define USEREVAL_EXTERNS_H
#include "../include/std_hdrs_all.h"  /* all the stdio.h stdlib.h string.h assert.h etc. */
#ifndef USEREVAL_TYPES_H
   #include "../include/UserEval_types.h"
#endif
#ifndef MMAP_TEMPLATE_H
   #include "../include/mmap_template.h"
#endif
/*
 * Some Globals -- Server Mainline and top Level
 */
extern int jgmDebug ;
extern int gen_num  ;
extern int prod_num ;
/* Vars Used  by the User Functions; Setup by the Server Infrastructure */
extern struct mmap_ptrs_st ptrs ;    /* all relevant mmap pointers */
extern struct mmap_off_st  offs ;    /* relevant offsets in case they are useful in debugging. */

extern DEALDATA_k           *p_dldat ;     /* the deal and handstat area */
extern USER_VALUES_k        *p_nsdat ;     /* results for North, South, and NS side */
extern USER_VALUES_k        *p_ewdat ;     /* results for East,  West,  and EW side */
extern HANDSTAT_k           *hs_ptr[4] ;   /* 4 pointers; one to each handstat struct */

extern enum suit_ek suit ;      /* CLUBS, DIAMONDS, HEARTS, SPADES  */
extern enum metric_ek metric ;  /* BERG=0,CPU,DKP,GOREN,JGM1,KAPLAN,KARPIN,KnR,LAR,MORSE,PAV,R22,SHEINW,ZAR, UNKN,EXCP=20,SET=88,Quit=-1  */

// Global vars used in most User metric and factor calculation functions.

/* prolog stuff; uses vars setup by the Server Infrastructure */
extern int seat[2] ;                /* Server is called with a side; these are the two seats related to that side. */
extern HANDSTAT_k *phs[2] ;         /* pointers to two related HS structs; N/S or E/W */
extern char compass[2];             /* seat names for debug statements */
extern USER_VALUES_k *p_uservals  ;

/* results stuff */
extern struct FitPoints_st TFpts ;
extern struct trump_fit_st trump ;
extern struct misfit_st misfit[4] ;  /* 4 possible misfits, one in each suit */
extern struct UserEvals_st UEv   ;  /* room for 64 integers */

// Temp vars used by most routines.
extern int h_dummy , h_decl ;
extern int side_nt_pts, seat_nt_pts[2], side_hldf_pts, seat_hdlf_pts[2] ; /* these are the most likely results wanted */
extern int hcp[2], hcp_adj[2], dpts[2], lpts[2], dfit_pts[2], syn_pts[2], hf_pts[2], Fn_pts[2], hand_adj[2], body_pts[2];
extern int freak[2][4], pav_body[2] ;
extern float_t fhcp[2], fhcp_adj[2], suit_qual[2][4]; /* Some calcs will be done in float; cast to int or int*100 at the end*/
extern int suits_by_len[4] ;  /* these will be shuffled so that suits_by_len[0] is the shortest e.g. Diam, [3]=longest e.g. hearts */


extern int set88[20] ;

#endif /* end file guard */



