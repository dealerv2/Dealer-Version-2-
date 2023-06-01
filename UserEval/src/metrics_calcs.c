/* File metrics_calcs.c   */
/* Date      Version  Author  Description
* 2022/11/25 1.0.0    JGM     The high level functions called from the Server main to do the hand evaluation.
* 2022/12/20 1.0.1    JGM     Added extra misc output fields to UEV struct for possible debugging.
* 2023/01/20 1.1      JGM     Several Metrics working. Addd two flavors of mixed metrics.
* 2023/02/07 2.0      JGM     Most interesting Metrics working. Redid KnR to put results in std locations.
* 2023/03/31 2.1      JGM     Mods to set88, Added metrics Bissell, (Goren, Zar, .. )
* 2023/05/24 2.2      JGM     Added ZarBasic and ZarFull metrics.
*/
/*
 * This file implements the higher level overall flow of a metric. It will often call routines from factors_subs.c
 * to implement the lower level calculations. The factors functions have names like:Fn_ptsBERG, DFIT_Berg etc.
 */
#include "../include/UserEval_types.h"
#include "../include/UserEval_externs.h"
#include "../include/UserEval_protos.h"
#include "../include/dbgprt_macros.h"

 /* The metric Tag numbers were originally assigned in roughly alpha order.
 * Later add ons will just be added to the end. There is no reason to be rigid about the alpha order thing.
 */

/* Bergen. From Book Points Schmoints and lots of iNet resources. Long suits, Adjust-3 and many other factors */
int bergen_calc( int side ) {    /* Tag Number: 0 */
   int berg_NTpts[2]     = {0}; /* Hand Value in NT: Honors, Length, Body, Syn/Qual etc.*/
   int berg_pts[2] = {0};       /* Hand Value in Suit: NT above + Dfit + Fn + Hf ... etc.*/
   int h, s , adj3_cnt;
   int slen ;
   HANDSTAT_k *p_hs ;

   prolog( side ) ;  /* zero globals, set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   /*
    * Bergen: hand[0] incl ADJ-3 to get starting points. Then hand[1] ditto.
    *         then Dfit, Hf, Fn_pts and misfit corrections for the pair; then final total
    */
   JGMDPRT( 7 , "++++++++++ berg_calc prolog done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) { /* for each hand in the side */
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      for (s = CLUBS ; s<= SPADES ; s++ ) {
         slen = p_hs->hs_length[s] ;
         if (0 == slen ) {
            fhcp_suit[h][s]  = 0.0 ;
            fhcp_adj_suit[h][s] = 0.0 ;
         }
         else if ( slen <= 2 ) {
         fhcp_suit[h][s]  = p_hs->hs_points[s]  ;
         fhcp_adj_suit[h][s] = shortHon_adj(p_hs, s, BERGEN ) ; /* Deduct in a std way in case we play the hand in NT */
         }
         else { /* no shortness pts; straight HCP */
            fhcp_suit[h][s]  = p_hs->hs_points[s] ;
            fhcp_adj_suit[h][s] = 0.0 ;
         }
         syn_pts_suit[h][s] = SynBERG(p_hs, s) ;  // Quality suit 3 of top5 in 4+ suit = +1; applies to all suits not just trump
         /* simpler to do inline than call Lpts_std  'Pts Schmoints' book does not say if Len or short method for opener. */
         if (p_hs->hs_length[s] > 4 ) {
            lpts_suit[h][s] = (p_hs->hs_length[s] - 4 ) ; // +1 for 5 card suit, +2 for six, +3 for 7 etc.
            JGMDPRT(8,"L_ptsBERG, suit=%d, len=%d, Lpts_suit=%d\n", s, p_hs->hs_length[s], lpts_suit[h][s] );
         }
         fhcp[h]     += fhcp_suit[h][s]    ;
         fhcp_adj[h] += fhcp_adj_suit[h][s];
         syn_pts[h]  += syn_pts_suit[h][s] ;
         lpts[h]     += lpts_suit[h][s]    ;
         JGMDPRT(8,"bergen_calc, Hand=%d, suit=%d, SuitLen=%d, fhcp[h][s]=%g, fhcp-adj[h][s]=%g, Lpts[h][s]=%d, Synpts=%d \n",
                     h, s, p_hs->hs_length[s], fhcp_suit[h][s], fhcp_adj_suit[h][s], lpts_suit[h][s],syn_pts_suit[h][s] );
      } /* end for s = CLUBS to SPADES*/

      if (p_hs->square_hand) { hand_adj[h] = -1 ;}
      else                   { hand_adj[h] =  0 ;}
      JGMDPRT(8,"SqBERG, seat[%c] hand_adj=%d\n", compass[h], hand_adj[h] );
      adj3_cnt = ( p_hs->hs_totalcounts[idxTens]   + p_hs->hs_totalcounts[idxAces]
                  -p_hs->hs_totalcounts[idxQueens] - p_hs->hs_totalcounts[idxJacks] ) ;
      if      (adj3_cnt <= -6 )  { body_pts[h] += -2 ; } // Downgrade
      else if (adj3_cnt <= -3 )  { body_pts[h] += -1 ; } // Downgrade
      else if (adj3_cnt >=  6 )  { body_pts[h] += +2 ; } // Upgrade
      else if (adj3_cnt >=  3 )  { body_pts[h] += +1 ; } // Upgrade
      berg_NTpts[h] = roundf(fhcp[h]+fhcp_adj[h]) + syn_pts[h] + lpts[h] + body_pts[h] + hand_adj[h] ; /* starting points for hand h */
      JGMDPRT(8,"Adj3 BERG, seat[%c] adj3_cnt=%d, body_pts=%d, HCP_Tot=%d\n", compass[h], adj3_cnt, body_pts[h], berg_NTpts[h] );
      UEv.nt_pts_seat[h] = berg_NTpts[h] ;
   } /* end for each hand */
   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
   JGMDPRT(7,"Berg Both Hands Start/NT pts done. pts[0]=%d, pts[1]=%d, UEv_Side_pts=%d\n", berg_NTpts[0],berg_NTpts[1], UEv.nt_pts_side );

   /* now do the stuff for the side in a suit contract: Hf, Dfit and Fn Mainly.
    * Berg does not do Waste or Mirror
    * But if there is a misfit he throws out any Lpts assigned
    */
      hf_pts[0] = Hf_ptsBERG( phs , 0 ) ; /* Hand 0 is the one getting Hf pts for help in pards long suit. */
      hf_pts[1] = Hf_ptsBERG( phs , 1 ) ;
      berg_pts[0] = berg_NTpts[0] + hf_pts[0];
      berg_pts[1] = berg_NTpts[1] + hf_pts[1];
      JGMDPRT(7,"Berg: Suit: Bpts[0]=%d, Hfpts[0]=%d, Bpts[1]=%d, Hfpts[1]=%d\n",berg_pts[0],hf_pts[0],berg_pts[1],hf_pts[1] );

   /* Bergen Dfit; No dfit with trumps <= 2 ; with 3: 3/2/1 ; with 4: 4/3/1 ; with 5: 5/3/1 */
      TFpts = Do_Trump_fit(phs, DfitBERG, Fn_ptsBERG) ; /* will also set the globals dfit_pts[] and Fn_pts[] */
      dfit_pts[0] = TFpts.df_val[0] ; dfit_pts[1] = TFpts.df_val[1];
      Fn_pts[0]   = TFpts.fn_val[0] ; Fn_pts[1]   = TFpts.fn_val[1];
      JGMDPRT(7,"Berg:Df[0]=%d,Fn[0]=%d, Df[1]=%d,Fn[1]=%d\n",dfit_pts[0],Fn_pts[0],dfit_pts[1],Fn_pts[1] ) ;
      berg_pts[0] += dfit_pts[0] + Fn_pts[0] ;
      berg_pts[1] += dfit_pts[1] + Fn_pts[1] ;
      UEv.hldf_pts_seat[0] = berg_pts[0] ;
      UEv.hldf_pts_seat[1] = berg_pts[1] ;
      UEv.hldf_pts_side = berg_pts[0] + berg_pts[1] ;
      JGMDPRT(7,"Berg HLDF Totals for side Before Misfit Check=%d,%d,%d\n",
                                    UEv.hldf_pts_side,UEv.hldf_pts_seat[0],UEv.hldf_pts_seat[1] );
  /* Bergen: Dont count L pts if there is a misfit;
   * Maybe he means don't count Lpts for a L suit facing shortness, keep other Lpts ??
   */
  /* check for misfits in each suit  This may matter if we play in NT even if there is an 8+ fit */
  /* We assume Bergen considers shortness vs 4+ suit a misfit; vs a 3crd suit not a misfit... */
     for (s=CLUBS ; s<=SPADES; s++ ) {
        misfit[s] = misfit_chk(phs, s) ;
        if (  misfit[s].mf_type > 3 ) { misfit_cnt = 1 ; } /* len pts only assigned for 4+ so waste wont matter */
     }
     if (0 < misfit_cnt ) { /* there is at least one misfit Subtract any Length pts previously assigned to the NT pts*/
        UEv.nt_pts_seat[0] -= lpts[0] ;
        UEv.nt_pts_seat[1] -= lpts[1] ;
        UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
        JGMDPRT(7, "Berg MISFIT calcs Done removed lpts %d, %d\n", lpts[0], lpts[1]);
        /* no further adj for wastage vs shortness,  or for shortness vs len*/
      }
// N:(hcpadj,Lpt,Body,Syn,Dpt,Dfit,Fn,hand_adj); S:(hcpadj,Lpt,Body,Syn,Dpt,Dfit,Fn,hand_adj)
   UEv.misc_count = 0 ;
      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[0];
      UEv.misc_pts[UEv.misc_count++] = lpts[0];       /* +1 for each card over 5, in all suits. */
      UEv.misc_pts[UEv.misc_count++] = body_pts[0];   /* adjust-3 values */
      UEv.misc_pts[UEv.misc_count++] = syn_pts[0];  /* Quality Suit; 3 of top 5 */
      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dpts[0];       /* distribution aka shortness pts before a fit is found */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, N-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      UEv.misc_pts[UEv.misc_count++] = hf_pts[0];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[0];  /* Square hand -- either or both */

      /* ditto hand 1 */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = dpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hf_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[1];
      // N:(hcpadj,Lpt,Body,Syn);(Dpt,Dfit,Fn,hand_adj); S:(hcpadj,Lpt,Body,Syn);(Dpt,Dfit,Fn,hand_adj)
  /* now put the results into the user result area at p_uservals */
  JGMDPRT(7, "Berg Calcs complete. Saving %d values at userval address %p \n", (6+UEv.misc_count), (void *)p_uservals );
  fsync(2);
  SaveUserVals( UEv , p_uservals ) ;
  return ( 6 + UEv.misc_count ) ; /*number of results calculated */
} /* end bergen_calc */

/* Bissell hand evaluation per the Bridge Encyclopedia and Pavlicek WebSite Unique. Nice way of Evaluating Honor cards
 * Bissell never mentions any sort of support points or Dfit or shortness pts of any kind.
 * Honors (incl T) are worth: (3 pts Minus the number of higher honors missing).
 * So JTx is worth zero(missing A,K,Q), but QJT is worth 1+1+1 = 3;(Missing only A and K)
 * 4 card suits are worth +1; 5 card or longer suits are worth 3*(L - 4)
 * Deductions for short honors are a bit from the iNet and a bit from JGM. Try to reflect that AJ is worth more than Ax e.g.
 */
int bissell_calc( int side ) {   /* Tag Number: 1 */

   int h, s, Length ;
   HANDSTAT_k *p_hs;

   int biss_pts[2] ;
   float_t biss_totpts[2], biss_shape_ded[2]; ;
   float_t biss_honpts[2][4],biss_adj[2][4],biss_lenpts[2][4],biss_suitpts[2][4]; /* need to track gross and net by suit */
   float_t honor_pts = 0 ;
   int missing_honors = 0 ;
   int len4_cnt = 0 ;
   int void_cnt = 0 ;
   prolog( side ) ;  /*zero globals,  set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT(7 , "++++++++++ Bissel_calc prolog  done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;

   for (h = 0 ; h < 2 ; h++) {         /* for each hand */
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      int seat = p_hs->hs_seat;
      len4_cnt = 0 ;
      void_cnt = 0 ;
      biss_totpts[h] = 0.0 ;
      for (s = SUIT_CLUB; s <= SUIT_SPADE; ++s) {
         biss_honpts[h][s]=0;
         biss_adj[h][s]=0;
         biss_lenpts[h][s]=0;
         biss_suitpts[h][s]=0; /* need to track gross and net by suit */
         honor_pts = 0 ;
         missing_honors = 0 ;

         Length = p_hs->hs_length[s];
         if ( Length == 4 ) len4_cnt++ ;
         if ( Length == 0 ) void_cnt++ ;
         JGMDPRT(8,"BISS for seat=%d, suit=%d, length=%d \n", seat, s, Length) ;

         int HasAce   = p_hs->Has_card[s][ACE] ;
         int HasKing  = p_hs->Has_card[s][KING];
         int HasQueen = p_hs->Has_card[s][QUEEN];
         int HasJack  = p_hs->Has_card[s][JACK];
         int HasTen   = p_hs->Has_card[s][TEN];

         JGMDPRT(8,"A=%d,K=%d,Q=%d,J=%d,T=%d,\n",HasAce,HasKing,HasQueen,HasJack,HasTen ) ;

    /* Honors Points. Each honor (incl T) is worth 3 - number of missing higher honors. */
         if (HasAce)   {  honor_pts += 3; }
         else          { missing_honors++ ; }
         JGMDPRT(8,"Has_Ace=%c honor_pts=%g, missing_honors=%d \n","NY"[HasAce],honor_pts,missing_honors ) ;
         if (HasKing) {  honor_pts += (3 > missing_honors) ? (3 - missing_honors) : 0 ; }
         else { missing_honors++ ; }
         JGMDPRT(8,"Has_King=%c honor_pts=%g, missing_honors=%d \n","NY"[HasKing],honor_pts,missing_honors ) ;
         if (HasQueen) {  honor_pts += (3 > missing_honors) ? (3 - missing_honors) : 0 ; }
         else { missing_honors++ ; }
         JGMDPRT(8,"Has_Queen=%c honor_pts=%g, missing_honors=%d \n","NY"[HasQueen],honor_pts,missing_honors ) ;
         if (HasJack) {  honor_pts += (3 > missing_honors) ? (3 - missing_honors) : 0 ; }
         else { missing_honors++ ; }
         JGMDPRT(8,"Has_Jack=%c honor_pts=%g, missing_honors=%d \n","NY"[HasJack],honor_pts,missing_honors ) ;
         if (HasTen ) { honor_pts += (3 > missing_honors) ? (3 - missing_honors) : 0 ; }
         else { missing_honors++ ; }
         JGMDPRT(8,"Has_Ten=%c honor_pts=%g, missing_honors=%d \n","NY"[HasTen],honor_pts,missing_honors ) ;
         biss_honpts[h][s] = honor_pts;
         biss_adj[h][s]    = shortHon_adj(p_hs, s, BISSEL) ;
         biss_lenpts[h][s] = (Length <= 3 ) ? 0.0 :
                             (Length == 4 ) ? 1.0 : 3.0*(Length - 4) ; /* 3 pts for every card over 4 */
         biss_suitpts[h][s] = biss_honpts[h][s] + biss_adj[h][s] + biss_lenpts[h][s] ;
         if (biss_suitpts[h][s] > 3.0 * Length ) { biss_suitpts[h][s] = 3.0*Length ; }
         JGMDPRT(8, "BISS Suit=%c, SuitTot=%g, Lenpts=%g, HonPts=%g, AdjPts=%g\n",
               "CDHS"[s],biss_suitpts[h][s],biss_lenpts[h][s],biss_honpts[h][s],biss_adj[h][s] ) ;
         biss_totpts[h] += biss_suitpts[h][s] ;
      } /* end for suit */
      if ( p_hs->square_hand || (len4_cnt == 3)       /* 4333 or 4441 */
        || ( (len4_cnt == 2) && (void_cnt == 0) ) ) {
         biss_shape_ded[h] = -1.0; /* or 44xy not 4450  therefore 4432 */
         biss_totpts[h] += -1.0 ;
      }
      biss_pts[h] = roundf(biss_totpts[h]); /* starting points for hand h */
      UEv.nt_pts_seat[h] = biss_pts[h] ;
      JGMDPRT(8, "Biss Hand[%d] UEv.nt=%d, biss_tot=%g \n",h, UEv.nt_pts_seat[h], biss_totpts[h] );
   } /* end for each hand */
   /*SaveUserEvals always puts these next values into the first six slots. Bissel does not have any HLDF but keep the std. */
    UEv.nt_pts_side = biss_pts[0] + biss_pts[1] ;
    UEv.nt_pts_seat[0] = biss_pts[0] ;
    UEv.nt_pts_seat[1] = biss_pts[1] ;
    UEv.hldf_pts_side = biss_pts[0] + biss_pts[1] ;
    UEv.hldf_pts_seat[0] = biss_pts[0] ;
    UEv.hldf_pts_seat[1] = biss_pts[1] ;

    UEv.misc_count = 0 ;

      /* Bissell pts breakdown; same in NT and in suit */
      for (h= 0 ; h < 2 ; h++ ) {
         for (s = SUIT_CLUB; s <= SUIT_SPADE; ++s) {
            UEv.misc_pts[UEv.misc_count++] = roundf(biss_suitpts[h][s] ) ;
         }
      }
      UEv.misc_pts[UEv.misc_count++] = biss_shape_ded[0];
      UEv.misc_pts[UEv.misc_count++] = biss_shape_ded[1];

      SaveUserVals( UEv , p_uservals ) ;
      JGMDPRT(7, "Bissell calcs Done UEV MIsc-count=%d\n", UEv.misc_count);
  return ( 6 + UEv.misc_count ) ;
/* Deductions for stiffs A(3-0=3), K(2-1 = 1) Q(1-1 = 0) ; Since stiff J and stiff T = 0 no deduct for themc
 * Deductions for HH     AK(6-1=5), AQ(5-1=4), AJ(4-.5=3.5), KQ(4-1 = 3), KJ(3-.5=2.5),QJ(2-.5=1.5),Qx(1-.5=.5)
*/
} /* end bissell_calc */

/* Danny Kleinman's "Little Jack Points" per his NoTrump Zone book.
 * He does not give any method for trump fit support points so go with a vanilla approach
 */
int dkp_calc( int side ) {       /* Tag Number: 2 */
   int dkp_pts[2] = {0, 0};
   int syn_pts[2] = {0, 0};
   float_t dkp_hcp[2][4] = { {0.0}, {0.0} };
   int h, s ;
   float_t adj_suit;
   int syn_suit ;
   HANDSTAT_k *p_hs;
   int temp = 0 ;

   prolog( side ) ;  /*zero globals,  set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT(7 , "++++++++++ DKP_calc prolog done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      for (s = CLUBS ; s<= SPADES ; s++ ) {
         dkp_hcp[h][s] = calc_alt_hcp(p_hs, s, DKP) ; /* calc DKP LJP points for this suit. */
         fhcp[h] += dkp_hcp[h][s] ;
         adj_suit = AdjDKP(p_hs, s ) ; /* -1 for any suit whose lowest card is higher than a Ten incl A, AK, AKQJ & unguarded Qx Jxx */
         fhcp_adj[h] += adj_suit;
         syn_suit = SynDKP(p_hs, s) ; /* add for K if with Ace, and Q or J if with higher honor and Tens with companions */
         syn_pts[h] += syn_suit;
         JGMDPRT(8,"DKP_Hidx=%d, suit=%c, dkp_hcp[s]=%g, adj[s]=%g,syn[s]=%d Tot_Raw_fhcp[h]=%g, Tot_fhcp_adj=%g, Tot_syn=%d, SuitLen=%d\n",
                     h, s, dkp_hcp[h][s], adj_suit, syn_suit, fhcp[h], fhcp_adj[h], syn_pts[h], p_hs->hs_length[s] );

/* DK does not say if he adds pts for length or not. But he is OldFashioned so assume +1 for each card > 4 max of 3 Lpts*/
         if (p_hs->hs_length[s] > 4 ) {
            temp = (p_hs->hs_length[s] - 4 ) ; // +1 for five carder, +2 for 6, +3 for longer. etc.
            if (temp > 3 ) { lpts[h] += 3 ; }
            else           { lpts[h] += temp; }
            JGMDPRT(8,"L_ptsDKP, Hand=%d, suit=%d, len=%d, Tot_Lpts=%d\n", h, s, p_hs->hs_length[s], lpts[h] );
          }
      } /* end for s = CLUBS to SPADES*/
      hcp_adj[h] = fhcp_adj[h]; /* just for debugging in the UE misc values */
      if (p_hs->square_hand) { hand_adj[h] += -2 ; }
      dkp_pts[h] =  roundf( (fhcp[h] + fhcp_adj[h]  + syn_pts[h] + hand_adj[h] )/3.0 ) ; /* hcp and synpts are div by 3 */
      dkp_pts[h] += lpts[h];      /* lpts are not div by 3 */
      JGMDPRT(7,"-------DKP for hidx=%d DONE. dkp_pts=%d, lpts=%d, hand_adj=%d, synpts=%d, fhcp=%g, fhcp_adj=%g \n",
               h, dkp_pts[h],lpts[h],hand_adj[h], syn_pts[h], fhcp[h], fhcp_adj[h] ) ;
      UEv.nt_pts_seat[h] = dkp_pts[h] ;
   } /* end for each hand */
   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
   JGMDPRT(7,"DKP NT pts done. pts[0]=%d, pts[1]=%d, UEv_Side_pts=%d\n", dkp_pts[0],dkp_pts[1], UEv.nt_pts_side );

   /* Done both hands -- Now check for a trump fit
    *  Assume DK is 'Standard' 531 with 4 trump, 321 with 3 trump. A 4-4 fit then both hands count Dfit
    */
    TFpts = Do_Trump_fit(phs, DfitSTD,  Fn_ptsNONE) ; /* will also set the globals dfit_pts[] and Fn_pts[] */

   dkp_pts[0] += dfit_pts[0]  ;
   dkp_pts[1] += dfit_pts[1]  ;
   UEv.hldf_pts_seat[0] = dkp_pts[0] ;
   UEv.hldf_pts_seat[1] = dkp_pts[1] ;
   UEv.hldf_pts_side = dkp_pts[0] + dkp_pts[1] ;
   UEv.misc_count = 0 ;
      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[0];
      UEv.misc_pts[UEv.misc_count++] = lpts[0];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[0];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[0];   /* 4333 deduction */
      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* DKP Fn_ptsMONE */
      /* ditto Hand 1 */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];

  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
     JGMDPRT(7, "DKP fit calcs Done; Tsuit=%d, Decl=%c, HLDFpts[%d : %d], Fn_pts[%d:%d], Dfit_pts=[%d:%d]\n",
         trump.tsuit,"NSEW"[seat[h_decl]], dkp_pts[0],dkp_pts[1], Fn_pts[0],Fn_pts[1],dfit_pts[0],dfit_pts[1] );
  return ( 6 + UEv.misc_count ) ;
} /* end dkp_calc */

/* Goren Calc. Shortness based Method, -- Dpts, Dfit_pts per Pavlicek, FnPts and HfPts per iNet */
int goren_calc( int side ) {     /* Tag number 3 */
   int gor_NTpts[2]     = {0};
   int gor_pts[2]       = {0};
   int h, s , body_cnt, slen;
   HANDSTAT_k *p_hs, *phs_dummy, *phs_decl ;

   prolog( side ) ;  /* zero globals, set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT( 7 , "++++++++++ gor_calc prolog done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   JGMDPRT( 7 ,"Check Globals Init fhcp_adj[0]=%g, gor_pts[0]=%d, fhcp_adj_suit[1][3]=%g \n",
                                   fhcp_adj[0],gor_pts[0],fhcp_adj_suit[1][3] );
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */

      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      for (s = CLUBS ; s<= SPADES ; s++ ) {
         slen = p_hs->hs_length[s] ;
         if (0 == slen ) {
            fhcp_suit[h][s]  = 0.0 ;
            fhcp_adj_suit[h][s] = 0.0 ;
            dpts_suit[h][s] = 3 ;
         }
         else if ( slen <= 2 ) {
            fhcp_suit[h][s]  = p_hs->hs_points[s]  ;
            fhcp_adj_suit[h][s] = shortHon_adj(p_hs, s, GOREN ) ; /* Deduct in a std way in case we play the hand in NT */
            dpts_suit[h][s] = DptsGOREN( p_hs, s ) ;
         }
         else { /* no shortness pts; straight HCP */
            fhcp_suit[h][s]  = p_hs->hs_points[s] ;
            fhcp_adj_suit[h][s] = 0.0 ;
            dpts_suit[h][s] = 0 ;
         }
         /*  end hcp and dpts calcs */
         fhcp[h] += fhcp_suit[h][s] ;
         fhcp_adj[h] += fhcp_adj_suit[h][s] ;
         dpts[h] += dpts_suit[h][s] ;
         JGMDPRT(8,"goren_calc, Hand=%d, suit=%d, SuitLen=%d, fhcp[h][s]=%g, fhcp-adj[h][s]=%g, Dpts[h][s]=%d \n",
                     h, s, p_hs->hs_length[s], fhcp_suit[h][s], fhcp_adj_suit[h][s], dpts_suit[h][s] );
      } /* end CLUBS <= s <= SPADES */
      /* Check for Aces or lack of */
      body_cnt = p_hs->hs_totalcounts[idxAces] ;
      if (body_cnt == 4 )      {body_pts[h] =  1 ; }
      else if (body_cnt == 0 ) {body_pts[h] = -1 ; }
      else                     {body_pts[h] =  0 ; }
      JGMDPRT(7,"goren_calc, Hand=%d, fhcp[h]=%g, fhcp-adj[h]=%g, Dpts[h]=%d, body_pts[h]=%d, Aces_cnt=%d\n",
                     h, fhcp[h], fhcp_adj[h], dpts[h],body_pts[h], body_cnt );
      /* In NT Do not count Dpts; so we just need the HCP + ShortHonor_ADJ + body */

      if (p_hs->square_hand == 1 ) { hand_adj[h] = -1 ; } /* Square hand Ded applies to NT and to dummy in suit contract */
      gor_NTpts[h] = roundf(fhcp[h] + fhcp_adj[h]) + body_pts[h] + hand_adj[h]  ; /* NT points for hand h -- dont count Dpts for NT */
      JGMDPRT(7,"GOR NT Result: Hand=%d, NT_pts=%d, fhcp=%g, fhcp_adj=%g, body_pts=%d, body_cnt=%d, Sq=%d \n",
                     h,  gor_NTpts[h],fhcp[h],fhcp_adj[h], body_pts[h],  body_cnt , hand_adj[h]);
      UEv.nt_pts_seat[h] = gor_NTpts[h] ;

      /* get Prelim value for hand in suit contract; will be affected by any later discovered misfits; also by Dfit_pts */
      /* in a suit contract we add the Dpts to the NT pts. The short Hon Ded are chosen so this comes out right */
      gor_pts[h] = roundf(fhcp[h] + fhcp_adj[h]) + body_pts[h] ; /* No Square Hand Deduction yet; Only Dummy in a suit contract */
      JGMDPRT(7,"GOR Suit Prelim Val[%d]=%d :: fhcp=%g fhcp_adj=%g dpts=%d body_pts=%d, body_cnt=%d\n",
                           h, gor_pts[h], fhcp[h], fhcp_adj[h], dpts[h], body_pts[h],  body_cnt);
   }
   /*---------------------- end for each hand --------------------------- */

   /* Begin Evaluating the side as a whole; look for fit, assign Dfit, Fn, Hf, Syn, etc */
   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;

   JGMDPRT(7,"GOR NT pts done.UEv_Side_pts=%d, UEv[0]=%d, UEv[1]=%d\n",  UEv.nt_pts_side, UEv.nt_pts_seat[0], UEv.nt_pts_seat[1] );\
   JGMDPRT(7,"Prelim gor_pts=[%d,%d], Dpts=[%d,%d] \n", gor_pts[0],gor_pts[1],dpts[0],dpts[1] );

  trump = trump_fit_chk(phs) ; /* fill the structure describing the trump fit */

   if ( trump.fit_len >= 8) {   /* Fit-len might 7 here which means a 5-2 fit */

        /* if there is a trump fit, Add Dfit and Fn Points to the Honors and Dpts already included  */
        /* Assume that Declarer and Dummy are defined as per PAV. Decl=Longest trumps or if equal the hand with the most HCP */
        /* If it's a 4-4 fit (equal len trumps), only Dummy gets Dfit, Declarer gets Fn (side suit points) */
        /* The Dfit and Fn stuff is from a teacher's post on BridgeWinners */
        /* Dummy does not count for Length, keeps its Dpts (incl +1 each Dblton) and adds for EACH stiff/void with 4+ trump
         */
      if(seat[0] == trump.dummy) {h_dummy = 0 ; h_decl = 1 ;  }
      else                       {h_dummy = 1 ; h_decl = 0 ;  }
      phs_dummy = phs[h_dummy] ;
      phs_decl  = phs[h_decl]  ;
      dfit_pts[h_dummy] = DfitGOREN  (phs_dummy, trump ) ; /* 4+T:2/1/0 These are in addition to Dpts awarded ealier */
      Fn_pts[h_decl]    = Fn_ptsGOREN(phs_decl,  trump ) ; /* 5th Trump +1, 6th & up +2 each.) */
      hand_adj[h_decl] = 0 ; /* hand_adj set ealier for both hands. But in a suit contract only Dummy gets the Ded if any */
      JGMDPRT(7, "GOR: dfit for dummy[%d]=%d, FN for Decl[%d]=%d\n",h_dummy,dfit_pts[h_dummy],h_decl,Fn_pts[h_decl]);

      /* Goren also counts Hf Pts for honors <= 3 hcp in the trump suit. */
      hf_pts[h_dummy] = Hf_ptsGOREN( phs_dummy , trump.tsuit ) ;
      gor_pts[h_dummy] += ( dfit_pts[h_dummy] + hand_adj[h_dummy] + hf_pts[h_dummy] );
      gor_pts[h_decl]  += Fn_pts[h_decl];
      JGMDPRT(7,"GOR TrumpFit Deal=%d, in suit=%d Len:%d Decl=[%d], Fn=%d, Hldf=%d Dummy=[%d], Dfit=%d Hf=%d, SQ=%d, Hldf=%d\n",
            gen_num,trump.tsuit,trump.tlen[0]+trump.tlen[1],h_decl,Fn_pts[h_decl],gor_pts[h_decl],
            h_dummy,dfit_pts[h_dummy],hf_pts[h_dummy], hand_adj[h_dummy],gor_pts[h_dummy] );
   } /* End Trump fit gor_pts all done for this case */

    /* Since there is no fit, Check for misfits which cannot be ignored. */
   else {
      /* If there is shortness vs pards long (4+) suit, and there is no trump fit, do not count for the shortness
       * We subtract ALL shortness pts, not just those in the misfit suit. Different from PAV
       * This may be wrong if we end up in a suit contract anyway in a 5-2 or 4-3 fit.
       */
      for (s=CLUBS; s<=SPADES; s++ ) {
         misfit[s] = misfit_chk(phs , s) ; /* return results in a struct */
         if (misfit[s].mf_type > 3) {  /* Misfit; Subtract ALL dpts not just the Dpts in the suit. */
            gor_pts[0] -= dpts[0]; gor_pts[1] -= dpts[1] ;
               JGMDPRT(7,"GOR MISFIT Deal=%d, in suit=%d Type:%d sh_len=%d, gor_pts[0]=%d, gor_pts[1]=%d\n",
                  gen_num,s,misfit[s].mf_type,misfit[s].sh_len,gor_pts[0], gor_pts[1] );
            break ; /* exit suit loop as soon as one misfit found */
         }
      } /* end for each suit */
   } /* end else check for misfit */
   JGMDPRT(7,"GOREN Side Fit/Misfit Eval Done  Deal=%d,  End Result for Suit Play: gor_pts[0]=%d, gor_pts[1]=%d\n",
                     gen_num, gor_pts[0], gor_pts[1] );
   /* gor_pts[] now done; [Dpts + (hcp+hcp_adj)] + body + Dfit + Fn + hand_adj */
   UEv.hldf_pts_seat[0] = gor_pts[0];
   UEv.hldf_pts_seat[1] = gor_pts[1];
   UEv.hldf_pts_side = UEv.hldf_pts_seat[0] + UEv.hldf_pts_seat[1] ;
   JGMDPRT(7,"GOREN HLDF pts done.UEv_HLDFSide_pts=%d, UEv_HLDF[0]=%d, UEv_HLDF[1]=%d\n",
            UEv.hldf_pts_side, UEv.hldf_pts_seat[0], UEv.hldf_pts_seat[1]);
/* now some debugging fields */
   UEv.misc_count = 0 ;
      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = roundf(fhcp_adj[0]);
      UEv.misc_pts[UEv.misc_count++] = body_pts[0];      /* +1 for 4 Aces, -1 for No Aces 0 otherwise */
      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dpts[0];       /* distribution aka shortness pts before a fit is found */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      UEv.misc_pts[UEv.misc_count++] = hf_pts[0];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[0];  /* Square Hand Deduction */
      /* Ditto for hand 1 */
      UEv.misc_pts[UEv.misc_count++] = roundf(fhcp_adj[1]);
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = dpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hf_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[1];
      SaveUserVals( UEv , p_uservals ) ;
      JGMDPRT(7,"GOR UEv misc_count=%d,[h0]=%d,%d,%d,%d,%d,%d,%d\n", UEv.misc_count, UEv.misc_pts[0],UEv.misc_pts[1],
                  UEv.misc_pts[2],UEv.misc_pts[3],UEv.misc_pts[4], UEv.misc_pts[5],UEv.misc_pts[6]);
      JGMDPRT(7,"GOR UEv misc_count=%d,[h1]=%d,%d,%d,%d,%d,%d,%d\n", UEv.misc_count, UEv.misc_pts[7],UEv.misc_pts[8],
                  UEv.misc_pts[9], UEv.misc_pts[10],UEv.misc_pts[11],UEv.misc_pts[12],UEv.misc_pts[13] );
  return ( 6 + UEv.misc_count ) ;
} /* end gor_calc */

int jgm1_calc( int side ) {      /*Tag Number: 4 Karpin method with BumWrap points (4.5/3/1.5/0.75/0.25)*/
   int jgm1_pts[2] = {0} ;
   float_t jgm_hcp[2][4] = { {0.0}, {0.0} } ;
   int h, s ;
   HANDSTAT_k *p_hs;

   prolog( side ) ;  /*zero globals,  set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT(7 , "++++++++++ JGM1_calc done prolog side=%d compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      for (s = CLUBS ; s<= SPADES ; s++ ) { /* I think I should total the adjustments, and THEN round them. use fhcp_adj[2] */
         jgm_hcp[h][s] = calc_alt_hcp(p_hs, s, JGM1) ; /* calc BumWrap points for this suit. */
         fhcp[h] += jgm_hcp[h][s] ;
         fhcp_adj[h] += shortHon_adj(p_hs, s, JGM1 ) ; /* -1 for K, Q, J, KQ, QJ, */
         JGMDPRT(8,"JGM1-Karp hcp_adj, Hidx=%d, suit=%d, hcp[s]=%g, Tot_Raw_fhcp[h]=%g, Tot_fhcp_adj=%g, SuitLen=%d\n",
                     h, s, jgm_hcp[h][s], fhcp[h], fhcp_adj[h], p_hs->hs_length[s] );

         /* simpler to do inline than call Lpts_std */
         if (p_hs->hs_length[s] > 4 ) {
            lpts[h] += (p_hs->hs_length[s] - 4 ) ; // +1 for 5 card suit, +2 for six, +3 for 7 etc.
            JGMDPRT(8,"L_pts_JGM_karp, Hand=%d, suit=%d, len=%d, Tot_Lpts=%d\n", h, s, p_hs->hs_length[s], lpts[h] );
          }
      } /* end for s = CLUBS to SPADES*/
      hcp_adj[h] = roundf( fhcp_adj[h] ) ;
      hcp[h] = roundf( fhcp[h] ) ;
      jgm1_pts[h] = roundf(fhcp[h] + fhcp_adj[h]) + lpts[h] ; /* NT pts for hand h omit + body_pts[h] */
      JGMDPRT(7,"JGM1-Karp hand=%d,jgm1_pts_net=%d, fhcp=%g, fhcp_adj=%g,   Lpts=%d,\n",
                  h, jgm1_pts[h],fhcp[h],fhcp_adj[h], lpts[h] );
      UEv.nt_pts_seat[h] = jgm1_pts[h] ;
   } /* end for each hand */
   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
   JGMDPRT(7,"JGM1Karp NT pts done. pts[0]=%d, pts[1]=%d, UEv_Side_pts=%d\n", jgm1_pts[0],jgm1_pts[1], UEv.nt_pts_side );

   /* Done both hands -- Now check for a trump fit */
   TFpts = Do_Trump_fit(phs, DfitKARPIN, Fn_ptsKARPIN) ; /* will also set the globals dfit_pts[] and Fn_pts[] */

   jgm1_pts[0] += dfit_pts[0] + Fn_pts[0] ;
   jgm1_pts[1] += dfit_pts[1] + Fn_pts[1] ;
   UEv.hldf_pts_seat[0] = jgm1_pts[0] ;
   UEv.hldf_pts_seat[1] = jgm1_pts[1] ;
   UEv.hldf_pts_side = jgm1_pts[0] + jgm1_pts[1] ;
   UEv.misc_count = 0 ;
/* now some debugging fields Standard set even tho some don't apply to this metric */

      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[0];
      UEv.misc_pts[UEv.misc_count++] = lpts[0];
      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      /* Ditto for hand 1 */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
     JGMDPRT(6, "JGM1_Karp fit calcs Done; Tsuit=%d, Decl=%c, HLDFpts[%d : %d], Fn_pts[%d:%d], Dfit_pts=[%d:%d]\n",
         trump.tsuit,"NSEW"[seat[h_decl]], jgm1_pts[0],jgm1_pts[1], Fn_pts[0],Fn_pts[1],dfit_pts[0],dfit_pts[1] );
  return ( 6 + UEv.misc_count ) ;
} /* end jgm1_karp_calc */

/* Kaplan per the Book Winning Contract Bridge Complete. 1960. Counts for Long suits and shortness once a fit is found */
int kaplan_calc (int side ) {    /* Tag Number: 5 */
   int kaplan_pts[2] = {0} ;
   int h, s ;
   HANDSTAT_k *p_hs;

   prolog( side ) ;  /*zero globals,  set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT( 7 , "++++++++++ Kaplan_calc prolog done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      for (s = CLUBS ; s<= SPADES ; s++ ) {
         fhcp_suit[h][s] =  p_hs->hs_points[s] ;
         fhcp_adj_suit[h][s] = shortHon_adj(p_hs, s, KAPLAN ) ; /* Stiff Q, J = 0; (unless pard bids the suit) QJ, Qx=2, Jx=1 */
         JGMDPRT(8,"KAPLAN hcp-adj, Hand=%d, suit=%d, hcp_suit[s]=%d, fhcp_adj_suit=%g\n",
                              h, s, p_hs->hs_points[s],  fhcp_adj_suit[h][s] );

         /* simpler to do inline than call function */
         if (p_hs->hs_length[s] > 4 ) {
            lpts_suit[h][s] = (p_hs->hs_length[s] - 4 ) ; // +1 for each card over 4 in each suit.
         }
         lpts[h] += lpts_suit[h][s] ;
         fhcp_adj[h] += fhcp_adj_suit[h][s] ;
         fhcp[h] += fhcp_suit[h][s] ;
         JGMDPRT(8,"L_ptsKAPLAN, Hand=%d, suit=%d, len=%d, lpts_suit=%d,RunTot_Lpts=%d, RunTotHCP=%g, RunTotAdj=%g\n ",
                      h, s, p_hs->hs_length[s], lpts_suit[h][s],lpts[h],fhcp[h], fhcp_adj[h]);
      } /* end for s = CLUBS to SPADES*/
      hcp_adj[h] = roundf( fhcp_adj[h] ) ;
      hcp[h] = roundf( fhcp[h] ) ;
      hf_pts[h] = Hf_ptsKAPLAN(p_hs, h ) ; /* give any Stiff Q or J in pards 4+ suit full value instead of zero */
      kaplan_pts[h] = hcp[h] + hcp_adj[h] + lpts[h] + hf_pts[h]; /* NT pts for hand h */
      JGMDPRT(7,"KAPLAN hand=%d,kaplan_pts_net=%d, hcp=%d, hcp_adj=%d, Lpts=%d, Hf_pts=%d\n",
                  h, kaplan_pts[h],hcp[h],hcp_adj[h],lpts[h], hf_pts[h] );
      UEv.nt_pts_seat[h] = kaplan_pts[h] ;
   } /* end for each hand */

   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
   JGMDPRT(7,"KAPLAN NT pts done. pts[0]=%d, pts[1]=%d, UEv_Side_pts=%d\n", kaplan_pts[0],kaplan_pts[1], UEv.nt_pts_side );

   /* Done both hands -- Now check for a trump fit */
  trump = trump_fit_chk(phs) ; /* fill the structure describing the trump fit */

   if (trump.fit_len >= 8 ) {
      if(seat[0] == trump.dummy) {h_dummy = 0 ; h_decl = 1 ;  }
      else                       {h_dummy = 1 ; h_decl = 0 ;  }
      dfit_pts[0] = DfitKAPLAN(phs[0], trump) ;
      kaplan_pts[0] += dfit_pts[0] ;
      dfit_pts[1] = DfitKAPLAN(phs[1], trump) ;
      kaplan_pts[1] += dfit_pts[1] ;
   }

   UEv.hldf_pts_seat[0] = kaplan_pts[0] ;
   UEv.hldf_pts_seat[1] = kaplan_pts[1] ;
   UEv.hldf_pts_side = kaplan_pts[0] + kaplan_pts[1] ;
   UEv.misc_count = 0 ;
      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[0];
      UEv.misc_pts[UEv.misc_count++] = lpts[0];
      UEv.misc_pts[UEv.misc_count++] = hf_pts[0];
      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */

       /* ditto hand 1 */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = hf_pts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];

  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
     JGMDPRT(7, "Kaplan fit calcs Done; Tsuit=%d, HLDFpts[%d : %d], Dfit_pts=[%d:%d]\n",
         trump.tsuit, kaplan_pts[0],kaplan_pts[1],dfit_pts[0],dfit_pts[1] );
  return ( 6 + UEv.misc_count ) ;
} /* end kaplan_calc */

/*  Karpin per Pavlicek web site Counts for Long suits, and shortness once a fit is found */
int karpin_calc( int side ) {    /* Tag Number: 6 */
   int karpin_pts[2] ;
   int h, s ;
   HANDSTAT_k *p_hs;

   prolog( side ) ;  /*zero globals,  set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT( 7 , "++++++++++ Karpin_calc prolog  done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      hcp[h] = p_hs->hs_totalpoints ; fhcp[h] = hcp[h];

      for (s = CLUBS ; s<= SPADES ; s++ ) { /* I think I should total the adjustments, and THEN round them. use fhcp_adj[2] */
         fhcp_adj[h] += shortHon_adj(p_hs, s, KARPIN ) ; /* -1 for K, Q, J, QJ, Qx, Jx. */
         JGMDPRT(8,"KARPIN hcp_adj, Hand=%d, suit=%d, hcp[s]=%d, Tot_fhcp_adj=%g SuitLen=%d\n",
                     h, s, p_hs->hs_points[s], fhcp_adj[h], p_hs->hs_length[s] );

         /* simpler to do inline than call Lpts_std */
         if (p_hs->hs_length[s] > 4 ) {
            lpts[h] += (p_hs->hs_length[s] - 4 ) ; // +1 for 5 card suit, +2 for six, +3 for 7 etc.
            JGMDPRT(8,"L_ptsKarpin, Hand=%d, suit=%d, len=%d, Tot_Lpts=%d\n", h, s, p_hs->hs_length[s], lpts[h] );
          }
      } /* end for s = CLUBS to SPADES*/
      hcp_adj[h] = roundf( fhcp_adj[h] ) ;
      karpin_pts[h] = lpts[h] + hcp_adj[h] + hcp[h] ; /* starting points for hand h */
      UEv.nt_pts_seat[h] = karpin_pts[h] ;
   } /* end for each hand */
   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
   JGMDPRT(7,"Karpin NT pts done. pts[0]=%d, pts[1]=%d, UEv_Side_pts=%d\n", karpin_pts[0],karpin_pts[1], UEv.nt_pts_side );

   /* Done both hands -- Now check for a trump fit */
   TFpts = Do_Trump_fit(phs, DfitKARPIN, Fn_ptsKARPIN) ; /* will also set the globals dfit_pts[] and Fn_pts[] */

   karpin_pts[0] += dfit_pts[0] + Fn_pts[0] ;
   karpin_pts[1] += dfit_pts[1] + Fn_pts[1] ;
   UEv.hldf_pts_seat[0] = karpin_pts[0] ;
   UEv.hldf_pts_seat[1] = karpin_pts[1] ;
   UEv.hldf_pts_side = karpin_pts[0] + karpin_pts[1] ;
   UEv.misc_count = 0 ;
/* now some debugging fields Standard set even tho some don't apply to this metric */

      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[0];
      UEv.misc_pts[UEv.misc_count++] = lpts[0];
      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      /* ditto Hand 1 */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];

  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
     JGMDPRT(7, "Karpin fit calcs Done; Tsuit=%d, Decl=%c, HLDFpts[%d : %d], Fn_pts[%d:%d], Dfit_pts=[%d:%d]\n",
         trump.tsuit,"NSEW"[seat[h_decl]], karpin_pts[0],karpin_pts[1], Fn_pts[0],Fn_pts[1],dfit_pts[0],dfit_pts[1] );
  return ( 6 + UEv.misc_count ) ;
} /* end karpin_calc */

/* The Four C's KnR algorithm, including the extra points for Fits and Support, and the deductions for misfits
 * These latter ones are specified in the article but have never been coded before
 * Probably because most programs do not evaluate two hands together but only separately then add them up.
 * But of course no bridge player actually works that way.
 * Plain vanilla CCCC is implemented in Dealer so is not needed here.
 */
int knr_calc ( int side ) {      /* Tag Number 7 */
   struct misfit_st KnR_misfit ; /* Misfits are on a per suit basis */
   struct KnR_points_st  KNR_pts[2] ;
   int h, trump_fit;
   HANDSTAT_k *p_hs;

   prolog( side ) ;  /*zero globals,  set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT( 7 , "++++++++++ KnR_calc done prolog for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) {         /* for each hand count up the KnR pts ignoring fit and misfit */
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      JGMDPRT(7,"++++++++++ Calling KnR_pts_all for hand_idx=%d, hsptr=%p\n",h, (void * )p_hs );
      KNR_pts[h] = KnR_pts_all (p_hs) ; /* Fill the Struct */
      JGMDPRT(7,"------------- KnR_pts_all for hand_idx=%d Done --------------\n",h ) ;

   }
      /* Now check for a trump fit
       * If trump fit calculate calc the Dfit pts and the Fn pts
       * mult the short pts  in each hand by 25%,59%,100% as needed
       * Return the len of the fit; a fit-len of 7 promises a 5-2 fit; a fit-len of 0 means no 8+fit and no 5-2 fit
       */
      trump_fit =  KnR_Trump_fit( phs, &KNR_pts[0], &KNR_pts[1] ) ; /* This will also calc the Dfit pts and the Fn pts */
      JGMDPRT(7,"KnR_Trump_fit length = %d \n",trump_fit );
      /* If no 8 + trump fit, check for misfits; trump_fit might be 7 */
      if ( trump_fit < 8 ) {  /* only deduct misfit pts if there is no other fit. RP advice */
         for (int s = CLUBS; s<=SPADES; s++ ) {
            KnR_misfit =  misfit_chk(phs, s) ;
            int sh = KnR_misfit.short_hand ;
            int ss_len = KnR_misfit.sh_len ;
            if (KnR_misfit.mf_type >= 5 ) {   /* assume a deduction if shortness vs 5+ crd suit; not if vs 4 or less suit */
               /* A 5-2 fit with shortness would play in the suit. Deduct the shortpts in the misfit only, not all*/
               if (trump_fit == 7 ) {
                  if      (ss_len == 0 ) { KNR_pts[sh].knr_misfit_adj -= 300 ; }
                  else if (ss_len == 1 ) { KNR_pts[sh].knr_misfit_adj -= 200 ; }
                  else { fprintf(stderr, "CANT HAPPEN in KnR Misfit 7 Fit check ss_len = %d \n",ss_len ); };
                  JGMDPRT(7,"KnR Misfit type=%d, Trump Fit Len=%d, short_hand=%d, ss_len=%d\n",
                           KnR_misfit.mf_type,trump_fit, sh, ss_len );
               }
               else { /* Not even a 7-fit; the hand will be played in NT; deduct ALL shortness pts */
                     KNR_pts[0].knr_misfit_adj -= KNR_pts[0].knr_short_pts ;
                     KNR_pts[1].knr_misfit_adj -= KNR_pts[1].knr_short_pts ;
                     JGMDPRT(7,"KnR Misfit type=%d, Trump Fit Len=%d, short_hand=%d, ss_len=%d BREAKING OUT \n",
                           KnR_misfit.mf_type,trump_fit, sh, ss_len );
                     break ;  /* end for loop now; nothing else to adjust */
               }
            } /* end misfit type test */
         } /* end for loop; might be more than one misfit in either hand */
      } /* end trump_fit < 8 test all misfits, if any, done */
      /* Now do a final total of Dfit, FN_pts, and Misfit adjustments */
      int KnR_side_tot = 0 ;
      int KnR_final_pts ;
      int KnR_final_ptsx100[2];
      for (h = 0 ; h <2 ; h++ ) {
         KnR_final_pts = KNR_pts[h].knr_tot_pts + KNR_pts[h].knr_dfit + KNR_pts[h].knr_Fn_pts + KNR_pts[h].knr_misfit_adj;
         KNR_pts[h].knr_rounded_pts = KnR_Round( KNR_pts[h].knr_body_val, KnR_final_pts ) ;
         KnR_side_tot += KnR_final_pts ;
         KnR_final_ptsx100[h] = KnR_final_pts;
      }
      DBGDO(7,show_knr_pts(7,KNR_pts[0], "Final For Hand[0]") );
      DBGDO(7,show_knr_pts(7,KNR_pts[1], "Final For Hand[1]") );
      JGMDPRT(7,"KnR Metric DONE. Side Total=%d, Filling uservals area with results \n",KnR_side_tot);
      UEv.misc_count = 0 ;
     /* Put the KNR vallues in the same general slots as the others. Makes for easier scripting. */
      p_uservals->u.res[1] = KnR_Round( KNR_pts[0].knr_body_val, KNR_pts[0].knr_tot_pts );  /* N-NT pts rounded to 0-40 pt range */
      p_uservals->u.res[2] = KnR_Round( KNR_pts[1].knr_body_val, KNR_pts[1].knr_tot_pts );  /* S-NT pts */
      p_uservals->u.res[0] = p_uservals->u.res[1] +  p_uservals->u.res[2] ; /* the NT total in a 0-40 pt range*/
      p_uservals->u.res[3] = KNR_pts[0].knr_rounded_pts + KNR_pts[1].knr_rounded_pts ; /* The HLDF total rounded to 0-40 pt range*/
      p_uservals->u.res[4] = KNR_pts[0].knr_rounded_pts; /* North-hldf pts */
      p_uservals->u.res[5] = KNR_pts[1].knr_rounded_pts; /* South-hldf pts */
      p_uservals->u.res[6] = KnR_side_tot ;  /* The x100 version of the complete KnR Points */
      p_uservals->u.res[7] = KnR_final_ptsx100[0]; /* The x100 version for north */
      p_uservals->u.res[8] = KnR_final_ptsx100[1]; /* The x100 version for south */
      /* hand[0] details only missing adj for square hand*/
      p_uservals->u.res[9]  = KNR_pts[0].knr_honor_pts;
      p_uservals->u.res[10] = KNR_pts[0].knr_short_pts;
      p_uservals->u.res[11] = KNR_pts[0].knr_qual_pts;
      p_uservals->u.res[12] = KNR_pts[0].knr_tot_pts ;
      p_uservals->u.res[13] = KNR_pts[0].knr_dfit;
      p_uservals->u.res[14]= KNR_pts[0].knr_Fn_pts;
      p_uservals->u.res[15]= KNR_pts[0].knr_misfit_adj;
      p_uservals->u.res[16]= KNR_pts[0].knr_body_val ;
      /* hand[1] details only missing adj for square hand */
      p_uservals->u.res[17]= KNR_pts[1].knr_honor_pts;
      p_uservals->u.res[18]= KNR_pts[1].knr_short_pts;
      p_uservals->u.res[19]= KNR_pts[1].knr_qual_pts;
      p_uservals->u.res[20]= KNR_pts[1].knr_tot_pts ;
      p_uservals->u.res[21]= KNR_pts[1].knr_dfit;
      p_uservals->u.res[22]= KNR_pts[1].knr_Fn_pts;
      p_uservals->u.res[23]= KNR_pts[1].knr_misfit_adj;
      p_uservals->u.res[24]= KNR_pts[1].knr_body_val ;
         /* also put mainresults in UEv structure in usual slots for possible use by other functions such as set88 etc.*/
      UEv.hldf_pts_side    = p_uservals->u.res[3] ;
      UEv.hldf_pts_seat[0] = p_uservals->u.res[4] ;
      UEv.hldf_pts_seat[1] = p_uservals->u.res[5] ;
      UEv.nt_pts_side      = p_uservals->u.res[0] ;
      UEv.nt_pts_seat[0]   = p_uservals->u.res[1] ;
      UEv.nt_pts_seat[1]   = p_uservals->u.res[2] ;

      #ifdef JGMDBG
      if(jgmDebug >= 7 ) {
         fprintf(stderr, "%s:%d KnR_calc Final Results {jgmDebug=%d} Prod_num=%d\n",__FILE__,__LINE__ , jgmDebug,prod_num ) ;
         for (int i = 0 ; i < 24 ; i++ ) {
            fprintf(stderr, "[%d]=%d, ",i, p_uservals->u.res[i] );
            if( (i+1)%10 == 0 ) {printf("\n"); }
         }
      }
      #endif
      return 1 ;
}

/*  Larsson per his book, Good, Better, Best. It is quite a good method.
 *  syn* pts, body-pts, Lpts=(Len-5) max of 3,
 * -1 for short honors. Do not deduct for misfit;
 * 5-3-1 with 4 trumps
 */
int lar_calc( int side ) {       /* Tag Number: 8 */
   int larsson_pts[2] = {0} ;
   int nines = 0 ;
   int h, s ;
   int temp = 0 ;
   HANDSTAT_k *p_hs;

   prolog( side ) ;  /*zero globals,  set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT( 7 , "++++++++++ Larsson_calc prolog done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      hcp[h] = p_hs->hs_totalpoints ; fhcp[h] = hcp[h];

      for (s = CLUBS ; s<= SPADES ; s++ ) { /* I think I should total the adjustments, and THEN round them. use fhcp_adj[2] */
         fhcp_adj[h] += shortHon_adj(p_hs, s, LAR ) ; /* -1 for K, Q, J, KQ, QJ, */
         JGMDPRT(8,"LAR hcp-adj, Hidx=%d, suit=%d, hcp[s]=%d, Tot_Raw_hcp[h]=%g, Tot_hcp_adj=%g, SuitLen=%d\n",
                              h, s, p_hs->hs_points[s],  fhcp[h],   fhcp_adj[h], p_hs->hs_length[s] );

         /* simpler to do inline than call function */
         if (p_hs->hs_length[s] > 5 ) {
            temp = (p_hs->hs_length[s] - 5 ) ; // +1 for six carder, +2 for 7, +3 for longer. etc.
            if (temp > 3 ) { lpts[h] += 3 ; }
            else           { lpts[h] += temp; }
            JGMDPRT(8,"L_ptsLAR, Hand=%d, suit=%d, len=%d, Tot_Lpts=%d\n", h, s, p_hs->hs_length[s], lpts[h] );
          }
          if ((p_hs->Has_card[s][Nine_rk] ) ) { nines++ ; } /* count nines for later body_pts */
      } /* end for s = CLUBS to SPADES*/
      syn_pts[h] = SynLAR(p_hs) ;  /* count suits with 2 of top 4 incl A or K; +1 for 2 such, +2 for 3 or 4 such*/
      /* Body_pts +1 if the hand has two Tens or one Ten and two+ Nines */
      if ( (p_hs->hs_totalcounts[idxTens] >= 2) || (p_hs->hs_totalcounts[idxTens] == 1 && nines >= 2) ) {body_pts[h] = 1 ;}
      hcp_adj[h] = roundf( fhcp_adj[h] ) ;
      larsson_pts[h] = hcp[h] + hcp_adj[h] + body_pts[h] + syn_pts[h] + lpts[h] ; /* NT pts for hand h */
      JGMDPRT(7,"LARSSON hand=%d,lar_pts_net=%d, hcp=%d, hcp_adj=%d,  body=%d, syn=%d, Lpts=%d,\n",
                  h, larsson_pts[h],hcp[h],hcp_adj[h],body_pts[h],syn_pts[h],lpts[h] );
      UEv.nt_pts_seat[h] = larsson_pts[h] ;
   } /* end for each hand */
   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
   JGMDPRT(7,"LARSSON NT pts done. pts[0]=%d, pts[1]=%d, UEv_Side_pts=%d\n", larsson_pts[0],larsson_pts[1], UEv.nt_pts_side );

   /* Done both hands -- Now check for a trump fit */
   TFpts = Do_Trump_fit(phs, DfitLAR, Fn_ptsNONE) ; /* will also set the globals dfit_pts[] and Fn_pts[] */

   larsson_pts[0] += dfit_pts[0] + Fn_pts[0] ;  /* there are no Fn pts currently */
   larsson_pts[1] += dfit_pts[1] + Fn_pts[1] ;
   UEv.hldf_pts_seat[0] = larsson_pts[0] ;
   UEv.hldf_pts_seat[1] = larsson_pts[1] ;
   UEv.hldf_pts_side = larsson_pts[0] + larsson_pts[1] ;
   UEv.misc_count = 0 ;
/* now some debugging fields Standard set even tho some don't apply to this metric */

      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[0];
      UEv.misc_pts[UEv.misc_count++] = lpts[0];
      UEv.misc_pts[UEv.misc_count++] = body_pts[0];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[0];
      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
       /* ditto hand 1 */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
     JGMDPRT(7, "Larsson fit calcs Done; Tsuit=%d, Decl=%c, HLDFpts[%d : %d], Fn_pts[%d:%d], Dfit_pts=[%d:%d]\n",
         trump.tsuit,"NSEW"[seat[h_decl]], larsson_pts[0],larsson_pts[1], Fn_pts[0],Fn_pts[1],dfit_pts[0],dfit_pts[1] );
  return ( 6 + UEv.misc_count ) ;
} /* end larsson_calc */

int morse_calc( int side ) {     /* Tag Number: 9 -- this is LAR with BumWrap HCP.*/
   int morse_pts[2] = {0} ;
   float_t morse_hcp[2][4] = { {0.0}, {0.0} } ;
   int nines = 0 ;
   int h, s ;
   int temp = 0 ;
   HANDSTAT_k *p_hs;
//fprintf(stderr, "****metrics_calcs:%d :: morse_calc Entered. side=%d, jgmDebug=%d \n",__LINE__,side,jgmDebug);
   prolog( side ) ;  /*zero globals,  set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT( 7 , "++++++++++ morse_calc prolog done side=%d compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      for (s = CLUBS ; s<= SPADES ; s++ ) { /* I think I should total the adjustments, and THEN round them. use fhcp_adj[2] */
         morse_hcp[h][s] = calc_alt_hcp(p_hs, s, MORSE) ; /* calc BumWrap points for this suit. */
         fhcp[h] += morse_hcp[h][s] ;
         fhcp_adj[h] += shortHon_adj(p_hs, s, MORSE ) ; /* -1 for K, Q, J, KQ, QJ, */
         JGMDPRT(8,"Adjmorse, Hidx=%d, suit=%d, hcp[s]=%g, Tot_Raw_fhcp[h]=%g, Tot_fhcp_adj=%g, SuitLen=%d\n",
                     h, s, morse_hcp[h][s], fhcp[h], fhcp_adj[h], p_hs->hs_length[s] );

         /* Using Larsson for now. No requirement to have 3 hcp in the suit. Simpler to do inline than call function */
         if (p_hs->hs_length[s] > 5 ) {
            temp = (p_hs->hs_length[s] - 5 ) ; // +1 for six carder, +2 for 7, +3 for longer. etc.
            if (temp > 3 ) { lpts[h] += 3 ; }
            else           { lpts[h] += temp; }
            JGMDPRT(8,"L_ptsmorse, Hand=%d, suit=%d, len=%d, Tot_Lpts=%d\n", h, s, p_hs->hs_length[s], lpts[h] );
          }
          if ((p_hs->Has_card[s][Nine_rk] ) ) { nines++ ; } /* count nines for later body_pts JGM no do since count T already?*/
      } /* end for s = CLUBS to SPADES*/
      syn_pts[h] = SynLAR(p_hs) ;  /* count suits with 2 of top 4 incl A or K; +1 for 2 such, +2 for 3 or 4 such*/
      /* Body_pts +1 if the hand has two Tens or one Ten and two+ Nines */
      if ( (p_hs->hs_totalcounts[idxTens] >= 2) || (p_hs->hs_totalcounts[idxTens] == 1 && nines >= 2) ) {body_pts[h] = 1 ;}
      hcp_adj[h] = roundf( fhcp_adj[h] ) ;
      hcp[h] = roundf( fhcp[h] ) ;
      morse_pts[h] = roundf(fhcp[h] + fhcp_adj[h]) + syn_pts[h] + lpts[h] ; /* NT pts for hand h omit + body_pts[h] */
      JGMDPRT(7,"morse hand=%d,morse_pts_net=%d, fhcp=%g, fhcp_adj=%g,  syn=%d, Lpts=%d,\n",
                  h, morse_pts[h],fhcp[h],fhcp_adj[h], syn_pts[h],lpts[h] );
      UEv.nt_pts_seat[h] = morse_pts[h] ;
   } /* end for each hand */
   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
   JGMDPRT(7,"morse NT pts done. pts[0]=%d, pts[1]=%d, UEv_Side_pts=%d\n", morse_pts[0],morse_pts[1], UEv.nt_pts_side );

   /* Done both hands -- Now check for a trump fit */
      TFpts = Do_Trump_fit(phs, DfitLAR, Fn_ptsNONE) ; /* will also set the globals dfit_pts[] and Fn_pts[] */
   morse_pts[0] += dfit_pts[0] + Fn_pts[0] ;  /* there are no Fn pts currently */
   morse_pts[1] += dfit_pts[1] + Fn_pts[1] ;
   UEv.hldf_pts_seat[0] = morse_pts[0] ;
   UEv.hldf_pts_seat[1] = morse_pts[1] ;
   UEv.hldf_pts_side    = morse_pts[0] + morse_pts[1] ;
   UEv.misc_count = 0 ;
/* now some debugging fields Standard set even tho some don't apply to this metric */

      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[0];
      UEv.misc_pts[UEv.misc_count++] = lpts[0];
      UEv.misc_pts[UEv.misc_count++] = body_pts[0];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[0];
      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */

      /* ditto for hand 1 */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
     JGMDPRT(7, "morse fit calcs Done; Tsuit=%d, Decl=%c, HLDFpts[%d : %d], Fn_pts[%d:%d], Dfit_pts=[%d:%d]\n",
         trump.tsuit,"NESW"[seat[h_decl]], morse_pts[0],morse_pts[1], Fn_pts[0],Fn_pts[1],dfit_pts[0],dfit_pts[1] );
  return ( 6 + UEv.misc_count ) ;
} /* end morse_calc */

/* Pavlicek Evaluation  per his web site. Count for shortness not length.*/
/* Some Short honors (K,Q,J,KQ,KJ,QK,Qx,Jx) count either the honor HCP (stiff K) or the shortness points (Dpts) but not both.
 * Other Short honors (A, AK, AQ, AJ Ax, Kx ) count both the HCP and the shortness pts. A=6, AK=8 etc.
 * +1 body point for any combination of 4 Aces and 4 Tens ( +2 if all 8 )
 * If a fit is found, Declarer can get length pts for longer trumps and/or 4+ side suit. Len pts depends on fit type. 4-4 or not
 * Dummy can get extra Dfit (suppt) points for a stiff or void with 4+ Trumps, but not for a doubleton.
 *
 */

int pav_calc ( int side ) {      /* Tag Number: 10 */
   int pav_NTpts[2] = {0}; /* Hand value if played in NT */
   int pav_pts[2]   = {0}; /* Hand value if played in a suit */
   int h, s , body_cnt;
   int h_NTdummy = 1 ;              /* will be set by LptsPAV function */
   int mf_sh ;              /* misfit long_hand and short_hand. temp var */
   int slen ;
   HANDSTAT_k *p_hs, *phs_dummy, *phs_decl ;

   prolog( side ) ;  /* zero globals, set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT( 7 , "++++++++++ pav_calc prolog done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */

      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      for (s = CLUBS ; s<= SPADES ; s++ ) {
         fhcp_suit[h][s]  = p_hs->hs_points[s]  ;
         fhcp_adj_suit[h][s] = shortHon_adj(p_hs, s, PAV ) ; /* Deduct in a std way in case we play the hand in NT */
         dpts_suit[h][s] = DptsPAV( p_hs, s ) ; /* strict 3/2/1 for _EACH_ shortness incl first dblton */
         fhcp[h] += fhcp_suit[h][s] ;
         fhcp_adj[h] += fhcp_adj_suit[h][s] ;
         dpts[h] += dpts_suit[h][s] ;

         JGMDPRT(8,"pav_calc, Hand=%d, suit=%d, SuitLen=%d, fhcp[h][s]=%g, fhcp_adj_suit[h][s]=%g, dpts_suit=%d\n",
                     h, s, slen, fhcp_suit[h][s], fhcp_adj_suit[h][s],dpts_suit[h][s] );
      } /* end for s = CLUBS to Spades*/

           /* Check for Aces and Tens */
      body_cnt = p_hs->hs_totalcounts[idxAces] + p_hs->hs_totalcounts[idxTens] ;
      body_pts[h] = body_cnt / 4 ; /* +1 when  4<= A+T <=7 and +2 if 8 == A+T */

      /* In NT PAV does not count Dpts; so we just need the NTpts + body */
      pav_NTpts[h] = roundf(fhcp[h] + fhcp_adj[h] ) + body_pts[h]   ; /* NT points for hand h -- dont count Dpts for NT */
      JGMDPRT(7,"PAV NT Result: Hand=%d, NT_pts=%d,  body_pts[h]=%d, body_cnt=%d,fhcp=%g, fhcp_adj=%g \n",
                     h,  pav_NTpts[h], body_pts[h],  body_cnt, fhcp[h], fhcp_adj[h] );
      UEv.nt_pts_seat[h] = pav_NTpts[h] ;
    /* Set the value for a suit contract */
      pav_pts[h] = pav_NTpts[h] + dpts[h];
      JGMDPRT(7,"PAV Suit Prelim Hand=%d pav_pts[h]=%d, NT_pts=%d, dpts=%d\n",  h,  pav_pts[h], pav_NTpts[h], dpts[h] );
   } /* end for each hand */

   /* add +1 for dummy's 5+ suit in NT */
   h_NTdummy =  LptsPAV( phs ) ; /* put the Lpt in one of the global lpts vars; return which hand is NT dummy */
   UEv.nt_pts_seat[h_NTdummy] += lpts[h_NTdummy] ;
   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ; /* dont add Lpt to pav_pts since applies to NT only */

   JGMDPRT(7,"PAV NT pts done.UEv_NTSide_pts=%d, UEv_NT[0]=%d, UEv_NT[1]=%d\n",  UEv.nt_pts_side, UEv.nt_pts_seat[0], UEv.nt_pts_seat[1] );\
   JGMDPRT(7,"Prelim pav_pts=[0]=%d, pav_pts[1]=%d \n",pav_pts[0],pav_pts[1] );

  trump = trump_fit_chk(phs) ; /* fill the structure describing the trump fit */

  if ( trump.fit_len >= 8 ) { /* fit-len might be 7 here for a 5-2 fit */
      /* if there is a fit, no need to correct for misfits (if any) so use pav_pts */
      /* if there is a trump fit, add Dfit and Fn Points to the already included Dpts and Honor pts  */
      /* Assume that Declarer and Dummy are defined as per PAV. Decl=Longest trumps or if equal the hand with the most HCP */
      /* If its a 4-4 fit (equal len trumps, only Dummy gets Dfit, Declarer gets Fn (side suit points) */
      if(seat[0] == trump.dummy) {h_dummy = 0 ; h_decl = 1 ;  }
      else                       {h_dummy = 1 ; h_decl = 0 ;  }
      phs_dummy = phs[h_dummy] ;
      phs_decl  = phs[h_decl]  ;
      dfit_pts[h_dummy] = DfitPAV (phs_dummy, trump ) ; /* 4+T:2/1/0 These are in addition to Dpts awarded ealier */
      /* Pavlicek adds FN pts to Decl for side suits and extra trump.
       * But his online evaluator does not match the text description. Here we use what the online evaluator gives.
       */
      Fn_pts[h_decl]    = Fn_ptsPAV(phs_decl, trump ) ; /* see comments in actual function */
      JGMDPRT(7, "PAV: dfit for dummy[%d]=%d, FN for Decl[%d]=%d\n",h_dummy,dfit_pts[h_dummy],h_decl,Fn_pts[h_decl]);
      if (  (1 == phs_dummy->square_hand) &&
            (0 == phs_dummy->hs_totalcounts[idxAces] ) &&
            (trump.tlen[h_dummy] < 4 ) ) {
         hand_adj[h_dummy] = -1 ; /* deduct 1 if 4333 && no Aces && 'minimal fit' */
      }
      pav_pts[h_dummy] += ( dfit_pts[h_dummy] + hand_adj[h_dummy] );
      pav_pts[h_decl]  += Fn_pts[h_decl] ;
      JGMDPRT(7,"PAV TrumpFit Deal=%d, in suit=%d Len:%d Decl[%d], Fn=%d Hldf=%d Dummy[%d], Dfit=%d Hldf=%d, SQ_dummy=%d\n",
            gen_num,trump.tsuit,trump.tlen[0]+trump.tlen[1],h_decl,Fn_pts[h_decl],pav_pts[h_decl],
            h_dummy,dfit_pts[h_dummy],pav_pts[h_dummy], hand_adj[h_dummy] );
   } /* End Trump fit pav_pts all done for this case */

   /* Since there is no 8+ fit, Check for misfits which cannot be ignored. */
   else {
      /* If there is shortness vs pards long (4+) suit, and there is no trump fit, "Do not count for the shortness"
       * This is implies that only the shortness in the misfit suits is not counted. Other shortness OK.
       * This differs from Goren where NO shortness is counted if there is no fit.
       * Unanswered Question: What if there is a 5-2 fit and we end in a suit contract? Should we treat like 8-fit?
       */
      /* we subtract the shortness pts from the hand with the short suit facing partner's long suit */
      for (s=CLUBS; s<=SPADES; s++ ) {
         misfit[s] = misfit_chk(phs , s) ; /* return results in a struct */
         if (misfit[s].mf_type > 3) {  /* Misfit; in the short suit count only the 'corrected' NT pts */
               mf_sh = misfit[s].short_hand ;
               pav_pts[mf_sh] -= dpts_suit[mf_sh][s] ;
               JGMDPRT(8,"PAV Deal#=%d, No 8+ fit && MISFIT Type:%d sh_len=%d, Deduct[%d] in suit=%d  sh_hand=%d, pav_pts[0]=%d, pav_pts[1]=%d\n",
                  gen_num,misfit[s].mf_type,misfit[s].sh_len, dpts_suit[mf_sh][s],s,mf_sh,pav_pts[0], pav_pts[1] );
         }
         /* No misfit in this suit. no adjustment to pav_pts for a suit contract */
      } /* end for each suit */
   } /* end check for misfit */
   JGMDPRT(7,"PAV Side Fit/Misfit Eval Done  Deal=%d,  End Result for Suit Play: pav_pts[0]=%d, pav_pts[1]=%d\n",
                     gen_num, pav_pts[0], pav_pts[1] );
   /* pav_pts[] now done;  Starting (NT_pts + Dpts) then added Dfit or Fn or corrected for Misfit */
   UEv.hldf_pts_seat[0] = pav_pts[0];
   UEv.hldf_pts_seat[1] = pav_pts[1];
   UEv.hldf_pts_side = UEv.hldf_pts_seat[0] + UEv.hldf_pts_seat[1] ;
   JGMDPRT(7,"PAV HLDF pts done.UEv_HLDFSide_pts=%d, UEv_HLDF[0]=%d, UEv_HLDF[1]=%d\n",
            UEv.hldf_pts_side, UEv.hldf_pts_seat[0], UEv.hldf_pts_seat[1]);
/* now some debugging fields Standard set even tho some don't apply to this metric */
   UEv.misc_count = 0 ;
      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = roundf(fhcp_adj[0]) ;
      UEv.misc_pts[UEv.misc_count++] = body_pts[0];
      UEv.misc_pts[UEv.misc_count++] = lpts[0];   /* possibly +1 for 5card suit in NT */
      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      UEv.misc_pts[UEv.misc_count++] = dpts[0] ;
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = hand_adj[0];   /* square, aceless dummy with minimal (<9) fit */
      /* Ditto for hand 1 */
      UEv.misc_pts[UEv.misc_count++] = roundf(fhcp_adj[1]) ;
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = dpts[1] ;
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[1];
      JGMDPRT(7,"PAV UEv misc_count=%d,[h0]=%d,%d,%d,%d,%d,%d,%d\n", UEv.misc_count, UEv.misc_pts[0],UEv.misc_pts[1],
                   UEv.misc_pts[2], UEv.misc_pts[3], UEv.misc_pts[4],UEv.misc_pts[5], UEv.misc_pts[6] );
      JGMDPRT(7,"PAV UEv misc_count=%d,[h1]=%d,%d,%d,%d,%d,%d,%d\n", UEv.misc_count, UEv.misc_pts[7],UEv.misc_pts[8],
                  UEv.misc_pts[9],  UEv.misc_pts[10],UEv.misc_pts[11],UEv.misc_pts[12],UEv.misc_pts[13] );

  SaveUserVals( UEv , p_uservals ) ;

  return ( 6 + UEv.misc_count ) ;
} /* end pav_calc */

/* Sheinwold Per Book: 5 Weeks to Winning Bridge 1959,1964 */
int sheinw_calc (int side) {     /* Tag Number: 11 */
   int sheinw_NTpts[2] = {0}; /* Hand value if played in NT */
   int sheinw_pts[2]   = {0}; /* Hand value if played in a suit */
   float_t qtrix[2]    = {0}; /* For interest only now. Maybe use to adjust hand value later? */
   int h, s, h_pard ;
   HANDSTAT_k *p_hs, *phs_dummy, *phs_decl, *phs_pard ;

   prolog( side ) ;  /* zero globals, set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT(7 , "++++++++++ sheinw_calc prolog done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */
      h_pard = (h == 0 ) ? 1 : 0 ;
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      phs_pard = phs[h_pard];
      for (s = CLUBS ; s<= SPADES ; s++ ) {
         fhcp_suit[h][s]  = p_hs->hs_points[s]  ;
         fhcp_adj_suit[h][s] = shortHon_adj(p_hs, s, SHEINW ) ; /* Deduct in a std way in case we play the hand in NT */
         dpts_suit[h][s] = DptsSHEINW( p_hs, s ) ; /* strict 3/2/1 for _EACH_ shortness incl first dblton */
         /*subtract half pt for Qxx or Jxx unless it is in an 8fit suit.  QJx no deduction */
         if ( (p_hs->hs_length[s] >= 3) && ( (phs_pard->hs_length[s] + p_hs->hs_length[s]) < 8) ) {
            if (p_hs->Has_card[s][QUEEN] && p_hs->hs_counts[idxTop4][s] == 1 ) { // Qxx with no A, K or J
               fhcp_adj_suit[h][s] = -0.5 ;
            }
            if (p_hs->Has_card[s][JACK]  && p_hs->hs_counts[idxTop3][s] == 0 ) { // Jxx with No A, K or Q
               fhcp_adj_suit[h][s] = -0.5 ;
            }
         }
         qtrix[h] += QuickTricks_suit(p_hs, s ) ;
         /* Should we adjust for QuickTricks?
          * Book says open any 14, To open 12 needs 2QT; 13 ok with 1.5QT if a Major, or 4=4 Majors
          * Not sure how or IF to allow for this. +/- 1 pt per 0.5QT?
          */
         /*  end hcp and dpts calc for this suit */
         fhcp[h] += fhcp_suit[h][s] ;
         fhcp_adj[h] += fhcp_adj_suit[h][s] ;
         dpts[h] += dpts_suit[h][s] ;

         JGMDPRT(7,"sheinw_calc, Hand=%d, suit=%d, SuitLen=%d, fhcp[h][s]=%g, fhcp-adj[h][s]=%g, Dpts[h][s]=%d, RunQT=%g\n",
                     h, s, p_hs->hs_length[s], fhcp_suit[h][s], fhcp_adj_suit[h][s], dpts_suit[h][s], qtrix[h] );
      } /* end CLUBS <= s <= SPADES */
      /* Check for Aces or lack of */
      body_pts[h] = (p_hs->hs_totalcounts[idxAces] >= 3 ) ?  1 :
                    (p_hs->hs_totalcounts[idxAces] == 0 ) ? -1 : 0 ;

      JGMDPRT(7,"sheinw_calc, Hand=%d, fhcp[h]=%g, fhcp-adj[h]=%g, Dpts[h]=%d, body_pts[h]=%d,Qtrix=%g \n",
                     h, fhcp[h], fhcp_adj[h], dpts[h],body_pts[h], qtrix[h] );
      /* In NT Do not count Dpts; so we just need the HCP + ShortHonor_ADJ + body */
      sheinw_NTpts[h] = roundf(fhcp[h] + fhcp_adj[h]) + body_pts[h]   ; /* NT points for hand h -- dont count Dpts for NT */
     JGMDPRT(7,"SHEINW NT Result: Hand=%d, NT_pts=%d, fhcp=%g, fhcp_adj=%g, body_pts=%d,Qtricks=%g\n",
                     h,  sheinw_NTpts[h],fhcp[h],fhcp_adj[h], body_pts[h], qtrix[h]);

      UEv.nt_pts_seat[h] = sheinw_NTpts[h] ;
      /* We will add to NT points later when we calculate the Hf and FN pts. Right now we do not know who is Declarer */
      /* get Prelim value for hand in suit contract; will be affected by any later discovered misfits; also by Dfit_pts */
      /* in a suit contract we add the Dpts to the NT pts. The short Hon Ded are chosen so this comes out right */
      sheinw_pts[h] = sheinw_NTpts[h] + dpts[h] ;

      JGMDPRT(7,"SHEINW Suit Prelim Val[%d]=%d :: fhcp=%g fhcp_adj=%g dpts=%d body_pts=%d\n",
                           h, sheinw_pts[h], fhcp[h], fhcp_adj[h], dpts[h], body_pts[h]);
    } /* end foreach hand */
    if (sheinw_NTpts[0] >= sheinw_NTpts[1] ) { h_decl = 0; h_dummy = 1 ; } /* mostly for debugging */
    else                                     { h_decl = 1; h_dummy = 0 ; }

    /* ------------------ now check the side as a whole. Fits, DFit_pts, Hf_pts, FN_pts */
    /* Sheinwold says as Dummy do not count Dpts, count Dfit pts on a 5-3-1 scale with 4+ Trump.
     * with only 3 trump 'subtract a point' so 4-2-0 instead of the PAV/GOR 3-2-1 ?
     */

  trump = trump_fit_chk(phs) ; /* fill the structure describing the trump fit */

  JGMDPRT(7,"TrumpFitCheck returns fit_len=%d  -- ",trump.fit_len) ;
   if ( trump.fit_len >= 8) {   /* Fit-len might 7 here which means a 5-2 fit */
      if(seat[0] == trump.dummy) {h_dummy = 0 ; h_decl = 1 ;  }
      else                       {h_dummy = 1 ; h_decl = 0 ;  }
      JGMDPRT(7," -- TrumpSuit=%d, Decl=%d, Dummy=%d \n", trump.tsuit, h_decl, h_dummy);
        /* if there is a trump fit, **Replace** Dummy Dpts with Dfit_pts; **Add** Hf_pts to Dummy.
         *                          **Add** Fn Points to Declarer
         */
      phs_dummy = phs[h_dummy] ;
      phs_decl  = phs[h_decl]  ;
      Fn_pts[h_decl]    = Fn_ptsSHEINW(phs_decl, phs_dummy, trump ) ; /* 5th Trump +1, 6th & up +2 each.) Also +1 for solid 5+side suit*/
      dfit_pts[h_dummy] = DfitSHEINW  (phs_dummy, trump ) ; /* REPLACE Dpts with Dfit pts 5/3/1 or 4/2/0 or 0/0/0*/
      sheinw_pts[h_dummy] -= dpts[h_dummy] ;
      hf_pts[h_dummy] = Hf_ptsSHEINW( phs_dummy , phs_decl, trump.tsuit ) ; /* +1 for Qx, Jx, in trump and QJ,Qx in side suit. */
      JGMDPRT(7, "SHEINW: dummy[%d] Dfit=%d, Hf=%d, FN for Decl[%d]=%d, Subtracted %d Dpts\n",
                         h_dummy,dfit_pts[h_dummy],hf_pts[h_dummy],h_decl,Fn_pts[h_decl], dpts[h_dummy]);
      sheinw_pts[h_dummy] += ( dfit_pts[h_dummy] + hf_pts[h_dummy] );
      sheinw_pts[h_decl]  += Fn_pts[h_decl];
      JGMDPRT(7,"SHEINW TrumpFit Deal=%d, in suit=%d Len:%d Decl=[%d], Fn=%d, Hldf=%d Dummy=[%d], Dfit=%d Hf=%d, Hldf=%d\n",
            gen_num,trump.tsuit,trump.tlen[0]+trump.tlen[1],h_decl,Fn_pts[h_decl],sheinw_pts[h_decl],
            h_dummy,dfit_pts[h_dummy],hf_pts[h_dummy],sheinw_pts[h_dummy] );
   } /* End Trump fit sheinw_pts all done for this case */
   else {
      JGMDPRT(7," -- Sheinwold No Trump fit, no Fn, no Dfit, keep Dpts \n") ;
      ; /* NO-OP when JGMDBG is not defined */
   }

   /* Now we add any Hf and Fn pts to the NT values as well */
      UEv.nt_pts_seat[h_decl]  += Fn_pts[h_decl] ;
      UEv.nt_pts_seat[h_dummy] += hf_pts[h_dummy];
      UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
      JGMDPRT(7,"SHEINW Final NTpts with Hf and Fn incl. Tot=%d,Decl[%d]=%d,Dummy[%d]=%d\n",
                  UEv.nt_pts_side, h_decl, UEv.nt_pts_seat[h_decl], h_dummy, UEv.nt_pts_seat[h_dummy] ) ;
      UEv.hldf_pts_seat[0] = sheinw_pts[0];
      UEv.hldf_pts_seat[1] = sheinw_pts[1];
      UEv.hldf_pts_side = UEv.hldf_pts_seat[0] + UEv.hldf_pts_seat[1] ;
      JGMDPRT(7,"SHEINW Side Fit/Misfit Eval Done  Deal=%d,  End Result for Suit Play:Tot=%d, Decl[%d]=%d, Dummy[%d]=%d\n",
                                         gen_num, UEv.hldf_pts_side,h_decl,sheinw_pts[h_decl],h_dummy,sheinw_pts[h_dummy] );
/* now some debugging fields */
   UEv.misc_count = 0 ;
      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = roundf(fhcp_adj[0]);
      UEv.misc_pts[UEv.misc_count++] = body_pts[0];
      UEv.misc_pts[UEv.misc_count++] = hf_pts[0];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dpts[0];       /* distribution aka shortness pts before a fit is found */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = roundf(qtrix[0]*100);

      /* same again for hand 1 */
      UEv.misc_pts[UEv.misc_count++] = roundf(fhcp_adj[1]);
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hf_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = dpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = roundf(qtrix[1]*100);

      JGMDPRT(7,"SHEINW UEv misc_count=%d,[h0]=%d,%d,%d,%d,%d,%d,%d\n", UEv.misc_count, UEv.misc_pts[0],UEv.misc_pts[1],
                  UEv.misc_pts[2], UEv.misc_pts[3],UEv.misc_pts[4],UEv.misc_pts[5], UEv.misc_pts[6]);
      JGMDPRT(7,"SHEINW UEv misc_count=%d,[h1]=%d,%d,%d,%d,%d,%d,%d\n", UEv.misc_count, UEv.misc_pts[7],UEv.misc_pts[8],
                  UEv.misc_pts[9], UEv.misc_pts[10],UEv.misc_pts[11],UEv.misc_pts[12], UEv.misc_pts[13]);

   SaveUserVals( UEv , p_uservals ) ;
   return ( 6 + UEv.misc_count ) ;
} /* end sheinw_calc */

/* This code makes several assumptions about edge cases that I could not get answered by ZP.
 * 1) Certain HH honor combos. AK? AQ? AJ? Downgraded or not? Assume AJ, not the other two.
 * 2) What HCP to use for the 2/3 suit adj. The raw or the Downgraded ones (assume raw; its simpler)
 * 3) Can Declarer get Hf points in Dummy's 4+ side suit? (assume yes) How long must the fit be? (assume 7+)
 * 4) In a 4-4 fit can Both hands get Hf pts (e.g. Kxxx vs Qxxx). Assume yes, to a max of 2 in the suit total.
 * 5) The Zar doc sometimes gives (2-d) pts per extra trump sometimes (3-d). Assume 3-d. (Pavlicek assumes 2-d)
 * 6) In the case of an 8 fit, Dummy counts Zar Ruffing with 3+ trump; Declarer with 6+ trump
 * 7) This code does not consider any Misfit points. Either Positive or Negative.
 * 8) The hand with the most length in trump is assumed to be Declarer. With Equal length, the stronger hand is Declarer. Else North/East.
 */
int zarbasic_calc( int side ) {   /* Tag Number: 12 Two Hands independent. No HF, no Fn, No Dfit */
   int suit_len[4];
   int zar_pts[2]   = {0}; /* Hand value if played in a suit */
   int zar_ctls[2]  = {0};
   int zar_Dist[2]  = {0};
   int zar_syn_pts[2] = {0}; /* Point for all HCP being in 2 or 3 suits or for 25 Zars and 4 card spade suit */

   int h, s;
   HANDSTAT_k *p_hs;

   prolog( side ) ;  /* zero globals, set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT(7 , "++++++++++ zar_basic_calc prolog done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      zar_ctls[h] = p_hs->hs_totalcontrol ;
      for (s = CLUBS ; s<= SPADES ; s++ ) {
         suit_len[s] = p_hs->hs_length[s] ;
         fhcp_suit[h][s]  = p_hs->hs_points[s]  ;
         fhcp_adj_suit[h][s] = shortHon_adj(p_hs, s, ZARBAS ) ; /* Deduct in a std way in case we play the hand in NT */
         fhcp[h] += fhcp_suit[h][s] ;
         fhcp_adj[h] += fhcp_adj_suit[h][s] ;

         JGMDPRT(8,"zarbasic_calc, Hand=%d, suit=%d, SuitLen=%d, fhcp[h][s]=%g, fhcp-adj[h][s]=%g\n",
                     h, s, p_hs->hs_length[s], fhcp_suit[h][s], fhcp_adj_suit[h][s] );
      } /* end CLUBS <= s <= SPADES */

      // insertionSort(4, suit_len, suit_id) ; /* sort suitlens in desc order suit_len[0] = longest. */
      qsort(suit_len, 4, sizeof(int), desc_cmpxy ) ;
      zar_Dist[h] = 2*suit_len[0] + suit_len[1] - suit_len[3] ;
      hcp_adj[h] = roundf(fhcp_adj[h]) ;
      hcp[h] = roundf( fhcp[h] + fhcp_adj[h] ) ;
      zar_syn_pts[h] = SynZAR( p_hs ) ; /* No Answ from ZP re AKxxx/KQxx/Kx/Jx - 16 HCP corrected to 15. +1 for 3 suits? */

      zar_pts[h] = hcp[h] + zar_ctls[h] + zar_Dist[h] + zar_syn_pts[h];
      if (25 == zar_pts[h] && p_hs->hs_length[SPADES] >= 4 ) {zar_pts[h] += 1 ; zar_syn_pts[h] += 1 ; }
      JGMDPRT(8,"ZARBAS hand=%d,zarbas_pts_net=%d, hcp=%d, hcp_adj=%d, CtlPts=%d, Dist_pts=%d Syn_pts=%d\n",
                  h, zar_pts[h],hcp[h],hcp_adj[h],zar_ctls[h], zar_Dist[h], zar_syn_pts[h] );

      UEv.nt_pts_seat[h] = zar_pts[h] ;
   } /* end for each hand */

   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
   /* for basic Zar pts there is no extra calculations for trump fit, and so on. so NT pts are same as Suit Pts. */

   UEv.hldf_pts_seat[0] = zar_pts[0] ;
   UEv.hldf_pts_seat[1] = zar_pts[1] ;
   UEv.hldf_pts_side = zar_pts[0] + zar_pts[1] ;

   UEv.misc_count = 0 ;
      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[0];     // Result[6]
      UEv.misc_pts[UEv.misc_count++] = zar_ctls[0];
      UEv.misc_pts[UEv.misc_count++] = zar_Dist[0];
      UEv.misc_pts[UEv.misc_count++] = zar_syn_pts[0];

       /* ditto hand 1 */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = zar_ctls[1];
      UEv.misc_pts[UEv.misc_count++] = zar_Dist[1];
      UEv.misc_pts[UEv.misc_count++] = zar_syn_pts[1]; // Result[13]

  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
  JGMDPRT(7,"ZARBAS pts done. pts[0]=%d, pts[1]=%d, UEv_Side_pts=%d MiscCount=%d\n",
                        zar_pts[0],zar_pts[1], UEv.nt_pts_side, UEv.misc_count );
  return ( 6 + UEv.misc_count ) ;
} /* end zar_basic_calc */

int zarfull_calc( int side ) {   /* Tag Number: 13 */

   zarbasic_calc( side ) ;  /* fill the UEv struct with the basic stuff */
   /* the basic zar pts are in the UEv.nt_pts_side, and UEv.nt_pts_seat[h] */
   /* Now add in Fn_pts, Hf_pts, Dfit_pts */
   int dc ;
   int zTrumps, zHf_pts, zDfit_pts ;
   TRUMP_SUIT_k trump_suit ;

   zTrumps = SetTrumps( phs , &trump_suit  ) ;
   dc = SetDeclarer( phs , zTrumps ) ;
   Fill_side_fitstat( phs , &fitstat ) ;
   JGMDPRT(8,"ZARFULL ztrumps=%d, decl=%d, t_fitlen=%d\n",zTrumps,dc,fitstat.t_fitlen );
   Fn_pts[1] = Fn_ptsZar( phs , zTrumps ) ;  /* one or two points for a secondary fit Arbitrary choose hand 1*/
   zHf_pts = Hf_ptsZar( phs ) ;              /* Sets the globals hf_pts[2] */
   zDfit_pts = DfitZAR( phs , &fitstat ) ;   /* Sets the globals dfit_pts[2] */
   JGMDPRT(8,"ZARFULL zFn_pts=%d, zHf_pts=%d, zDfit_pts=%d\n",Fn_pts[1], zHf_pts, zDfit_pts );
   /* now add the extra pts to the UEv array */
   UEv.hldf_pts_seat[0] += hf_pts[0] + dfit_pts[0] ;
   UEv.hldf_pts_seat[1] += hf_pts[1] + dfit_pts[1]  + Fn_pts[1] ; /* arbitrarily give the Fn pts to Hand 1 */
   UEv.hldf_pts_side = UEv.hldf_pts_seat[0] + UEv.hldf_pts_seat[1],
  /* some Extra debugging values */
   UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];  // Result[14]
   UEv.misc_pts[UEv.misc_count++] = hf_pts[0];
   UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];
   UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
   UEv.misc_pts[UEv.misc_count++] = hf_pts[1];
   UEv.misc_pts[UEv.misc_count++] = dfit_pts[1]; // result[19]
  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
  JGMDPRT(7,"ZARFULL pts done. pts[0]=%d, pts[1]=%d, UEv_Side_pts=%d\n", UEv.hldf_pts_seat[0],UEv.hldf_pts_seat[1], UEv.nt_pts_side );
  return ( 6 + UEv.misc_count ) ;
} /* end zarfull_calc */

/* These next ones are place holders until the actual code is written */
int unkn_err(   int side ) { fprintf(stderr,"Unknown Metric Not Implemented. Returning -1\n"); return -1 ; }

/* Tag 88 is a way of getting all possible evaluations done with one call. This can be useful because
 * the cache is filled and all the numbers can then be got without calling the  server so many times.
 * The downside is that only a limited number of results per metric can be obtained.
 * But in practice this is not a drawback as usually we are only interested in at most two results:
 * The total for the side if played in NT and the total for the side if played in the longest suit.
 * If we never exceed 16 metrics, then we can get 4 results for each one, or 2 results for 32 metrics.
 */
 /* we could also adapt this approach and have tags 77, or 89 etc. for other special cases. */

/* Tag Number 88 -- do all the metrics flagged in the matrix set88
 * The HLDF total for the side will be in slot 'm' the NT value will be in slot m+32
 * Currently m varies from 0 (BERG) to 14 (ZAR) ; Room for 32 such metrics using this scheme
 */
/* the set88 array allows us filter or choose which of the implemented metrics to do the evaluations for
 * Recompile or patch the array with a hex editor or a Debugger
 */
 char FIND88[]="8888 SET 8888";  /*marker in case we want to patch next array with GDB */
 //              B,b,D,  G,J,E,  K,k,L, M,P,R,  S, Z, U    1 means include in set, zero skip. -0 not implemented
 int  set88[20]={1,1,1,  1,1,1,  1,1,1, 1,1,-0, 1,-0,-0 } ;
 #define SET88_SZ sizeof(set88)/sizeof(int)

typedef int (*pCALC_FUNC_k)( int ) ;
pCALC_FUNC_k p_cfunc[] = {bergen_calc,  bissell_calc, dkp_calc,      goren_calc,   jgm1_calc,
                          kaplan_calc,  karpin_calc,  knr_calc,      lar_calc,     morse_calc,
                          pav_calc,     sheinw_calc,  zarbasic_calc, zarfull_calc, unkn_err } ;
 char cf_name[][10] = {"Bergen", "BISSELL", "DKP", "Goren","JGM1","Kaplan","Karpin","KnR","Larsson","Morse",
                       "Pavlicek","Sheinwold","ZarBasic","ZarFull","!Unkn" };
/*
 * These next ones have not been given entries in the p_cfunc[] table. But the UserEval mainline will call them
 * if given the right tag number.
 */

int set88_calc (int side ) {
   int m;
   int m_cnt = 0 ;
   USER_VALUES_k *p_88_results;

   int set88_HLDF_pts[SET88_SZ] = { -1 };   /* set88 returns the Suit evals in res[0] to [res31]*/
   int set88_NT_pts[SET88_SZ]   = { -1 };   /* set88 returns the NT evals in res[32] to [res63]*/
   prolog( side ) ;
   p_88_results   = p_uservals;
   for (m=0 ; m<SET88_SZ ; m++ ) {
      if (set88[m] == 1 ) {
         JGMDPRT(7," Metric=%d, Set88=%d, name=%s\n",m,set88[m],cf_name[m] );
         (*p_cfunc[m])( side ) ;  /* will return number of results calculated; we only care about the side total */
         set88_HLDF_pts[m] = UEv.hldf_pts_side ; /* we need to store these temporarily bec the metrics modify the userresults area*/
         set88_NT_pts[m]   = UEv.nt_pts_side;
         m_cnt++;
         JGMDPRT(7,"%s_calc done; side HLDF_pts=%d, side_NT_pts=%d, valid_cnt=%d\n",
                     cf_name[m],set88_HLDF_pts[m],set88_NT_pts[m], m_cnt ) ;
      }
   }
   for (m=0; m<SET88_SZ; m++ ) {
      p_88_results->u.res[m] = set88_HLDF_pts[m] ; /* some of these will be invalid i.e. -1; USER BEWARE */
      p_88_results->u.res[m+32] = set88_NT_pts[m];
   }

   return m_cnt; /* the number of valid results in the 0 .. SET88_SZ and 32 .. SET88_SZ slots */
}

int mixed_JGM1calc ( int side ) { /* call karpin_calc then jgm1_calc  tag = 20 */
   int karpin_pts[6] = { 0 };
   int jgm_pts[6] = { 0 };
   int i ;
   karpin_calc( side ) ;
   for (i = 0 ; i < 6 ; i++ ) {
      karpin_pts[i] = p_uservals->u.res[i] ; /* save results since they will be overwritten by jgm1_calc */
   }
   jgm1_calc( side ) ;
   for (i = 0 ; i < 6 ; i++ ) {
      jgm_pts[i] = p_uservals->u.res[i] ; /* save results since they will be overwritten by lar_calc */
   }
   /* now copy the saved karpin results to the next available slots in the p_uservals area */
   for (i = 0 ; i < 6 ; i++ ) {
      p_uservals->u.res[i]   = jgm_pts[i]    ;
      p_uservals->u.res[i+6] = karpin_pts[i] ;

   }
   /* we should now have:
    * in slots 0 .. 5 for JGM:    NT_side, NT_north, NT_south, HLDF_side, HLDF_north, HLDF_south
    * in slots 6 ..11 for Karpin: NT_side, NT_north, NT_south, HLDF_side, HLDF_north, HLDF_south (overwriting jgm dbg vals)
    * The misc extra debug values in slots 12 .. nn are still there, but will be ignored by Dealer
    */
    return 12 ;
} /* end mixed_JGM1calc */

int mixed_LARcalc  ( int side ) { /* call lar_calc    then morse_calc tag = 21 */

   int lar_pts[6] = { 0 };
   int morse_pts[6] = { 0 };
   int i ;
   lar_calc( side ) ;
   for (i = 0 ; i < 6 ; i++ ) {
      lar_pts[i] = p_uservals->u.res[i] ; /* save results since they will be overwritten by jgm1_calc */
   }
   morse_calc( side ) ;
   for (i = 0 ; i < 6 ; i++ ) {
      morse_pts[i] = p_uservals->u.res[i] ; /* save results since they will be overwritten by lar_calc */
   }
   /* now copy the saved lar results to the next available slots in the p_uservals area */
   for (i = 0 ; i < 6 ; i++ ) {
      p_uservals->u.res[i]   = morse_pts[i] ;
      p_uservals->u.res[i+6] = lar_pts[i] ;
   }
   /* we should now have:
    * in slots 0 .. 5 for MORSE NT_side, NT_north, NT_south, HLDF_side, HLDF_north, HLDF_south
    * in slots 6 ..11 for LAR NT_side, NT_north, NT_south, HLDF_side, HLDF_north, HLDF_south (overwriting jgm dbg vals)
    * The misc extra debug values in slots 12 .. nn are still there, but will be ignored by Dealer
    */
    return 12 ;
} /* end mixed_LAR_calc */

int make_test_evals(struct detailed_res_st *p_ures) ;  /* prototype from metrics_utils_subs.c*/
int test_calc ( int side, struct query_type_st *pqt) {
/* put a sequence of values in the results area so we can verify that the Dealer Input file syntax picks up the right ones */
   int nres = 0 ;
   int res_idx ;
   char lf = ' ';
   // HANDSTAT_k *p_hs;
   struct detailed_res_st *p_ures ;

   prolog( side ) ;  /*zero globals,  set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT( 7 , "test_calc done prolog for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1] ) ;

   fprintf(stderr, "------- Testing Tag %d with dealnum=%d-------\n",21, prod_num );
   p_ures = &(p_uservals->u.dr) ;
   nres = make_test_evals( p_ures ) ;
   #ifdef JGMDBG
      if(jgmDebug >= 7 ) {
         fprintf(stderr,"%d Test Evals Created\n",nres);
         for (res_idx = 0 ; res_idx < nres ; res_idx++ ) {
            lf = ((res_idx+1) % 16 == 0 ) ? '\n' : ' ' ;
            fprintf(stderr, "%d=%d,%c",res_idx,p_uservals->u.res[res_idx], lf ) ;
         }
         fprintf(stderr, "\n");
      }
   #endif
  return nres ;
} /* end test calc */









