#ifndef KnR_H
#define KnR_H

#define HAS_CARD2p(p,s,r) p->Has_card[(s)][(r)]  // use ptr to seat handstat struct filled in by analyze()

struct KnR_points_dbg { /* The values will be x100 so we can use ints */
   int knr_honor_pts ;  /* A=3, K=2, Q=1? etc. x100*/
   int knr_short_pts;   /* Each Void=3, Each Stiff=2, Each Dblton except the first = 1 etc. x100*/
   int knr_qual_pts ;   /* (Suitlen/10) * Suithcp. Suit HCP are different from Honor pts x100*/
   int knr_adj ;        /* for Square hand mostly x100*/
   int knr_tot_pts ;    /* Total of the above; The traditional ones with No Dfit or Fn x100*/
   int knr_dfit ;       /* Dummy pts: +50% or +100% of shortness pts with an 8fit or a 9 fit. Per the text */
   int knr_Fn_pts ;     /* Declarer pts: +25%, 50%, 100% of the shortness pts for 8, 9, or longer fits. Per the text */
   int knr_misfit_adj;  /* Deduction of shortness pts if a misfit; assume short pts vs Parner's 5+ suit */
   int knr_body_val;    /* Pavlicek Rounding Factor sum(3*Tens + 2*Nines + Eights */
   int knr_rounded_pts; /* all x100 pts added up and brought into the 0 - 40 pt range using Pav rounding method */
} ;

int KnR_suit_quality(HANDSTAT_k *phs, int suit   ) ;
int KnR_pts (   HANDSTAT_k *phs);
int KnR_Body(   HANDSTAT_k *phs ) ;
int KnR_Round(  int Body, int KnR_pts ) ;
struct KnR_points_st KnR_all_pts(HANDSTAT_k *phs[2] );
int KnR_Trump_fit(HANDSTAT_k *phs[2], struct KnR_points_st *Ph0, struct KnR_points_st *Ph1 );
int KnR_Misfit(   HANDSTAT_k *phs[2], struct KnR_points_st *Ph0, struct KnR_points_st *Ph1 );

void show_knr_pts(int lvl, struct KnR_points_st Pts, char *descr );

#endif /* KnR_H */

