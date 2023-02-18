/* JGM -- File Serverdebug_subs.c -- Routines for debugging output mostly */
/* Try not to include any functions that are defined elsewhere in dealer
 * so that including this one will not result in conflicts
 */
#include <assert.h>
#include <stdio.h>          /* for perror(), printf, fprintf */
#include <stdlib.h>         /* for srand48, mrand48 and friends */
#include <string.h>        /*strlen etc. also memcpy */
#include <unistd.h>        /* fsync, sysconf, read, write, */


#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/UserEval_types.h"
#include "../include/dbgprt_macros.h"

extern int hascard(char deal[], int p,  char card ) ;
extern int jgmDebug ;
void dump_curdeal(deal d) { /* ouput deal in easy to read, verbose fmt */
    int  pnum , cardcount;
    char *pname="NESW";
    char *sname="CDHS";
    char *rname="23456789TJQKA";
    int r, s;
    char sid ;
    pnum = 0 ;

    cardcount = 0;
    int crd ;
    char pn, rn ;
    pn='*'; rn='-';
    /* this is already a debugging routine; no need to use DBGPRT */
    fprintf(stderr, "Debugging Deal in dump_curdeal\n");
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
              crd = MAKECARD (s, r);
              fprintf(stderr, "%02X => %d:%c ", crd, r, rn); // Comment out.
              if (hascard(d, pnum, crd)) {
                     fprintf(stderr, "%c%c ",sid, rn );
                  cardcount++ ;
              } /* end if hascard */
            } /* end for r = ACE */
            fprintf (stderr, "}\n");
        } /* end for s suit */
        fprintf(stderr, "Done for player%d=%c, found=%d\n", pnum, pn, cardcount);
        assert(cardcount == 13 );  /* must find 13 cards for each player */
  } /* end for player */
  fprintf(stderr, "----------------dump curr deal done ----------------\n");
  fsync(2) ;
} /* end dump curr deal */

void hexdeal_show(deal dl ) { /* one line output of the coded cards in hex */
    int i ;
    i = 0;
    fprintf (stderr, " dl=[");
    for (i=0; i<52 ; i++ ) {  /* print hex deal */
        fprintf(stderr, "%02x ", dl[i] );
    } /* end hex deal */
    fprintf(stderr, "/]\n");
    fsync(2);
} /* end hexdeal_show */

void sr_deal_show(deal dl ) { /* two line output of the cards in SuitRank */
    char rns[] = "23456789TJQKA-";
    char sns[] ="CDHS";
    char rn, sn , sp;
    int s, r , i ;
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

void sr_hand_show(int p, deal dl ) {  /* two line output of the cards in SuitRank */
    char rns[] = "23456789TJQKA";
    char sns[] ="CDHS";
    char rn, sn ;
    int s, r ,  di ;
    di = p*13 ;
    fprintf (stderr," Showing Hand  using CARD_SUIT and CARD_RANK in sr_hand_show\n");
    for (s=0; s<p+1 ; s++ ) {fprintf(stderr, "        "); }
    fprintf (stderr,"SR1=[");
    for (di = p*13 ; di < (p+1)*13 ; di++ ) {
             s=C_SUIT(dl[di]); sn=sns[s];
             r=C_RANK(dl[di]); rn=rns[r];
             fprintf (stderr,"%c%c ", sn,rn );
    }
    fprintf (stderr, "]\n");
    fsync(2);
} /* end sr_hand_show */

char *Hand52_to_pbnbuff (int p, char *dl, char *buff ) {  //pbn fmt suit sep is dot, hand sep is space. voids are deduced from that.
/* hand sorted. dl[p*13+0] = Highest Spade; dl[p*13+12] = lowest club. */
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
            JGMDPRT(6, "Wrote Void for suit %d \n",curr_suit ) ;
            curr_suit-- ;
        } /* end while curr_suit != card_suit */
        assert(card_suit == curr_suit) ;
        while ( (curr_suit == card_suit) && (count < 13) ) { /* write the cards in this suit */
            kard = dl[di]; card_suit = C_SUIT(kard); card_rank = C_RANK(kard) ;
            if (curr_suit != card_suit ) break;
           *bp++ = r_ids[card_rank];
           count++; di++;
           JGMDPRT(7," Num[%d]=%c%c ", count, "CDHS"[curr_suit], *(bp-1) ) ;
        } // end while curr_suit
        JGMDPRT(7,"\n");
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

void show_query_type( struct query_type_st *pqt) {
 #ifdef JGMDBG
   DBGLOC( "Query Type:: Descr[%s], Tag=[%i]: [", pqt->query_descr, pqt->query_tag);
   for (int v=0 ; v<8; v++) {fprintf(stderr,"%d,", pqt->q_vals[v] ) ;  }
    fprintf(stderr, "]\n");
 #endif
   return ;
}

int show_mmap(char *mm_ptr, int len ) {            /* verbose debug */

   char cbuff[128], xbuff[128];
   char *cptr=cbuff;
   char *xptr=xbuff;
   int i = -1 ;
 #ifdef JGMDBG
   sprintf(cbuff,"%4x:",0); /* starting offset */
   sprintf(xbuff,"    :");
   cptr = cbuff + 5 ;
   xptr = xbuff + 5 ;

   for (i = 1 ; i <= len ; i++, mm_ptr++ ) {
      sprintf(cptr, "%c  ", (*mm_ptr) ? *mm_ptr : '.' ) ; cptr += 3 ;
      sprintf(xptr, "%2x ",  *mm_ptr                  ) ; xptr += 3 ;
      if ( (i%32) == 0 && i > 0 ) {
         *cptr++ = ';'; *cptr++ = '\n'; *cptr = '\0';
         *xptr++ = ';'; *xptr++ = '\n'; *xptr = '\0';
         fprintf(stderr,"%s", cbuff);
         fprintf(stderr,"%s", xbuff);
         cptr = cbuff;
         xptr = xbuff;
         sprintf(cbuff,"%4x:",i);
         sprintf(xbuff,"    :");
         cptr = cbuff + 5 ;
         xptr = xbuff + 5 ;
      }
   } /* end for */
   --i ;
    if( (i%32) != 0 ) {    /* new line and new offset every 32 bytes */
         *cptr++='<';  *cptr++='\n'; *cptr='\0';
         *xptr++='^';  *xptr++='\n'; *xptr='\0';
         fprintf(stderr,"%s", cbuff) ;
         fprintf(stderr,"%s", xbuff) ;
   }
      //fprintf(stderr,"\n");
      fsync(2);
   #endif
  return (i) ;
}

#if 0
 Server Base Pointer: 0x7f5bba7e9000
        header_ptr  = 0x7f5bba7e9000
        query_ptr   = 0x7f5bba7e9090
        reply_ptr   = 0x7f5bba7e90ec
        dldata_ptr  = 0x7f5bba7e9130
        nsres_ptr   = 0x7f5bba7e9ac4
        ewres_ptr   = 0x7f5bba7e9bc4
        cache_ptr   = 0x7f5bba7e9cc4
        ptrs.phs[0] = 0x7f5bba7e9138
        ptrs.phs[1] = 0x7f5bba7e937c
        ptrs.phs[2] = 0x7f5bba7e95c0
        ptrs.phs[3] = 0x7f5bba7e9804
 header_off  = 0
 query_off   = 144
 reply_off   = 236
 dl_data_off = 304
 nsres_off   = 2756
 ewres_off   = 3012
 cache_off   = 3268
 Handstat_off= 312
Size of map_template_k 3296  <=======
Size of mmap_hdr_k     144
Size of query_data_k   92
Size of reply_data_k   68
Size of DEALDATA_k     2452
Size of USER_VALUES_k  256
Size of UEVAL_CACHE_k  24
Size of HANDSTAT_k     580, 0x244
#endif







