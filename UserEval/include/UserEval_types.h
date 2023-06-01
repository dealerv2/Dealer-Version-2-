/* File UserEval_types.h  -- Types and Typedefs
 *  Version  Date          Comment
 *    0.5    2022-12-02    First
 *    1.0    2023-03-24    Production
 *    1.1    2023-03-27    Replaced 'CPU' with 'BISSEL' (bissell)
 *    1.2    2023-?? ??    Coded Goren
 *    1.5    2023-05-24    Coded all metrics including ZarBasic and ZarFull. Dropped Rule22.
 */
#ifndef USEREVAL_TYPES_H
#define USEREVAL_TYPES_H
#ifndef MMAP_TEMPLATE_H
   #include "../include/mmap_template.h"        /* Will also include dealtypes.h */
#endif
/* may need to invent another set of these, for custom mods.
 * cant insert stuff in the middle now because New Features in DealerV2 and Pavlicek's Library of solved Dealsadj_short_honors table is tied to these. New ones may use CPU, GOREN etc. Not in alpha order
 * The Query tags in alpha order: The adj_hcp arrays use these values to lookup adjustments.
 *                     0        1       2     3     4      5       6      7    8    9     10     11       12      13      14        20          21    */
enum metric_ek    { BERGEN=0, BISSEL,  DKP, GOREN, JGM1, KAPLAN, KARPIN, KnR, LAR, MORSE, PAV, SHEINW,  ZARBAS, ZARFULL, metricEND, MixJGM=20, MixMOR,
// possibly add metrics in the 50 - 79 range to implement different hand factors like quicktricks, or quicklosers, or shortest suit etc.
                    SET=88, TSTALL=99, Quit=-1} ;
enum card_rank_ek { Two_rk=0, Three_rk, Four_rk, Five_rk, Six_rk, Seven_rk, Eight_rk, Nine_rk, Ten_rk, Jack_rk, Queen_rk, King_rk, Ace_rk,spot_rk=-1 };
enum pt_count_ek  { Tens=0, Jacks, Queens, Kings, Aces, Top2, Top3, Top4, Top5, C13, HCP, none=-1 } ;  /* for altcount[count][suit]  */
enum ss_type_ek { ss_A, ss_K, ss_Q, ss_J, ss_T, ss_x, ss_AK, ss_AQ, ss_AJ, ss_AT, ss_Ax,
                  ss_KQ, ss_KJ, ss_KT, ss_Kx, ss_QJ, ss_QT, ss_Qx, ss_JT, ss_Jx, ss_Tx, ss_xx, ss_END, ss_xxx=-1 } ;
struct FitPoints_st {
   int df_val[2];
   int fn_val[2] ;
} ;
struct KnR_points_st {  /* The values will be x100 so we can use ints */
   int knr_honor_pts ;  /* A=3, K=2, Q=1? etc. */
   int knr_short_pts;   /* Each Void=3, Each Stiff=2, Each Dblton except the first = 1 etc. */
   int knr_qual_pts ;   /* (Suitlen/10) * Suithcp. Suit HCP are different from Honor pts */
   int knr_adj ;        /* for Square hand mostly -- the first Dblton adj is not tracked separately */
   int knr_tot_pts ;    /* Total of the above; The traditional ones with No Dfit or Fn */
   int knr_dfit ;       /* Dummy pts: +50% or +100% of shortness pts with an 8fit or a 9 fit. Per the text */
   int knr_Fn_pts ;     /* Declarer pts: +25%, 50%, 100% of the shortness pts for 8, 9, or longer fits. Per the text */
   int knr_misfit_adj;  /* Deduction of shortness pts if a misfit; assume short pts vs Parner's 5+ suit */
   int knr_body_val;    /* Pavlicek Rounding Factor sum(3*Tens + 2*Nines + Eights) */
   int knr_rounded_pts; /* the x100 pts brought into the 0 - 40 pt range using Pav rounding method */
} ;
/* Struct to collect several details re fit(s) one place */
typedef struct side_fitstat_st {
   int t_suit ;
   int t_fitlen;
   int t_len[2];  /* by hand index: 0=N or E, 1=S or W */
   int decl_h;   /* 0 or 1 */
   int dummy_h;
   int side ;     /* 0 = NS 1 = EW */
   int decl_seat; /* Compass Number 0=North, 3=West */
   int dummy_seat;
   int sorted_slen[2][4];  /* suit lengths in desc order longest first */
   int sorted_sids[2][4];  /* the suit_ids in desc order of length e.g. 4=1=5=3 would give D,S,C,H i.e. 1,3,0,2 */
   int fitlen[4] ;
// add more fields so collect everything in one place?
} SIDE_FIT_k ;

typedef struct trump_fit_st {
   int dummy ;    // The compass North, East, South, West for Dummy
   int decl  ;    // ditto for Declarer. We use the decl and dummy fields to indx the tlen and ss_len fields. */
   int tsuit ;
   int tlen[2]  ;  /* Trump Len for hand[0] and hand[1]; Not by Decl or Dummy */
   int fit_len  ;  /* Longest fit; if = 7 promises a 5-2 Not 4-3 or 6-1 If no fit should be -1 */
   int ss_len[2];  /* ShortSuit len for hand[0] and hand[1]; Not by Decl or Dummy */
} TRUMP_FIT_k ;

typedef struct trump_suit_st { /* relevant characteristics of trump suit */
   int t_suit  ;
   int t_len[2];
   int t_fitlen;
   int t_rank  ;
} TRUMP_SUIT_k ;

/* Misfit, Waste, and No Waste struct.
 * there will be an array of four of these, one per suit -- we use this struct to calc misfit pts, and waste/nowaste pts
 * misfit values are coded: 0 => no misfit; 3 => shortness vs 3 card suit; 4 => vs 4 card suit; 5 vs 5+ suit
 * waste  values are coded: -3 => HCP vs a void, -2 => HCP vs a stiff, 0 => no waste deductions either no short, or +ve for no_waste
 * no_waste vals are coded: +3 =>xxx(x) vs Void, +2 =>xxx(x) vs Stiff, +1 => Ax(x) vs stiff, 0 No shortness, or else wastage.
 * */
typedef struct misfit_st {
   int mf_type   ;   /* 0 no misfit; +3 misfit vs 3 card suit ; +4 misfit vs 4 card suit; +5 misfit vs 5+ suit */
   int waste     ;   /* 0 no waste -3 waste vs a void, -2 waste vs a stiff (not Ax(x) */
   int no_waste  ;   /* 0 no no_waste; +3 xxx(x) vs a void. +2 xxx(x) vs a stiff. +1 Ax(x) vs a stiff */
   int long_hand  ;     /* 0 or 1 the hand with at least 3 in the suit this is the hand index i.e. 0 or 1 not the compass */
   int short_hand ;     /* 0 or 1 the hand with a stiff or void in the suit */
   int lh_len;          /* Long hand len 3+ */
   int sh_len;          /* 0 (void) or 1 (stiff) Short hand len */
   int lh_hcp;          /* hcp in long hand. if xxx(x) or Axx(x) get a plus; otherwise get a minus; minus cannot exceed hcp value */
} MIS_FIT_k ;

typedef  // this struct holds the results for one side. the usual case for this kind of query
   union res_ut {
      int res[64] ;             // quick easy access if user can keep track himself.
      struct    {  // suggested layout for convenience. will correspond to syntax in dealer input file
         int side_tot[16] ;        //0-15 16 metrics for side as a whole. Usual case otherwise dealer is adequate.
         int side_by_suit[4][4] ;  //16-31 4 diff metrics for the side, broken down by suit.[CDHS][wxyz]
         int hand_tot[2][8];       //32-47 hand results  8 diff metrics total for each hand
         int hand_suit[2][4][2];   //48-63 results by suit; 2 diff metrics for each hand-suit combo.
      } dr ;                       // detailed result
      // Syntax to access the above: uval.res[i] or uval.dr.hand_suit[h][s][i] or ueval_ptr->res[j] or ueval_ptr->dr.hand_tot[h][i]
} UE_VALUES_k;

struct UserEvals_st {
   /* the most useful set; NT pts, and pts when playing in longest/best (or asked for) fit. */
   int nt_pts_side;
   int nt_pts_seat[2];
   int hldf_pts_side;
   int hldf_pts_seat[2] ;
   /* misc fields in case we want details re adj, length, syn, Dfit, Fn etc. */
   int misc_pts[58] ;
   int misc_count ;     /* SaveUserEvals will use this field  */
} ;

#endif /* file guard */
