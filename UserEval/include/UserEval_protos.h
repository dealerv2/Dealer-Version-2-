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

/*
 * Protos for various factors sub-functions of the main metric_calc functions
 *   Those funcs that COULD take on fractions are def'd as float. If the func can only return an int it is int
 */
extern float_t calc_alt_hcp      (HANDSTAT_k *phs, int seat, int suit, int type ) ;
extern float_t shortHon_adj      ( HANDSTAT_k *phs, int seat, int suit, int tag ) ;
extern float_t lookup_adj_stiff_H( enum metric_ek m, int w1) ;
extern float_t lookup_adj_HH     ( enum metric_ek m, int w2) ;


/* Naming Convention for Factors sub funcs; Factor, Upper Case metric. Abbreviations as shown */

/* Factors in a single suit */
/* The AdjXXX are not needed since there is a table lookup for all the adjustments for each Metric Type */
 float_t  AdjBERG  ( HANDSTAT_k *phs, int suit ) ;  /* adjust HCP for short honors */
 float_t  AdjCPU   ( HANDSTAT_k *phs, int suit ) ;  // A=4.5,K=3,Q=1.5, J=0.75, T=0.25
 float_t  AdjDKP   ( HANDSTAT_k *phs, int suit ) ;
 float_t  AdjGOREN ( HANDSTAT_k *phs, int suit ) ;
 float_t  AdjJGM1  ( HANDSTAT_k *phs, int suit ) ;
 float_t  AdjKARPIN( HANDSTAT_k *phs, int suit ) ;
 float_t  AdjLAR   ( HANDSTAT_k *phs, int suit ) ;
 float_t  AdjMORSE ( HANDSTAT_k *phs, int suit ) ;
 float_t  AdjPAV   ( HANDSTAT_k *phs, int suit ) ;

 float_t  AdjKAPLAN(HANDSTAT_k  *phs, int suit ) ; // Future
 float_t  AdjNONE(HANDSTAT_k    *phs, int suit ) ; // KnR,
 float_t  AdjR22(HANDSTAT_k     *phs, int suit ) ; // Future
 float_t  AdjSHEINW(HANDSTAT_k  *phs, int suit ) ; // Future
 float_t  AdjZAR(HANDSTAT_k     *phs, int suit ) ; // Future


/* Dpts (Distribution pts) are points for shortness in Opener's hand BEFORE a fit is found */
int  DptsGOREN ( HANDSTAT_k *phs, int suit ) ;        // 3/2/1 each Dblton?*/
int  DptsJGM1  ( HANDSTAT_k *phs, int suit ) ;
int  DptsMORSE  ( HANDSTAT_k *phs, int suit ) ;
int  DptsPAV   ( HANDSTAT_k *phs, int suit ) ;        // V=3/S=2/Dblton=1 each; but not in pards long suit.
int  DptsNONE  ( HANDSTAT_k *phs, int suit ) ;

int   LptsSTD   ( HANDSTAT_k *phs, int suit ) ;   /* standard points for length +1 for each card over 4 in any suit */
int   LptsBERG  ( HANDSTAT_k *phs, int suit ) ;   // done inline for now. Cant tell responder from Opener yet.
int   LptsCPU   ( HANDSTAT_k *phs, int suit ) ;
int   LptsDKP   ( HANDSTAT_k *phs, int suit ) ;
int   LptsJGM1  ( HANDSTAT_k *phs, int suit ) ;   // if suit has 3+ hcp, then +1 for each card OVER 4
int   LptsKARPIN( HANDSTAT_k *phs, int suit ) ;   // +1 for each card over 4 in any suit -- done inline
int   LptsLAR   ( HANDSTAT_k *phs, int suit ) ;   // 6 suit +1, 7 suit +2
int   LptsMORSE ( HANDSTAT_k *phs, int suit ) ;   // with 3hcp in suit, +1 for 5 cards, +2 for each above that. <3hcp -1 from prev.
int   LptsPAV   ( HANDSTAT_k *phs[]         ) ;   /* 1 pt for dummy's 5+ suit in NT -- need to figure out dummy */
int   LptsNONE  ( HANDSTAT_k *phs, int suit ) ;

int   SynBERG  ( HANDSTAT_k *phs, int suit ) ;    /* In any 4+ suit, +1 for 3 of top 5 */
int   SynCPU   ( HANDSTAT_k *phs, int suit ) ;    /* In any 4+ suit, +1 for 3 of top 5 */
int   SynDKP   ( HANDSTAT_k *phs, int suit ) ;    /* +1 Each K with A, and/or Q or J with Ace or King. (before div by 3) */
int   SynJGM1  ( HANDSTAT_k *phs, int suit ) ;    /* Varies depending... U can't count too much in one suit for high cards */
int   SynMORSE ( HANDSTAT_k *phs, int suit ) ;    /* Varies depending.... */
int   SynNONE  ( HANDSTAT_k *phs, int suit ) ;
int   SynLAR   ( HANDSTAT_k *phs  ) ;        /* +1 for two 3+L suits with HH; +2 with 3or4 such suits */
                                             /* Possibly exclude QJ suits; what if 3 honors? count that suit? */

/* Factors in a single hand  -- mostly done inline */
int   BodyBERG  ( HANDSTAT_k *phs ) ;                 // -1 for 4333
int   BodyCPU   ( HANDSTAT_k *phs ) ;
int   BodyDKP   ( HANDSTAT_k *phs ) ;                 // -2 for 4333 (pre div by 3)
int   BodyGOREN ( HANDSTAT_k *phs ) ;                 // +1 if 4 Aces -1 if no Aces. -1 for 4333
int   BodyJGM1  ( HANDSTAT_k *phs ) ;
int   BodyKARPIN( HANDSTAT_k *phs ) ;
int   BodyLAR   ( HANDSTAT_k *phs ) ;                 // +1 if #Tens >=2 or 2 Nines and one Ten
int   BodyMORSE ( HANDSTAT_k *phs ) ;
int   BodyPAV   ( HANDSTAT_k *phs ) ;                 // +1 if Aces + Tens >= 4 -- done inline
int   BodyNONE  ( HANDSTAT_k *phs ) ;


/* Factors Using Both Hands */
/* +1 Honor Fit (Hf) points for 1-3 hcp in partner's long suit; or in an 8+ fit suit */
/* Typically the Hf function will be called on both hands first hand=0 vs other, then hand=1 vs other */
int   Hf_ptsBERG  ( HANDSTAT_k *phs[], int hand ) ;   // +1 with 1-3hcp in ALL of pards 5+ suits;Hx vs 5, Stiff H vs 6+ suit;  Max of 2pts per hand
int   Hf_ptsCPU   ( HANDSTAT_k *phs[], int hand ) ;   //  same as Bergen
int   Hf_ptsDKP   ( HANDSTAT_k *phs[], int suit ) ;
int   Hf_ptsGOREN ( HANDSTAT_k *phs[], int suit ) ;   // Kx(x), Qx(x) Jx(x), QJx(x), in trump suit only = +1 (<4hcp)
int   Hf_ptsJGM1  ( HANDSTAT_k *phs[], int suit ) ;   // 1-3 hcp/2+ suit; vs any of pards 5+ suit(s) = +1 (like opc; but no 4-4)
/* int   Hf_ptsKARPIN( HANDSTAT_k *phs[], int suit ) ;   // No Hf pts in Karpin algo. */
int   Hf_ptsLAR   ( HANDSTAT_k *phs[], int suit ) ;
int   Hf_ptsMORSE ( HANDSTAT_k *phs[], int suit ) ;
int   Hf_ptsPAV   ( HANDSTAT_k *phs[], int suit ) ; /* PAV says he follows Goren so use the same approach? No Mention on his site*/
int   Hf_ptsNONE  ( HANDSTAT_k *phs[], int suit ) ;

/* Dfit is support pts for shortness in the short trump hand. - 4-4 fits are both? or only "dummy" No common answer */
int   DfitSTD   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // 4+ Trumps: 5/3/1 ; 3 trumps: 3/2/1 ; Not used by any metric currently
int   DfitBERG  ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // 5Trumps: 5/3/1 ; 4T: 4/3/1 3T: 3/2/1 ; Decl(shortness): V=4,S=2,2+Dblton=1
int   DfitCPU   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // Same as Berg since CPU is Berg with fractions
int   DfitDKP   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // Unknown since DKP is only for NT. Could assume any version of Dfit
int   DfitGOREN ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // 4+ Trumps: add +2 for V and +1 for stiff=>5/3/1 ;Nothing extra if 3T.
int   DfitJGM1  ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ;
int   DfitKARPIN( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // 4 Trump:3/2/1 ; 3 Trump: 2/1/0 ; Decl:5T=>V=2/S=1;6+T=V=3/S=2/D=1
int   DfitLAR   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // 4+ Trumps: 3/2/1 ;
int   DfitMORSE ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; /* The KnR way */
int   DfitPAV   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // As Goren 4T V=+2, S=+1 ; nothing added to original shortness if 3T
int   DfitNONE  ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ;   // Dummy in case we need to satisfy calling Do_Trump_fit() */

/* Fn points for any 8+ fit; or any upgrade to Declarer's hand when a fit is found */
int   Fn_ptsSTD   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // F8 = +1, F9/up=+2 trump suit only. Mayby not used?
int   Fn_ptsBERG  ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // Decl: +1 for 5 trump; +2 each trump>5; +1 for ONE side suit
int   Fn_ptsCPU   ( HANDSTAT_k *ph,  TRUMP_FIT_k trump ) ;
int   Fn_ptsDKP   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ;
int   Fn_ptsGOREN ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // Decl: 5Trumps +1; each trump over 5 +2
int   Fn_ptsJGM1  ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // F8 = +1, F9/up=+2 all suits ; in NT do not count >4 in short hand. i.e. 5=4 is counted as 8 not 9;
int   Fn_ptsKARPIN( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ;
int   Fn_ptsLAR   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ;
int   Fn_ptsMORSE ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ;  /* The KnR Way only adds opc style FN pts if Declarer has shortness*/
int   Fn_ptsPAV   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // Decl: +2 for each trump>5 AND +1 for each side suit card >3
int   Fn_ptsNONE  ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; // Dummy in case we need to satisfy calling Do_Trump_fit() */

/* Special for KnR */
float_t DfitKnR   ( HANDSTAT_k *phs, TRUMP_FIT_k trump ) ; //  See text; Dummy and Decl are Different.
float_t MisfitKnR ( HANDSTAT_k *phs, MIS_FIT_k   misfit) ;

/* protos for debugging */
extern int arr_min(int arr[], size_t nelem ) ;
extern void dump_curdeal( void ) ; /* for debugging; uses only the Has_card array in handstat[p] */
extern void sr_deal_show(deal dl) ;
extern void sortDeal52(deal dl );

#endif /* end file guard */

