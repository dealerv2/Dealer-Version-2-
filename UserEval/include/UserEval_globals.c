/* File UserEval_globals.c -- Global Vars -- allocate storage in this one.
 *  Version  Date          Comment
 *    0.5    2022-12-02    First
 *    0.6    2022-12-15    Removed Globals that only the Server main uses. This file only for the calulation routine globals.
 */
#include "../include/std_hdrs_all.h"
#include "../include/UserEval_types.h"
#ifndef MMAP_TEMPLATE_H
   #include "../include/mmap_template.h"        /* Will also include dealtypes.h */
#endif
 enum suit_ek suit ;  // CLUBS, DIAMONDS, HEARTS, SPADES
 enum metric_ek metric ; // BERG=0,CPU,DKP,GOREN,JGM1,KARPIN,KnR,LAR,MORSE,OPC,PAV, R22,SHEINW,ZAR, UNKN, SET=88, Quit=-1
 float DBG_run = 1.50;

/* prolog stuff Uses Stuff setup by Server Infrastructure */
HANDSTAT_k *phs[2] ;         /* pointers to two related HS structs; N/S or E/W */
USER_VALUES_k *p_uservals  ; /* pointer to the results area of mmap; either to NS side or EW side */
int gen_num  ;
int prod_num ;
int seat[2] ;                /* valid values are 0,2 if side is NS or 1,3 if side is EW */
char compass[2] ;            /* compass directions for debug statements.N & S,  or E & W etc. */

// Global vars used in most of the metric routines
/* results stuff */
 struct trump_fit_st trump ;
 struct misfit_st misfit[4]; /* could have 4 misfits e.g. 6=5=1=1 vs 1=1=5=6 :(!  */
 struct UserEvals_st UEv   ; /* six values; 1 side and 2 seat for NT and HLDF pts. and up to 58 other vals for dbg + a  ctr*/
 struct FitPoints_st TFpts = { .df_val={0,0}, .fn_val={0,0} }; /* Dummy Dfit_pts, and Declarer FNpts when a fit is found */

/* Temp vars used by most routines.*/
 int h_dummy , h_decl ;
 int side_nt_pts, seat_nt_pts[2], side_hldf_pts, seat_hdlf_pts[2] ; /* these are the most likely results wanted */
 int hcp[2], hcp_adj[2], dpts[2], lpts[2], dfit_pts[2], syn_pts[2], hf_pts[2], Fn_pts[2], hand_adj[2], body_pts[2];
 int freak[2][4] ;
 float_t fhcp[2], fhcp_adj[2], suit_qual[2][4]; /* Some calcs will be done in float; cast to int or int*100 at the end*/
 int suits_by_len[4] = {0, 1, 2, 3 }; /* these will be shuffled so that suits_by_len[0] is the shortest e.g. Diam, [3]=longest e.g. hearts */



/* end file globals for User Calc Routines */

