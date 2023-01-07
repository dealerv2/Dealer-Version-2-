/* File dealcard_subs.c
 * Code related to dealing and shuffling the cards. Pulled out of dealeval_subs when dealing re-organized
 * Date        Version  Author   Comment
 * 2023/01/06     1.0   JGM      Sorting deal to facilitate DDS functions broke the Predeal logic of previous code. Majore rewrite needed
 */
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
#include "../include/std_hdrs.h"
#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/dealexterns.h"
#include "../include/dealprotos.h"
#include "../include/dealdebug_subs.h"   /* for DBGPRT, etc.? */
void swap2 (deal d, int p1, int p2) { /*keep for swapping */
  /* functions to assist "simulated" shuffling with player
     swapping or loading from Ginsberg's library.dat -- AM990423 */
  card t;
  int i;
  p1 *= 13;
  p2 *= 13;
  for (i = 0; i < 13; ++i) {
    t = d[p1 + i];
    d[p1 + i] = d[p2 + i];
    d[p2 + i] = t;
  }
} /* end swap2 */

/* Algorithm Per Knuth */
void Shuffle(deal deck, int sz ) {  /* used for curdeal or small_pack */
   card t ;
   int i, j;
   JGMDPRT(9,"Shuffle deck size=%d\n",sz ) ;
   for (i = 0; i < sz; i++) {         /* for each card in the   deck of interest ... */
        j = gen_rand_slot(sz) ;          /* generate a random slot to switch with */
        t = deck[j];
        deck[j] = deck[i];
        deck[i] = t;
    }    /* end for each card in the deck */
    if (jgmDebug >= 8 ) {
        fprintf(stderr, "*---- Post Shuffle Deal of size=%d  ----- *\n", sz);
        hexdeal_show(deck, sz) ;
    }
  }
/* put newly shuffled small_pack into curdeal which already has predeal cards in it from stacked_pack */
void merge_deal(deal d) {
   int j = 0 ;
   int i = 0 ;
   for (i = 0 ; i < 52 ; i++) {
      if (curdeal[i] == NO_CARD) {
         curdeal[i] = small_pack[j++] ; /* the non-predealt cards can go in any free slot in curdeal */
      }
   }
   assert( j == small_size ) ;              /* all of the leftover cards should have been copied to curdeal */
   return ;
}

/* take current deal, d, and change it either by shuffling, swapping, or stacking and shuffling */
void deal_cards(deal d ) {
   if (stacked_size == 0 && swapping == 0 ) {  /* the vanilla case; no predeal no swapping */
      Shuffle(d, 52) ;
      JGMDPRT(8,"Vanilla Case Shuffle Done. \n");
      return ;
   }
   /* Note if you have predeal (stacked_size >= 1 ) you cannot have swapping and vice versa */
   if (stacked_size >= 1 ) {
      memcpy(d , stacked_pack, 52 ) ;   /* initialize the deal with the predealt cards and NO_CARD otherwise */
      Shuffle(small_pack, small_size) ; /* Knuth algorithm on the remaining cards */
      merge_deal(d) ;                    /* merge the shuffled remaining cards into the curdeal setup by stacked_pack*/
      JGMDPRT(8,"Predeal Case Merge Deal Done. \n");
      return ;
   }
   /* Swapping should always be non zero here; if its not time for a new deal yet, then do the swap */
   if (swapping != 0 && swapindex != 0 ) {  //Swapindex gets set to zero after each swapping group so we gen a new random deal
         JGMDPRT(8,"Swapping=%d, Swapindex=%d \n",swapping, swapindex);
         switch (swapindex) {
         case 1:        swap2 (d, 1, 3);        break;
         case 2:        swap2 (d, 2, 3);        break;
         case 3:        swap2 (d, 1, 2);        break;
         case 4:        swap2 (d, 1, 3);        break;
         case 5:        swap2 (d, 2, 3);        break;
         }
      }
  else { /* swapping != 0 and swapindex == 0 so must shuffle a new random deal */
     JGMDPRT(8,"Swapping=%d, swapindex=%d (zero?) calling Shuffle for new random deal \n",swapping,swapindex ) ;
      Shuffle(d, 52) ; /* per Knuth algorithm */
  }
  if(swapping != 0 ) { ++swapindex; } /* update the swapping status */
  if ((swapping == 2 && swapindex > 1) || (swapping == 3 && swapindex > 5)) {
      swapindex = 0;  /* reset to zero so we gen a new random deal next time */
  }
  return ;
} /* end deal_cards */

void newpack (deal d) { /* Fill a deck with cards from SA downto C2 */
  int place;
  int rank;
  int suit;
  place = 0;
  for (suit = SPADES; suit >= CLUBS; suit--) { /* newpack in order from AS down to Club deuce. */
    for (rank = ACE; rank >=TWO; rank--)     {
      d[place++] = MAKECARD (suit, rank);
    }
   }
   return ;
#if 0

  for (suit = CLUBS; suit <= SPADES; suit++) {  /* newpack is in order from Club deuce up to spade Ace. */
    for (rank = TWO; rank <=ACE; rank++)     {
      d[place++] = MAKECARD (suit, rank);
    }
  }
#endif
}  /* end newpack */

void setup_deal() {   /* One Time Initialize curdeal taking into account the Predeal requirements in stacked pack  */
  int i, j;
  int stk_cnt   = 0 ;
  int full_cnt  = 0 ;
  int small_cnt = 0 ;
  j = 0;
  sortDeal(stacked_pack);
  memcpy(curdeal, stacked_pack, 52) ;
   if (jgmDebug >= 4 ) {
      JGMDPRT(4, "Setting up First Time deal with stacked_size = %d \n",stacked_size);
      hexdeal_show(curdeal, 52);
   }
  /* put the predealt cards into curdeal-- note predealt cards are hand specific you cant just put them anywhere */
   for ( i = 0 ; i < 52 ; i++ ) {
      if (stacked_pack[i] != NO_CARD ) {
         stk_cnt++ ;
         curdeal[i] = stacked_pack[i]; /* predealt card is in the right hand in curdeal */
      }
   }
   /* put the cards remaining in fullpack into small_pack, and init the rest of curdeal also */
  for (i = 0; i < 52; i++) {
     if (fullpack[i] != NO_CARD ) {
        full_cnt++;
        small_pack[small_cnt++] = fullpack[i] ;
      }
   }
   JGMDPRT(4,"Small Pack setup with %d cards Stack_cnt=%d, Total=%d\n",small_cnt, stk_cnt, small_cnt+stk_cnt ) ;
   assert( (small_cnt + stk_cnt) == 52 ) ; /* the predealt and non-predealt cards should total 52*/
   small_size = small_cnt ;                /* save the small_pack size for later shuffling */

   /* now init the curdeal by copying in the leftover cards. This may not be necessary as shuffle will do it anyway. */
   j = 0 ;
   for (i = 0 ; i < 52 ; i++) {
      if (curdeal[i] == NO_CARD) {
         curdeal[i] = small_pack[j++] ; /* the non-predealt cards can go in any free slot in curdeal */
      }
   }
   assert( j == small_cnt ) ;              /* all of the leftover cards should have been copied to curdeal */

#ifdef JGMDBG
   if (jgmDebug >= 4 ) {
      JGMDPRT(4,"Starting Main loop with Deal \n");
      hexdeal_show(curdeal, 52) ;
      hexdeal_show(stacked_pack, 52) ;
   }
#endif
} /* end setup_deal */




