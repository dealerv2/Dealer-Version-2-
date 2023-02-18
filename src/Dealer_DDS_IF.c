/* File Dealer_DDS_IF.c 2021/12/27

 */

#include <string.h>
#include <stdio.h>

#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/dds_dll.h"         /* minimal api for DDS library */
#include "../include/deal_dds.h"        /* the link between DDS and Dealer */

// File Globals used in our code go here
extern int jgmDebug ;

unsigned int dds_holding[DDS_HANDS][DDS_SUITS] ;
      // Masks to set rank bits in DDS deal: deuce   trey    4       5       6       7
unsigned int    DDS_BITMASK[13] = {         0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
      // Masks to set rank bits contd          8       9       T       J       Q       K       A      --
                                            0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000} ;

/* what follows should be strictly intenal to us */
// Code Prototypes used here
int validPBNline(char *pbn_ptr );
struct ddTableDeal Deal52_to_DDSBIN(deal d);      // ddTableDeal uses [hands][suits] index order
struct ddTableDealPBN Deal52_to_DDSPBN (deal d );
int Deal52_to_Holding(deal d , unsigned int kards[DDS_HANDS][DDS_SUITS] ) ;

    /* a couple of debugging routines */
    void showRawResults (  struct ddTableResults *dd );  // ddTableResults uses [strains][hands] index order
    void showReturns(  DDSRES_k *dd ) ;

/* Begin the code. Supporting routines for Dealer to DDS interface. Convert Deal52 to DDSBIN and DDSPBN */
int Deal52_to_Holding(deal d , unsigned int kards[DDS_HANDS][DDS_SUITS] ) {
  int s, r, h, dds_s;
  int di ;
  for (h = 0 ; h < DDS_HANDS; h++ ) {
      for (s = 0 ; s < DDS_SUITS; s++ ) {
          kards[h][s] = 0;
      }
  } /* end for h < DDS_HANDS */
  h = 0 ;
  di = 0;
  while (di < 52 ) {
      s = CARD_SUIT(d[di]);
      dds_s = 3 - s ;        /* Clubs: s=0, dds_s=3, Diam: s=1, dds_s=2; Hearts s=2, dds_s=1; Spades: s= 3 dds_s=0 */
      r = CARD_RANK(d[di]);  /* deuce = 0, .... Ace=12 */
      kards[h][dds_s]  |= DDS_BITMASK[r]; //DDS_BITMASK[0]=0x0004 which is the Deuce; [12]=0x4000 which is Ace */
      di++ ;
      if  (di % 13 == 0 )  h++ ; /* next hand coming up */
  } /* end while di < 52 */
  return 1 ;
} /* end Deal52_to_Holding */

struct ddTableDeal Deal52_to_DDSBIN(deal d) {
  struct ddTableDeal dl;
  int s, r, h, dds_s;
  int di ;
  for (h = 0 ; h < DDS_HANDS; h++ ) {
      for (s = 0 ; s < DDS_SUITS; s++ ) {
          dl.cards[h][s] = 0;
      }
  } /* end for h < DDS_HANDS */
  h = 0 ;
  di = 0;
  while (di < 52 ) {
      s = CARD_SUIT(d[di]);
      dds_s = 3 - s ;        /* Clubs: s=0, dds_s=3, Diam: s=1, dds_s=2; Hearts s=2, dds_s=1; Spades: s= 3 dds_s=0 */
      r = CARD_RANK(d[di]);  /* deuce = 0, .... Ace=12 */
      dl.cards[h][dds_s]  |= DDS_BITMASK[r]; //DDS_BITMASK[0]=0x0004 which is the Deuce; [13]=0x800 which is Ace */
      di++ ;
      if  (di % 13 == 0 )  h++ ; /* next hand coming up */
  } /* end while di < 52 */
  return dl ;
} /* end Deal52_to_DDSBIN */

/* simple  Descending Order Ace to Deuce Insertion sort:
 * Test show that for 13 elems Insertion is faster than Shell or Selection
 * for a 52 element array as opposed to 4x13 elements Shell sort is about 10% faster than Insertion */
void sortHand(card *hnd_ptr, int size) {
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

void sortDeal(deal dl ) {
    int p ;
    card *h_ptr ;
    for (p = 0 ; p < 4 ; p++ ) { /* p is the player, 0=North, etc. We sort each quarter of the deal separately */
        h_ptr = &dl[p*13];
#ifdef JGMDBG
        if(jgmDebug > 6 ) {
             fprintf(stderr, "Sorting hand[%d] starting with card [%02x] -> [%02x]\n", p, dl[p*13], *h_ptr );
        }
#endif
        sortHand( h_ptr, 13 ) ;
    }
} /* end sortDeal */

int validPBNline(char *pbn_ptr ) { // check that not too long and that first char is seat, 2nd char : or space
    size_t bufflen;
    bufflen = strlen(pbn_ptr) ;
    if (*pbn_ptr != 'N' && *pbn_ptr != 'n' && *pbn_ptr != 'S' && *pbn_ptr != 's' &&
        *pbn_ptr != 'E' && *pbn_ptr != 'e' && *pbn_ptr != 'W' && *pbn_ptr != 'w' )  {
  #ifdef JGMTST
        fprintf(stderr, "PBN Deal ERROR. Seat %c is Invalid\n",  *pbn_ptr );
  #endif
        return -1 ;
    }
    if (*(pbn_ptr+1) != ' ' && *(pbn_ptr+1) != ':' ) { // not strictly necessary? but safer.
  #ifdef JGMTST
        fprintf(stderr, "PBN Deal ERROR. 2nd Char %c is Invalid\n",  *(pbn_ptr+1) );
  #endif
        return -2 ;
    } /* end check for space or colon */
    if (bufflen < PBNLINE || bufflen > PBNLINE+1) {
 #ifdef JGMTST
        fprintf(stderr, "PBN Deal ERROR. Length of deal[%ld] is not equal to %d \n",  bufflen, PBNLINE );
 #endif
        return -3 ;
    }
 #ifdef JGMTST
        fprintf(stderr, "OK PBN Deal; Len=[%ld] :: [%s] \n",  bufflen, (char *)pbn_ptr );
 #endif

    return TRUE ;
} /* end validPBNline */

struct ddTableDealPBN Deal52_to_DDSPBN (deal d ) {  /* d will be modified by sort. Caused a problem for Predeals pre2023*/
    // N:832.T763.J42.K72 AKJ6.A984.Q6.Q43 T4.Q52.T8753.T65 Q975.KJ.AK9.AJ98
    // N:...AKQJT98765432 ..AKQJT98765432. .AKQJT98765432.. AKQJT98765432...  is the pathological case
  char pt[] = "nesw";
  char UCrep[13]="23456789TJQKA";
  int s, p, r, di, kard,suit;
  struct ddTableDealPBN pbn_deal ;
  char *pbn_ptr ;
  pbn_ptr = &pbn_deal.cards[0] ;

    *pbn_ptr++  =  pt[0];  // DDS takes the first char in the 'deal' as the leader; returns tricks for him
    *pbn_ptr++  = ':';
     sortDeal(d) ;         // sort each hand in descending order i.e the pbn order DDS wants
     suit = SPADES ; p = 0 ; di = p*13 ;
     kard = d[di++]; s = C_SUIT(kard) ; r = C_RANK(kard) ;
     while (di <= 52 ) {
         while ( s != suit ) {
             if (suit != CLUBS ) {
                *pbn_ptr++ = '.' ; /* always need suit separator even if void */
                --suit ;
            }
            else {
                *pbn_ptr++ = ' ' ;  /* suit is clubs; hand is done. put hand sep in and incr player p */
                suit = SPADES ;
                p++;
            }
         } /* end while s != suit */
         /* assert s == suit */
         *pbn_ptr++ = UCrep[r] ;
         kard = d[di++]; s = C_SUIT(kard) ; r = C_RANK(kard) ;
     } /* end while di <= 52. All cards done */
#ifdef JGMDBG
     if ( jgmDebug  > 6 ) { printf("End While di[%d] suit=%d, s:r=[%d:%d]\n", di, suit, s, r ); }
#endif
     while( suit != CLUBS  ) {  *pbn_ptr++ = '.' ; --suit ; }
     *pbn_ptr++ = ':';
     *pbn_ptr = '\0'; /* Null terminate the pbn string */
#ifdef JGMDBG
     if ( jgmDebug  > 6 ) fprintf(stderr,"PBN[%s]\n", pbn_deal.cards);
#endif
     return pbn_deal ;
} /* end Deal52_to_DDSPBN */

int GIB_to_Deal52( deal dl , char *t ) { /* convert a GIB style ( printoneline fmt ) text string to Deal52 */
    size_t slen ;
    int dlcnt ;
    int tc , seat ;
    enum rank_ek r;
    enum suit_ek s;
    int endofdeal = 0;
    char *exdl = "n T952.842.A98.432 e KQJ.A3.KQJT2.AKT s 64.KJT7.75.QJ875 w A873.Q965.643.96:";
    /* get a text string such as:
     * n T952.842.A98.432 e KQJ.A3.KQJT2.AKT s 64.KJT7.75.QJ875 w A873.Q965.643.96:
     * and convert it into a deal.
     * The input string shoud have exactly (2+13+4)*4 = 76 chars.
     */
     slen = strlen(t) ;
     if (slen != DEALSTRSIZE ) {
         fprintf(stderr, "The source deal is in the wrong format.\n Submit deal as a COLON terminated %d char string \nExample[%s]\n",
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
             case 'A' : r = ACE;   dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case 'K' : r = KING;  dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case 'Q' : r = QUEEN; dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case 'J' : r = JACK;  dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case 'T' : r = TEN;   dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case '9' : r = NINE;  dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case '8' : r = EIGHT; dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case '7' : r = SEVEN; dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case '6' : r = SIX;   dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case '5' : r = FIVE;  dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case '4' : r = FOUR;  dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case '3' : r = THREE; dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case '2' : r = TWO;   dl[dlcnt++] = MAKECARD(s,r) ; break ;
             case ':' : endofdeal = 1 ;  break ;
             default:
                fprintf(stderr, "Invalid Char in deal source string\n") ;
                return -1 ;
         } /* end switch */
   } /* end for tc */

    #ifdef JGMDBG
      if (jgmDebug > 6 ) {
          fprintf(stderr, "Created Deal OK [%s]\n",t);
          fprintf(stderr, "Deal in Hex=[");
          for (dlcnt=0 ; dlcnt < 26 ; dlcnt++ ) {
              fprintf(stderr, "%02X ", dl[dlcnt] );
          }
          fprintf(stderr,"\n             ");
          for (dlcnt=26; dlcnt<52 ; dlcnt++) {
              fprintf(stderr, "%02X ", dl[dlcnt] );
          }
          fprintf(stderr,"]\n");
      } /* end if jgmDebug */
    #endif

    return 1 ;
} /* end GIB_to_Deal52 */

/* Debugging Routines */
void showRawResults (  struct ddTableResults *dd ) {
    int h, s;
    #define  xHANDS   4
    #define  xSTRAINS 5
    fprintf(stderr, "--------- RAW DDS resTable[Strain][Hand] Result ------\n");
    for (s=0; s < DDS_STRAINS ; s++ ) {
        fprintf(stderr, "Strain=%d : ", s );
      for (h=0 ; h < DDS_HANDS ; h++ ) {
        fprintf(stderr, "%d,", dd->resTable[s][h] );  // results uses [strains][hands] index order
      }
      fprintf(stderr, "\n");
    } /* end for s */
    for (h=0 ; h < DDS_HANDS ; h++ ) {
        fprintf(stderr, "Hand=%d : ", h );
        for (s=0; s < DDS_STRAINS ; s++ ) {
            fprintf(stderr, "%d,", dd->resTable[s][h] ); // results uses [strains][hands] index order
        }
        fprintf(stderr, "\n");
    } /* end for s */
} /* end ShowRaw() */

void showReturns(  DDSRES_k *dd ) {
    int h, s;
    fprintf(stderr, "--------- Returning DDSRES_k tricks[hand][strain] Result ------\n");
    for (h=0 ; h < DDS_HANDS ; h++ ) {
      fprintf(stderr, "Hand=%d [ ", h );
      for (s=0; s < DDS_STRAINS ; s++ ) {
        fprintf(stderr, "%d,", dd->tricks[h][s] );
      }
      fprintf(stderr, "]\n");
    } /* end for h */
    fprintf(stderr, " NSPar=[%d] Contract=[%s]\n",dd->parScore_NS, dd->ddsmsg );
} /* end ShowReturns() */

void dump_Deal(struct deal_st dl) { // deal_st uses [hands][suits] coding
    int  pnum , cardcount;
    char *pname="NESW";
    char *sname="SHDC";          /* In DDS Spades is suit 0, Clubs is suit 4 */
    char *rname="23456789TJQKA"; /* 2 is subscript 0 in the following bit mask; Ace is 12 */
        // Masks to set rank bits in DDS deal:    deuce   trey    4       5       6       7
    unsigned int    DDS_BITMASK[13] = {         0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
        // Masks to set rank bits contd          8       9       T       J       Q       K       A      --
                                                0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000} ;
    int r, s;  /* no point in making them enums; compiler could just screw us up with wrong choice of type */
    char sid ;
    pnum = 0 ;
    cardcount = 0;
    char pn, rn ;
    pn='*'; rn='-';

    for (pnum = COMPASS_NORTH; pnum <= COMPASS_WEST; pnum++) {
        cardcount = 0;
        pn = pname[pnum];
        fprintf (stderr, "\ndump_DDS deal Looping for Player number %d : %c ",pnum, pname[pnum]);
        fprintf (stderr,  " Player[%c ] \n", pname[pnum]);
        for (s = 0; s <= 3; s++) {  /* Spades are suit 0 in DDS binary*/
          sid = sname[s];
          fprintf (stderr, "Suit %2d=[%c] { ", s, sid);
          for (r = 12; r >= 0; r--) {     /* r goes from Ace to Deuce. r must be a signed int, not an unsigned */
              rn = rname[r];
              if ( dl.remainCards[pnum][s] & DDS_BITMASK[r] ) {        /* player p has this card .. */
                     fprintf (stderr, "%c%c ",sid, rn );
                     cardcount++ ;
              } /* end if DDS_BITMASK[r] */
            } /* end for r = 12 downto 2 */
            fprintf (stderr, "}\n");
        } /* end for s suit */
        fprintf (stderr, "Done for player%d=%c, found=%d\n", pnum, pn, cardcount);
  } /* end for player pnum */
  fprintf (stderr, "----------------dump DDS deal done ----------------\n");
} /* end dump_DDSdeal */


