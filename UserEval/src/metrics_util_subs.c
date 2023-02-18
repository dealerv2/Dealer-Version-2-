/* File metrics_util_subs.c */
/* 2022-12-21 JGM  Redid the misfit_chk routine. Removed the misfit and waste point calculations. Added compass[] to prolog */
#include "../include/std_hdrs_all.h"
#include "../include/UserEval_types.h"
#include "../include/UserEval_externs.h"
#include "../include/dbgprt_macros.h"
#include "../include/mmap_template.h"
void zero_globals ( void ) {
   size_t sz ;
   for (int h = 0 ; h < 2 ; h++ ) {
      hcp[h] = 0 ;
      hcp_adj[h] = 0 ;
      dpts[h] = 0 ;
      lpts[h] = 0 ;
      syn_pts[h] = 0 ;
      hf_pts[h] = 0 ;
      dfit_pts[h] = 0 ;
      TFpts.df_val[h] = 0 ;
      Fn_pts[h] = 0 ;
      TFpts.fn_val[h] = 0 ;
      hand_adj[h] = 0 ;
      body_pts[h] = 0 ;
      seat_hdlf_pts[h] = 0 ;
      seat_nt_pts[h] = 0 ;
      fhcp[h] = 0.0;
      fhcp_adj[h] = 0.0 ;

  } /* end for h=0 or 1 */
   side_nt_pts = 0;
   side_hldf_pts = 0 ;
   memset(&UEv , 0 , sizeof(UEv)  ) ;
   memset(&trump, 0, sizeof(trump)) ;
   sz = sizeof(struct misfit_st) * 4 ;
   memset(&misfit, 0, sz );
  return ;
} /* end zero globals */

void prolog ( int side ) {  /* Server mainline has filled the ptrs struct of type mmap_ptrs_st  */
   gen_num = ptrs.dldata->curr_gen ;
   prod_num = ptrs.dldata->curr_prod;
   zero_globals() ;

   if (SIDE_NS == side ) {
      seat[0] = COMPASS_NORTH ;
      seat[1] = COMPASS_SOUTH ;
      phs[0] = ptrs.phs[COMPASS_NORTH] ;
      phs[1] = ptrs.phs[COMPASS_SOUTH] ;
      compass[0] = 'N';
      compass[1] = 'S';
      p_uservals = ptrs.nsres ;
   }
   else  {
      seat[0] = COMPASS_EAST ;
      seat[1] = COMPASS_WEST ;
      phs[0] = ptrs.phs[COMPASS_EAST] ;
      phs[1] = ptrs.phs[COMPASS_WEST] ;
      compass[0] = 'E';
      compass[1] = 'W';
      p_uservals = ptrs.ewres ;
   }
   return ;
}
int arr_min(int arr[], size_t nelem ) {  /* really like to return both the min value and the index that points there. */
   int min = arr[0] ;
   for (int i = 1 ; i < nelem ; i++) {
      if (arr[i] < min ) {min = arr[i] ; }
   }
   return min ;
}

struct trump_fit_st trump_fit_chk( HANDSTAT_k *phs[] ) {
   /* check if there is an 8+ fit;
    * if yes choose suit of longest fit;
    * if two fits same len choose the Major;
    * if both fits are same rank, then choose the more symmetrical fit (i.e. 4=4 instead of 5=3, or 5=4 instead of 6=3
    * if both fits are the same rank, and same symmetry then choose the one with the FEWER total hcp.(Vondracek effect)
    *
    * We dont calc actual Dfit points here since there are so many different variations
    * If in a 4-4 fit we treat both hands as Dummy and call this routine twice, we will need to swap the Dummy / Declarer fields.
    */
#define ISMAJOR(s) ( (s) >= HEARTS  ) ? 1 : 0
#define ISMINOR(s) ( (s) <= DIAMONDS) ? 1 : 0
#define WEAKER(x,y) ( (x) < (y) ) ? (x) : (y)
    int s ;
    int fit_suit = -1  ;         /* no fit if fit_suit = -1 */
    int fit_rank = -1  ;
    int fit_len  = -1  ;
    int fit_diff = 13 ;         /* max possible */
    int new_rank =  0  ;
    int new_diff ;
    int new_flen  ;
    int hcp_new, hcp_old ;       /* choose the WEAKER trump suit; Vondracek effect */
    char p_du , p_dc ;           /* compass/player letters for debug statements */
    struct trump_fit_st trump ;
    trump.tsuit = -1 ;           /* set to -1 if no fit */
    trump.fit_len = -1 ;
    trump.tlen[0] = 0 ; trump.tlen[1] = 0 ;
    JGMDPRT(7,"Doing Trump Fit check for ngen=%d\n",gen_num );
    for (s = CLUBS ; s<= SPADES ; s++ ) {                         /* C 'continue' jumps back to the s++ statement */
        new_flen = phs[0]->hs_length[s] + phs[1]->hs_length[s] ;
        if (new_flen < 7 || new_flen < fit_len ) { continue ; } /* ignore fits worse than current one or < 7 */
        /* Check for semi fit; a semi fit must be 5-2 or 2-5 Moysian 4-3 or 6-1 dont count */
        if  (new_flen == 7 && phs[0]->hs_length[s] != 5 && phs[0]->hs_length[s] != 2 )  { continue ; } /* No semi-fit*/
        /*
         * There is at least one 8+ fit or a 5-2 semi fit.
         */
        if (new_flen > fit_len ) { /* the new fit is longer choose it */
           fit_len = new_flen ;
           fit_suit = s ;
           fit_diff = abs( phs[0]->hs_length[s] - phs[1]->hs_length[s] );
           fit_rank = ISMAJOR(s) ;  /* 1=Yes, 0 = No */
            JGMDPRT(8,"Found New Fit;New suit=[%d:%c], Major=%c, fit_len=%d fit_diff=%d len[0]=%d, len[1]=%d\n",
              s, "CDHS"[s], "NY"[fit_rank], fit_len, fit_diff,  phs[0]->hs_length[s], phs[1]->hs_length[s] );
            continue ;
        }

        /* two fits same length  Choose between them; Major over minor else more balanced, else weaker trump suit*/
        new_diff = abs( phs[0]->hs_length[s] - phs[1]->hs_length[s] ) ;
        new_rank = ISMAJOR(s) ;
        JGMDPRT(8,"Two fits Len=%d OldSuit[%c] is %c fit_diff=%d NewSuit[%c] is %c new_diff=%d\n", fit_len,
            "CDHS"[fit_suit],"mM"[fit_rank],fit_diff,"CDHS"[s],"mM"[new_rank], new_diff );
        if  ( new_rank > fit_rank ) { /* Major over minor? choose it */
               fit_suit = s ;
               fit_rank=new_rank;
               fit_diff=new_diff;
               fit_len = new_flen;
               continue ;
         }
         /* since suits tested in order C,D,H,S it should never happen that the new_rank is LESS than the old one.*/
         else if ( new_rank < fit_rank ) { continue ; } /* New fit is minor; old fit was major; ignore new one.*/

         // the two longest so far are same rank -- choose the more balanced;
         if ( new_diff < fit_diff ) { // the new suit is more balanced choose it
               fit_suit = s ;
               fit_rank = new_rank;
               fit_diff = new_diff;
               fit_len  = new_flen;
               continue ;
         }
         else if ( new_diff > fit_diff ) { continue ; } /* new fit is less balanced; ignore it */
         // same length fit, same rank, same balance 3=5 & 5=3 or both 4=4 etc.
         // Vondracek effect choose fit with the least hcp.
         hcp_new = phs[0]->hs_points[s] + phs[1]->hs_points[s] ;
         hcp_old = phs[0]->hs_points[fit_suit] + phs[1]->hs_points[fit_suit] ;
         if ( hcp_new < hcp_old ) { /* choose the weaker of the two suits all else being equal. Vondracek effect */
            fit_suit = s ;
            fit_rank=new_rank;
            fit_diff=new_diff;
            fit_len = new_flen;
            continue ;
        }
        /* Two fit suits essentiall same rank, len, balance, but old one is weaker (and therefore chosen) than new one.*/
      }    /* end for checking all suits */
    if (fit_suit >= 0 ) {
         JGMDPRT(7,"FitCheck Done! Fit or SemiFit Found in suit=%c, rank=%c fit_len=%d fit_diff=%d Setting Up Trump struct\n",
               "CDHS"[fit_suit], "mM"[fit_rank], fit_len, fit_diff );

         trump.tsuit = fit_suit ;

         trump.tlen[0] = phs[0]->hs_length[fit_suit] ;
         trump.tlen[1] = phs[1]->hs_length[fit_suit] ;
         trump.ss_len[0] = arr_min(phs[0]->hs_length, 4) ;
         trump.ss_len[1] = arr_min(phs[1]->hs_length, 4) ;
         trump.fit_len = fit_len ;
        JGMDPRT(8,"Trump Fit Check; Trump Struct filled. fit_suit=%d ss_len[0]=%d, ss_len[1]=%d, tlen[0]=%d, tlen[1]=%d,Fit_len=%d\n",
            trump.tsuit, trump.ss_len[0], trump.ss_len[1], trump.tlen[0], trump.tlen[1], trump.fit_len );
         /* Set Declaring hand. The longer trumps are the Declarer. If same length, the stronger hand is Declarer */
         if      (phs[0]->hs_length[fit_suit] > phs[1]->hs_length[fit_suit] ) {
            trump.decl    = seat[0] ;
            trump.dummy   = seat[1] ;
            p_dc = compass[0];
            p_du = compass[1];
         }
         else if (phs[1]->hs_length[fit_suit] > phs[0]->hs_length[fit_suit] ) {
            trump.decl  = seat[1] ;
            trump.dummy = seat[0] ;
            p_dc = compass[1];
            p_du = compass[0];
         }
         else if (phs[0]->hs_totalpoints >  phs[1]->hs_totalpoints ) {
            trump.decl  = seat[0] ;
            trump.dummy = seat[1] ;
            p_dc = compass[0];
            p_du = compass[1];
         }
         else {
            trump.decl  = seat[1] ;
            trump.dummy = seat[0] ;
            p_dc = compass[1];
            p_du = compass[0];
         }
         JGMDPRT(7,"TrumpFit Check done; fitsuit=%d, trump.decl=%c, trump.dummy=%c\n",trump.tsuit, p_dc, p_du );
   }
   else {
         JGMDPRT(7,"FitCheck Done! No 8+ fit or 5-2 semi fit found; trump.tsuit = %d \n",trump.tsuit);
         trump.fit_len = -1 ; /* a no-op to prevent complaints about an empty else clause */
   }
   return (trump) ;  // return the trump_fit structure; trumps.fit_suit = -1 if no fit. and no semi fit
}  /* end trump_fit() */
/* end trump_fit_chk() */
/*
 * Fill the misfit structure;
 * Called once for each suit; Will use this struct to figure out Misfit pts, and/or waste/nowaste points if any
 * no_waste values are coded: 3 => xxx  vs void,  2 => xxx vs stiff, 1 => Ax(x) vs stiff, 0 there is no short, or there is waste.
 * waste values are coded:   -3 => H(x) vs void, -2 => H(x) vs stiff
 * misfit values are coded:   0 => no misfit, 5 => shortness vs 5+ suit, 4 => short vs 4 card suit, 3 => short vs 3 card suit
 */
struct misfit_st misfit_chk(HANDSTAT_k *phs[], int s ) {   /* Assume Void or Stiff vs 5+ suit; OPC says also xx vs 5+ suit */
   int lh, sh ;
   struct misfit_st mf = {.mf_type = 0, .waste = 0, .no_waste = 0  } ;
   /* misfit fields are the hand-index (0,1) not the compass value (0..3) */
   JGMDPRT(8,"Deal=%d, seat[0]=%c, Misfit Check for suit=%d Entered \n", gen_num, "NSEW"[seat[0]], s);
      mf.no_waste = 0 ; mf.waste = 0 ; mf.mf_type = 0 ;
      sh = 0 ; lh = 1 ; /* assume   */
      if (phs[0]->hs_length[s] >= phs[1]->hs_length[s] ) { lh = 0 ; sh = 1; }
      /* now proceed to check if the shorter hand has shortness - stiff or void -- in this suit */
      if (0 == phs[sh]->hs_length[s] ) {  /* Is there a void in the sh hand? */
         if    (  (0 == phs[lh]->hs_points[s]) && (phs[lh]->hs_length[s] >= 3) ) { mf.no_waste = 3; } /*xxx vs void */
         else if (0 <  phs[lh]->hs_points[s] ) { mf.waste = -3 ; } /* wasted values vs void */

         if       (phs[lh]->hs_length[s] >= 5) { mf.mf_type = 5 ; } /* void vs 5+ suit */
         else if  (phs[lh]->hs_length[s] == 4) { mf.mf_type = 4 ; } /* void vs 4+ suit */
         else if  (phs[lh]->hs_length[s] == 3) { mf.mf_type = 3 ; } /* void vs 3+ suit may not apply; or may apply to LAR */
         else     { mf.mf_type = 0 ; }                              /* no misfit vs suits 2 or less */
         mf.short_hand = sh; mf.sh_len = 0 ;
         mf.long_hand = lh;  mf.lh_len = phs[lh]->hs_length[s]; mf.lh_hcp=phs[lh]->hs_points[s];
          JGMDPRT(8,"Hand=%d VOID in %c ; lh_len=%d, lh_hcp=%d, waste=%d, Type=%d, \n",
             mf.short_hand, "CDHS"[s], mf.lh_len, mf.lh_hcp, mf.waste, mf.mf_type );      } /* end void */
      else if (phs[sh]->hs_length[s] == 1) {  /* Is there a stiff ? */
         if      ((0 == phs[lh]->hs_points[s]) && (phs[lh]->hs_length[s] >= 3) ) { mf.no_waste = 2; } /* xxx vs stiff */
         else if ((4 == phs[lh]->hs_points[s]) && (phs[lh]->Has_card[s][Ace_rk]) && (phs[lh]->hs_length[s] >= 2)) { /* Ax(x) vs stiff*/
                        mf.no_waste = 1;
         }
         else if (0 <  phs[lh]->hs_points[s] ) { mf.waste = -2 ;  } /* H(x) vs stiff */

         if      (phs[lh]->hs_length[s] >= 5) { mf.mf_type = 5 ; } /* stiff vs 5+ suit */
         else if (phs[lh]->hs_length[s] == 4) { mf.mf_type = 4 ; } /* stiff vs 4+ suit */
         else if (phs[lh]->hs_length[s] == 3) { mf.mf_type = 3 ; } /* stiff vs 3+ suit may not apply; or may apply to LAR */
         mf.short_hand = sh;  mf.sh_len = 1 ;
         mf.long_hand  = lh;  mf.lh_len = phs[lh]->hs_length[s];
         mf.lh_hcp=phs[lh]->hs_points[s];
         JGMDPRT(8,"Hand=%d STIFF in %c ; lh_len=%d, lh_hcp=%d, waste=%d, Type=%d, \n",
             mf.short_hand, "CDHS"[s], mf.lh_len, mf.lh_hcp, mf.waste, mf.mf_type );
      } /* end stiff */
      JGMDPRT(8,"Misfit/Waste Check Done:gen_num=%d, suit=%d misfit=%d, waste=%d, no_waste=%d, lh_len=%d, sh_len=%d, lh_hcp=%d \n",
           gen_num, s,  mf.mf_type, mf.waste, mf.no_waste, mf.lh_len, mf.sh_len, mf.lh_hcp ) ;
   return (mf) ;
} /* end mis_fit() */

/* 6 most likely results and optionally several components results for possible debugging */
void SaveUserVals(struct UserEvals_st UEv , USER_VALUES_k *p_ures ) {
   int j, n ;
   JGMDPRT(7, "SavingUserVals: @UserRes= %p, NT:%d,%d,%d  HLDF:%d,%d,%d Number of Misc=%d\n", (void *)p_uservals,
         UEv.nt_pts_side,UEv.nt_pts_seat[0],UEv.nt_pts_seat[1],
         UEv.hldf_pts_side,UEv.hldf_pts_seat[0],UEv.hldf_pts_seat[1], UEv.misc_count );

   p_ures->u.res[0] = UEv.nt_pts_side ;
   p_ures->u.res[1] = UEv.nt_pts_seat[0] ;
   p_ures->u.res[2] = UEv.nt_pts_seat[1] ;
   p_ures->u.res[3] = UEv.hldf_pts_side ;
   p_ures->u.res[4] = UEv.hldf_pts_seat[0] ;
   p_ures->u.res[5] = UEv.hldf_pts_seat[1] ;
   n = 6 ;
   for (j = 0 ; j < UEv.misc_count ; j++, n++ ) {  /* copy over the misc fields if any */
      p_ures->u.res[n] = UEv.misc_pts[j] ;
   }
   return ;
}
//pbn fmt suit sep is dot, hand sep is space. voids are deduced from that.
/* hand sorted. dl[p*13+0] = Highest Spade; dl[p*13+12] = lowest club. */
char *Hand52_to_pbnbuff (int p, char *dl, char *buff ) {
   char r_ids[] = "23456789TJQKA";
   int curr_suit, card_rank, card_suit;
   int di, count;
   char *bp ;
   unsigned char kard ;
   char suit_sep = '.';
   di = p*13 ;
   bp = buff ;
   count = 0 ;
   curr_suit = 3 ; // spades
   while (count < 13 ) {  // a hand ALWAYS has exactly 13 cards
       kard = dl[di] ; card_suit = C_SUIT(kard); card_rank = C_RANK(kard) ;
       while( curr_suit != card_suit ) { /* write a suit separator for missing suits spades downto first one*/
            *bp++ = suit_sep;
            JGMDPRT(9, "Wrote Void for suit %d \n",curr_suit ) ;
            curr_suit-- ;
        } /* end while curr_suit != card_suit */
        assert(card_suit == curr_suit) ;
        while ( (curr_suit == card_suit) && (count < 13) ) { /* write the cards in this suit */
            kard = dl[di]; card_suit = C_SUIT(kard); card_rank = C_RANK(kard) ;
            if (curr_suit != card_suit ) break;
           *bp++ = r_ids[card_rank];
           count++; di++;
           // JGMDPRT(10," Card Num[%d] in suit = %c%c written to pbnbuff", count, "CDHS"[curr_suit], *(bp-1) ) ;
        } // end while curr_suit
        JGMDPRT(10,"\n");
       *bp++ = suit_sep;
        curr_suit-- ; /* Move to next suit */
    } /* end while count < 13*/
    assert(count == 13 ) ;
    // Normal case curr_suit is -1; void clubs curr_suit = 0, void clubs, diamonds, and hearts curr_suit = 2
    // In case there were voids at the end of 13 cards
        while ( curr_suit >= 0 ) { /* write a suit separator for missing suits after the last one downto clubs*/
            *bp++ = suit_sep ;
            curr_suit-- ;
        }
        /* the last char is the suit separator which we don't need after the club suit, so replace it with a space */
        if ( *(bp-1) == suit_sep ) { *(bp-1) = ' ' ; }
        else { fprintf(stderr, "CANT HAPPEN in Hand52_to_Buff, last char is not a suit_separator %c \n", *(bp-1) ); }
        *bp = '\0' ; // terminate the buffer as a string
        return bp  ; /* return pointer to null byte in case we want to append another hand to the buffer */
} /* end Hand52_to_pbnbuff */

/* no test for jgmDebug since that should be done before calling if reqd */
void show_hands_pbn( int mask, deal d ) {
/* Clone of printhands_pbn in action subs.c except always goes to stderr */
  char pt[] = "nesw";
  int  p;
  char pbn_buff[128], *pbn_ptr ;
  pbn_ptr=pbn_buff ;
  for (p=0 ; p< 4; p++ ) {
     if ( !(mask & 1 << p ) ) continue ; /* skip this player if he was not called for */
    *pbn_ptr++ = pt[p]; *pbn_ptr++ = ' ' ; // player names are followed by a space */
    pbn_ptr = Hand52_to_pbnbuff (p, (char *)d, pbn_ptr ); // append a hand to end of pbnbuff; returns ptr to null at end.
  }
  /* pbnbuff formatted now print it out */
  *pbn_ptr++ = '\n'; *pbn_ptr='\0';
  fprintf(stderr, "%s",pbn_buff ) ; /* no newline at this point. */

} /* end show_hands_pbn */




inline void swap(int* xp, int* yp) {
   int temp = *xp;
   *xp = *yp;
   *yp = temp;
}
/* simple  Descending Order Ace to Deuce Insertion sort:
 * Test show that for 13 elems Insertion is faster than Shell or Selection
 * for a 52 element array as opposed to 4x13 elements Shell sort is about 10% faster than Insertion */
void sortHand52(card *hnd_ptr, int size) {
    int key, j, step;
    /* we can use this to sort the 'hands' in deal. pass it deal[0], deal[13], deal[26] or deal[39] with size of 13 */
  for (step = 1; step < size; step++) {
    key = *(hnd_ptr + step);
    j = step - 1;
    while (key > *(hnd_ptr + j) && j >= 0) {
      *(hnd_ptr + j+1) = *(hnd_ptr + j);
      --j;
    }
    *(hnd_ptr + j+1) = key;
  } /* end for step */
} /* end sortHand */

void sortDeal52(deal dl ) {
    int p ;
    card *h_ptr ;
    JGMDPRT(8, "Sorting Deal52 Desc Order c[0]=%02x c[51]=%02x\n",dl[0],dl[51] );
    for (p = 0 ; p < 4 ; p++ ) { /* p is the player, 0=North, etc. We sort each quarter of the deal separately */
        h_ptr = &dl[p*13];
        sortHand52( h_ptr, 13 ) ;
    }
} /* end sortDeal */

// maybe not used -- selection sort to get suit numbers in order of short suit to long; e.g. 3=4=5=1 -> 3,0,1,2
void selectionSort(int arr[], int n, int idx_list[]) {
   int i, j, min_idx;
   for (i = 0; i < n - 1; i++) {
   min_idx = i;
      for (j = i + 1; j < n; j++) {
         if (arr[j] < arr[min_idx]) { min_idx = j; }
      }
      // Swap the found minimum element with the first element, and swap the indices also
      swap(&arr[min_idx], &arr[i]);
      swap(&idx_list[min_idx], &idx_list[i] ) ;
   } /* end for i */
} /* end selection sort */

float_t quick_tricks(HANDSTAT_k *phs ) {
   int s ;
   int w1, w2;
   float qt = 0.0;
   for (s=CLUBS ; s<=SPADES; s++ ) {
      w1 = phs->topcards[s][0] ;
      w2 = phs->topcards[s][1] + w1 ;
      if ( 0 == phs->hs_length[s] ) { continue ; }
      if ( 1 == phs->hs_length[s] && (32 == w1 ) ) { qt += 1.0 ; continue ; } /* stiff Ace */
      // else if (1 == phs->hs_length[s] && (16 == w1 ) ) { qt += 0.5 ; continue; } /* PAV says count 0.5 for stiff King */
      else if ( 2 <=  phs->hs_length[s] ) {
         switch ( w2 ) {                  /* top two cards only */
         case 48:             /* AK */
                  qt += 2.0 ; break ;
         case 40 :            /* AQ */
                  qt += 1.5 ; break ;
         case 36 :            /* AJ */
         case 34 :            /* AT */
         case 33 :            /* Ax */
         case 24 :            /* KQ */
                   qt += 1.0 ; break ;
         case 20 :            /* KJ */
         case 18 :            /* KT */
         case 17 :            /* Kx */
                   qt += 0.5 ; break ;
         default : break ;
         } /* end switch */
      } /* end else length */
   } /* end for suit */
   return qt ;
} /* end quick_tricks */
void sr_deal_show(deal real_dl ) { /* two line output of the cards in SuitRank */
    char rns[] = "23456789TJQKA-";
    char sns[] ="CDHS";
    char rn, sn , sp;
    int s, r , i ;
    deal dl ;
    memcpy(dl, real_dl, 52) ;
    sortDeal52(dl) ;
    i = 0;
    fprintf (stderr," NOCARD=[%02x] Showing Deal using CARD_SUIT and CARD_RANK in sr_deal_show\n",NO_CARD);
    fprintf (stderr,"SR1=[");
   for (i=0; i<52 ; i++ ) {
       if (dl[i] == NO_CARD) { sn='-';rn='x' ; }
       else {
          s=CARD_SUIT(dl[i]); sn=sns[s];
          r=CARD_RANK(dl[i]); rn=rns[r];
       }
       sp = ((i+1)%13 == 0) ? ':' : ' ';
       fprintf(stderr,"%c%c%c", sn,rn,sp );
       if ( 25 == i ) fprintf(stderr, "\n     ");

    }
    fprintf (stderr,"]\n");
    fsync(2);
} /* end sr_deal_show */

void dump_curdeal( void ) { /* ouput deal in easy to read, verbose fmt Uses the Has_card[][] array in hand stat*/
    int  pnum , cardcount;
    char *pname="NESW";
    char *sname="CDHS";
    char *rname="23456789TJQKA";
    int r, s;
    char sid ;
    pnum = 0 ;
    /* using globals hs_ptr[4] to access the hand stat */

    cardcount = 0;
    char pn, rn ;
    pn='*'; rn='-';
    /* this is already a debugging routine; no need to use DBGPRT -- all output goes to stderr */
    fprintf(stderr, "Debugging Deal in dump_curdeal; ngen=%d, nprod=%d\n",ptrs.dldata->curr_gen, ptrs.dldata->curr_prod );
 //    for (pnum = COMPASS_NORTH; pnum <= COMPASS_NORTH; pnum++) {
    for (pnum = COMPASS_NORTH; pnum <= COMPASS_WEST; pnum++) {
        cardcount = 0;
        pn = pname[pnum];
        fprintf(stderr, "\ndump_curdeal Looping for Player number %d : %c ",pnum, pname[pnum]);
        fprintf (stderr,  " Player[%c ] \n", pname[pnum]);
        for (s = SPADES; s >= CLUBS; s--) {  /* s must be signed for this to work */
          assert(CLUBS <= s && s <= SPADES); /* if s is not signed this will not be true */
          sid = sname[s];
          fprintf(stderr, "Suit %2d=[%c] { ", s, sid);
          for (r = ACE; r >= TWO; r--) {     /* r goes from Ace to Deuce. r must be a signed int, not an unsigned */
              assert(TWO <= r && r<= ACE );  /* if r is not signed this will not be true */
              rn = rname[r];
              if(hs_ptr[pnum]->Has_card[s][r] ) {
                  fprintf(stderr, "%c%c ",sid, rn );
                  cardcount++ ;
              } /* end if haskard */
            } /* end for r = ACE */
            fprintf (stderr, "}\n");
        } /* end for s suit */
        fprintf(stderr, "Done for player%d=%c, found=%d\n", pnum, pn, cardcount);
        assert(cardcount == 13 );  /* must find 13 cards for each player */
  } /* end for player */
  fprintf(stderr, "----------------dump curr deal done ----------------\n");
  fsync(2) ;
} /* end dump curr deal */

/* Honor combinations, and length => playing tricks Used when Pre-Empting at 3+ level*/
float_t PAV_playing_tricks (HANDSTAT_k *phs ) {
   float_t ptrix = 0.0 ;
   int w1, w2, w3 ; /* weights of the top 3 cards in the suit; */
   int s;
   int s_len ;
   for (s=CLUBS ; s<=SPADES; s++ ) {
      s_len = phs->hs_length[s] ;
      w1 = phs->topcards[s][0] ;          /* Highest card might be just 1 for a spot */
      w2 = phs->topcards[s][1] + w1 ;     /* Top two cards; might be just 2 for two spots */
      w3 = phs->topcards[s][2] + w2 ;     /* Top three cards; might be just 3 for three spots */
      if ( 0 == s_len ) { continue ; }
      if ( 1 == s_len ) {
         switch ( w1 ) {
            case 32 : ptrix += 1.0 ; continue ; /* stiff A */
            case 16 : ptrix += 0.5 ; continue ; /* stiff K */
            default : continue ;
         }
      } /* end len == 1 */
      else if (2 == s_len ) {
         switch ( w2 ) {
            case 48 : ptrix += 2.0 ; continue ; /* AK */
            case 40 : ptrix += 1.5 ; continue ; /* AQ */
            case 36 : ptrix += 1.5 ; continue ; /* AJ */
            case 34 : ptrix += 1.0 ; continue ; /* AT */
            case 33 : ptrix += 1.0 ; continue ; /* Ax */
            case 24 : ptrix += 1.0 ; continue ; /* KQ */
            case 20 : ptrix += 0.5 ; continue ; /* KJ */
            case 18 : ptrix += 0.5 ; continue ; /* KT */
            case 17 : ptrix += 0.5 ; continue ; /* Kx */
            case 12 : ptrix += 0.5 ; continue ; /* QJ */
            case 10 : ptrix += 0.5 ; continue ; /* QT */
            case  9 : ptrix += 0.5 ; continue ; /* Qx */
            default : continue ;
         }
      } /* end len == 2 */
      else { /* len >= 3 */
         switch ( w3 ) {
            case 56 : ptrix += 3.0 ; break ;    /* AKQ */
            case 52 : ptrix += 2.5 ; break ;    /* AKJ */
            case 50 : ptrix += 2.0 ; break ;    /* AKT */
            case 49 : ptrix += 2.0 ; break ;    /* AKx */
            case 44 : ptrix += 2.5 ; break ;    /* AQJ */
            case 42 : ptrix += 2.0 ; break ;    /* AQT */
            case 41 : ptrix += 1.5 ; break ;    /* AQx */
            case 38 : ptrix += 1.5 ; break ;    /* AJT */
            case 37 : ptrix += 1.5 ; break ;    /* AJx */
            case 35 : ptrix += 1.0 ; break ;    /* ATx */
            case 34 : ptrix += 1.0 ; break ;    /* Axx */
            case 28 : ptrix += 2.0 ; break ;    /* KQJ */
            case 26 : ptrix += 1.5 ; break ;    /* KQT */
            case 25 : ptrix += 1.0 ; break ;    /* KQx */
            case 22 : ptrix += 1.5 ; break ;    /* KJT */
            case 21 : ptrix += 1.0 ; break ;    /* KJx */
            case 19 : ptrix += 0.5 ; break ;    /* KTx */
            case 18 : ptrix += 0.5 ; break ;    /* Kxx */
            case 14 : ptrix += 1.0 ; break ;    /* QJT */
            case 13 : ptrix += 1.0 ; break ;    /* QJx */
            case 11 : ptrix += 0.5 ; break ;    /* QTx */
            case 10 : ptrix += 0.5 ; break ;    /* Qxx */
            case  7 : ptrix += 0.5 ; break ;    /* JTx */
            default : break ;
         } /* end switch ( w3 ) */
         /* Now add playing tricks for extra length */
         if (s_len > 3 ) { ptrix += s_len - 3 ; } /* extra playing trick for each card over 3 */
      } /* end else len >= 3 */
   } /* end for each suit */

   return ptrix ;
} /* end PAV_playing_tricks */


/* side stuff -ve or 0 - 999 ; suits clubs=100+, diam=200+ etc. hand[0]= 1000+, hand[1] = 2000+ */
int make_test_evals(struct detailed_res_st *p_ures) {
   int idx, s, pts ;
   for (idx=0 ; idx < 16 ; idx++ ) { p_ures->side_tot[idx] = -(idx+1) ; }
   pts = 0 ;
   for (s= 0 ; s<4 ; s++ ) {
      pts += 100 ;
      for (idx = 0 ; idx < 4 ; idx ++ ) {
         p_ures->side_by_suit[s][idx] = pts + idx ;
      }
   }
   for (idx = 0 ; idx < 8; idx++ ) {
      p_ures->hand_tot[0][idx] = 1000 + idx ;
      p_ures->hand_tot[1][idx] = 2000 + idx ;
   }
   pts = 0 ;
   for (s = 0 ; s< 4 ; s++ ) {
      pts += 100 ;
      p_ures->hand_suit[0][s][0] = 1000 + pts;
      p_ures->hand_suit[0][s][1] = 1001 + pts;
      p_ures->hand_suit[1][s][0] = 2000 + pts;
      p_ures->hand_suit[1][s][1] = 2001 + pts;
   }
   return 64 ;
}

