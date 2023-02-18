/* File metrics_calcs.c   */
/* Date      Version  Author  Description
* 2022/11/25 1.0.0    JGM     The high level functions called from the Server main to do the hand evaluation.
* 2022/12/20 1.0.1    JGM     Added extra misc output fields to UEV struct for possible debugging.
* 2023/01/20 1.1      JGM     Several Metrics working. Addd two flavors of mixed metrics.
* 2023/02/07 2.0      JGM     Most interesting Metrics workig. Redid KnR to put results in std locations.
*/
/*
 * This file implements the higher level overall flow of a metric. It will often call routines from factors_subs.c
 * to implement the lower level calculations. The factors functions have names like:Fn_ptsBERG, DFIT_Berg etc.
 */
#include "../include/UserEval_types.h"
#include "../include/UserEval_externs.h"
#include "../include/UserEval_protos.h"
#include "../include/dbgprt_macros.h"

int bergen_calc( int side ) {  /* Tag Number: 0 */
   int berg_pts[2] ; // these are the starting points; presumably for NT;
   int h, s ;
   int adj3_cnt;
   int berg_misfit ;
   HANDSTAT_k *p_hs ;

   prolog( side ) ;  /* zero globals, set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   /*
    * Bergen: hand1 incl ADJ-3 to get starting points. Then hand2 ditto.
    *         then Dfit, Hf, Fn_pts and misfit corrections for the pair; then final total
    */
   JGMDPRT( 7 , "++++++++++ berg_calc prolog done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   for (h = 0 ; h < 2 ; h++) { /* for each hand in the side */
      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      hcp[h] = p_hs->hs_totalpoints ;

      for (s = CLUBS ; s<= SPADES ; s++ ) {
         fhcp_adj[h] += shortHon_adj(p_hs, h, s, BERG ) ;  /* Berg uses -0.5 pts for AJ, KQ, KJ*/
         syn_pts[h] += SynBERG(p_hs, s) ;  // Quality suit 3 of top5 in 4+ suit = +1; applies to all suits not just trump
         /* simpler to do inline than call Lpts_std  pts schmoints does not say if Len or short method for opener. */
         if (p_hs->hs_length[s] > 4 ) {
            lpts[h] += (p_hs->hs_length[s] - 4 ) ; // +1 for 5 card suit, +2 for six, +3 for 7 etc.
            JGMDPRT(8,"L_ptsBERG, suit=%d, len=%d, Tot_Lpts=%d\n", s, p_hs->hs_length[s], lpts[h] );
         }
      } /* end for s = CLUBS to SPADES*/
      hcp_adj[h] = (int) roundf(fhcp_adj[h]) ;
      if (p_hs->square_hand) { hand_adj[h] = -1 ;}
      else                   { hand_adj[h] =  0 ;}
      JGMDPRT(7,"SqBERG, seat[%c] hand_adj=%d\n", compass[h], hand_adj[h] );
      adj3_cnt = ( p_hs->hs_totalcounts[Tens]   + p_hs->hs_totalcounts[Aces]
                  -p_hs->hs_totalcounts[Queens] - p_hs->hs_totalcounts[Jacks] ) ;
      if      (adj3_cnt <= -6 )  { body_pts[h] += -2 ; } // Downgrade
      else if (adj3_cnt <= -3 )  { body_pts[h] += -1 ; } // Downgrade
      else if (adj3_cnt >=  6 )  { body_pts[h] += +2 ; } // Upgrade
      else if (adj3_cnt >=  3 )  { body_pts[h] += +1 ; } // Upgrade
      JGMDPRT(7,"Adj3 BERG, seat[%c] adj3_cnt=%d, body_pts=%d\n", compass[h], adj3_cnt, body_pts[h] );
      berg_pts[h] = body_pts[h] + hand_adj[h] + syn_pts[h] + lpts[h] + hcp_adj[h] + hcp[h] ; /* starting points for hand h */
      UEv.nt_pts_seat[h] = berg_pts[h] ;
   } /* end for each hand */
   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
   JGMDPRT(7,"Berg Start/NT pts done. pts[0]=%d, pts[1]=%d, UEv_Side_pts=%d\n", berg_pts[0],berg_pts[1], UEv.nt_pts_side );

   /* now do the stuff for the side as a whole, Hf, Dfit and Fn Mainly.
    * Berg does not do Waste or Mirror
    * But if there is a misfit he throws out any Lpts assigned
    */
      hf_pts[0] = Hf_ptsBERG( phs , 0 ) ; /* Hand 0 is the one getting Hf pts for help in pards long suit. */
      hf_pts[1] = Hf_ptsBERG( phs , 1 ) ;
      JGMDPRT(7,"Berg: Hf Pts; h=%c Hf=%d, h=%c Hf=%d\n",compass[0],hf_pts[0],compass[1], hf_pts[1] );

   /* Bergen Dfit; No dfit with trumps <= 2 ; with 3: 3/2/1 ; with 4: 4/3/1 ; with 5: 5/3/1 */
      TFpts = Do_Trump_fit(phs, DfitBERG, Fn_ptsBERG) ; /* will also set the globals dfit_pts[] and Fn_pts[] */
      dfit_pts[0] = TFpts.df_val[0] ; dfit_pts[1] = TFpts.df_val[1];
      Fn_pts[0]   = TFpts.fn_val[0] ; Fn_pts[1]   = TFpts.fn_val[1];

      berg_pts[0] += dfit_pts[0] + Fn_pts[0] ;
      berg_pts[1] += dfit_pts[1] + Fn_pts[1] ;
      UEv.hldf_pts_seat[0] = berg_pts[0] ;
      UEv.hldf_pts_seat[1] = berg_pts[1] ;
      UEv.hldf_pts_side = berg_pts[0] + berg_pts[1] ;

  /* check for misfits in each suit  This may matter if we play in NT even if there is an 8+ fit */
  /* We assume Bergen considers shortness vs 4+ suit a misfit; vs a 3crd suit not a misfit... */
     berg_misfit = 0 ;
     for (s=CLUBS ; s<=SPADES; s++ ) {
        misfit[s] = misfit_chk(phs, s) ;
        if (  misfit[s].mf_type > 3 ) { berg_misfit = 1 ; } /* len pts only assigned for 4+ so waste wont matter */
     }
     if (1 == berg_misfit ) { /* there is at least one misfit Subtract any Length pts previously assigned to the NT pts*/
        UEv.nt_pts_seat[0] -= lpts[0] ;
        UEv.nt_pts_seat[1] -= lpts[1] ;
        UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ;
        JGMDPRT(7, "Berg MISFIT calcs Done removed lpts %d, %d\n", lpts[0], lpts[1]);
        /* no further adj for wastage vs shortness,  or for shortness vs len*/
      }
/* now some debugging fields Standard set even tho some don't apply to this metric */
// N:(hcpadj,Lpt,Body,Syn); S:(hcpadj,Lpt,Body,Syn); N:(Dpt,Dfit,Fn,hand_adj);S:(Dpt,Dfit,Fn,hand_adj)
   UEv.misc_count = 0 ;
      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[0];
      UEv.misc_pts[UEv.misc_count++] = lpts[0];
      UEv.misc_pts[UEv.misc_count++] = body_pts[0];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[0];

      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[1];

      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dpts[0];       /* distribution aka shortness pts before a fit is found */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      UEv.misc_pts[UEv.misc_count++] = hand_adj[0];  /* square, aceless dummy with minimal (<4) fit */

      UEv.misc_pts[UEv.misc_count++] = dpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[1];
      // N:(hcpadj,Lpt,Body,Syn); S:(hcpadj,Lpt,Body,Syn); N:(Dpt,Dfit,Fn,hand_adj);S:(Dpt,Dfit,Fn,hand_adj)
  /* now put the results into the user result area at p_uservals */
  JGMDPRT(7, "Berg Calcs complete. Saving %d values at userval address %p \n", (6+UEv.misc_count), (void *)p_uservals );
  fsync(2);
  SaveUserVals( UEv , p_uservals ) ;
  return ( 6 + UEv.misc_count ) ; /*number of results calculated */
} /* end bergen_calc */

/*
 * Karpin Evaluation per PAV; an example of counting for Long suits, not short ones.
 * Seems v. Old fashioned. I may replace later with Kaplan count (who is a long suit counter).
 * Sheinwold is a short suit counter
 */
int karpin_calc( int side ) {  /* Tag Number: 7 */
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
         fhcp_adj[h] += shortHon_adj(p_hs, h, s, KARPIN ) ; /* -1 for K, Q, J, QJ, Qx, Jx. */
         JGMDPRT(8,"AdjKarpin, Hand=%d, suit=%d, hcp[s]=%d, Tot_fhcp_adj=%g SuitLen=%d\n",
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
      UEv.misc_pts[UEv.misc_count++] = body_pts[0];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[0];

      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[1];

      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dpts[0];       /* distribution aka shortness pts before a fit is found */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      UEv.misc_pts[UEv.misc_count++] = hand_adj[0];  /* square, aceless dummy with minimal (<4) fit */

      UEv.misc_pts[UEv.misc_count++] = dpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[1];
  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
     JGMDPRT(7, "Karpin fit calcs Done; Tsuit=%d, Decl=%c, HLDFpts[%d : %d], Fn_pts[%d:%d], Dfit_pts=[%d:%d]\n",
         trump.tsuit,"NSEW"[seat[h_decl]], karpin_pts[0],karpin_pts[1], Fn_pts[0],Fn_pts[1],dfit_pts[0],dfit_pts[1] );
  return ( 6 + UEv.misc_count ) ;
} /* end karpin_calc */

/*
 * Evaluation chosen by Pavlicek per his web site. Count for shortness not length.
 * Short honors count either the honor HCP (stiff K) or the shortness points (Dpts) but not both.
 * +1 body point for any combination of 4 Aces and 4 Tens (assuming +2 if all 8 )
 * If a fit is found, Declarer can get length pts for longer trumps and/or 4+ side suit.
 * Dummy can get extra Dfit (suppt) points for a stiff or void, but not for a doubleton.
 * This means that a Doubleton Honor such as Ax, Kx, Qx, or Jx gets neither Dpts NOR Dfit pts.
 */
int pav_calc ( int side ) {
   float_t fhcp_suit_adj[2][4] = { {0.0}, {0.0} };
   int pav_NTpts[2]     = {0};
   int pav_pts[2]       = {0};
   int  hcp_suit[2][4]  = { {0},{0} };
   int  dpts_suit[2][4] = { {0},{0} };
   int h, s , body_cnt;
   int h_NTdummy = 1 ;              /* will be set by LptsPAV function */
   int tmp_pts;
   HANDSTAT_k *p_hs, *phs_dummy, *phs_decl ;

   prolog( side ) ;  /* zero globals, set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT( 7 , "++++++++++ pav_calc prolog done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   JGMDPRT( 9 ,"Check Globals Init fhcp_adj[0]=%g, pav_pts[0]=%d, fhcp_suit_adj[1][3]=%g \n",
                                   fhcp_adj[0],pav_pts[0],fhcp_suit_adj[1][3] );
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */

      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      for (s = CLUBS ; s<= SPADES ; s++ ) { /* I think I should total the adjustments, and THEN round them. use fhcp_adj[2] */
         fhcp_suit_adj[h][s] = shortHon_adj(p_hs, h, s, PAV ) ; /* Deduct in a std way in case we play the hand in NT */
         fhcp_adj[h] += fhcp_suit_adj[h][s] ;
         dpts_suit[h][s] = DptsPAV( p_hs, s ) ; /* strict 3/2/1 for _EACH_ shortness */
         dpts[h] += dpts_suit[h][s] ;
         hcp_suit[h][s]  = p_hs->hs_points[s]  ;
         hcp[h] += hcp_suit[h][s] ;

         JGMDPRT(8,"AdjPAV, Hand=%d, suit=%d, SuitLen=%d, hcp[h][s]=%d, fhcp-adj[h][s]=%g, Dpts[h][s]=%d \n",
                     h, s, p_hs->hs_length[s], hcp_suit[h][s], fhcp_suit_adj[h][s], dpts_suit[h][s] );
      } /* end for s = CLUBS to Spades*/
      hcp_adj[h] = roundf(fhcp_adj[h]) ;
      body_cnt = p_hs->hs_totalcounts[Aces] + p_hs->hs_totalcounts[Tens] ;
      body_pts[h] = body_cnt / 4 ; /* +1 when  4<= A+T <=7 and +2 if 8 == A+T */
      /* In NT PAV does not count Dpts; so we just need the HCP + ShortHonor_ADJ + body */
      pav_NTpts[h] = hcp[h] + hcp_adj[h] + body_pts[h]   ; /* NT points for hand h -- dont count Dpts for NT */
      JGMDPRT(7,"PAV NT Result: Hand=%d, NT_pts=%d, hcp=%d, hcp_adj=%d, body_pts[h]=%d, body_cnt=%d \n",
                     h,  pav_NTpts[h],hcp[h],hcp_adj[h], body_pts[h],  body_cnt );
      /* get Prelim value for hand in suit contract; will be affected by any later discovered misfits */

      for (s=CLUBS ; s <= SPADES; s++ ) {
         tmp_pts = hcp_suit[h][s]; /*Count either UNCORRECTED hcp, or Dpts. stiff K=3>2=>3; Qx=2>1=>2, J=1<2=>2 */
         if (dpts_suit[h][s] > tmp_pts ) {  pav_pts[h] += dpts_suit[h][s] ; }
         else { pav_pts[h] += tmp_pts ; }
         JGMDPRT(8,"PAV::Dpts/HCP hidx=%d, suit=%c, dpts_suit=%d, RawHCp_suit=%d HandTot=%d\n",h,"CDHS"[s],dpts_suit[h][s],tmp_pts,pav_pts[h]);
      }
      pav_pts[h] += body_pts[h];
      JGMDPRT(7,"PAV Suit Prelim Val[%d]=%d :: [(hcp=%d plus hcp_adj=%d) and/or dpts=%d], body_pts=%d, body_cnt=%d\n",
                     h,  pav_pts[h], hcp[h], hcp_adj[h], dpts[h], body_pts[h],  body_cnt );

      UEv.nt_pts_seat[h] = pav_NTpts[h] ;
   } /* end for each hand */
   /* add +1 for dummy's 5+ suit in NT */
   h_NTdummy =  LptsPAV( phs ) ; /* put the pt in one of the global lpts vars; return which hand is NT dummy */
   UEv.nt_pts_seat[h_NTdummy] += lpts[h_NTdummy] ;
   UEv.nt_pts_side = UEv.nt_pts_seat[0] + UEv.nt_pts_seat[1] ; /* dont add Lpt to pav_pts since applies to NT only */

   JGMDPRT(7,"PAV NT pts done.UEv_Side_pts=%d, UEv[0]=%d, UEv[1]=%d\n",  UEv.nt_pts_side, UEv.nt_pts_seat[0], UEv.nt_pts_seat[1] );\
   JGMDPRT(7,"Prelim pav_pts=[%d,%d], Dpts=[%d,%d] \n",pav_pts[0],pav_pts[1],dpts[0],dpts[1] );

   /* Now check for a trump fit; if there is one we can count Dpts misfit or not.
    * If there is no trump fit then we might still play in a suit. e.g. 2M on a 5-2 fit. There are two scenarios:
    *   1) Our shortness (whether dummy or declarer) is NOT in one of partner's suits, so we can count for distribution.
    *   2) Our shortness is in one of partner's suits. In that case we count only the adjusted hcp value not the shortness.
    */

/* Dont use Do_Trump_fit here since we also need to adjust other parts of the hand evaluation -- picking Dpts or not */
  trump = trump_fit_chk(phs) ; /* fill the structure describing the trump fit */
   if ( trump.tsuit >= 0 ) {
         /* if there is a fit, no need to correct for misfits (if any) so just choose Dpts or adjusted HCP whichever is greater */
        for (h = 0 ; h < 2 ; h++ ) {
           pav_pts[h] = body_pts[h];
           for (s=CLUBS; s<=SPADES; s++ ){
               tmp_pts = roundf(hcp_suit[h][s]+fhcp_suit_adj[h][s]); /* either hcp after shortness adjustments */
               if (dpts_suit[h][s] > tmp_pts ) { tmp_pts = dpts_suit[h][s] ; }      /* shortness pts, whichever is greater */
               pav_pts[h] += tmp_pts ;
            }
         } /* end for h -- pav_pts ready for Dfit and Fn*/

        /* if there is a trump fit, add Dfit and Fn Points to the already included Dpts  */
        /* Assume that Declarer and Dummy are defined as per PAV. Decl=Longest trumps or if equal the hand with the most HCP */
        /* If its a 4-4 fit (equal len trumps, only Dummy gets Dfit, Declarer gets Fn (side suit points) */

      dfit_pts[0] = 0 ; dfit_pts[1] = 0 ;
      Fn_pts[0]   = 0 ; Fn_pts[1]   = 0 ;

      if(seat[0] == trump.dummy) {h_dummy = 0 ; h_decl = 1 ;  }
      else                       {h_dummy = 1 ; h_decl = 0 ;  }
      phs_dummy = phs[h_dummy] ;
      phs_decl  = phs[h_decl]  ;
      dfit_pts[h_dummy] = DfitPAV (phs_dummy, trump ) ; /* 4+T:2/1/0 These are in addition to Dpts awarded ealier */
      Fn_pts[h_decl]    = Fn_ptsPAV(phs_decl, trump ) ; /* 5T: V+=2,S+=1 ; 6+T V+=3, S=+2, D+=1 (each dblton) */
      JGMDPRT(7, "PAV: dfit for dummy[%d]=%d, FN for Decl[%d]=%d\n",h_dummy,dfit_pts[h_dummy],h_decl,Fn_pts[h_decl]);
      if (  (1 == phs_dummy->square_hand) &&
            (0 == phs_dummy->hs_totalcounts[Aces] ) &&
            (trump.tlen[h_dummy] < 4 ) ) {
         hand_adj[h_dummy] = -1 ;
      }
      pav_pts[h_dummy] += dfit_pts[h_dummy] + hand_adj[h_dummy];
      pav_pts[h_decl]  += Fn_pts[h_decl] ;
      JGMDPRT(7,"PAV TrumpFit Deal=%d, in suit=%d Len:%d Decl=[%d], Fn=%d Hldf=%d Dummy=%d, Dfit=%d Hldf=%d\n",
            gen_num,trump.tsuit,trump.tlen[0]+trump.tlen[1],h_decl,Fn_pts[h_decl],pav_pts[h_decl], h_dummy,dfit_pts[h_dummy],pav_pts[h_dummy] );

   } /* End Trump fit pav_pts all done for this case */
   else {  /* Since there is no fit, Check for misfits which cannot be ignored. */
      /* If there is shortness vs pards long (4+) suit, and there is no trump fit, do not count for the shortness */
      pav_pts[0] = body_pts[0] ; pav_pts[1] = body_pts[1] ;
      for (s=CLUBS; s<=SPADES; s++ ) {
         misfit[s] = misfit_chk(phs , s) ; /* return results in a struct */
         if (misfit[s].mf_type > 3) {  /* Misfit; ignore dpts, count only corrected hcp -- one of the hands will not be short */
               pav_pts[0] += roundf(hcp_suit[0][s]+fhcp_suit_adj[0][s]);
               pav_pts[1] += roundf(hcp_suit[1][s]+fhcp_suit_adj[1][s]);
               JGMDPRT(7,"PAV MISFIT Deal=%d, in suit=%d Type:%d sh_len=%d], pav_pts[0]%d, pav_pts[1]=%d",
                  gen_num,s,misfit[s].mf_type,misfit[s].sh_len,pav_pts[0], pav_pts[1] );
         }
         else {  /* No misfit; count the greater of the corrected hcp, or Dpts. */
               tmp_pts = roundf(hcp_suit[0][s]+fhcp_suit_adj[0][s]);              /* either hcp after shortness adjustments */
               if (dpts_suit[0][s] > tmp_pts ) { tmp_pts = dpts_suit[0][s] ; }    /* or shortness pts, whichever is greater */
               pav_pts[0] += tmp_pts ;
               tmp_pts = roundf(hcp_suit[1][s]+fhcp_suit_adj[1][s]);              /* either hcp after shortness adjustments */
               if (dpts_suit[1][s] > tmp_pts ) { tmp_pts = dpts_suit[1][s] ; }    /* or shortness pts, whichever is greater */
               pav_pts[1] += tmp_pts ;
               JGMDPRT(7,"PAV  No MISFIT Deal=%d, in suit=%d Counting shortness or Corrected HCP pav_pts[0]=%d, pav_pts[1]=%d\n",
                     gen_num,s, pav_pts[0], pav_pts[1] );
         }
      } /* end for each suit */
   } /* end check for misfit */

   /* pav_pts[] now done; [Dpts or (hcp+hcp_adj)] + body + Dfit + Fn + hand_adj */
   UEv.hldf_pts_seat[0] = pav_pts[0];
   UEv.hldf_pts_seat[1] = pav_pts[1];
   UEv.hldf_pts_side = 0 ;
   UEv.hldf_pts_side = UEv.hldf_pts_seat[0] + UEv.hldf_pts_seat[1] ;

/* now some debugging fields Standard set even tho some don't apply to this metric */
   UEv.misc_count = 0 ;
      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[0];
      UEv.misc_pts[UEv.misc_count++] = lpts[0];
      UEv.misc_pts[UEv.misc_count++] = body_pts[0];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[0];

      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[1];

      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dpts[0];       /* distribution aka shortness pts before a fit is found */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      UEv.misc_pts[UEv.misc_count++] = hand_adj[0];  /* square, aceless dummy with minimal (<4) fit */

      UEv.misc_pts[UEv.misc_count++] = dpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[1];

  SaveUserVals( UEv , p_uservals ) ;

  return ( 6 + UEv.misc_count ) ;
} /* end pav_calc */

/*
 * larsson_calc  -- syn* pts, body-pts, Lpts=(Len-5) max of 3,
 * -1 for short honors. Do not deduct for misfit;
 * 5-3-1 with 4 trumps
 */

int lar_calc( int side ) {  /* Tag Number: 8 */
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
         fhcp_adj[h] += shortHon_adj(p_hs, h, s, LAR ) ; /* -1 for K, Q, J, KQ, QJ, */
         JGMDPRT(8,"AdjLAR, Hidx=%d, suit=%d, hcp[s]=%d, Tot_Raw_hcp[h]=%g, Tot_hcp_adj=%g, SuitLen=%d\n",
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
      if ( (p_hs->hs_totalcounts[0] >= 2) || (p_hs->hs_totalcounts[0] == 1 && nines >= 2) ) {body_pts[h] = 1 ;}
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

      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[1];

      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dpts[0];       /* distribution aka shortness pts before a fit is found */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      UEv.misc_pts[UEv.misc_count++] = hand_adj[0];  /* square, aceless dummy with minimal (<4) fit */

      UEv.misc_pts[UEv.misc_count++] = dpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[1];
  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
     JGMDPRT(7, "Larsson fit calcs Done; Tsuit=%d, Decl=%c, HLDFpts[%d : %d], Fn_pts[%d:%d], Dfit_pts=[%d:%d]\n",
         trump.tsuit,"NSEW"[seat[h_decl]], larsson_pts[0],larsson_pts[1], Fn_pts[0],Fn_pts[1],dfit_pts[0],dfit_pts[1] );
  return ( 6 + UEv.misc_count ) ;
} /* end larsson_calc */

int morse_calc( int side ) {  /* Tag Number: 9 -- this is LAR with BumWrap HCP.*/
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
         morse_hcp[h][s] = calc_alt_hcp(p_hs, h, s, MORSE) ; /* calc BumWrap points for this suit. */
         fhcp[h] += morse_hcp[h][s] ;
         fhcp_adj[h] += shortHon_adj(p_hs, h, s, MORSE ) ; /* -1 for K, Q, J, KQ, QJ, */
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
      if ( (p_hs->hs_totalcounts[0] >= 2) || (p_hs->hs_totalcounts[0] == 1 && nines >= 2) ) {body_pts[h] = 1 ;}
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

      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[1];

      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dpts[0];       /* distribution aka shortness pts before a fit is found */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      UEv.misc_pts[UEv.misc_count++] = hand_adj[0];  /* square, aceless dummy with minimal (<4) fit */

      UEv.misc_pts[UEv.misc_count++] = dpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[1];
  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
     JGMDPRT(7, "morse fit calcs Done; Tsuit=%d, Decl=%c, HLDFpts[%d : %d], Fn_pts[%d:%d], Dfit_pts=[%d:%d]\n",
         trump.tsuit,"NESW"[seat[h_decl]], morse_pts[0],morse_pts[1], Fn_pts[0],Fn_pts[1],dfit_pts[0],dfit_pts[1] );
  return ( 6 + UEv.misc_count ) ;
} /* end morse_calc */

int dkp_calc( int side ) {  /* Tag Number: 2 */
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
         dkp_hcp[h][s] = calc_alt_hcp(p_hs, h, s, DKP) ; /* calc DKP LJP points for this suit. */
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
/* now some debugging fields Standard set even tho some don't apply to this metric */

      /* The factors that apply to both NT and Suit */
      UEv.misc_pts[UEv.misc_count++] = hcp_adj[0];
      UEv.misc_pts[UEv.misc_count++] = lpts[0];
      UEv.misc_pts[UEv.misc_count++] = body_pts[0];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[0];

      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[1];

      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dpts[0];       /* distribution aka shortness pts before a fit is found */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      UEv.misc_pts[UEv.misc_count++] = hand_adj[0];  /* square, aceless dummy with minimal (<4) fit */

      UEv.misc_pts[UEv.misc_count++] = dpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[1];
  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
     JGMDPRT(7, "DKP fit calcs Done; Tsuit=%d, Decl=%c, HLDFpts[%d : %d], Fn_pts[%d:%d], Dfit_pts=[%d:%d]\n",
         trump.tsuit,"NSEW"[seat[h_decl]], dkp_pts[0],dkp_pts[1], Fn_pts[0],Fn_pts[1],dfit_pts[0],dfit_pts[1] );
  return ( 6 + UEv.misc_count ) ;
} /* end dkp_calc */
int jgm1_calc( int side ) {  /* Alternate Tag Number: 4 Karpin method with BumWrap points*/
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
         jgm_hcp[h][s] = calc_alt_hcp(p_hs, h, s, JGM1) ; /* calc BumWrap points for this suit. */
         fhcp[h] += jgm_hcp[h][s] ;
         fhcp_adj[h] += shortHon_adj(p_hs, h, s, JGM1 ) ; /* -1 for K, Q, J, KQ, QJ, */
         JGMDPRT(8,"AdjJGM1, Hidx=%d, suit=%d, hcp[s]=%g, Tot_Raw_fhcp[h]=%g, Tot_fhcp_adj=%g, SuitLen=%d\n",
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
      JGMDPRT(7,"JGM1Karp hand=%d,jgm1_pts_net=%d, fhcp=%g, fhcp_adj=%g,   Lpts=%d,\n",
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
      UEv.misc_pts[UEv.misc_count++] = body_pts[0];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[0];

      UEv.misc_pts[UEv.misc_count++] = hcp_adj[1];
      UEv.misc_pts[UEv.misc_count++] = lpts[1];
      UEv.misc_pts[UEv.misc_count++] = body_pts[1];
      UEv.misc_pts[UEv.misc_count++] = syn_pts[1];

      /* Factors that apply to suit contracts only */
      UEv.misc_pts[UEv.misc_count++] = dpts[0];       /* distribution aka shortness pts before a fit is found */
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[0];   /* Dummy support pts typically 3-2-1 with 3, 5-3-1 with 4+ */
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[0];     /* Declarer side suit and extra trump length */
      UEv.misc_pts[UEv.misc_count++] = hand_adj[0];  /* square, aceless dummy with minimal (<4) fit */

      UEv.misc_pts[UEv.misc_count++] = dpts[1];
      UEv.misc_pts[UEv.misc_count++] = dfit_pts[1];
      UEv.misc_pts[UEv.misc_count++] = Fn_pts[1];
      UEv.misc_pts[UEv.misc_count++] = hand_adj[1];
  /* now put the results into the user result area at p_uservals */
  SaveUserVals( UEv , p_uservals ) ;
     JGMDPRT(6, "JGM1_Karp fit calcs Done; Tsuit=%d, Decl=%c, HLDFpts[%d : %d], Fn_pts[%d:%d], Dfit_pts=[%d:%d]\n",
         trump.tsuit,"NSEW"[seat[h_decl]], jgm1_pts[0],jgm1_pts[1], Fn_pts[0],Fn_pts[1],dfit_pts[0],dfit_pts[1] );
  return ( 6 + UEv.misc_count ) ;
} /* end jgm1_karp_calc */

int mixed_calc ( int side ) { /* call karpin_calc then jgm1_calc tag = 20 */
   int nkarpin_res = 0 ;
   int njgm_res = 0 ;
   int karpin_pts[6] = { 0 };
   int jgm_pts[6] = { 0 };
   int i ;
   nkarpin_res = karpin_calc( side ) ;
   for (i = 0 ; i < 6 ; i++ ) {
      karpin_pts[i] = p_uservals->u.res[i] ; /* save results since they will be overwritten by jgm1_calc */
   }
   njgm_res = jgm1_calc( side ) ;
   for (i = 0 ; i < 6 ; i++ ) {
      jgm_pts[i] = p_uservals->u.res[i] ; /* save results since they will be overwritten by lar_calc */
   }
   /* now copy the saved karpin results to the next available slots in the p_uservals area */
   for (i = 0 ; i < 6 ; i++ ) {
      p_uservals->u.res[i+6] = karpin_pts[i] ;
   }
   /* we should now have:
    * in slots 0 .. 5 for JGM:    NT_side, NT_north, NT_south, HLDF_side, HLDF_north, HLDF_south
    * in slots 6 ..11 for Karpin: NT_side, NT_north, NT_south, HLDF_side, HLDF_north, HLDF_south (overwriting jgm dbg vals)
    * The misc extra debug values in slots 12 .. nn are still there, but will be ignored by Dealer
    */
    return 12 ;
} /* end mixed_calc */

int mixed_LAR_calc ( int side ) { /* call lar_calc then jgm1_calc */
   int nlar_res = 0 ;
   int njgm_res = 0 ;
   int lar_pts[6] = { 0 };
   int jgm_pts[6] = { 0 };
   int i ;
   nlar_res = lar_calc( side ) ;
   for (i = 0 ; i < 6 ; i++ ) {
      lar_pts[i] = p_uservals->u.res[i] ; /* save results since they will be overwritten by jgm1_calc */
   }
   njgm_res = jgm1_calc( side ) ;
   for (i = 0 ; i < 6 ; i++ ) {
      jgm_pts[i] = p_uservals->u.res[i] ; /* save results since they will be overwritten by lar_calc */
   }
   /* now copy the saved lar results to the next available slots in the p_uservals area */
   for (i = 0 ; i < 6 ; i++ ) {
      p_uservals->u.res[i+6] = lar_pts[i] ;
   }
   /* we should now have:
    * in slots 0 .. 5 for JGM NT_side, NT_north, NT_south, HLDF_side, HLDF_north, HLDF_south
    * in slots 6 ..11 for LAR NT_side, NT_north, NT_south, HLDF_side, HLDF_north, HLDF_south (overwriting jgm dbg vals)
    * The misc extra debug values in slots 12 .. nn are still there, but will be ignored by Dealer
    */
    return 12 ;
} /* end mixed_calc */
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

int knr_calc ( int side ) { /* Tag Number 7 */
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
                  JGMDPRT(3,"KnR Misfit type=%d, Trump Fit Len=%d, short_hand=%d, ss_len=%d\n",
                           KnR_misfit.mf_type,trump_fit, sh, ss_len );
               }
               else { /* Not even a 7-fit; the hand will be played in NT; deduct ALL shortness pts */
                     KNR_pts[0].knr_misfit_adj -= KNR_pts[0].knr_short_pts ;
                     KNR_pts[1].knr_misfit_adj -= KNR_pts[1].knr_short_pts ;
                     JGMDPRT(3,"KnR Misfit type=%d, Trump Fit Len=%d, short_hand=%d, ss_len=%d BREAKING OUT \n",
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
      p_uservals->u.res[16]= KNR_pts[1].knr_honor_pts;
      p_uservals->u.res[17]= KNR_pts[1].knr_short_pts;
      p_uservals->u.res[18]= KNR_pts[1].knr_qual_pts;
      p_uservals->u.res[19]= KNR_pts[1].knr_tot_pts ;
      p_uservals->u.res[20]= KNR_pts[1].knr_dfit;
      p_uservals->u.res[21]= KNR_pts[1].knr_Fn_pts;
      p_uservals->u.res[22]= KNR_pts[1].knr_misfit_adj;
      p_uservals->u.res[23]= KNR_pts[1].knr_body_val ;
         /* also put main results in standard locations for possible use by other functions such as set88 etc.*/
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
int cpu_err(    int side ) { fprintf(stderr,"CPU Not Implemented. Returning -1\n"); return -1 ; }
int goren_err(  int side ) { fprintf(stderr,"Goren Not Implemented. Returning -1\n"); return -1 ; }
int kaplan_err( int side ) { fprintf(stderr,"Kaplan Not Implemented. Returning -1\n"); return -1 ; }
int sheinw_err( int side ) { fprintf(stderr,"Sheinwold Not Implemented. Returning -1\n"); return -1 ; }
int r22_err(    int side ) { fprintf(stderr,"Rule of 22 Not Implemented. Returning -1\n"); return -1 ; }
int zar_err(    int side ) { fprintf(stderr,"Zar Not Implemented. Returning -1\n"); return -1 ; }
int unkn_err(   int side ) { fprintf(stderr,"Unknown Metric Not Implemented. Returning -1\n"); return -1 ; }

 char FIND88[]="8888 SET 8888";  /*marker in case we want to patch next array with GDB */
 //              B,C,D, G,J,E, K,k,L,M,P,R ,S, Z    1 means include in set, zero skip.
 int  set88[20]={1,0,1,-0,1,-0,1,1,1,1,1,-0,-0,-0,-0 } ;
 #define SET88_SZ sizeof(set88)/sizeof(int)

typedef int (*pCALC_FUNC_k)( int ) ;
pCALC_FUNC_k p_cfunc[] = {bergen_calc, cpu_err, dkp_calc, goren_err, jgm1_calc,
                           kaplan_err , karpin_calc, knr_calc, lar_calc, morse_calc,
                           pav_calc, r22_err, sheinw_err, zar_err, unkn_err  } ;
 char cf_name[][10] = {"Bergen", "CPU", "DKP", "!Goren","JGM1","!Kaplan","Karpin","KnR","Larsson","Morse",
                            "Pavlicek","!Rule_22","!Sheinwold","!Zar","!Unkn" };
/* Tag Number 88 -- do all the metrics in set88 with set88[m] == 1 and return results in slots 0 .. set88_sz - 1 */
/* if metric 'm' implemented the value for the side will be put into slot 'm'; if not implemented slot m will be -1 */
int set88_calc (int side ) {
   int m;
   int m_cnt = 0 ;
   USER_VALUES_k *p_88_results;
   int set88_pts[SET88_SZ] = { -1 };
   int set89_pts[SET88_SZ] = { -1 };
   prolog( side ) ;
   p_88_results = p_uservals;
   for (m=0 ; m<SET88_SZ ; m++ ) {
      if (set88[m] == 1 ) {
         JGMDPRT(4," Metric=%d, Set88=%d, name=%s\n",m,set88[m],cf_name[m] );
         (*p_cfunc[m])( side ) ;  /* will return number of results calculated; we only care about the side total */
         set88_pts[m] = UEv.hldf_pts_side ;
         set89_pts[m] = UEv.nt_pts_side;
         m_cnt++;
         JGMDPRT(4,"%s_calc done; side pts=%d, valid_cnt=%d\n",cf_name[m],set88_pts[m],m_cnt ) ;
      }
   } /* Future -- tag89 will also run this code, then put if tag=89 use the set89_pts values */
   for (m=0; m<SET88_SZ; m++ ) {
      p_88_results->u.res[m] = set88_pts[m] ; /* some of these will be invalid i.e. -1; USER BEWARE */
   }

   return m_cnt; /* the number of valid results in the 0 .. SET88_SZ slots */
}




