/* File Server_protos.h  -- Protos Only needed by ServerMain not by User Calc routines
 *  Version  Date          Comment
 *    0.1    2022-11-26    First Draft
 *
 */
#ifndef SERVER_PROTOS_H
#define SERVER_PROTOS_H

#include "../include/std_hdrs_all.h"
#include "../include/UserEval_types.h"
#include "../include/dbgprt_macros.h"
#ifndef MMAP_TEMPLATE_H
   #include "../include/mmap_template.h"        /* Will also include dealtypes.h */
#endif


/*
 * Evaluation metrics methods -- these include all factors used.
 */
extern int  bergen_calc( int side  ) ; // incl: adj-3; qual-suit; len; fit ; short Hon adj
extern int  bissell_calc(int side  ) ; // Bissell per Bridge Encyclopedia and internet. not so much PAV.
extern int  dkp_calc   ( int side  ) ; // LJP per DK; Sht_H adj; no Len; no D ; no dfit ; basic;
extern int  goren_calc ( int side  ) ; // Per the PAV web page; shortness; dfit ; shtH ded;
extern int  hcp_calc   ( int side  ) ; // HCP only; no D, no Len, No dfit
extern int  jgm1_calc  ( int side  ) ; // JGM Currently Karpin with Bum_Wrap pts
extern int  kaplan_calc( int side  ) ; // per the book, Contract Bridge Complete
extern int  karpin_calc( int side  ) ; // per PAV web page; HCP + Len + Dfit + short H ded
extern int  knr_calc   ( int side  ) ; // per JGM fixes, incl Dfit as in the text
extern int  lar_calc   ( int side  ) ; // from Larsson's book; hcp + len + body + syn + dfit
extern int  morse_calc ( int side  ) ; // Currently Larsson with Bum_Wrap points. prob will change.
extern int  pav_calc   ( int side  ) ; // per PAV website; hcp + shortness + some dfit + shortH adj
extern int  sheinw_calc( int side  ) ; // per the book, 5 Weeks to Winning Bridge 1959,1964
extern int zarbasic_calc(int side  ) ; // per the 2005 download PDF from Zar Petkov HCP+CTLS+(a+b)+(a-d)+SynZar+short H/HH adj
extern int  zarfull_calc(int side  ) ; // Basic + HF pts, Fn pts and Dfit_pts (Trump Ruff Power). No misfit pts.
extern int  test_calc  ( int side, struct query_type_st *pqt  ) ; // returns bogus numbers in various slots.
extern int  set88_calc ( int side  ) ; // fills slots 0 .. set88_sz with the side_total value from the various metrics
extern int  mixed_JGM1calc( int side  ) ; // returns two sets of HLDF numbers, jgm1  in 0..5 and karpin  in 6 .. 11
extern int  mixed_LARcalc ( int side  ) ; // returns two sets of HLDF numbers, morse in 0..5 and larsson in 6 .. 11

/*
 * Future: Kaplan, Sheinw, Rule22, Zar, Bissel, ....
 */

#endif /* end file guard */

