/* File user_metricsubs.c -- by :JGM : code that implements calculating the various metrics factors */
/* Date      Version  Author  Description
* 2022/11/25 1.0.0    JGM  -- adjShort Honor subs.
* 2022/12/20 1.0.1    JGM     Mods to remove OPC and HCP from the code. Placeholders for R22, Kaplan, Sheinwold, Zar,
* 2022/12/26 1.1.0    JGM     Rethink of how to handle stiff honors, and Honors in 2 card suits, for PAV, Goren, Sheinw shortness guys
* 2023/01/13 1.2.0    JGM     Redo hcp adj. No more dispatch table. more use of common code.
* 2023/04/15 1.3.0    JGM     Add code for Metrics Goren, Kaplan, Sheinwold. Make Dfit for EACH shortness the usual case for all.
*/

#include <assert.h>
#include "../include/dealdefs.h"  /* for enum ranks etc. */
#include "../include/UserEval_types.h"
#include "../include/UserEval_protos.h"  /* for all the Adj functions */

#include <math.h>  /* for type float_t */
#if 0
/* These next two lines for doc only. actual definitions are in UserEval_types.h
 *                     0        1       2     3     4      5~      6      7    8    9     10     11       12      13      14        20          21
enum metric_ek    { BERGEN=0, BISSEL,  DKP, GOREN, JGM1, KAPLAN, KARPIN, KnR, LAR, MORSE, PAV, SHEINW,  ZARBAS, ZARADV, metricEND, MixJGM=20, MixMOR,
                    SET=88, SYNTST=99, Quit=-1} ;
*/
 /* Notes re various ways evaluating 'points' esp as relates to short Honors.
  * BERGEN: Varies. Usually counts for length as Opener, and for Trump Length AND shortness once a fit is found. lots of uniqueness
  * BISSEL: Deduct -1 or -0.5 as reqd to preserve the relative strengths. Bissell honors are 3,2,1,0 depending on missing higher
  * DKP:    Deduct for any suit where lowest card > Ten. These deductions are BEFORE div by 3;
  * GOREN:  Count shortness initially. Deductions per Pavlicek web site.
  * JGM1:   Various mods. Currently reflects Karpin.
  * KAPLAN: Count for length; Deductions for short honors inferred from his book.
  * KARPIN: Count for length; Deductions from Pavlicek website
  * KnR:    As per the BW article. Table Entries are placeholders only since the code does it all, these values never referenced
  * LAR:    Count for length; Deductions per his book.
  * MORSE:  Various mods. Currently reflects Larsson
  * PAV:    Count for shortness. Per the Pavlicek website and online evaluator
  * ZARBAS: Basic Zar Points; HCP + CTLS + (A+B) + (A -D)
  * ZARADV: Basic + Hf pts + adj for HCP in 2/3 suits + FN pts for extra trump length etc. No Misfit points.
  */
#endif
/*                           std              BUM-RAP/OPC           C13          KnR          DKP*3        Andrews Fifths  */
float_t hcp_val[7][5] = { {4,3,2,1,0}, {4.5,3.0,1.5,0.75,0.25}, {6,4,2,1,0}, {3,2,1,0,0} , {13,9,5,2,0}, {4.,2.8,1.8,1.,0.4 },
                                       {4.5,3.0,1.75,0.75} /* Woolsey */
};
float_t stiff_H_adj[14][5] = {
   //              A   K   Q   J   T                       A    K   Q   J   T
   /* Bergen */ {-.5, -1, -1, -1, -0.},     /* BISSEL */ { -0, -1, -1, -1, -0. },
   /* DKP    */ { -0, -3, -3, -2, -0.},     /* Goren  */ { -0, -2, -2, -1. -0. }, /* Stiff K in NT = 1*/
   /* JGM1   */ { -0, -1,-1,-.75,-.25},
   /* Kaplan */ { -0, -0, -2, -1, -0.},     /* Karpin */ { -0, -1, -1,  -1, -0. }, /* Based on PAV web page JGM added -1 for J */
   /* KnR    */ { -0,-2.5,-1, -0, -0.},     /* LAR    */ { -0, -1, -1,  -1, -0. }, /* based on his book. LAR counts for Len */
   /* MORSE  */ { -0, -1, -1,-.75,-.25},    /* PAV    */ { -0, -2, -2,  -1, -0. }, /* He says he follows Goren */
   /* SHEINW */ { -0, -1, -1, -1, -0.},     /* per 5 Weeks To Winning Bridge */
   /* ZARBAS */ { -1, -1, -1, -1, 0. },     /* ZARADV */ { -1, -1, -1, -1, 0.}    /* Stiff A is questionable ?*/
   // OPC Stiff H values after Deductions (for ref Only): A=3.5, K=2, Q=0.5, J=0, T=0
} ;

/* dbl_HH_Adj:
 * Morse using BumWrap[4.5,3,1.5,.75,.25] for HCP and Ded to reflect H and HH in OPC
 * JGM1  using BUmWrap with Ded similar to standard Deductions
 * OPC Deductions: (Reference Only. Not used in this code)
 *             { -1, -1,-.5,  0,  0., -.5,  0, 0,  0., -1, -1,-.5,-1.5, -.5,  0,  0.0}
 */
/*                AK, AQ, AJ, AT, Ax,  KQ, KJ, KT, Kx,  QJ, QT, Qx,  JT, Jx, Tx,   xx */
float_t dbl_HH_adj[14][16] = {
   /* Bergen */ { -0, -0,-.5, -0, -0.,-.5,-.5, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Book gives -.5*/
   /* Bissell*/ { -0,-0, -0,  -0, -0.,  0,  0, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Bissell per PAV */
   /* DKP    */ { -0, -0, -0, -0, -0., -1, -0,  0, -0., -3, -3, -3., -2, -2, -0,  -0.0}, /* unguarded H; but only 1/3 of shwn */
   /* Goren  */ { -0, -0, -0, -0, -0., -0, -0, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Goren KQ? KJ? */
   /* JGM1   */ { -1, -1,-.5, -0, -0., -1, -0, -0, -0., -1, -1, -1., -1,-.75,-.25,-0.0}, /* Ded for BumWrap? 4.5,3,1.5,0.75,0.25 */
   /* Kaplan */ { -0, -0, -0, -0, -0., -0, -0, -0, -0., -1, -0, -0., -0, -0, -0,  -0.0}, /* Per the book. 1964*/
   /* Karpin */ { -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Per PAV web Page.  */
   /* KnR    */ { -0, -0, -0, -0, -0., -0, -0, -0, -0., -0, -0, -0., -0, -0, -0,  -0.0}, /* KnR Dont deduct for dblton Honors */
   /* LAR    */ { -0, -0, -0, -0, -0., -1, -0, -0, -0., -1, -0, -0., -0, -0, -0,  -0.0}, /* based on book */
   /* Morse  */ { -0, -.5,-.25,-0,-0., -0, -0, -0,-0.,-.25,-.25,-.5,-.5,-.75,-.25,-0.0}, /* reflects opc values for HH dblton*/
   /* PAV    */ { -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* KQ -1? KJ -1? K -2? */
   /* SHEINW */ { -0, -0, -0, -0, -0., -0, -0, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* KQ? KJ? assuming NO.*/
   /* ZARBAS */ { -0, -0, -1, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Based on 2005 PDF */
   /* ZARADV */ { -0, -0, -1, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Based on 2005 PDF */
} ;
//                  0          1    2    3     4      5       6      7    8    9     10     11      12      13      14        20    88    -1
//enum metric_ek { BERGEN=0,BISSEL,DKP,GOREN, JGM1, KAPLAN, KARPIN, KnR, LAR, MORSE, PAV, SHEINW, ZARBAS, ZARADV, metricEND,EXCP=20,SET=88,Quit=-1} ;

/* Dpts_vals is 'cooked' such that hcp + hcp_adj + Dpts_val gives the correct answer for the given metric.
 * does not always come out to +2 for a stiff and +1 for a dblton
 *
 * Looks like it comes out to just the usual 2 pts stiff, 1 pt dblton thanks to the deductions above.
*/
enum {GOREN_dpts_idx=0, PAV_dpts_idx, SHEINW_dpts_idx} ;
int Dpts_vals[3][22] = {
   //               A         x  AK     Ax  KQ    Kx  QJ  Qx JT  Tx,xx
   {  /* Goren   */ 2,2,2,2,2,2, 1,1,1,1,1, 1,1,1,1,  1,1,1, 1,1, 1,1 }, // Stiff K = 3 - 2 + 3 = 4
   {  /* Pavlicek*/ 2,2,2,2,2,2, 1,1,1,1,1, 1,1,1,1,  1,1,1, 1,1, 1,1 },
   {  /*Sheinwold*/ 2,2,2,2,2,2, 1,1,1,1,1, 1,1,1,1,  1,1,1, 1,1, 1,1 },
} ;

float_t lookup_adj_stiff_H(enum metric_ek m, int w1) { /* metric  and top card bitmask */
   float_t adj_pts;
   int adjx100 ;
   switch(w1) {  /* should really use CardAttr_RO[1][ACE] ... etc.  instead of 64 */
      case 64 : adj_pts = stiff_H_adj[m][0]  ; break ;  /* stiff A */
      case 32 : adj_pts = stiff_H_adj[m][1]  ; break ;  /* stiff K */
      case 16 : adj_pts = stiff_H_adj[m][2]  ; break ;  /* stiff Q */
      case 8  : adj_pts = stiff_H_adj[m][3]  ; break ;  /* stiff J */
      case 4  : adj_pts = stiff_H_adj[m][4]  ; break ;  /* stiff T */
      default : adj_pts = 0.0                ; break ;  /* stiff spot */
   } /* end switch(w1) */
   adjx100 = adj_pts * 100. ;
   JGMDPRT(8,"Adj for stiff; weight=%d, Metric=%d StiffAdjx100=%d\n", w1, m, adjx100 );
   return (adj_pts) ;
} /* end lookup adj stiff honor */

float_t lookup_adj_HH(enum metric_ek m, int w2) { /* metric and top two cards bit mask */
   float_t adj_pts;
   int adjx100 ;
   switch(w2) { /* should really use (CardAttr_RO[1][ACE] + CardAttr_RO[1][KING] )  here instead of 96*/
      case 96 : adj_pts = dbl_HH_adj[m][0]  ; break ; // AK 64 + 32
      case 80 : adj_pts = dbl_HH_adj[m][1]  ; break ; // AQ
      case 72 : adj_pts = dbl_HH_adj[m][2]  ; break ; // AJ
      case 68 : adj_pts = dbl_HH_adj[m][3]  ; break ; // AT
      case 65 : adj_pts = dbl_HH_adj[m][4]  ; break ; // Ax 64 + 1
      case 48 : adj_pts = dbl_HH_adj[m][5]  ; break ; // KQ 32 + 16
      case 40 : adj_pts = dbl_HH_adj[m][6]  ; break ; // KJ
      case 36 : adj_pts = dbl_HH_adj[m][7]  ; break ; // KT
      case 33 : adj_pts = dbl_HH_adj[m][8]  ; break ; // Kx
      case 24 : adj_pts = dbl_HH_adj[m][9]  ; break ; // QJ 16 + 8
      case 20 : adj_pts = dbl_HH_adj[m][10] ; break ; // QT
      case 17 : adj_pts = dbl_HH_adj[m][11] ; break ; // Qx
      case 12 : adj_pts = dbl_HH_adj[m][12] ; break ; // JT 8 + 4
      case  9 : adj_pts = dbl_HH_adj[m][13] ; break ; // Jx 8 + 1
      case  5 : adj_pts = dbl_HH_adj[m][14] ; break ; // Tx 4 + 1
      default : adj_pts = 0.0               ; break ; // xx
   } /* end switch(w2) */
   adjx100 = adj_pts * 100.0 ;
   JGMDPRT(8,"AdjShortHonors: metric=%d, HH weight=%d, adjx100=%d \n",m, w2, adjx100 );
   return (adj_pts) ;
} /* end lookup adj Two Honors Dblton */

float_t excp_adj_fhcp(HANDSTAT_k *phs, int suit, int tag ) { /* switch statement goes here */ return 0.0 ; }

/* shortHon_adj handles the cases where the HCP adjustments are in the table. Other cases will need new code */
float_t shortHon_adj (HANDSTAT_k *phs, int suit, int tag ) {
   float_t adj_pts ;   // usually negative.
   int w1, w2 ;
   if (tag > metricEND ) { adj_pts = excp_adj_fhcp(phs, suit, tag) ; return adj_pts; }
   assert(BERGEN <= tag && tag <= ZARFULL);
/*
 * The Query tags in alpha order: The adj_hcp arrays use these values to lookup adjustments. ~ means not coded yet.
 *                     0        1       2     3     4      5~      6      7    8    9     10     11       12      13      14        20          21
                    BERGEN=0, BISSEL,  DKP, GOREN, JGM1, KAPLAN, KARPIN, KnR, LAR, MORSE, PAV, SHEINW,  ZARBAS, ZARADV, metricEND, MixJGM=20, MixMOR,
// possibly add metrics in the 50 - 79 range to implement different hand factors like quicktricks, or quicklosers, or shortest suit etc.
                    SET=88, SYNTST=99, Quit=-1} ;
*/
   if (phs->hs_length[suit] >= 3 || 0 == phs->hs_length[suit] ) { return 0.0 ; }

   w1 = phs->topcards[suit][0] ;
   w2 = phs->topcards[suit][1] + w1 ;
   JGMDPRT(8,"Metric=%d, suit=%c, len=%d, w1=%d, w2=%d \n", tag, "CDHS"[suit], phs->hs_length[suit], w1, w2 ) ;

   if (phs->hs_length[suit] == 1 ) {
      adj_pts = lookup_adj_stiff_H( tag , w1 ) ;
      return (adj_pts) ;
   } /* end length == 1 */
   // Len must be two here
   adj_pts = lookup_adj_HH( tag , w2 ) ;
   return (adj_pts);
} /* end shortHon_adj */

//                             Work=0        BumWrap/OPC=1         C13/Aces=2    KnR=3          DKP=4         Andrews Fiths=5
// float_t hcp_val[6][5] = { {4,3,2,1,0}, {4.5,3.0,1.5,0.75,0.25}, {6,4,2,1,0}, {3,2,1,0,0} , {13,9,5,2,0}, {4.,2.8,1.8,1.,0.4 } };
float_t calc_alt_hcp (HANDSTAT_k *phs, int suit, int mtype ) {
   float alt_hcp = 0.0 ;
   float alt_syn = 0.0 ;
   int r, j ;
   /* Translate the metric type: Berg ..       Zar and Fut to the alt hcp table to use */
   int hcp_type[] = {0,0,4,0,1,0,0,3,0,  1,0,0,  0,0,  0,0,0,0,0,0,0} ;
   /*                  DKP  JGM   KNR  Morse     Zar    Future          */
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
/* Defaults when a placeholder needed for DoTrumpFit */
int Fn_ptsNONE( HANDSTAT_k *phs_dc, TRUMP_FIT_k trump ) { return 0 ; }
int DfitNONE(   HANDSTAT_k *phs_dc, TRUMP_FIT_k trump ) { return 0 ; }

float_t AdjDKP(HANDSTAT_k  *phs, int suit ) {
    /*     Note DKP adj are in effect 1/3 of what is calc here since final result is div by 3 */
    /*     Adj rules are simple: Any suit who's lowest card is higher than a Ten gets -1 adj
     *     This includes all stiff honors (stiff A incl) and all Dblton Honors (AK incl, down to QJ)
     *     Also any unguarded Q (QJ, QT, Qx) or Jack (JT(x) or Jx(x) ) gets -1 adj; but QJx is not adjusted
     */
      JGMDPRT(8,"AdjDKP, suit=%c, len=%d Top4=%d, QUEEN=%d, JACK=%d\n",
         "CDHS"[suit], phs->hs_length[suit], phs->hs_counts[idxTop4][suit] , phs->Has_card[suit][QUEEN], phs->Has_card[suit][JACK] ) ;
     if (phs->hs_counts[idxTop4][suit] == phs->hs_length[suit] && phs->hs_length[suit] > 0 ) { return (-1.0) ; } /* higher than T*/
     /* unguarded Queen. Note: KQ, AQ, QJ covered by previous statement. */
     if (phs->Has_card[suit][QUEEN] && phs->hs_length[suit] == 2 ) { return (-1.0) ; } /* QT, or Qx */
     /* test for unguarded Jack. i.e. No higher honor and length <= 3 */
     if (phs->Has_card[suit][JACK] && phs->hs_counts[idxTop4][suit] == 1 && phs->hs_length[suit] <= 3  ) { return (-1.0) ; }
     // fprintf(stderr, "!!!!!! AdjDKP reached NO adjustment \n");
     return (0.0) ; /* no adjustment */
} /* end adjDKP */

/* DfitBergen: Count for each shortness -- Voids = number of trumps */
int DfitBERG( HANDSTAT_k *phs, TRUMP_FIT_k trump ) {
   int du, ss_len, t_len, dfit ;
   du = 0; //* assume N or E are dummy */
   if (trump.dummy == seat[1] ) { du = 1 ; }
   t_len  = trump.tlen[du] ;
   if (t_len <= 2 ) { return 0 ; }
   JGMDPRT(7, "BERG Calc Dfit for du seat=%d:%c, du=%d, tlen=%d, and ss_len=%d\n",
               trump.dummy,compass[du], du, trump.tlen[du], trump.ss_len[du] );
   dfit = 0 ;
   for (int s=CLUBS; s<=SPADES; s++ ) { /* Count for EACH shortness as that seems to be the usual approach */
      if (s == trump.tsuit ) continue ;
      ss_len = phs->hs_length[s] ;
      if      ( 0 == ss_len ) { dfit += t_len ; }
      else if ( 1 == ss_len ) { dfit += (t_len == 3) ? 2 : 3 ; } /* 4+ trumps, stiff gets 3 pts */
      else if ( 2 == ss_len ) { dfit += 1 ; }
   }
   JGMDPRT(7, "DfitBERG Returning, Dummy=[%d:%c], Tlen[du]=%d, dfit_pts=%d\n",du, compass[du],t_len, dfit ) ;
   return dfit ;
} /* end DfitBERG */

/* Kaplan Dfit: given an 8+ fit add for ALL short suits 3/2/1. Add same to Opener, ALL shortsuits, 3/2/1 */
/* call this routine only if there is an 8 fit ; assume that if we are in a 6-2 fit 6 card suit still gets Dfit */
int DfitKAPLAN( HANDSTAT_k *phs, TRUMP_FIT_k trump ) { /* phs will point to Dummy's hand -- the one getting the Dfit pts */
   int  t_suit, t_len, s, s_len, dfit ;
   int DF_KAPLAN[4] =  {3,2,1,0}  ; /* Dfit pts with 3/4 trump for V/S/D/Longer */
   t_suit = trump.tsuit;
   t_len = phs->hs_length[t_suit] ;
   if (t_len <= 2 ) return 0 ;
   dfit = 0 ;
   for (s=CLUBS; s<=SPADES; s++ ) {  /* count for EACH shortness */
      if (s == t_suit) continue ;
      s_len = phs->hs_length[s];
      if (s_len < 3 ) {
         dfit += DF_KAPLAN[s_len];
      }
   }
   JGMDPRT(7,"DfitKAPLAN for compass=%c, t_len=%d, DfitTotal=%d\n","NESW"[phs->hs_seat],t_len,dfit );
   return dfit ;
} /* end DfitKAPLAN */
int DfitKARPIN( HANDSTAT_k *phs, TRUMP_FIT_k trump ) { /* phs will point to Dummy's hand -- the one getting the Dfit pts */
   int du, s_len, t_len, dfit ;
   int DF_KARPIN[2][4] = { {2,1,0,0},{3,2,1,0} } ; /* Dfit pts with 3/4 trump for V/S/D/Longer */
   int v0, s1, db ; /* Debug vars */
   du = 0; //* assume N or E are dummy */
   if (trump.dummy == seat[1] ) { du = 1 ; }
   t_len = trump.tlen[du];
   JGMDPRT(7, "KARPIN Calc Dfit for du seat=%d:%c, du=%d, tlen=%d, and ss_len=%d\n",
               trump.dummy,compass[du], du, trump.tlen[du], trump.ss_len[du] );
   v0 = 0 ; s1 = 0 ; db = 0 ;
   if (trump.tlen[du] <= 2 ) { return 0 ; }

   /* count for all short suits; that seems to have been standard back then */
   dfit = 0 ;
   for (int s=CLUBS; s<=SPADES; s++ ) {
      if (s == trump.tsuit) continue ;
      s_len = phs->hs_length[s];
      if (s_len < 3 ) {
         dfit += DF_KARPIN[du][s_len];
      }
   }
   JGMDPRT(7, "DfitKARPIN Return, Dummy=[%d], dfit=%d, from t_len[du]=%d: DBG:%d,%d,%d\n", du, dfit, t_len,v0,s1,db ) ;
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
/* A common way of counting support points; Any 8+ fit; 5/3/1 with 4+ suppt, 3/2/1 with 3+ suppt -- ONE short suit only */
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

    /*  Hf Points:
     *     *   Traditional way: 1..3 hcp in an 8+ trump fit (even stiff K vs 7 card suit) add +1
     *   Conservative way: Qx(x), Jx(x), or QJx in trump suit. Some also do Qx(x) in any 4+ Side suit.
     *   Bergen? way:     1..3 hcp in any suit with an 8+ fit even a 4-4; does not need to be the short hand. QJxxx vs AKx add +1
     *    Opc Method:     Same as Bergen but in addition Hx vs 5 (7 fit) add 1Hf pt for Jx,Qx,Kx in any of pard's 5+ suits.
     * With at least a doubleton and 1-3 hcp vs 5+ length in pards hand, add 1 Hf pt. e.g. Kxx vs 4+ or Jx vs 5+ etc.
     * Assume Bergen does not count Hf in NT, since no mention of them in 'starting points'
     */
int Hf_ptsBERG( HANDSTAT_k *phs[] , int Hf_hand ) { /* Fairly agressive; counts for a stiff K,Q,J vs 6+ suit */
   JGMDPRT(7,"Hf_ptsBERG, for short? hand # %d, seat=%c\n",Hf_hand, compass[Hf_hand]) ;
    /* Bergen Hf Points:  With 1-3 hcp in ALL 8+ fit suits,  add +1 Hf pts; max 2 Hf pts per hand */
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
int Hf_ptsKAPLAN ( HANDSTAT_k *p_hs , int h) { /*give any Stiff Q or J in pards 4+ suit full value instead of zero */
   int Hf_pts = 0;
   int h_pard = 0 ;
   int sh_cnt = 0 ;
   if ( 0 == h ) { h_pard = 1 ; }
   for (int s = CLUBS; s <= SPADES ; s++ ) {
      if (phs[h]->hs_length[s] == 1 && phs[h]->Has_card[s][QUEEN] && phs[h_pard]->hs_length[s] >= 4 ) {
         Hf_pts += 2 ;
         sh_cnt++ ;
      }
      if (phs[h]->hs_length[s] == 1 && phs[h]->Has_card[s][JACK] && phs[h_pard]->hs_length[s] >= 4 ) {
         Hf_pts += 1 ;
         sh_cnt++ ;
      }
   }
   JGMDPRT(7,"Hf_ptsKAPLAN Hand=%d, Short Honors=%d, Returning %d Tot_Hf pts \n",h,sh_cnt, Hf_pts ) ;
   return Hf_pts ;
}


inline int LptsSTD(HANDSTAT_k *phs, int suit ) { // such a simple function make it inline
   if (phs->hs_length[suit] > 4 ) {
            return (phs->hs_length[suit] - 4 ) ; // +1 for 5 card suit, +2 for six, +3 for 7 etc.
   }
   return 0 ;
}

int SynBERG(HANDSTAT_k *phs, int suit ) { /* Bergen 'Quality Suit' pts */
    JGMDPRT(8,"SynBERG, Suit=%c Len=%d, Top5=%d\n","CDHS"[suit],phs->hs_length[suit],phs->hs_counts[idxTop5][suit]) ;
      if ( (phs->hs_length[suit] >= 4 ) && (phs->hs_counts[idxTop5][suit] >= 3 ) ) { //  Bergen Quality +1 for 3/top5 in 4+ suit;
         return(1) ;
      }
      return(0) ;
}

int SynDKP(HANDSTAT_k *phs, int s ) {     /* Honor cards with A (incl K) or K -> +1 syn pt per card */
   /*  e.g. +1: AKx, KQx, KJx, +2: KQJx, AKQx, AQJx, 0: QJx,*/
    int synpts = 0 ;
    if (phs->Has_card[s][KING]  && phs->Has_card[s][ACE] )          { synpts += 1 ; } // King with the Ace
    if (phs->Has_card[s][QUEEN] && phs->hs_counts[idxTop2][s] > 0 ) { synpts += 1 ; } // Queen with Ace or King
    if (phs->Has_card[s][JACK]  && phs->hs_counts[idxTop2][s] > 0 ) { synpts += 1 ; } // Jack with Ace or King

    /* Tens with two 'companions' worth +2; with one 'companion' +1 (not T9 dblton) */
    if (phs->Has_card[s][TEN] ) {
       if ( (phs->hs_counts[idxTop4][s] >= 2 ) || (phs->hs_counts[idxTop4][s] == 1 && phs->Has_card[s][NINE]) ) {
          synpts += 2 ;
       }
       else if ( (phs->hs_counts[idxTop4][s] == 1)  || (phs->Has_card[s][NINE] && phs->hs_length[s] > 2 ) ) {
           synpts += 1 ;
       }
    } /* end Has_Ten */
    JGMDPRT(8,"SynDKP, SynPTS=%d, Suit=%c Len=%d, Top4=%d, HAS_Ten=%d, Has_Nine=%d \n",
         synpts, "CDHS"[s],phs->hs_length[s],phs->hs_counts[idxTop4][s], phs->Has_card[s][TEN], phs->Has_card[s][NINE]) ;
    return synpts ;
} /* end SynDKP */

/* Larsson Synergy. His own invention? */
/* if there are two suits (len = 3+) with two+ of top4 honors (one of which must be A or K), then +1; if 3 such suits, +2 */
int SynLAR(HANDSTAT_k *phs ) {
   int syn_cnt = 0 ;
   int syn_pts = 0 ;
   int suit ;
   for (suit = CLUBS; suit <=SPADES ; suit++ ) {
      if ( phs->hs_counts[idxTop2][suit] >= 1 && phs->hs_counts[idxTop4][suit] >= 2 && phs->hs_length[suit] >= 3 ) {
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

   /* Combo Pts Type=1: Void=2, Stiff=1, L = (length - 4) Gives: (4333=0, 4432=0, 5332 = 1 etc. for each suit. */
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

   /* Type=2 : longest + 2nd longest - shortest - 5 :: 4333 = -1 4432 = 1 ; 5332 = 1 etc. */
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

/* PAV: +1 for 5+ suit in Dummy if playing NT contract*/
int LptsPAV( HANDSTAT_k *phs[] )  {
   int s ;
   int NTdummy = 1;
   if ( phs[0]->hs_totalpoints <= phs[1]->hs_totalpoints ) { NTdummy = 0 ; }
   lpts[NTdummy] = 0 ;
   for (s = CLUBS; s <= SPADES ; s++ ) {
      if ( phs[NTdummy]->hs_length[s] >= 5 ) { lpts[NTdummy] = 1 ; }
   }
   JGMDPRT(7,"LptsPAV: NTDummy=%d, lpts[%d]=%d \n",NTdummy, NTdummy, lpts[NTdummy] );
   return NTdummy ;
}

int DptsPAV( HANDSTAT_k *p_hs, int s ) { /* Count for shortness; deduct the shortness pts later if there is a misfit. Count even short honors */
   int dpts ;
   int ss_idx ;
   int weight ;
   dpts = 0 ;
   if ( 0 == p_hs->hs_length[s] ) { JGMDPRT(8,"PAV Shortness pts returns 3 from Void \n");    return 3 ; }
   if ( 3 <= p_hs->hs_length[s] ) { JGMDPRT(8,"PAV Shortness pts returns 0 from 3+ suit \n"); return 0 ; }
   if ( 1 <= p_hs->hs_length[s] &&  p_hs->hs_length[s] <= 2 ) {
      weight = p_hs->topcards[s][0] + p_hs->topcards[s][1] ;
      ss_idx = ss_index(weight) ;
      dpts = Dpts_vals[PAV_dpts_idx][ss_idx] ;
      JGMDPRT(8,"PAV Shortness pts returns %d from short suit \n",dpts);
      return dpts ;
   }
   /* NOTREACHED */
   return dpts ;
} /* end Dpts_PAV */

int DfitPAV( HANDSTAT_k *phs_du, TRUMP_FIT_k trump ) { /* phs will point to Dummy's hand -- the one getting the Dfit pts */
   int du, ss_len, ss_cnt, t_len, DFit_pts, s ;
   int DF_PAV[4] =  {2,1,0,0} ; /* Dfit pts  added to orginal Dpts if dummy has 4+ Trump Nothing for Doubletons*/
   du = 0; //* assume N or E are dummy */
   if (trump.dummy == seat[1] ) { du = 1 ; }
   t_len = trump.tlen[du];
   JGMDPRT(7, "PAV Calc Dfit for du seat=%d:%c, du=%d, tlen=%d, and ss_len=%d\n",
               trump.dummy,compass[du], du, trump.tlen[du], trump.ss_len[du] );

   if (trump.tlen[du] <= 3 ) { return 0 ; }
   ss_cnt = 0 ;
   DFit_pts = 0 ;
  /* PAV with 4+ Trump counts Dfit for EACH shortness; but Dbltons dont get any extra, so would need two stiffs, or stiff and void */
   for (s=0 ; s<4 ; s++ ) {
      ss_len = phs_du->hs_length[s] ;
      if( ss_len < 3) { DFit_pts += DF_PAV[ ss_len ] ; ss_cnt++ ; }
   }
     JGMDPRT(7, "DfitPAV, Dummy=[%d], t_len[du]=%d, from sslen=%d, Tot_dfit=%d, sscnt=%d\n", du,t_len,ss_len,DFit_pts,ss_cnt);

   return DFit_pts ;
} /* end DfitPAV */

/* Fn Points PAV: With an 8+ fit Declarer adds pts for extra trump length, and for side suit length
 * Rule 1: Every trump over five +2 pts
 * Rule 2: with 9+ fit +1 for 5th trump
 * Rule 3: Every side suit (only 1 unless 5440, or 4441) +1 for each card over 3
 * Rule 4: With a 4-4 fit, -1 from EACH side suit value, i.e. every 4 card side suit=0, 5 cards=1, 6 cards=2 etc.
 */
int Fn_ptsPAV( HANDSTAT_k *phs_dc, TRUMP_FIT_k trump ) { /* phs will point to Declarer's hand -- the one getting the Fn pts */
   int  fn_pts ;
   int s ;
   int dc = 1; //* assume S or W are declarer */
   int tlen_pts, side_suit_pts;
   if (trump.decl == seat[0] ) { dc = 0 ; }
   JGMDPRT(7, "PAV FN_pts Declarer *handstat = %p, dc=%d, seat[dc]=%d, \n",(void *)phs_dc,dc,seat[dc] ) ;
   fn_pts  = 0 ; tlen_pts = 0 ; side_suit_pts = 0 ;
   if (trump.tlen[dc] > 5 ) { tlen_pts = (trump.tlen[dc]-5) * 2 ; } /* always +2 for each trump >5 even in 6-2, 7-1 etc. fit */
   if (trump.fit_len >= 9 ) { tlen_pts++ ; } /* if there is a 9 fit the 5th trump gets +1 5-4, 6-3, 7-2 etc. */
   for (s = CLUBS; s <=SPADES ; s++ ) {
         if (s != trump.tsuit && phs_dc->hs_length[s] > 3 ) {
            side_suit_pts += (phs_dc->hs_length[s] - 3) ; }
            if ( (trump.tlen[dc] == 4) && (side_suit_pts > 0) ) { /* must be a 4-4 fit if Decl (longest trump hand) has only 4*/
                  side_suit_pts-- ; /* if Decl has only 4 Trump, EVERY side suit downgraded by a pt */
   }
         JGMDPRT(8,"**** FN-SideSuit=%d,  SideSuitLen=%d, FN_SideSuitPts=%d\n",s,phs_dc->hs_length[s], side_suit_pts ) ;
   } /* end side suit loop */
   if ( (trump.tlen[dc] == 4) && (side_suit_pts > 0) ) { /* must be a 4-4 fit if Decl (longest trump hand) has only 4*/
      side_suit_pts-- ; /* subtract 1 from side suit pts in a 4-4 fit; 4 card side-suit=0, 9 card side-suit = 5 */
   }
   fn_pts = side_suit_pts + tlen_pts ;
   JGMDPRT(7,"PAV FN pts for hand=[%d] Tot=%d, Tlen=%d, side_suit=%d, fit_len=%d, Decl_Tlen=%d \n",
                                 dc, fn_pts,tlen_pts,side_suit_pts,trump.fit_len,trump.tlen[dc] );
   return fn_pts ;
} /* end Fn_ptsPAV */

int DptsGOREN( HANDSTAT_k *p_hs, int s ) { /* Count for shortness; deduct the shortness pts later if there is a misfit. Count even short honors */
   int dpts ;
   int ss_idx ;
   int weight ;
   dpts = 0 ;
   if ( 0 == p_hs->hs_length[s] ) { JGMDPRT(8,"GOR Shortness pts returns 3 from Void \n");    return 3 ; }
   if ( 3 <= p_hs->hs_length[s] ) { JGMDPRT(8,"GOR Shortness pts returns 0 from 3+ suit \n"); return 0 ; }
   if ( 1 <= p_hs->hs_length[s] &&  p_hs->hs_length[s] <= 2 ) {
      weight = p_hs->topcards[s][0] + p_hs->topcards[s][1] ;
      ss_idx = ss_index(weight) ;
      dpts = Dpts_vals[GOREN_dpts_idx][ss_idx] ;
      JGMDPRT(8,"GOR Shortness pts returns %d from short suit \n",dpts);
      return dpts ;
   }
     /* NOT REACHED */
   return dpts ;
} /* end Dpts_GOREN */

/* Extra Ruffing pts for shortness: 4 Trump: +2/+1/0 ; 3 Trump: +1/0/0 so void is worth 4 pts not 3. */
int DfitGOREN( HANDSTAT_k *phs_du, TRUMP_FIT_k trump ) { /* phs will point to Dummy's hand -- the one getting the Dfit pts */
   int du, t_len, DFit_pts, s, ss_cnt, ss_len;
   int DF_GOREN[4] =  {2,1,0,0} ;
   du = 0;  /* determine which hand is dummy */
   if (trump.dummy == seat[1] ) { du = 1 ; }
   t_len = trump.tlen[du];

   /* Goren with 4+ Trump counts Dfit for EACH shortness; but Dbltons dont get any extra, so would need two stiffs, or stiff and void */
   JGMDPRT(7, "GOR Calc Dfit for du seat=%d:%c, du=%d, tlen=%d, and ss_len=%d\n",
               trump.dummy,compass[du], du, trump.tlen[du], trump.ss_len[du] );

   if (trump.tlen[du] <= 3 ) { return 0 ; }  /* no extra Dfit with only 3 trump but keep +1 for EACH dblton */

   ss_cnt = 0 ;
   DFit_pts = 0 ;
   for (s=CLUBS; s<=SPADES; s++ ) {  /* count for EACH shortness */
      if ( s == trump.tsuit ) continue ;
      ss_len = phs_du->hs_length[s] ;
      if (ss_len < 2 ) { DFit_pts += DF_GOREN[phs_du->hs_length[s]] ; ss_cnt++ ;}
    }
    JGMDPRT(7, "DfitGOR, Dummy=[%d], t_len[du]=%d, from sslen=%d, Tot_DFit_pts=%d, sscnt=%d\n", du,t_len,ss_len,DFit_pts,ss_cnt);
   return DFit_pts ;
} /* end DfitGOREN */

/* Declarer adds pts for extra trump length, and for side suit length */
int Fn_ptsGOREN( HANDSTAT_k *phs_dc, TRUMP_FIT_k trump ) { /* phs will point to Declarer's hand -- the one getting the Fn pts */
   int  fn_pts ;
   int dc = 1; //* assume S or W are declarer */
   int tlen_pts;
   if (trump.decl == seat[0] ) { dc = 0 ; }
   JGMDPRT(7, "GOR FN_pts Declarer *handstat = %p, dc=%d, seat[dc]=%d, \n",(void *)phs_dc,dc,seat[dc] ) ;
   fn_pts  = 0 ; tlen_pts = 0 ;
   /* GOREN add for length in Declarer's hand once a trump fit is found: extra trump suit length, NO side suit length */

   /* Check for extra trump length: +1 if 5 */
   if ( trump.tlen[dc] >= 5 ) { tlen_pts++ ; }  /* +1 pt for the 5th trump always -- Differs from Pav */
   if (trump.tlen[dc] > 5 ) {tlen_pts += (trump.tlen[dc] - 5 )*2 ; }         /* +2 for each trump over 5; */
   fn_pts = tlen_pts;
   JGMDPRT(7,"Fn_ptsGOR Decl=[%d;%c] Fn_pts=%d Tlen=%d\n", dc,"NESW"[trump.decl],fn_pts,trump.tlen[dc] );
   return fn_pts ;
} /* end Fn_ptsGOREN */

int Hf_ptsGOREN (  HANDSTAT_k *p_hs , int tsuit) { /* p_hs points to dummy hand; the only one getting Hf pts */
   /* Goren Hf Points:  With 1-3 hcp in the trump suit, add +1 ; PAV says Dummy only */
   int Hf_pts = 0;
   if ( 0 < p_hs->hs_points[tsuit] &&  p_hs->hs_points[tsuit] <= 3 ) {Hf_pts = 1 ;}
   JGMDPRT(7,"Hf_ptsGOREN =hand=%d, suit=%d, hcp=%d, Hf_pts=%d\n",p_hs->hs_seat, tsuit,p_hs->hs_points[tsuit],Hf_pts ) ;
   return Hf_pts ;
}
/* give any Stiff Q or J in pards 4+ suit full value instead of zero */

int DptsSHEINW( HANDSTAT_k *phs, int s ) { /* Count for shortness; deduct the shortness pts later if there is a misfit. Count even short honors */
   int dpts ;
   int ss_idx ;
   int weight ;
   dpts = 0 ;
   if ( 0 == phs->hs_length[s] ) { JGMDPRT(7,"SHEINW Shortness pts returns 3 from Void \n");    return 3 ; }
   if ( 3 <= phs->hs_length[s] ) { JGMDPRT(7,"SHEINW Shortness pts returns 0 from 3+ suit \n"); return 0 ; }
   if ( 1 <= phs->hs_length[s] &&  phs->hs_length[s] <= 2 ) {
      weight = phs->topcards[s][0] + phs->topcards[s][1] ;
      ss_idx = ss_index(weight) ;
      dpts = Dpts_vals[SHEINW_dpts_idx][ss_idx] ;
      JGMDPRT(7,"SHEINW Shortness pts returns %d from short suit[%d] \n",dpts,s);
      return dpts ;
   }
     /* NOT REACHED */
   return dpts ;
} /* end Dpts_SHEINW */

int DfitSHEINW( HANDSTAT_k *phs_du, TRUMP_FIT_k trump ) { /* phs_du will point to Dummy's hand -- the one getting the Dfit pts */
   int du, t_len, DFit_pts, s, ss_cnt, ss_len;
   int DF_SHEINW[4] =  {5,3,1,0} ;     /* The values with 4 Trump. 3 Trump is one less, and 2 Trump is zero. */
   du = 0;  /* determine which hand is dummy */
   if (trump.dummy == seat[1] ) { du = 1 ; }
   t_len = trump.tlen[du];

   JGMDPRT(7, "SHEINW Calc Dfit for du seat=%d:%c, du=%d, tlen=%d, and all short suits\n",
               trump.dummy,compass[du], du, trump.tlen[du] );

   if (trump.tlen[du] <= 2 ) { return 0 ; }  /* this will replace any Dpts previously assigned */

   ss_cnt = 0 ;
   DFit_pts = 0 ;
   for (s=CLUBS; s<=SPADES; s++ ) { /* Dfit for EACH shortness */
      if ( s == trump.tsuit ) continue ;
      ss_len = phs_du->hs_length[s] ;
      if (ss_len <= 2 ) {
         DFit_pts += DF_SHEINW[ss_len] ; ss_cnt++ ; /* V + Stiff will get 8 Df pts with 4 trump*/
         if (t_len == 3 )  { DFit_pts-- ; } /*Subtract 1 if only 3 trumps; thus Void=4, Stiff=2, Dblton=0 V+Stiff= 6 still */
         JGMDPRT(7, "Dfit_SHEINW--Dummy=%d, suit=%d, ss_len=%d, DfitPts=%d, Tot_DFit_pts=%d, sscnt=%d\n",
                                 du, s, ss_len, DF_SHEINW[ss_len], DFit_pts, ss_cnt);
      }
    }
   JGMDPRT(7, "Total Dfit_SHEINW, Dummy=[%d], t_len[du]=%d, Tot_DFit_pts=%d, sscnt=%d\n", du,t_len,DFit_pts,ss_cnt);

   return DFit_pts ;
} /* end DfitSHEINW */

/* Declarer adds pts for extra trump length, and for side suit length */
int Fn_ptsSHEINW( HANDSTAT_k *phs_dc, HANDSTAT_k *phs_du, TRUMP_FIT_k trump ) {
   int  fn_pts = 0 ;
   int dc = 1; //* assume S or W are declarer */
   int tlen_pts = 0 ;
   int s;
   if (trump.decl == seat[0] ) { dc = 0 ;  }
   JGMDPRT(7, "SHEIN FN_pts Declarer *handstat = %p, trump.decl=%d, dc=%d, seat[0]=%d,seat[1]=%d \n",
                                       (void *)phs_dc,trump.decl, dc, seat[0], seat[1] ) ;
   fn_pts  = 0 ; tlen_pts = 0 ;
   /* SHEINW add for length in Declarer's hand once a trump fit is found: extra trump suit length, SOLID side suit length */

   /* Check for extra trump length: +1 if 5 */
   if ( trump.tlen[dc] >= 5 ) { tlen_pts++ ; }  /* +1 pt for the 5th trump always -- Differs from Pav */
   if ( trump.tlen[dc] >  5 ) {tlen_pts += (trump.tlen[dc] - 5 )*2 ; }         /* +2 for each trump over 5; */
   JGMDPRT(7,"Fn_ptsSHEINW for hand[%d] Trump suit[%d] Tlen=%d, Extra FN_pts=%d\n",dc,trump.tsuit,trump.tlen[dc],tlen_pts );
   for (s=CLUBS; s<=SPADES ; s++ ) {
      if (s != trump.tsuit && phs_dc->hs_counts[idxTop4][s] == 4 && phs_dc->hs_length[s] >= 5 ) { fn_pts++ ; } /* +1 for SOLID 5+ side suit */
      if (s != trump.tsuit && phs_dc->hs_length[s] >=5 && phs_du->hs_length[s] >= 3 ) {
          fn_pts += 1 + ( phs_dc->hs_length[s] - 5) * 2 ;
       } /* 1 + (L-5)*2 for ANY 5+ suit that has been raised. */
   JGMDPRT(7,"Fn_ptsSHEINW for hand[%d] Sidesuit[%d] Extra FN_pts=%d\n",dc,s,fn_pts );
   }
   fn_pts += tlen_pts ;
   JGMDPRT(7,"Tot Fn_pts SHEINW Decl=[%d;%c] Tot_Fn_pts=%d Tlen_pts=%d, Tlen=%d\n",
                                 dc,"NESW"[trump.decl],fn_pts,tlen_pts,trump.tlen[dc] );
   return fn_pts ;
} /* end Fn_ptsSHEINW */

int Hf_ptsSHEINW ( HANDSTAT_k *phs_du, HANDSTAT_k *phs_dc, int tsuit) {
   /* Sheinwold Hf: Either adds +1 for Qx(x), Jx(x), or QJx(x) in Tsuit; Dummy +1 also for Qx or QJ in side suit */
   int Hf_pts = 0;
   int Hf_side_suit = 0 ;
   int s ;
   //fprintf(stderr, "########################## HfPts_Sheinwold for Trump suit=%d ###################\n",tsuit) ;
   //fprintf(stderr, "phs_Decl->hs_seat=%d, phs_du->hs_seat=%d\n",phs_dc->hs_seat, phs_du->hs_seat );
   //fprintf(stderr, "Declarer Trump HCP=%d, Dummy Trump HCP=%d \n",phs_dc->hs_points[tsuit], phs_du->hs_points[tsuit]);

   /* This next bit adds 1 Hf pt for Q, J, or QJ in the trump suit by EITHER. Jxxx vs Qxxx BOTH get 1 Hf pt */
   if ( (phs_du->hs_points[tsuit] <= 3) && (phs_du->Has_card[tsuit][QUEEN] ||  phs_du->Has_card[tsuit][JACK]) ) {
      Hf_pts++ ;
      JGMDPRT(7,"Hf Point assigned to DUMMY    in Trump suit %d\n",tsuit);
   }
    if ( (phs_dc->hs_points[tsuit] <= 3) && (phs_dc->Has_card[tsuit][QUEEN] ||  phs_dc->Has_card[tsuit][JACK]) ) {
      Hf_pts++ ;
      JGMDPRT(7,"Hf Point assigned to DECLARER in Trump suit %d\n",tsuit);
   }
    /* Dummy can also add an Hf pt for the Q or QJ in Declarer's (4+ ?) side suit */
   for (s = CLUBS; s <= SPADES ; s++ ) {   /* +1 for Qx or QJ in pard's side suit */
 /*     fprintf(stderr, "Checking Side Suit=%d, DeclLen=%d, Dummy: Len=%d,HCP=%d,Top3=%d,HasQueen=%d\n",
                  s,  phs_dc->hs_length[s],phs_du->hs_length[s],phs_du->hs_points[s],phs_du->hs_counts[idxTop3][s],phs_du->Has_card[s][QUEEN] );
*/

      if ( (phs_dc->hs_length[s] >=4 ) && (s != tsuit) &&
           (phs_du->hs_counts[idxTop3][s] == 1) && phs_du->Has_card[s][QUEEN] && (phs_du->hs_length[s] >= 2) ) {
         Hf_side_suit++ ;
         JGMDPRT(7,"Hf Point assigned to Dummy in Side Suit %d\n",s);
      }
   }
   /* The Hf pts are all ascribed to 'dummy' even in the case when it is only Decl getting them e.g. Jxx vs AQxxx say
    * but the long hand is the declarer by the Pavlicek logic
    */
   JGMDPRT(7,"Hf_ptsSHEINW =Dummyhand=%d, Tsuit=%d, Tsuit_hcp=%d, Tot_Hf_pts=%d, Hf_ssPts=%d\n",
               phs_du->hs_seat, tsuit, phs_du->hs_points[tsuit], Hf_pts, Hf_side_suit ) ;
   return (Hf_pts + Hf_side_suit) ;
} /* end Hf_ptsSHEINW */


/* add a pt if all the HCP are in 2 or 3 suits similar to Larsson */
int SynZAR( HANDSTAT_k *p_hs ) {
   int syn_cnt = 0 ;
   int syn_pts = 0 ;
   int s ;
   for (s=CLUBS; s<=SPADES; s++ ) {
      if (p_hs->hs_points[s] > 0 ) syn_cnt++ ;
   }
   if (11 <= p_hs->hs_totalpoints && p_hs->hs_totalpoints <= 14 && syn_cnt <= 2 ) {syn_pts = 1 ; }
   else if ( p_hs->hs_totalpoints >= 14 && syn_cnt <= 3 ) {syn_pts = 1 ; }
   JGMDPRT(8,"ZarSyn:: TotPts=%d, syn_cnt=%d, syn_pts=%d\n",p_hs->hs_totalpoints,syn_cnt,syn_pts );
   return syn_pts ;
} /* end SynZAR */

int Fn_ptsZar(HANDSTAT_k *phs[], int t_suit) {
   /* add 1 pt for a second 8 fit, 2Pts for a second 9/up fit */
   /* Pav says +1 for second 9fit +2 for 2nd 10 fit; will that ever happen? */
   /* Zar says +3 HC points for a secondary 7+ fit; but that may be old info. JGM invented the following as a compromise */
   int MaxFit = 0 ;
   int fitpt = 0 ;
   int s, fitlen ;
   for (s=CLUBS; s<=SPADES; s++) {
      if( s == t_suit ) { continue ; }
      fitlen = phs[0]->hs_length[s] + phs[1]->hs_length[s] ;
      if (fitlen > MaxFit) {MaxFit = fitlen ; }
   }
   fitpt = (MaxFit > 8) ? 2 :
           (MaxFit ==8) ? 1 : 0 ;
   JGMDPRT(8,"%d Zar Fn Pts for 2nd fit of len=%d\n",fitpt, MaxFit);
   return fitpt ;
}
/* Hf Zar; Does not depend on trump suit or who is dummy; any suit with a 7+ fit, and both hands can have Hf Pts */
int Hf_ptsZar( HANDSTAT_k *phs[] ) {
   /* Either hand (but not both in same suit) can add +1 for each honor,incl Ten, (max of 2)
    * in a suit where partner has shown 4+ cards.
    * Max of two such suits per hand.
    * In this code I only add Hf pts to the shorter hand; e.g. QJTxxx facing AKxx only 2 Hf pts total, not 4.
    * In a 4-4 Fit I add Hf only to the hand with the FEWER Tops or HCP. AKJx facing QTxx only Q, T count = 2Hf pts.
    * a holding like Jx which starts out as being worth zero (1 - 1 dwngrade) now becomes worth +2 if pard shows 4+ there.
    * See page 84 ex2 in pdf where Jx facing AKxx becomes 1+1=2, instead of 1 - 1 = 0 (so Stiff J facing xxxx becomes 2pts!)
    * reverse the downgrade and add +1 for an honor in pards long suit(s)
    * Pavlicek website is much more conservative with the Hf points only Dummy in the trump suit.
    */
   int suit_idx[2][4] = { {0,1,2,3}, {0,1,2,3} } ;
   int top5[2] = {0};
   int s , h_Hf;
   float_t temp;

   /* Track the Hf pts by seat so that we know if our hand has improved enough for an invite etc. */
   /* Use the globals hf_pts[2] and hf_pts_suit[2][4] */

   for (s = CLUBS; s <= SPADES ; s++ ) {
      if ( (4 > phs[0]->hs_length[s]) && (4 > phs[1]->hs_length[s]) )  { continue ; } /* bypass suits where Neither has 4+ */
      top5[0] = (phs[0]->hs_counts[idxTop5][s] < 2 ) ? phs[0]->hs_counts[idxTop5][s] : 2 ; /* count max of two honors per suit */
      top5[1] = (phs[1]->hs_counts[idxTop5][s] < 2 ) ? phs[1]->hs_counts[idxTop5][s] : 2 ;
      /* Assign Hf only to the hand with the shorter suit. */
      if      ( phs[0]->hs_length[s] > phs[1]->hs_length[s] ) {  h_Hf = 1 ; }
      else if ( phs[0]->hs_length[s] < phs[1]->hs_length[s] ) {  h_Hf = 0 ; }
      /* Both hands equal length.(e.g 4=4 fit) Assign Hf to hand with FEWER honors */
      else if ( phs[0]->hs_counts[idxTop5][s] > phs[1]->hs_counts[idxTop5][s] ) { h_Hf = 1 ;}
      else if ( phs[0]->hs_counts[idxTop5][s] < phs[1]->hs_counts[idxTop5][s] ) { h_Hf = 0 ;}
      /* Both hands equal Length, equal honors e.g. Axxx vs Kxxx - Give Hf to hand with Weaker suit */
      else if ( phs[0]->hs_points[s] > phs[1]->hs_points[s] ) { h_Hf = 1 ;}
      else    { h_Hf = 0 ;}
      JGMDPRT(9,"Suit=%c, FitLen=%d, Top5[0]=%d, Top5[1]=%d, h_Hf=%d\n","CDHS"[s],(phs[0]->hs_length[s] + phs[1]->hs_length[s]),
                                     top5[0],top5[1],h_Hf) ;
      /* we have Honor(s) facing a 4+ suit This code even counts Stiff J facing xxxx since the PDF implies this is the case */
      if (phs[h_Hf]->hs_length[s] < 3 ) { temp = shortHon_adj(phs[h_Hf], s, ZARBAS) ;} /* we need to reverse the deduction we previously made if any */
      hf_pts_suit[h_Hf][s] = top5[h_Hf] - temp ;  /* top5 <= 2 here; temp is negative */
      JGMDPRT(9,"%d Hf Points assigned to hand=%d in suit =%d reversing %g deduction\n",hf_pts_suit[h_Hf][s], h_Hf, s,temp);
   }
   qsort(hf_pts_suit[0], 4, sizeof(int), desc_cmpxy ) ;
   qsort(hf_pts_suit[1], 4, sizeof(int), desc_cmpxy ) ;
   insertionSort(4,hf_pts_suit[0], suit_idx[0]);            /* sort the HfPts for hand 0 in Desc order We need the sorted suit_ids also*/
   hf_pts[0] = hf_pts_suit[0][0] + hf_pts_suit[0][1] ;      /* keep only the two suits with the highest Hf */
   insertionSort(4,hf_pts_suit[1], suit_idx[1]);            /* sort the HfPts for hand 1 in Desc order */
   hf_pts[1] = hf_pts_suit[1][0] + hf_pts_suit[1][1]  ;     /* keep only the two suits with the highest Hf */
   JGMDPRT(9,"Zar Hf for hand[0]=%d, for hand[1]=%d\n", hf_pts[0], hf_pts[1] ) ;
   return (hf_pts[0] + hf_pts[1]) ;
} /* end Hf_ptsZar */

/* Dfit = ( ExtraTrumps ) * ( 3 - shortestSuitLength) :: Both hands can do this */
int DfitZAR( HANDSTAT_k *phs[], SIDE_FIT_k *sf)  {
   /* Use the global dfit_pts[2] */
   int du, dc;
   int excess[2] ;
   dc = sf->decl_h;
   du = sf->dummy_h;
   excess[dc] = sf->t_len[dc] - 5 ; if (excess[dc] < 0 ) excess[dc] = 0 ;
   excess[du] = sf->t_len[du] - 3 ; if (excess[du] < 0 ) excess[du] = 0 ;
   if(sf->t_fitlen > 8 ) {
      dfit_pts[du] = excess[du] * (3 - sf->sorted_slen[du][3] ) ; /* 3 - len of shortest suit */
      dfit_pts[dc] = excess[dc] * (3 - sf->sorted_slen[dc][3] ) ;
   }
   else if ( sf->t_fitlen == 8 ) { /* Even if Dummy has the minimum trump holding can still get ruffing value */
      /* A 6-2 fit Declarer does not get any Dfit because Partner would not raise. */
      if ( (sf->t_len[du] >= 3) && ((sf->t_len[du] - sf->sorted_slen[dc][3] ) >= 2 ) ) {
         dfit_pts[du] = (sf->t_len[du] - sf->sorted_slen[dc][3] ) ;
      }
   } /* end 8 fit */
   JGMDPRT(9,"Dfitzar:: fitlen=%d, decl=%d, Du=%d, XS_dc=%d, XS_du=%d, SSlenDC=%d, SSlenDU=%d, DfitDC=%d, DfitDU=%d\n",
            sf->t_fitlen,dc,du,excess[dc],excess[du],sf->sorted_slen[dc][3],sf->sorted_slen[du][3],dfit_pts[dc],dfit_pts[du] );
   return (dfit_pts[0] + dfit_pts[1] ) ;
} /* end DfitZAR */

struct FitPoints_st Do_Trump_fit(
         HANDSTAT_k *phs[2],
         int (*calc_dfval)(HANDSTAT_k *p_hs, TRUMP_FIT_k trump),
         int (*calc_fnval)(HANDSTAT_k *p_hs, TRUMP_FIT_k trump) )
   /*
    * Check if there is an 8+ fit. If there is call the metric specific function to
    * a) calculate the Dummy support points, and
    * b) calculate any other points that Declarer might get in the case of a trump fit. (Called Fn_pts )
    * The Reference material specifying the metric does not usually address these two issues:
    * i) how to handle a 4-4 (or 5-5) fit.
    * ii) how many short suits is dummy allowed to count for ?
    * This code assumes (i) that if the trumps are equal in both hands (4-4 or 5-5) then BOTH hands count dummy points
    * and (ii) Dummy (or both) can count for ONLY ONE short suit. So if Dummy is 6=4=2=1 with 4 trump it counts only for the stiff
    * When the metric DOES specify then Declarer is the hand with the longest trumps, or if equal, the hand with the most HCP.
    * Some metrics call the Dfit and Fnpts functions directly when these assumptions not true.
   */
{  // ---- Do Trump fit ----
   HANDSTAT_k  *phs_dummy, *phs_decl ;
   TRUMP_FIT_k trump2 ;
   struct FitPoints_st TFpts = { {0,0}, {0,0} } ;
   trump = trump_fit_chk(phs) ; /* fill the structure describing the trump fit */
   if(seat[0] == trump.dummy) {h_dummy = 0 ; h_decl = 1 ;  }
   else                       {h_dummy = 1 ; h_decl = 0 ;  }
   if ( trump.fit_len < 8 ) { return TFpts ; } /* if no 8-fit return zeroes This routine ignores 5-2 fits */

    /*
     * if there is a 8+ trump fit, calc Dfit and Fn Points.
     * What kind of Df and Fn will depend on which func ptrs are passed in
     */
      phs_dummy = phs[h_dummy] ;   // could also do phs_dummy = &p_dldat->hs[dummy]
      phs_decl  = phs[h_decl]  ;
      JGMDPRT(7,"DO_TRUMP_FIT: h_dummy=%c, h_decl=%c trump.dummy=%d, Seat[0] = %d\n",compass[h_dummy], compass[h_decl], trump.dummy, seat[0] );
      if (trump.tlen[h_dummy] < trump.tlen[h_decl]) {   // trump_fit_chk() func will ensure tlen[du] < tlen[dc] UNLESS they are equal
         TFpts.df_val[h_dummy]   = (*calc_dfval) (phs_dummy, trump ) ;  /* call Dfit routine; returns single value */
         TFpts.fn_val[h_decl]    = (*calc_fnval) (phs_decl,  trump ) ;  /* call Fn function; returns single value */
         JGMDPRT(7, ": dfit for dummy[%c]=%d, FN for Decl[%c]=%d\n",
            compass[h_dummy],TFpts.df_val[h_dummy],compass[h_decl],TFpts.fn_val[h_decl] );
      }
      else if (trump.tlen[h_dummy] == trump.tlen[h_decl]) {  /* probably a 4=4 fit; assign Dfit to both hands, and no Fn  */
         TFpts.df_val[h_dummy]   = (*calc_dfval) (phs_dummy, trump ) ;
         /* now call dfit again swapping the dummy and decl fields */
         trump2 = trump ;  /* trump is a global used by others. make our own copy to modify */
         trump2.decl = trump.dummy ;
         trump2.dummy = trump.decl ;
         // JGMDPRT(7, "Do_Trump_Fit: Calling swapped Dfit with trump2; trump2.decl=%d, trump2.dummy=%d\n",trump2.decl, trump2.dummy );
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
} /* end Do Trump Fit  */
/* end Do_Trump_Fit */







