/* File user_metricsubs.c -- by :JGM : code that implements calculating the various metrics factors */
/* Date      Version  Author  Description
* 2022/11/25 1.0.0    JGM  -- adjShort Honor subs.
* 2022/12/20 1.0.1    JGM     Mods to remove OPC and HCP from the code. Placeholders for R22, Kaplan, Sheinwold, Zar,
* 2022/12/26 1.1.0    JGM     Rethink of how to handle stiff honors, and Honors in 2 card suits, for PAV, Goren, Sheinw shortness guys
* 2023/01/13 1.2.0    JGM     Redo hcp adj. No more dispatch table. more use of common code.
*/

#include <assert.h>
#include "../include/dealdefs.h"  /* for enum ranks etc. */
#include "../include/UserEval_types.h"
#include "../include/UserEval_protos.h"  /* for all the Adj functions */


#include <math.h>  /* for type float_t */
#if 0
/* These next two lines for doc only. actual definitions are in UserEval_types.h
 *                    0      1    2     3     4      5       6      7    8    9     10   11    12     13   14   ??        -1
 * enum metric_ek { BERG=0, CPU, DKP, GOREN, JGM1, KAPLAN, KARPIN, KnR, LAR, MORSE, PAV, R22, SHEINW, ZAR, UNKN, SET=88, Quit=-1} ;
 */
#endif
/* For these deductions will apply to values in NT; and also for most length metrics (Bergen, Kaplan, etc.) */
/*                           std              CPU/BUM-RAP           C13          KnR          DKP*3        Andrews Fifths  */
float_t hcp_val[6][5] = { {4,3,2,1,0}, {4.5,3.0,1.5,0.75,0.25}, {6,4,2,1,0}, {3,2,1,0,0} , {13,9,5,2,0}, {4.,2.8,1.8,1.,0.4 } };
float_t stiff_H_adj[14][5] = {
   //              A   K   Q   J   T                       A    K   Q    J   T
   /* Berg   */ {-.5, -1, -1, -1, -0.},     /* CPU    */ {-.5, -1, -1,-.75,-.25 }, /* CPU is Bergen with fractions */
   /* DKP    */ { -0, -3, -3, -2, -0.},     /* GOR    */ { -0, -2, -2,  -1, -0. }, /* Stiff K in NT = 1; Stiff Q and J in NT = 0 */
   /* JGM1   */ { -0, -1,-1,-.75,-.25},
   /* Kaplan */ { -0, -1, -1, -1, -0.},     /* Karpin */ { -0, -1, -1,  -1, -0. }, /* Based on PAV web page JGM added -1 for J */
   /* KnR    */ { -0,-2.5,-1, -0, -0.},     /* LAR    */ { -0, -1, -1,  -1, -0. }, /* based on his book. LAR counts for Len */
   /* MORSE  */ { -0, -1, -1,-.75,-.25},    /* PAV    */ { -0, -1, -1,  -1, -0. }, /* Assume these values for NT. In suits N/A */
   /* R22    */ { -0, -1, -1, -1, -0.},     /* SHEINW */ { -0, -1, -1,  -1, -0. }, /* Check if this is Goren Also for NT. */
   /* ZAR    */ { -0, -0, -0,  -0, -0.}

   /* Berg: Varies. Usually counts for length as Opener, and for Trump Length AND shortness once a fit is found. lots of uniqueness*/
   /* Pav: says count for EITHER the stiff OR the honor; not both; so stiff Q or J = 2pts for the stiff; Stiff K = 3 pts for honor*/
   /* GOR: Sheinwold and Goren also count shortness start by assuming same as PAV for them */
   /* Kap: Kaplan counts for length, so assume he deducts the same as Bergen as starting point. */
   /* R22: Unless we have evidence to contrary assume others like R22, etc. count like Bergen. */
   /* DKP: These counts are BEFORE div by 3; DK deducts for any suit where lowest card > Ten */
   /*MORSE: MORSE and JGM1 are whatever tweaks JGM is trying out. */
   /* KnR: As per the text. Placeholder only since the code does it all, these values never referenced */
} ;
/* I Need to check the books for DKP ********** */
/* Morse is using 4.5,3,1.5,.75,.25 for HCP; so the deductions reflect that to make H and HH approx opc values */
/*                AK, AQ, AJ, AT, Ax,  KQ, KJ, KT, Kx,  QJ, QT, Qx,  JT, Jx, Tx, xx */
float_t dbl_HH_adj[14][16] = {
   /* Berg   */ { -0, -0,-.5, -0, -0.,-.5,-.5, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0},  /* Book gives -.5*/
   /* CPU    */ { -1, -1,-.5, -0, -0.,-.5,-.5, -0, -0., -1, -1, -1., -1,-.75,-.25,-0.0}, /* Berg with fractions */
   /* DKP    */ { -0, -0, -0, -0, -0., -1, -0,  0, -0., -3, -3, -3., -2, -2, -0,  -0.0}, /* unguarded H; but only 1/3 of shwn */
   /* GOR    */ { -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0},
   /* JGM1   */ { -1, -1,-.5, -0, -0., -1, -0, -0, -0., -1, -1, -1., -1,-.75,-.25,-0.0}, /* Using CPU count as starting pt. 4.5,3,1.5,0.75,0.25 */
   /* Kaplan */ { -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Same as Berg for now ???? */
   /* Karpin */ { -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Per PAV web Page.  */
   /* KnR    */ { -0, -0, -0, -0, -0., -0, -0, -0, -0., -0, -0, -0., -0, -0, -0,  -0.0}, /* KnR Dont deduct for dblton Honors */
   /* LAR    */ { -0, -0, -0, -0, -0., -1, -0, -0, -0., -1, -0, -0., -0, -0, -0,  -0.0}, /* based on book */
   /* MORSE  */ { -0, -.5,-.25,-0,-0., -0, -0, -0,-0.,-.25,-.25,-.5,-.5,-.75,-.25,-0.0}, /* reflects opc values for HH dblton*/
   /* PAV    */ { -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Applies to NT values. Suit N/A */
   /* R22    */ { -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0},
   /* SHEINW */ { -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* assume same as Goren for now ??? */
   /* ZAR    */ { -0, -0, -0, -0, -0., -0, -0, -0, -0., -0, -0, -0., -0, -0, -0,  -0.0}, /*Assume nothing for now. ZAR not impl yet */
} ;
/* OPC    { -1, -1,-.5, -0, -0.,-.5, -0, -0, -0., -1, -1,-.5,-1.5,-.5, -0,  -0.0}, No OPC since all done in Perl Keep for Ref */
//                  0      1   2    3     4      5       6      7    8    9     10   11    12     13    14      20    88    -1
//enum metric_ek { BERG=0,CPU,DKP,GOREN, JGM1, KAPLAN, KARPIN, KnR, LAR, MORSE, PAV, R22, SHEINW, ZAR, UNKNW,EXCP=20,SET=88,Quit=-1} ;

/* to handle exceptions if there are any */

float_t lookup_adj_stiff_H(enum metric_ek m, int w1) { /* metric  and top card bitmask */
   float_t adj_pts;
   int adjx100 ;
   switch(w1) {
      case 32 : adj_pts = stiff_H_adj[m][0]  ; break ;  /* stiff A */
      case 16 : adj_pts = stiff_H_adj[m][1]  ; break ;  /* stiff K */
      case 8  : adj_pts = stiff_H_adj[m][2]  ; break ;  /* stiff Q */
      case 4  : adj_pts = stiff_H_adj[m][3]  ; break ;  /* stiff J */
      case 2  : adj_pts = stiff_H_adj[m][4]  ; break ;  /* stiff T */
      default : adj_pts = 0.0                ; break ;  /* stiff spot */
   } /* end switch(w1) */
   adjx100 = adj_pts * 100. ;
   JGMDPRT(8,"Adj for stiff; weight=%d, Metric=%d StiffAdjx100=%d\n", w1, m, adjx100 );

   return (adj_pts) ;
} /* end lookup adj stiff honor */
float_t lookup_adj_HH(enum metric_ek m, int w2) { /* metric and top two cards bit mask */
   float_t adj_pts;
   int adjx100 ;
   switch(w2) {
      case 48 : adj_pts = dbl_HH_adj[m][0]  ; break ; // AK
      case 40 : adj_pts = dbl_HH_adj[m][1]  ; break ; // AQ
      case 36 : adj_pts = dbl_HH_adj[m][2]  ; break ; // AJ
      case 34 : adj_pts = dbl_HH_adj[m][3]  ; break ; // AT
      case 33 : adj_pts = dbl_HH_adj[m][4]  ; break ; // Ax
      case 24 : adj_pts = dbl_HH_adj[m][5]  ; break ; // KQ
      case 20 : adj_pts = dbl_HH_adj[m][6]  ; break ; // KJ
      case 18 : adj_pts = dbl_HH_adj[m][7]  ; break ; // KT
      case 17 : adj_pts = dbl_HH_adj[m][8]  ; break ; // Kx
      case 12 : adj_pts = dbl_HH_adj[m][9]  ; break ; // QJ
      case 10 : adj_pts = dbl_HH_adj[m][10] ; break ; // QT
      case  9 : adj_pts = dbl_HH_adj[m][11] ; break ; // Qx
      case  6 : adj_pts = dbl_HH_adj[m][12] ; break ; // JT
      case  5 : adj_pts = dbl_HH_adj[m][13] ; break ; // Jx
      case  3 : adj_pts = dbl_HH_adj[m][14] ; break ; // Tx
      default : adj_pts = 0.0               ; break ; // xx
   } /* end switch(w2) */
   adjx100 = adj_pts * 100.0 ;
   JGMDPRT(8,"AdjShortHonors: metric=%d, HH weight=%d, adjx100=%d \n",m, w2, adjx100 );
   return (adj_pts) ;
} /* end lookup adj Two Honors Dblton */

float_t excp_adj_fhcp(HANDSTAT_k *phs, int suit, int tag ) { /* switch statement goes here */ return 0.0 ; }

/* shortHon_adj handles the cases where the HCP adjustments are in the table. Other cases will need new code */
float_t shortHon_adj (HANDSTAT_k *phs, int seat, int suit, int tag ) {
   float_t adj_pts ;   // usually negative.
   int w1, w2 ;
   if (tag > END ) { adj_pts = excp_adj_fhcp(phs, suit, tag) ; return adj_pts; }
   assert(BERG <= tag && tag <= ZAR);
   /*
    * The Query tags in alpha order: The adj_hcp arrays use these values to lookup adjustments. ~ means not coded yet.
    *  0     1~   2    3~     4      5~      6      7    8    9     10   11~   12~    13~  14     20       88       89~     -1
    BERG=0, CPU, DKP, GOREN, JGM1, KAPLAN, KARPIN, KnR, LAR, MORSE, PAV, R22, SHEINW, ZAR, UNKN, EXCP=20, SET=88, NTSET=89, Quit=-1} ;

     */
   w1 = phs->topcards[suit][0] ;
   w2 = phs->topcards[suit][1] + w1 ;
   JGMDPRT(8,"Metric=%d, suit=%c, len=%d, w1=%d, w2=%d \n", tag, "CDHS"[suit], phs->hs_length[suit], w1, w2 ) ;

   if (phs->hs_length[suit] >= 3 || 0 == phs->hs_length[suit] ) { return 0.0 ; }
   if (phs->hs_length[suit] == 1 ) {
      adj_pts = lookup_adj_stiff_H( tag , w1 ) ;
      return (adj_pts) ;
   } /* end length == 1 */
   // len must be two here
   adj_pts = lookup_adj_HH( tag , w2 ) ;
   return (adj_pts);
} /* end shortHon_adj */

//                             Work=0        CPU/BumWrap=1         C13/Aces=2    KnR=3          DKP=4         Andrews Fiths=5
// float_t hcp_val[6][5] = { {4,3,2,1,0}, {4.5,3.0,1.5,0.75,0.25}, {6,4,2,1,0}, {3,2,1,0,0} , {13,9,5,2,0}, {4.,2.8,1.8,1.,0.4 } };
float_t calc_alt_hcp (HANDSTAT_k *phs, int seat, int suit, int mtype ) {
   float alt_hcp = 0.0 ;
   float alt_syn = 0.0 ;
   int r, j ;
   /* Translate the metric type: Berg .. Pav and beyond to the alt hcp table to use */
   int hcp_type[] = {0,0,4,0,1,0,0,3,0,1,0,0,0,0,0,0,0,0,0,0,0} ;
   int type = hcp_type[mtype];
   for (r= ACE , j=0 ; r >= TEN ; r--, j++ ) {
      if ( phs->Has_card[suit][r]   ) {
         alt_hcp += hcp_val[type][j] ;
         JGMDPRT(9, "calc_alt_hcp mType=%d, type=%d, suit=%c,  HasCard rank=%d, j=%d, alt_hcp_tot=%g \n",
                                    mtype, type, "CDHS"[suit], r, j, alt_hcp ) ;
      }
      /* For metric 9 aka Morse:
       *     give Q (1.5)  an extra 0.5 if with K or A in a 3+ suit
       *     give J (0.75) an extra 0.25 if with Q or T in a 3+ suit
       *     give T (0.25) an extra 0.25 if with J or 9 in a 3+ suit
       */
   }
   if (phs->hs_length[suit] >= 3 && mtype == 9 ) {
          if (phs->Has_card[suit][QUEEN] && ( phs->Has_card[suit][ACE]   || phs->Has_card[suit][KING]) ) {alt_syn += 0.5 ; }
          if (phs->Has_card[suit][JACK]  && ( phs->Has_card[suit][QUEEN] || phs->Has_card[suit][TEN] ) ) {alt_syn += 0.25; }
          if (phs->Has_card[suit][TEN]   && ( phs->Has_card[suit][JACK]  || phs->Has_card[suit][NINE]) ) {alt_syn += 0.25; }
      }
      alt_hcp += alt_syn;
      JGMDPRT(8,"Alt_Syn pts(Q,J,T) for mtype[%d] = %g New Suit Total=%g\n",mtype,alt_syn, alt_hcp );
   return alt_hcp ;
/* What code is doing in above loop
   if ( phs->Has_card[suit][ACE]   ) { alt_hcp += hcp_val[type][0] ; }
   if ( phs->Has_card[suit][KING]  ) { alt_hcp += hcp_val[type][1] ; }
   if ( phs->Has_card[suit][QUEEN] ) { alt_hcp += hcp_val[type][2] ; }
   if ( phs->Has_card[suit][JACK]  ) { alt_hcp += hcp_val[type][3] ; }
   if ( phs->Has_card[suit][TEN]   ) { alt_hcp += hcp_val[type][4] ; }
*/
}

float_t AdjDKP(HANDSTAT_k  *phs, int suit ) {
    /*     Note DKP adj are in effect 1/3 of what is calc here since final result is div by 3 */
    /*     Adj rules are simple: Any suit who's lowest card is higher than a Ten gets -1 adj
     *     This includes all stiff honors (stiff A incl) and all Dblton Honors (AK incl, down to QJ)
     *     Also any unguarded Q (QJ, QT, Qx) or Jack (JT(x) or Jx(x) ) gets -1 adj; but QJx is not adjusted
     */
      JGMDPRT(8,"AdjDKP, suit=%c, len=%d Top4=%d, QUEEN=%d, JACK=%d\n",
         "CDHS"[suit], phs->hs_length[suit], phs->hs_counts[Top4][suit] , phs->Has_card[suit][QUEEN], phs->Has_card[suit][JACK] ) ;
     if (phs->hs_counts[Top4][suit] == phs->hs_length[suit] && phs->hs_length[suit] > 0 ) { return (-1.0) ; }
     /* unguarded Queen. Note: KQ, AQ, QJ covered by previous statement. */
     if (phs->Has_card[suit][QUEEN] && phs->hs_length[suit] == 2 ) { return (-1.0) ; } /* QT, or Qx */
     /* test for unguarded Jack. i.e. No higher honor and length <= 3 */
     if (phs->Has_card[suit][JACK] && phs->hs_counts[Top4][suit] == 1 && phs->hs_length[suit] <= 3  ) { return (-1.0) ; }
     // fprintf(stderr, "!!!!!! AdjDKP reached NO adjustment \n");
     return (0.0) ; /* no adjustment */
} /* end adjDKP */

/* Dont need phs for this; only the trump struct with trump.decl and trump.dummy set correctly */
int DfitBERG( HANDSTAT_k *phs, TRUMP_FIT_k trump ) {
   int du, ss_len, t_len, dfit ;
   du = 0; //* assume N or E are dummy */
   if (trump.dummy == seat[1] ) { du = 1 ; }
   ss_len = trump.ss_len[du] ;
   t_len  = trump.tlen[du] ;
   if (t_len <= 2 ) { return 0 ; }
   JGMDPRT(7, "BERG Calc Dfit for du seat=%d:%c, du=%d, tlen=%d, and ss_len=%d\n",
               trump.dummy,compass[du], du, trump.tlen[du], trump.ss_len[du] );
   if      ( 0 == ss_len ) { dfit = t_len ; }
   else if ( 1 == ss_len ) { dfit = (t_len == 3) ? 2 : 3 ; } /* 4+ trumps, stiff gets 3 pts */
   else if ( 2 == ss_len ) { dfit = 1 ; }   /* count 1 doubleton only */
   else                    { dfit = 0 ; }  /* no dfit pts if shortest suit is 3+ */
   JGMDPRT(7, "DfitBERG Returning, Dummy=[%d:%c], ss_len[du]=%d, Tlen[du]=%d, dfit_pts=%d\n",du, compass[du], ss_len, t_len, dfit ) ;
   fsync(2) ;
   return dfit ;
} /* end DfitBERG */

int DfitKARPIN( HANDSTAT_k *phs, TRUMP_FIT_k trump ) { /* phs will point to Dummy's hand -- the one getting the Dfit pts */
   int du, ss_len, t_len, dfit ;
   int idx_short ;
   int DF_Karpin[2][4] = { {2,1,0,0},{3,2,1,0} } ; /* Dfit pts with 3/4 trump for V/S/D/Longer */
   int v0, s1, db ; /* Debug vars */
   du = 0; //* assume N or E are dummy */
   if (trump.dummy == seat[1] ) { du = 1 ; }
   t_len = trump.tlen[du];
   ss_len = trump.ss_len[du] ;
   JGMDPRT(7, "KARPIN Calc Dfit for du seat=%d:%c, du=%d, tlen=%d, and ss_len=%d\n",
               trump.dummy,compass[du], du, trump.tlen[du], trump.ss_len[du] );
   v0 = 0 ; s1 = 0 ; db = 0 ;
   if (trump.tlen[du] <= 2 ) { return 0 ; }

   /* count for 1 shortest suit only */
   if      ( 0 == ss_len ) { idx_short = 0 ; v0++ ;}
   else if ( 1 == ss_len ) { idx_short = 1 ; s1++ ;}
   else if ( 2 == ss_len ) { idx_short = 2 ; db++ ;}
   else                    { idx_short = 3 ; }
   if (3 == t_len  ) { dfit = DF_Karpin[0][idx_short] ; }
   else              { dfit = DF_Karpin[1][idx_short] ; } /* 4+ support; since tlen >2 here */
   JGMDPRT(7, "DfitKARPIN Return, Dummy=[%d], dfit=%d, from trump.ss_len=%d,t_len[du]=%d: DBG:%d,%d,%d\n", du, dfit, ss_len, t_len,v0,s1,db ) ;
   return dfit ;
} /* end DfitKARPIN */
int DfitLAR( HANDSTAT_k *phs, TRUMP_FIT_k trump ) { /* phs will point to Dummy's hand -- the one getting the Dfit pts */
   int du, ss_len, t_len, dfit ;
   int idx_short ;
   int DF_LAR[2][4] = { {3,2,1,0},{5,3,1,0} } ; /* Dfit pts with 3/4 trump for V/S/D/Longer -- Only if 9+ fit */
   int v0, s1, db ; /* Debug vars */
   du = 0; //* assume N or E are dummy */
   if (trump.dummy == seat[1] ) { du = 1 ; }
   t_len = trump.tlen[du];
   ss_len = trump.ss_len[du] ;
   JGMDPRT(7, "LAR Calc Dfit for du seat=%d:%c, du=%d, tlen=%d, and ss_len=%d\n",
               trump.dummy,compass[du], du, trump.tlen[du], trump.ss_len[du] );
   v0 = 0 ; s1 = 0 ; db = 0 ;
   /* Larsson only gives for "9 fit"  he says then assign 5-3-1; but I am not sure about 6-3? 5-3-1 with only 3 trump is a lot.
    * So I compromise a 9 fit with 4 trump in dummy gets 5-3-1; a 9+ fit with only 3 trump in dummy 3-2-1
    */
   if ( (trump.tlen[du] <= 2 ) || ((trump.tlen[0] + trump.tlen[1]) < 9) ) { return 0 ; }
   // if ( (trump.tlen[0] + trump.tlen[1] < 9 ) || trump.tlen[du] < 4 ) { return 0 ;} /* this is the other choice for larsson */
   /* We have a 9 card fit. count for 1 shortest suit only */
   if      ( 0 == ss_len ) { idx_short = 0 ; v0++ ;}
   else if ( 1 == ss_len ) { idx_short = 1 ; s1++ ;}
   else if ( 2 == ss_len ) { idx_short = 2 ; db++ ;}
   else                    { idx_short = 3 ; }
   if (3 == t_len  ) { dfit = DF_LAR[0][idx_short] ; }  /* 3 card support; */
   else              { dfit = DF_LAR[1][idx_short] ; }  /* 4+ support; */
   JGMDPRT(7, "DfitLAR Returning, Dummy=[%d], dfit=%d, from trump.ss_len=%d,t_len[du]=%d: DBG:%d,%d,%d\n",
                  du, dfit, ss_len, t_len,v0,s1,db ) ;
   return dfit ;
} /* end DfitLAR */
int DfitMOPC( HANDSTAT_k *phs, TRUMP_FIT_k trump ) { /* phs will point to Dummy's hand -- the one getting the Dfit pts */
   int du, ss_len, t_len, dfit ;
   int idx_short ;
   int DF_MOPC[2][4] = { {3,2,1,0},{4,3,2,0} } ; /* Dfit pts with 3/4 trump for V/S/D/Longer -- Only if 9+ fit */
   int v0, s1, db ; /* Debug vars */
   du = 0; //* assume N or E are dummy */
   if (trump.dummy == seat[1] ) { du = 1 ; }
   t_len = trump.tlen[du];
   ss_len = trump.ss_len[du] ;
   JGMDPRT(7, "MOPC Calc Dfit for du seat=%d:%c, du=%d, tlen=%d, and ss_len=%d\n",
               trump.dummy,compass[du], du, trump.tlen[du], trump.ss_len[du] );
   v0 = 0 ; s1 = 0 ; db = 0 ;
   /*
    * Modfied OPC -- Only for "9 fit"  Dfit = #Trumps - short_suit Length; also no Dfit for 2 trumps (opc would count V=2,S=1)
    */
   if ( (trump.tlen[du] <= 2 ) || ((trump.tlen[0] + trump.tlen[1] ) < 9)  ) { return 0 ; }
   // if ( (trump.tlen[0] + trump.tlen[1] < 9 ) || trump.tlen[du] < 4 ) { return 0 ;} /* this is the other choice for larsson */
   /* We have a 9 card fit. count for 1 shortest suit only */
   if      ( 0 == ss_len ) { idx_short = 0 ; v0++ ;}
   else if ( 1 == ss_len ) { idx_short = 1 ; s1++ ;}
   else if ( 2 == ss_len ) { idx_short = 2 ; db++ ;}
   else                    { idx_short = 3 ; }
   if (3 == t_len  ) { dfit = DF_MOPC[0][idx_short] ; }  /* 3 card support; */
   else              { dfit = DF_MOPC[1][idx_short] ; }  /* 4+ support; */
   JGMDPRT(7, "DfitMOPC Returning, Dummy=[%d], dfit=%d, from trump.ss_len=%d,t_len[du]=%d: DBG:%d,%d,%d\n",
                  du, dfit, ss_len, t_len,v0,s1,db ) ;
   return dfit ;
} /* end DfitOPC */
/* The common way of counting support points; Any 8+ fit; 5/3/1 with 4+ suppt, 3/2/1 with 3+ suppt -- ONE short suit only */
int DfitSTD( HANDSTAT_k *phs, TRUMP_FIT_k trump ) { /* phs will point to Dummy's hand -- the one getting the Dfit pts */
   int du, ss_len, t_len, dfit ;
   int idx_short ;
   int DF_STD[2][4] = { {3,2,1,0},{5,3,1,0} } ; /* Dfit pts with 3/4 trump for V/S/D/Longer any 8+ fit  */
   int v0, s1, db ; /* Debug vars */
   du = 0; //* assume N or E are dummy */
   if (trump.dummy == seat[1] ) { du = 1 ; }
   t_len = trump.tlen[du];
   ss_len = trump.ss_len[du] ;
   JGMDPRT(7, "STD Calc Dfit for du seat=%d:%c, du=%d, tlen=%d, and ss_len=%d\n",
               trump.dummy,compass[du], du, trump.tlen[du], trump.ss_len[du] );
   v0 = 0 ; s1 = 0 ; db = 0 ;
   if ( (trump.tlen[du] <= 2 ) ) { return 0 ; } /* a 6-2 or 7-2 fit does not get any suppt pts */
   if      ( 0 == ss_len ) { idx_short = 0 ; v0++ ;}
   else if ( 1 == ss_len ) { idx_short = 1 ; s1++ ;}
   else if ( 2 == ss_len ) { idx_short = 2 ; db++ ;}
   else                    { idx_short = 3 ; }
   if (3 == t_len  ) { dfit = DF_STD[0][idx_short] ; }  /* 3 card support; */
   else              { dfit = DF_STD[1][idx_short] ; }  /* 4+ support; */
   JGMDPRT(7, "DfitSTD Returning, Dummy=[%d], dfit=%d, from trump.ss_len=%d,t_len[du]=%d: DBG:%d,%d,%d\n",
                  du, dfit, ss_len, t_len,v0,s1,db ) ;
   return dfit ;
} /* end DfitSTD */


/* Bergen FN: A sort of catchall routine for the points that Opener adds to his hand after a raise; Not for a 4-4 fit */
/* Only needs the phs for the hand getting the Fn pts usually -- not both as would be the case for OPC . */
int Fn_ptsBERG( HANDSTAT_k *phs, TRUMP_FIT_k trump ) {
   int  dbltons, fn_pts, t5, v0, s1 ;
   int suit ;
   int dc = 1; //* assume S or W are declarer */
   if (trump.decl == seat[0] ) { dc = 0 ; }
   JGMDPRT(7, "BERG Calc FN_pts for dc seat=%d:%c, dc=%d, tlen=%d, and ss_len=%d\n",
               trump.decl,compass[dc], dc, trump.tlen[dc], trump.ss_len[dc] );
   fn_pts  = 0 ;
   dbltons = 0 ;
   t5=0; v0=0; s1=0;
   if (5 == trump.tlen[dc] ) { fn_pts += 1 ; t5 = 1; }
   else if ( 5 < trump.tlen[dc] ) { fn_pts += 1 + (trump.tlen[dc] - 5)*2 ; t5 = fn_pts ;  } /* 2 pts for every trump >5 +1 for 5th trump */
   /* now add for short suits in Declarer's hand */
   for (suit = CLUBS; suit<=SPADES; suit++ ) {
      if      ( 0 == phs->hs_length[suit] ) { fn_pts += 3 ; v0 = 3 ; } /* 3 pts for a void  -- Internet page says 4*/
      else if ( 1 == phs->hs_length[suit] ) { fn_pts += 2 ; s1 = 2 ; } /* 2 pts for a stiff */
      else if ( 2 == phs->hs_length[suit] ) { dbltons += 1; } /* Count doubletons. */
   }
   if (dbltons >= 2 ) { fn_pts += 1 ; } /* 1 pt for 2 or 3 dbltons */
   JGMDPRT(7,"Fn_BERG Done. Decl=[%d:%c] Fn_pts=%d from 5+Trump=%d, void_pts=%d, stiff_pts=%d, dbltons_cnt=%d \n",
                           dc, compass[dc], fn_pts, t5, v0, s1, dbltons ) ;
   return fn_pts ;
} /* end Fn_ptsBERG */
int Fn_ptsKARPIN( HANDSTAT_k *phs, TRUMP_FIT_k trump ) {
   int  fn_pts ;
   int suit ;
   int idx_short ;
   int dc = 1; //* assume S or W are declarer */
   if (trump.decl == seat[0] ) { dc = 0 ; }
   JGMDPRT(7, "KARPIN Calc FN_pts for dc seat=%d:%c, dc=%d, tlen=%d, and ss_len=%d\n",
               trump.decl,compass[dc], dc, trump.tlen[dc], trump.ss_len[dc] );
   int v0, s1, db ;     /*Debug Vars */
   fn_pts  = 0 ;
   int FN_Karpin[2][4] = { {2,1,0,0}, {3,2,1,0} } ; /* 5 Trumps V=2,S=1,D=0; 6+ trumps V=3, S=2, D=1 each ; longer = 0 */
   /* Karpin add for short suits in Declarer's hand once a trump fit is found */
   v0 = 0 ; s1 = 0 ; db = 0 ;
   for (suit = CLUBS; suit<=SPADES; suit++ ) {
      if(suit == trump.tsuit) continue ;
      if      ( 0 == phs->hs_length[suit] ) { idx_short = 0 ; v0++ ;}
      else if ( 1 == phs->hs_length[suit] ) { idx_short = 1 ; s1++ ;}
      else if ( 2 == phs->hs_length[suit] ) { idx_short = 2 ; db++ ;} /* assume Karpin gives for each dblton in Decl Hand*/
      else if ( 3 <= phs->hs_length[suit] ) { idx_short = 3 ; }
      if      (trump.tlen[dc] == 5 ) { fn_pts += FN_Karpin[0][idx_short] ; }
      else if (trump.tlen[dc] >  5 ) { fn_pts += FN_Karpin[1][idx_short] ; }
   } /* end for each suit */
   JGMDPRT(7,"Fn_KARPIN Done. Decl=[%d:%c] Fn_pts=%d from T_len=%d, void_cnt=%d, stiff_cnt=%d, dblton_cnt=%d \n",
                           dc, compass[dc], fn_pts, trump.tlen[dc], v0, s1, db );
   return fn_pts ;
} /* end Fn_ptsKARPIN */

    /*
     * Hf Points:
     *   Traditional way: 1..3 hcp in an 8+ trump fit (even stiff K vs 7 card suit) add +1
     *   Bergen? way:     1..3 hcp in any suit with an 8+ fit even a 4-4; does not need to be the short hand. QJxxx vs AKx add +1
     *    Opc Method:     Same as Bergen but in addition Hx vs 5 (7 fit) add 1Hf pt for Jx,Qx,Kx in any of pard's 5+ suits.
     * With at least a doubleton and 1-3 hcp vs 5+ length in pards hand, add 1 Hf pt. e.g. Kxx vs 4+ or Jx vs 5+ etc.
     * Assume Bergen does not count Hf in NT, since no mention of them in 'starting points'
     */
int Hf_ptsBERG( HANDSTAT_k *phs[] , int Hf_hand ) {
   JGMDPRT(7,"Hf_ptsBERG, for short? hand # %d, seat=%c\n",Hf_hand, compass[Hf_hand]) ;
    /* Bergen Hf Points:  With 1-3 hcp in any/all 8+ fit suits,  add +1 Hf pts; max 2 Hf pts per hand */
    int long_hand , s, Hf_pts  ;
    int lh_len, Hf_len, Hf_hcp ;
    long_hand = ( 1 == Hf_hand ) ? 0 : 1 ; /* the 'other' hand is long_hand*/
    Hf_pts = 0 ;
    for (s=CLUBS; s<=SPADES; s++ ) {
       lh_len = phs[long_hand]->hs_length[s] ;
       Hf_len = phs[Hf_hand]->hs_length[s] ;
       Hf_hcp = phs[Hf_hand]->hs_points[s] ;
       if ( 1 <= Hf_hcp && Hf_hcp <= 3 &&
          ( ( (lh_len + Hf_len) >= 8 ) || ( lh_len >=5 && (lh_len + Hf_len) >= 7 ) ) ) {
          Hf_pts += 1 ; /* Kxxx vs Qxxx (both), or Axxxx vs Jx (+1), or AJxxxx vs Q (+1) etc. */
       }

    } /* end for Hf Points calc */
    JGMDPRT(7,"BERG Hand=%c, gets a total of %d Hf points (Max of 2) added\n", compass[Hf_hand],Hf_pts );
    if (Hf_pts > 2 ) { Hf_pts = 2 ; }

    return Hf_pts ;
} /* end Hf_pts_berg */

inline int LptsSTD(HANDSTAT_k *phs, int suit ) { // such a simple function make it inline
   if (phs->hs_length[suit] > 4 ) {
            return (phs->hs_length[suit] - 4 ) ; // +1 for 5 card suit, +2 for six, +3 for 7 etc.
   }
   return 0 ;
}
/* Check which hand would be dummy in a NT contract. Assume the hand with the most hcp will declare. */
int LptsPAV( HANDSTAT_k *phs[] )  {
   int s ;
   int NTdummy ;
   if ( phs[0]->hs_totalpoints <= phs[1]->hs_totalpoints ) { NTdummy = 0 ; }
   else { NTdummy = 1 ; }
   lpts[NTdummy] = 0 ;
   for (s = CLUBS; s <= SPADES ; s++ ) {
      if ( phs[NTdummy]->hs_length[s] >= 5 ) { lpts[NTdummy] = 1 ; }
   }
   JGMDPRT(7,"LptsPAV: NTDummy=%d, lpts[%d]=%d \n",NTdummy, NTdummy, lpts[NTdummy] );
   return NTdummy ;
}

int SynBERG(HANDSTAT_k *phs, int suit ) {
    JGMDPRT(8,"SynBERG, Suit=%c Len=%d, Top5=%d\n","CDHS"[suit],phs->hs_length[suit],phs->hs_counts[Top5][suit]) ;
      if ( (phs->hs_length[suit] >= 4 ) && (phs->hs_counts[Top5][suit] >= 3 ) ) { //  Bergen Quality +1 for 3/top5 in 4+ suit;
         return(1) ;
      }
      return(0) ;
}
int SynDKP(HANDSTAT_k *phs, int s ) {
    int synpts = 0 ;
    if (phs->Has_card[s][KING]  && phs->Has_card[s][ACE] )       { synpts += 1 ; } // King with the Ace
    if (phs->Has_card[s][QUEEN] && phs->hs_counts[Top2][s] > 0 ) { synpts += 1 ; } // Queen with Ace or King
    if (phs->Has_card[s][JACK]  && phs->hs_counts[Top2][s] > 0 ) { synpts += 1 ; } // Jack with Ace or King

    /* Tens with two 'companions' worth +2; with one 'companion' +1 (not T9 dblton) */
    if (phs->Has_card[s][TEN] ) {
       if ( (phs->hs_counts[Top4][s] >= 2 ) || (phs->hs_counts[Top4][s] == 1 && phs->Has_card[s][NINE]) ) {
          synpts += 2 ;
       }
       else if ( (phs->hs_counts[Top4][s] == 1)  || (phs->Has_card[s][NINE] && phs->hs_length[s] > 2 ) ) {
           synpts += 1 ;
       }
    } /* end Has_Ten */
    JGMDPRT(8,"SynDKP, SynPTS=%d, Suit=%c Len=%d, Top4=%d, HAS_Ten=%d, Has_Nine=%d \n",
         synpts, "CDHS"[s],phs->hs_length[s],phs->hs_counts[Top4][s], phs->Has_card[s][TEN], phs->Has_card[s][NINE]) ;
    return synpts ;
} /* end SynDKP */

/* Larsson Synergy. His own invention? */
/* if there are two suits (len = 3+) with two+ of top4 honors (one of which must be A or K), then +1; if 3 such suits, +2 */
int SynLAR(HANDSTAT_k *phs ) {
   int syn_cnt = 0 ;
   int syn_pts = 0 ;
   int suit ;
   for (suit = CLUBS; suit <=SPADES ; suit++ ) {
      if ( phs->hs_counts[Top2][suit] >= 1 && phs->hs_counts[Top4][suit] >= 2 && phs->hs_length[suit] >= 3 ) {
         syn_cnt++ ;
      }
   }
   syn_pts = (syn_cnt >  2 ) ? 2 :
             (syn_cnt == 2 ) ? 1 : 0 ;
   JGMDPRT(7,"SynLAR, syn_cnt=%d, syn_pts=%d\n", syn_cnt, syn_pts) ;
   return syn_pts ;
}

/* From Wikipedia: Combo Count for Length AND shortness in a hand; seem to be two different methods. Not sure why. */
int ComboLD_pts(HANDSTAT_k *phs , int type ) {
   JGMDPRT(7, "ComboLD_pts, Type=%d \n",type) ;
   int s ;
   int ss_len, ls_len, ls2_len, s_len ; /* suit lens needed for calcs. suit nums needed for Debug */
   ss_len = 13 ;
   ls_len = 0  ;
   ls2_len = 0 ;
   int ld_pts = 0 ; /* points for length AND shortness */

   /* Combo Pts Type1: Void=2, Stiff=1, L = (length - 4) Gives: (4333=0, 4432=0, 5332 = 1 etc. for each suit. */
   if (1 == type ) {
      for (s = CLUBS ; s <=SPADES ;  s++ ) {
         s_len = phs->hs_length[s] ;
         if      ( 0 == s_len ) { ld_pts += 2 ; }
         else if ( 1 == s_len ) { ld_pts += 1 ; }
         else if ( 4 <  s_len ) { ld_pts += ( s_len - 4 ) ; }
         //printf( "Type1: suit[%c] len = %d, cumul ld pts = %d \n","CDHS"[s], length[s], ld_pts ) ;
      }  /* end for each suit*/
      JGMDPRT(7,"ComboLD_pts, Type=%d, Combo_ld_pts=%d \n",type,ld_pts) ;
      return (ld_pts ) ;
   } /* end if type 1 */

   /* Type: 2 longest + 2nd longest - shortest - 5 :: 4333 = -1 4432 = 1 ; 5332 = 1 etc. */
   for (s = CLUBS ; s <=SPADES ;  s++ ) {  /* find shortest and longest suits */
      s_len = phs->hs_length[s] ;
      if (s_len <  ss_len ) {ss_len = s_len ;  }
      if (s_len >= ls_len ) {ls2_len = ls_len;  ls_len = s_len ;  } /* demote the previous longest to 2nd longest */
      else if (s_len > ls2_len ) { ls2_len = s_len ; }
      // printf( "Type2: After suit[%c] of len=%d, ss = %d, ls = %d, ls2 = %d\n",
      //    "CDHS"[s], s_len, ss, ls, ls2 ) ;
   } /* end for each suit type2 */
   ld_pts = ls_len + ls2_len - ss_len - 5 ;
   // printf("Result COMBO LD Pts Type 2 = %d\n",ld_pts);
   JGMDPRT(7,"ComboLD_pts, Type=%d, ss=%d, ls2=%d, ls=%d ComboPts=%d\n",type,ss_len, ls2_len, ls_len, ld_pts) ;
   return (ld_pts) ;
} /* end combo_LD_pts() */

int DptsPAV( HANDSTAT_k *phs, int s ) { /* Count for shortness; deduct the shortness pts later if there is a misfit. Count even short honors */
   int dpts ;
   int v0 = 0 ; int s1 = 0 ; int db = 0 ; /* debug vars */
   dpts = 0 ;
   switch ( phs->hs_length[s] ) {
      case 0 : dpts += 3; v0++ ; break ;
      case 1 : dpts += 2; s1++ ; break ;
      case 2 : dpts += 1; db++ ; break ;
      default: break ;
   }
   JGMDPRT(8,"PAV Shortness pts returns %d from :DBG-V,S,D: %d,%d,%d \n", dpts, v0,s1,db );
   return dpts ;
} /* end Dpts_PAV */

int DfitPAV( HANDSTAT_k *phs_du, TRUMP_FIT_k trump ) { /* phs will point to Dummy's hand -- the one getting the Dfit pts */
   int du, ss_len, t_len, dfit ;
   int DF_PAV[4] =  {2,1,0,0} ; /* Dfit pts  added to orginal Dpts if dummy has 4+ Trump Nothing for Doubletons*/
   du = 0; //* assume N or E are dummy */
   if (trump.dummy == seat[1] ) { du = 1 ; }
   t_len = trump.tlen[du];
   ss_len = trump.ss_len[du] ;
   JGMDPRT(7, "PAV Calc Dfit for du seat=%d:%c, du=%d, tlen=%d, and ss_len=%d\n",
               trump.dummy,compass[du], du, trump.tlen[du], trump.ss_len[du] );

   if (trump.tlen[du] <= 3 ) { return 0 ; }

   /* Dummy has 4+ trumps; add some extra for Void or Stiff -- Count 1 short suit only */
   dfit = DF_PAV[ss_len] ;
   JGMDPRT(7, "DfitPAV, Dummy=[%d], dfit=%d, from trump.ss_len=%d,t_len[du]=%d\n", du, dfit, ss_len, t_len ) ;
   return dfit ;
} /* end DfitPAV */

/* Declarer adds pts for extra trump length, and for side suit length */
int Fn_ptsPAV( HANDSTAT_k *phs_dc, TRUMP_FIT_k trump ) { /* phs will point to Declarer's hand -- the one getting the Fn pts */
   int  fn_pts ;
   int s ;
   int dc = 1; //* assume S or W are declarer */
   int tlen_pts, slen_pts;
   if (trump.decl == seat[0] ) { dc = 0 ; }
   JGMDPRT(7, "PAV FN_pts Declarer *handstat = %p, dc=%d, seat[dc]=%d, \n",(void *)phs_dc,dc,seat[dc] ) ;
   fn_pts  = 0 ; tlen_pts = 0 ; slen_pts = 0 ;
   /* PAV add for length in Declarer's hand once a trump fit is found: side suit length and extra trump suit length*/

      /* +1 for EACH 4+ side suit */
      for (s = CLUBS; s <=SPADES ; s++ ) {
         if (s != trump.tsuit && phs_dc->hs_length[s] > 3 ) { slen_pts++ ; }
         JGMDPRT(8,"**** FN-SideSuit=%d,  SideSuitLen=%d, FN_SideSuitPts=%d\n",s,phs_dc->hs_length[s], slen_pts ) ;
   } /*end side suit loop */

   /* Check for extra trump length: +1 if 5 and not promised 5 */
   if ( (trump.tsuit <= DIAMONDS ) && trump.tlen[dc] >= 5 ) { tlen_pts++ ; } /* Hack: a minor does not promise 5; a Major does */
   if (trump.tlen[dc] > 5 ) {tlen_pts += (trump.tlen[dc] - 5 )*2 ; }         /* +2 for each trump over 5; */
   fn_pts = slen_pts + tlen_pts;
   JGMDPRT(7,"Fn_ptsPAV Decl=[%d;%c] Fn_pts=%d Tlen=%d, SideSuitCnt=%d \n", dc,"NESW"[trump.decl],fn_pts,trump.tlen[dc], slen_pts );
   return fn_pts ;
} /* end Fn_ptsPAV */
int Fn_ptsNONE( HANDSTAT_k *phs_dc, TRUMP_FIT_k trump ) { return 0 ; }
int DfitNONE(   HANDSTAT_k *phs_dc, TRUMP_FIT_k trump ) { return 0 ; }


struct FitPoints_st Do_Trump_fit(
         HANDSTAT_k *phs[2],
         int (*calc_dfval)(HANDSTAT_k *p_hs, TRUMP_FIT_k trump),
         int (*calc_fnval)(HANDSTAT_k *p_hs, TRUMP_FIT_k trump) )
   /*
    * Check if there is an 8+ fit. If there is call the metric specific function to
    * a) calculate the Dummy support points, and
    * b) calculate any other points that Declarer might get in the case of a trump fit. (Called Fn_pts )
    * The material specifying the metric does not usually address these two issues:
    * i) how to handle a 4-4 (or 5-5) fit.
    * ii) how many short suits is dummy allowed to count for ?
    * This code assumes (i) that if the trumps are equal in both hands (4-4 or 5-5) then BOTH hands count dummy points
    * and (ii) Dummy (or both) can count for ONLY ONE short suit. So if Dummy is 6=4=2=1 with 4 trump it counts only for the stiff
    * When the metric DOES specify then Declarer is the hand with the longest trumps, or if equal, the hand with the most HCP.
   */
{
   HANDSTAT_k  *phs_dummy, *phs_decl ;
   TRUMP_FIT_k trump2 ;
   struct FitPoints_st TFpts = { {0,0}, {0,0} } ;
   trump = trump_fit_chk(phs) ; /* fill the structure describing the trump fit */
   if ( trump.fit_len < 8 ) { return TFpts ; } /* if no 8-fit return zeroes This routine ignores 5-2 fits */

    /*
     * if there is a 8+ trump fit, calc Dfit and Fn Points.
     * What kind of Df and Fn will depend on which func ptrs are passed in
     */
      if(seat[0] == trump.dummy) {h_dummy = 0 ; h_decl = 1 ;  }
      else                       {h_dummy = 1 ; h_decl = 0 ;  }
      phs_dummy = phs[h_dummy] ;   // could also do phs_dummy = &p_dldat->hs[dummy]
      phs_decl  = phs[h_decl]  ;
      JGMDPRT(7,"DO_TRUMP_FIT: h_dummy=%c, h_decl=%c trump.dummy=%d, Seat[0] = %d\n",compass[h_dummy], compass[h_decl], trump.dummy, seat[0] );
      if (trump.tlen[h_dummy] < trump.tlen[h_decl]) {   // trump_fit_chk() func will ensure tlen[du] < tlen[dc] UNLESS they are equal
         TFpts.df_val[h_dummy]   = (*calc_dfval) (phs_dummy, trump ) ;
         TFpts.fn_val[h_decl]    = (*calc_fnval) (phs_decl,  trump ) ;
         JGMDPRT(7, ": dfit for dummy[%c]=%d, FN for Decl[%c]=%d\n",
            compass[h_dummy],TFpts.df_val[h_dummy],compass[h_decl],TFpts.fn_val[h_decl] );
      }
      else if (trump.tlen[h_dummy] == trump.tlen[h_decl]) {  /* probably a 4=4 fit; assign dfit to both hands, and no Fn  */
         TFpts.df_val[h_dummy]   = (*calc_dfval) (phs_dummy, trump ) ;
         /* now call dfit again swapping the dummy and decl fields */
         trump2 = trump ;  /* trump is a global used by others. make our own copy to modify */
         trump2.decl = trump.dummy ;
         trump2.dummy = trump.decl ;
         // JGMDPRT(7, "BERG: Calling dfitBERG with trump2; trump2.decl=%d, trump2.dummy=%d\n",trump2.decl, trump2.dummy );
         TFpts.df_val[h_decl]   = (*calc_dfval) (phs_decl, trump2 ) ;
         JGMDPRT(7, ": Dfit for dummy[%c]=%d, AND Dfit for Decl[%c]=%d\n",
            compass[h_dummy],TFpts.df_val[h_dummy],compass[h_decl],TFpts.df_val[h_decl] );
      }
      else {
         fprintf (stderr, "***ABORT*** Cant happen in Do_Trump_fit, Dummy_tlen[%d]=%d, Decl_tlen[%d]=%d\n",
                           h_dummy, trump.tlen[h_dummy], h_decl, trump.tlen[h_decl] );
         assert(0) ;
      }
      /* set the globals as a convenience */
      dfit_pts[0] = TFpts.df_val[0]; dfit_pts[1] = TFpts.df_val[1];
      Fn_pts[0]   = TFpts.fn_val[0]; Fn_pts[1]   = TFpts.fn_val[1];
      return TFpts ;
}







