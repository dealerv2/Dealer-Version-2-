#ifndef _GNU_SOURCE
   #define _GNU_SOURCE
#endif
#define JGMDPRT(l,fmt,...) do {if (jgmDebug >= (l)) { fprintf(stderr, "%s:%d " fmt, __FILE__,__LINE__,## __VA_ARGS__) ; } } while(0)
#define CARD_IDX(s,r)    ( (s)*13+(r) )
#define CODEDCARD(s,r)   ( (card)( ((s)<<4)&0xF0 ) | ( (r)&0xF ) )
#define C_Suit(c)    (   ((c)>>4)&0xF     )
#define C_Rank(c)    (   ((c)&0x0F)       )
#define NO_CARD 0x7F

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
   int jgmDebug = 1 ;
   int side_hand[2][2] = { {0,2},{1,3} };
   char seat_id[4] = {'N','E','S','W'};
   char suit_id[4] = {'C','D','H','S'};
   char rank_id[13]= {'2','3','4','5','6','7','8','9','T','J','Q','K','A'};
char pbn_test_deals[][80]={
{"e T5KA..KQJT75432. w Q62..A986.AQT843 n .AQ9652..KJ75962 s J98743.8743TJK..\n"},
{"e AKT5..KQJT75432. w Q62..A986.AQT843 n .AQ9652..KJ75962 s J98743.KJT8743..;\n"},
{"n AKT5..KQJT75432. e Q62..A986.AQT843 s .AQ9652..KJ75962 w J98743.KJT8743..;\n"},
{"n ...AKQJT98765432 s ..AKQJT98765432. e .AKQJT98765432.. w AKQJT98765432...\n"},
{"n ..AKQ.JT98765432 s .AKQ.JT98765432. e AKQ.JT98765432.. w JT98765432...AKQ\n"},
{"n .432.AKQ.JT98765 s 432.AKQ.JT98765. e AKQ.JT98765.432. w JT98765...AKQ432\n"},
};

typedef char DEAL_k[52];
int dcmp_card(  const void *x, const void *y ) {  // Descending compare function for cards which are just chars
   if ( *(char *)x < *(char *)y ) return  1 ;
   if ( *(char *)x > *(char *)y ) return -1 ;
   return 0 ;                       // Should never happen
}

void handsort( char *hnd_ptr) { /* Sort 13 cards in Descending Order Spade Ace downto Club Deuce */
   qsort(hnd_ptr, 13, sizeof(char), dcmp_card ) ;
}
void dealsort( char *dl_ptr ) { /* Sort the 4 hands individually */
   char * h_ptr ;
   for (int p = 0 ; p < 4 ; p++ ) {
      h_ptr = dl_ptr + 13*p ;
      handsort(h_ptr) ;
   }
}

int PBN_to_Deal52( DEAL_k  dl , char *t ) { /* convert a PBN style text string to Deal52 */
    #define PBNSTRSIZE 76  // the mim size possible; longer allowed if there is a semicolon
    #define END_PBN ';'
    size_t slen ;
    int dlcnt ;
    int card_taken[4][13] = {{0},{0},{0},{0} };
    int err_cnt = 0 ;
    int tc , seat ;
    enum rank_ek r;
    enum suit_ek s;
    int endofdeal = 0;
    char *exdl = "n T952.842.A98.432 e KQJ.A3.KQJT2.AKT s 64.KJT7.75.QJ875 w A873.Q965.643.96;";
    /* get a text string such as:
     * n T952.842.A98.432 e KQJ.A3.KQJT2.AKT s 64.KJT7.75.QJ875 w A873.Q965.643.96;
     * and convert it into a deal.
     * The input string should have at least (2+13+4)*4 = 76 chars; extras need a semicolon after the first 76.
     */
     slen = strlen(t) ;
     if (slen < PBNSTRSIZE ) {  // allow the deal to have extra chars at the end if there is a semicolon.
         fprintf(stderr, "The source deal is in the wrong format.\n"\
          "Submit deal as a %d char string with optional Semicolon then extra chars\nExample[%s]\n",
                          DEALSTRSIZE, exdl);
         return -1 ;
     }
     s=0; r=0; dlcnt = 0;
     for ( tc=0; tc<slen; tc++) {
         if(endofdeal == 1 ) { break ; } /* ignore extra chars at end of deal. should never be any */
         switch (t[tc]) {
             case 'n' : s=3 ; seat = 0 ; dlcnt = seat*13; break ;/* player 0 is north ; reset suit to spades */
             case 'e' : s=3 ; seat = 1 ; dlcnt = seat*13; break ;
             case 's' : s=3 ; seat = 2 ; dlcnt = seat*13; break ;
             case 'w' : s=3 ; seat = 3 ; dlcnt = seat*13; break ;
             case ' ' : break ;
             case '.' : --s   ; break ; /* marks the end of a suit. decr to next lower suit */
             /* I could group next cases with statements like:
              *  r =  t[tc] - '0' and r = t[tc] -'A' + 10
              * but I like the way this looks :)!
              */
             case 'A' : r = ACE;   dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case 'K' : r = KING;  dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case 'Q' : r = QUEEN; dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case 'J' : r = JACK;  dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case 'T' : r = TEN;   dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case '9' : r = NINE;  dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case '8' : r = EIGHT; dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case '7' : r = SEVEN; dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case '6' : r = SIX;   dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case '5' : r = FIVE;  dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case '4' : r = FOUR;  dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case '3' : r = THREE; dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case '2' : r = TWO;   dl[dlcnt++] = MAKECARD(s,r) ; card_taken[s][r]++; break  ;
             case '\n':
             case END_PBN : endofdeal = 1 ;  break ;
             default:
                fprintf(stderr, "Invalid Char [%c] in deal source string\n",t[tc]) ;
                return -1 ;
         } /* end switch */
   } /* end for tc */
   for (s = 0 ; s <= 3 ; s++ ) {
      for (r = 0 ; r<13 ; r++ ) {
         // fprintf(stderr, "[s=%d,r=%d] ",s,r);
         if (card_taken[s][r] != 1 ) {
            fprintf(stderr, "%c%c: %d\n", "CDHS"[s],"23456789TJQKA"[r],card_taken[s][r] ) ;
            err_cnt++;
         }
      /*
         else {
            fprintf(stderr, "%c%c: %d\n", "CDHS"[s],"23456789TJQKA"[r],card_taken[s][r] ) ;
         }
      */
      }
   }
   if ( err_cnt > 0 ) {
      fprintf(stderr, "%d Errors found in Deal. ***\n", err_cnt);
      return -1 ;
   }
    return 1 ;
} /* end PBN_to_Deal52 */

char *Hand52_to_PBN (char *buff,  char *dl, int p ) {  // Hands must be sorted Spades (Ace) downto Clubs (deuce)
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
             curr_suit-- ;
        } /* end while curr_suit != card_suit */
        assert(card_suit == curr_suit) ;
        while ( (curr_suit == card_suit) && (count < 13) ) { /* write the cards in this suit */
            kard = dl[di]; card_suit = C_SUIT(kard); card_rank = C_RANK(kard) ;
            if (curr_suit != card_suit ) break;
           *bp++ = r_ids[card_rank];
           count++; di++;
        } // end while curr_suit
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
} /* end Hand52_to_PBN */

int Deal52_to_PBN(char *pbn_ptr, DEAL_k d, int hand_mask ) {
/* Clone of printhands_pbn in action subs.c outputs to string */
  char pt[] = "nesw";
  int  p;
  char *pbn_base;
  pbn_base = pbn_ptr ;
  for (p=0 ; p< 4; p++ ) {
     if ( !(hand_mask & 1 << p ) ) continue ; /* skip this player if he was not called for */
    *pbn_ptr++ = pt[p]; *pbn_ptr++ = ' ' ; // player names are followed by a space */
    pbn_ptr = Hand52_to_PBN (pbn_ptr ,(char *)d,  p ); // append a hand to end of pbnbuff; returns ptr to null at end.
  }
  /* pbnbuff formatted now print it out */
  *pbn_ptr='\0';
  return (int ) (pbn_ptr - pbn_base) ;
} /*End Deal52 to PBN */

void init_export_buff( char *bp , size_t buff_len) {
     memset(bp, '\0', buff_len ) ;
}

char *Hand52_export(char *buff, int p, deal dl ) {
/* hand is sorted already. dl[p*13+0] = Highest Spade; dl[p*13+12] = lowest club. */
/* Output to look like -N Sxxxx,Hxxxx,Dxx,Cxxx ; no need to put anything for void suits. */
   char suit_sep = ',';
   int curr_suit, card_rank, card_suit;
   int di, count;
   unsigned char kard ;
   char *bp ;
   bp = buff ;
   JGMDPRT(8, "Hand52_export Start:: bp=%p \n",bp ) ;
   di = p*13 ;
   count = 0 ;
   curr_suit = 3 ; // spades
#ifdef JGMDBG
   if (jgmDebug >= 8 ) {
       fprintf(stderr, "Hand52_Export:: p=%d, di=%d, dl[di]=%02x\n",p,di,dl[di] ) ;
       fprintf(stderr, "buff=%p, bp=%p \n", buff, bp );
       }
#endif
   *bp++ = '-'; *bp++ = seat_id[p] ; *bp++ = ' ';
   while (count < 13 ) {
       kard = dl[di] ; card_suit = C_SUIT(kard); card_rank = C_RANK(kard) ;
        JGMDPRT(8, "Hand52_Export While 13::Kard=%02x, card_suit=%d, card_rank=%d, count=%d, curr_suit=%d\n",
                                  kard, card_suit, card_rank, count, curr_suit ) ;
       while( curr_suit != card_suit ) curr_suit-- ;
        assert(card_suit == curr_suit) ;
        *bp++ = suit_id[card_suit];  /* Write the suit Letter */
        while ( (curr_suit == card_suit) && (count < 13) ) { /* write the cards in this suit */
            kard = dl[di]; card_suit = C_SUIT(kard); card_rank = C_RANK(kard) ;
            JGMDPRT(8,"Hand52_Export While suit::Kard=%02x, card_suit=%d, card_rank=%d, count=%d, curr_suit=%d\n",
                                        kard, card_suit, card_rank, count, curr_suit ) ;
            if (curr_suit != card_suit ) break;
           *bp++ = rank_id[card_rank];
           count++; di++;
           JGMDPRT(8," Num[%d]=%c%c ", count, "CDHS"[curr_suit], *(bp-1) );
        } /* end while curr_suit == card_suit */
        JGMDPRT(8,"\n");
       *bp++ = suit_sep;
        curr_suit-- ; /* Move to next suit */
    } /* end while count < 13 */
    assert(count == 13 ) ;
     /* no need to write anything for voids here. wrote 13 cards that's it */
        /* the last char is the suit separator which we don't need after the club suit, so replace it with a space */
        if ( *(bp-1) == suit_sep ) { *(bp-1) = ' ' ; }
        else { fprintf(stderr, "CANT HAPPEN in Hand52_to_Predeal, last char is not a suit_separator %c \n", *(bp-1) ); }
        *bp = '\0' ; // terminate the buffer as a string
        return bp  ; /* return pointer to null byte in case we want to append another hand to the buffer */
} /* end Hand52_export*/

char *Side52_export(char *buff, int side, deal dl ) {
   char *pdbp = buff ;
   init_export_buff(buff, 256 ) ;
   JGMDPRT(8,"Export Side=%d start for Hand=%d\n",side, side_hand[side][0]);
   pdbp = Hand52_export( pdbp, side_hand[side][0], dl ) ;
   *pdbp++ = ' ';
   JGMDPRT(8,"Export Side=%d start for Hand=%d\n",side, side_hand[side][1]);
   pdbp = Hand52_export( pdbp, side_hand[side][1], dl ) ;
   JGMDPRT(8,"Export Side[%d] Result:[%s]\n------------------------------------------\n", side, buff );
   return pdbp ;
} /* end Side52_export */

/* A true Deal52 always has players in order N,E,S,W. If PBN enters them different the PBN_to_Deal52 is supposed to fix it.*/
void show_Deal52(DEAL_k d) {  // If hands pre-sorted then printout looks nicer.
    int  pnum ;
    char *pname="NESW";
    int r, s;  /* no point in making them enums; compiler could just screw us up with wrong choice of type */
    int i ;
    char kard , *pkard;
    pnum = 0 ;
    for (pnum = 0; pnum < 4; pnum++) {
        printf (" Player[%c ] ", pname[pnum]);
        pkard = d + pnum*13 ;
        for (i=0; i<13 ; i++ ) {
           kard = *(pkard + i) ;
           s = C_Suit( kard  ) ;
           r = C_Rank( kard  ) ;
           printf("%c%c ","CDHS"[s],"23456789TJQKA"[r] ) ;
         }
         printf("\n");
  } /* end for player */
  fprintf(stdout, "----------------dump curr deal done ----------------\n");
} /* end show_Deal52 */

int main (int argc, char **argv) {
    char pbndl[256];
    char expbuff[256] ;
    DEAL_k dl;
    int rc ;
    char *eos_ptr ;
    char eos_set[4]={';',':','\n','\0'};
 //  printf("PBN Deal?> ");
 //   while ( fgets(pbndl, 256, stdin) ) { // returns NULL at EOF; includes \n

   for (int k=0; k<6 ; k++ ) {
      printf(" Starting Deal[%d] Size=%ld, [%s]\n",k, sizeof(pbn_test_deals[k]), pbn_test_deals[k]);
       memcpy(pbndl , pbn_test_deals[k],sizeof(pbn_test_deals[k]) );
       //printf("memcpy Done OK \n");

       printf("Deal:%s",pbndl) ;       // There is an end of line here when reading from stdin
       eos_ptr=strpbrk(pbndl,eos_set);
       rc = PBN_to_Deal52(dl, pbndl ) ;
       if (rc >= 0 ) {
         dealsort( dl ) ;
         printf("----Sorted Deal-----\n") ;
         show_Deal52( dl ) ;
         Side52_export(expbuff, 0, dl ) ;
         printf("Export NS[%s]\n",expbuff ) ;
         Side52_export(expbuff, 1, dl ) ;
         printf("Export EW[%s]\n",expbuff ) ;
      }
      // printf("PBN Deal?> ");
   } /* end while / for */

   printf("****** Got EOF ****** \n");
   return 0 ;
} /* end main */
# if 0
/* Some samples that work
 * These two show that the order of the cards or hands does not matter.
e T5KA..KQJT75432. w Q62..A986.AQT843 n .AQ9652..KJ75962 s J98743.8743TJK..
e AKT5..KQJT75432. w Q62..A986.AQT843 n .AQ9652..KJ75962 s J98743.KJT8743..

 * These next ones show that some 'pathological cases parse correctly

n AKT5..KQJT75432. e Q62..A986.AQT843 s .AQ9652.KJ75.962 w J98743...KJT8743;
n ...AKQJT98765432 s ..AKQJT98765432. e .AKQJT98765432.. w AKQJT98765432...
n ..AKQ.JT98765432 s .AKQ.JT98765432. e AKQ.JT98765432.. w JT98765432...AKQ
n .432.AKQ.JT98765 s 432.AKQ.JT98765. e AKQ.JT98765.432. w JT98765...AKQ432
*
* These next ones give errors for missing and duplicate cards
* n 432..AKQ.JT98765 s 432.AKQ.JT98765. e AKQ.JT98765.AKQ. w JT98765...AKQ432
*
*/
#endif
