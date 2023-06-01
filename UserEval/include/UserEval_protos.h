/* File UserEval_protos.h  -- Definitions of Calculation sub funcs for use by Calc main funcs
 *  Version  Date          Comment
 *    0.2    2022-12-04    2nd Draft
 *    0.3    2022-12-09    3rd Draft -- Most protos now only need HANDSTAT_k, not DEALDATA_k
 *    0.4    2022-12-25    Removed a bunch of defs that are not used
 *
 */
#ifndef USEREVAL_PROTOS_H
#define USEREVAL_PROTOS_H
#include "../include/std_hdrs_all.h"  // the stdlib headers including the semaphore ones.
#include "../include/UserEval_types.h"
#include "../include/UserEval_externs.h"
#include "../include/dbgprt_macros.h"

/*
 * Util/Helper Functions used by the metrics_calc.c high level code and some by the Factors code
 */
extern void prolog( int side ) ;
extern void insertionSort(int size, int arr_val[], int arr_idx[] ) ;
extern int asc_cmpxy ( const void *x, const void *y) ;
extern int desc_cmpxy( const void *x, const void *y) ;
extern struct trump_fit_st trump_fit_chk( HANDSTAT_k *phs[] );
extern struct misfit_st misfit_chk(HANDSTAT_k *phs[], int s ) ;
extern void SaveUserVals(struct UserEvals_st UEv , USER_VALUES_k *p_ures ) ;
extern struct FitPoints_st Do_Trump_fit(
         HANDSTAT_k *phs[2],
         int (*calc_dfval)(HANDSTAT_k *p_hs, TRUMP_FIT_k trump),
         int (*calc_fnval)(HANDSTAT_k *p_hs, TRUMP_FIT_k trump) ) ;
struct KnR_points_st KnR_pts_all (HANDSTAT_k *phs) ;
int KnR_Trump_fit(HANDSTAT_k *phs[2], struct KnR_points_st *Ph0, struct KnR_points_st *Ph1 ) ;
int KnR_Round ( int Body, int KnR_pts ) ;
void show_knr_pts(int lvl, struct KnR_points_st Pts, char *descr ) ;
int Fill_side_fitstat( HANDSTAT_k *phs[] , SIDE_FIT_k *sf ) ;
int SetTrumps(HANDSTAT_k *phs[], TRUMP_SUIT_k *trumps ) ;
int SetDeclarer( HANDSTAT_k *phs[], int suit );


/*
 * Protos for various factors sub-functions of the main metric_calc functions
 *   Those funcs that COULD take on fractions are def'd as float. If the func can only return an int it is int
 */
extern float_t calc_alt_hcp      (HANDSTAT_k *phs,  int suit, int type ) ;
extern float_t shortHon_adj      ( HANDSTAT_k *phs, int suit, int tag ) ;
extern float_t lookup_adj_stiff_H( enum metric_ek m, int w1) ;
extern float_t lookup_adj_HH     ( enum metric_ek m, int w2) ;
extern float_t get_ss_value(int metric, HANDSTAT_k *p_hs, int suit ) ;
extern int get_ss_weight(HANDSTAT_k *p_hs, int suit ) ;
extern int ss_index(int weight) ;
float_t QuickTricks_suit( HANDSTAT_k *phs , int s ) ;
float_t QuickTricks( HANDSTAT_k *phs );

/* Naming Convention for Factors sub funcs; Factor, Upper Case metric. Abbreviations as shown */

/* Factors in a single suit */
 float_t  AdjDKP   ( HANDSTAT_k *phs, int suit ) ;

/* Dpts (Distribution pts) are points for shortness in Opener's hand BEFORE a fit is found */
int  DptsGOREN ( HANDSTAT_k *phs, int suit ) ;        // 3/2/1 each Dblton?*/
int  DptsPAV   ( HANDSTAT_k *phs, int suit ) ;        // V=3/S=2/Dblton=1 each; but not in pards long suit.
int  DptsSHEINW( HANDSTAT_k *phs, int suit ) ;
// int  DptsNONE  ( HANDSTAT_k *phs, int suit ) ;

// int   LptsJGM1  ( HANDSTAT_k *phs, int suit ) ;   // if suit has 3+ hcp, then +1 for each card OVER 4
// int   LptsMORSE ( HANDSTAT_k *phs, int suit ) ;   // with 3hcp in suit, +1 for 5 cards, +2 for each above that. <3hcp -1 from prev.
int   LptsPAV   ( HANDSTAT_k *phs[]         ) ;   /* 1 pt for dummy's 5+ suit in NT -- need to figure out dummy */

int   SynBERG  ( HANDSTAT_k *phs, int suit ) ;    /* In any 4+ suit, +1 for 3 of top 5 */
int   SynDKP   ( HANDSTAT_k *phs, int suit ) ;    /* +1 Each K with A, and/or Q or J with Ace or King. (before div by 3) */
int   SynLAR   ( HANDSTAT_k *phs  ) ;        /* +1 for two 3+L suits with HH; +2 with 3or4 such suits */
                                             /* Possibly exclude QJ suits; what if 3 honors? count that suit? */

/* Factors Using Both Hands */
/* +1 Honor Fit (Hf) points for 1-3 hcp in partner's long suit; or in an 8+ fit suit */
/* Typically the Hf function will be called on both hands first hand=0 vs other, then hand=1 vs other */
int   Hf_ptsBERG  ( HANDSTAT_k *phs[], int hand ) ;   // +1 with 1-3hcp in ALL of pards 5+ suits;Hx vs 5, Stiff H vs 6+ suit;  Max of 2pts per hand
int   Hf_ptsGOREN ( HANDSTAT_k *p_hs,  int suit ) ;   // Kx(x), Qx(x) Jx(x), QJx(x), in trump suit only = +1 (<4hcp)
int   Hf_ptsKAPLAN( HANDSTAT_k *p_hs , int h    ) ;   // Give Q or J in pards 4+ suit full value
int   Hf_ptsPAV   ( HANDSTAT_k *phs[], int suit ) ;   // Goren does Hf; PAV does not per his online evaluator
int   Hf_ptsSHEINW( HANDSTAT_k *phs_du, HANDSTAT_k *phs_dc, int suit ) ;/*Sheinwold does Hf in both Trump and side suits */
                  /* +1 for Qx,Jx,QJ in Trumps and +1 for Qx(or QJ) in 4+ side suit. */
//int   Hf_ptsJGM1  ( HANDSTAT_k *phs[], int suit ) ;   // 1-3 hcp/2+ suit; vs ANY of pards 5+ suit(s) = +1 (like opc; but no 4-4)
//int   Hf_ptsMORSE ( HANDSTAT_k *phs[], int suit ) ;   // None so far, but maybe in future

/* Dfit is support pts for shortness in the short trump hand. - 4-4 fits are both? or only "dummy" No common answer */
int   DfitSTD   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // 4+ Trumps: 5/3/1 ; 3 trumps: 3/2/1 ; Not used by any metric currently
int   DfitBERG  ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // 5Trumps: 5/3/1 ; 4T: 4/3/1 3T: 3/2/1 ; Decl(shortness): V=4,S=2,2+Dblton=1
int   DfitGOREN ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // 4+ Trumps: add +2 for V and +1 for stiff=>5/3/1 ;Nothing extra if 3T.
int   DfitKAPLAN( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // Both hands get 3/2/1 for EACH with 3+ trump and an 8+ fit
int   DfitKARPIN( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // 4 Trump:3/2/1 ; 3 Trump: 2/1/0 ; Decl:5T=>V=2/S=1;6+T=V=3/S=2/D=1
int   DfitLAR   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // 4+ Trumps: 3/2/1 ;
int   DfitPAV   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // As Goren 4T V=+2, S=+1 ; nothing added to original shortness if 3T
int   DfitSHEINW( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // REPLACE Dpts: Trump: 4+T[5/3/1] 3T[4/2/0] with 2-T[0/0/0]
// int DfitOPC  (HANDSTAT_k *phs[],TRUMP_FIT_k trump ) ; // The OPC way; 4-4 both count. 5-5 No-one. Dfit=Ntrump-Shortest-SuitLen

/* Fn points for any 8+ fit; or any upgrade to Declarer's hand when a fit is found */
//int   Fn_ptsJGM1  ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // F8 = +1, F9/up=+2 all suits ; in NT do not count >4 in short hand. i.e. 5=4 is counted as 8 not 9;
//int   Fn_ptsSTD   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // F8 = +1, F9/up=+2 trump suit only. Mayby not used?
//int   Fn_ptsOPC   (HANDSTAT_k *phs[],TRUMP_FIT_k trump ) ; // F8=+1,F9=+2,F10/up=+3; Suit and NT all suits incl side suits
int   Fn_ptsNONE  ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // KEEP. Used by Do_Trump_fit
int   Fn_ptsBERG  ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // Decl: +1 for 5 trump; +2 each trump>5; +1 for ONE side suit
int   Fn_ptsGOREN ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // Decl: 5Trumps +1; each trump over 5 +2
int   Fn_ptsKARPIN( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // Add for short suits in Decl hand once trump fit found.
int   Fn_ptsPAV   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // Decl: +2 for each trump>5 AND +1 for each side suit card >3
int   Fn_ptsSHEINW( HANDSTAT_k *phs_dc, HANDSTAT_k *phs_du, TRUMP_FIT_k trump ) ;
       // +1 for SOLID 5+ sidesuit; also +1 for 5th and +2 for 6th etc card in ANY suit that has been raised.

/* Special for KnR */
float_t DfitKnR   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; //  See text; Dummy and Decl are Different.
                                                           /* The KnR Way only adds opc style FN pts if Declarer has shortness*/
float_t MisfitKnR ( HANDSTAT_k *phs, MIS_FIT_k   misfit) ;

/* Special for ZAR */
int Hf_ptsZar( HANDSTAT_k *phs[] )  ;
int Fn_ptsZar( HANDSTAT_k *phs[], int t_suit )  ;
int DfitZAR(   HANDSTAT_k *phs[], SIDE_FIT_k *sf) ;
int SynZAR(    HANDSTAT_k *p_hs  ) ;        /* +1 if the HCP are in 2 suits (11-14) or 3 suits(15+) */


/* protos for debugging */
extern int arr_min(int arr[], size_t nelem ) ;
extern void dump_curdeal( void ) ; /* for debugging; uses only the Has_card array in handstat[p] */
extern void sr_deal_show(deal dl) ;
extern void sortDeal52(deal dl );

#endif /* end file guard */

