/* File PBN_to_Holding.c -- deal_v3/DDS
 * Copied from the DDS distribution but ? the code there had remaincards[hand][suit]
 * but the type ddTableDeal has [suit][hand] so I changed the code here.
 *---- The above is WRONG -- 2022/02/15  -- ddTableDeal has [hand][suit] the only struct that has [suit][hand] is the RESULT struct from the CalcDDtable
 *
 *
 */

#include <string.h>
#include <stdio.h>

#include "../include/dll_solve.h"
#define COMPASS_NORTH 0
#define COMPASS_EAST  1
#define COMPASS_SOUTH 2
#define COMPASS_WEST  3
#define handID(hand, relative) ((hand + relative) & 3)


/* This code originally mostly from DDS distribution for converting a DDS style PBN buff to a DDS binary Deal Struct.
*  Some nice code here.
*  DDS Style PBN has only one compass name specified, the very first one. A GIB style PBN has all the compass letters in it.
*/
// RankBitMap[absolute rank] is the absolute suit corresponding to that rank. (2 .. 14 )
// Array modified by JGM to avoid the shift <<2 on every invocation.
// e.g Rank=2 now just uses mask 0x0004 rather than ( x0001 << 2 )

int PBNbuff_to_Holding(
  char const *dealBuff,
  unsigned int remainCards[DDS_HANDS][DDS_SUITS]);

  /* Note that DDS considers a Deuce to have rank 2; ranks 0 and 1 are not used. Ace has rank 14, rank 15 Not used  */
  /* DEALER considers a deuce to have rank 0, and an Ace to have Rank 12 so we need a different array where 0x0004 is item zero not item 2 */
unsigned short int RankBitMap[16] = {  0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
                                       0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000 };

unsigned char DDS_Ranks[16] ={  'x', 'x', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A', '-'};

unsigned char Strains[DDS_STRAINS] ={  'S', 'H', 'D', 'C', 'N'};
unsigned char Seats[DDS_HANDS] = {  'N', 'E', 'S', 'W'};


int IsCard(const char cardChar);

//PBN in DDS terms is string that looks like this. There is only one compass pt specified at the very beginning
// n:T952.842.A98.432 KQJ.A3.KQJT2.AKT 64.KJT7.75.QJ875 A873.Q965.643.96
//   Spades.hearts.diamonds.clubs Sp.He.Di.Cl Sp.He.Di.Cl Sp.He.Di.cl
//                                              chars that come here ^^^^^^ or later are ignored up to 80 chars.
int PBNbuff_to_Holding(
  char const * dealBuff,
  unsigned int remainCards[DDS_HANDS][DDS_SUITS])
  {
  int handRelFirst = 0;
  int suitInHand = 0;
  int hand;
  int card ;
      // init to zero so that the |= will set the right bits
  for (int h = 0; h < DDS_HANDS; h++)
    for (int s = 0; s < DDS_SUITS; s++)
      remainCards[h][s] = 0;
  int bp = 0;
  // look for the leader in the first 3 chars of the deal
  while (((dealBuff[bp] != 'W') && (dealBuff[bp] != 'N') &&
          (dealBuff[bp] != 'E') && (dealBuff[bp] != 'S') &&
          (dealBuff[bp] != 'w') && (dealBuff[bp] != 'n') &&
          (dealBuff[bp] != 'e') && (dealBuff[bp] != 's')) && (bp < 3))
    bp++;

  if (bp >= 3)  return 0;  // Failure -- retOK is always 1 in DDS
  int first;
  if      ((dealBuff[bp] == 'N') || (dealBuff[bp] == 'n'))    first = 0;
  else if ((dealBuff[bp] == 'E') || (dealBuff[bp] == 'e'))    first = 1;
  else if ((dealBuff[bp] == 'S') || (dealBuff[bp] == 's'))    first = 2;
  else                                                        first = 3;
 // after the leader should come a colon or a space skip over it
  bp++;
  bp++;

// Even tho the PBN string may start with ANY leader, we keep the same subscripts in the remainCards[h][s] array.
// HandRelFirst is adjusted every time we see a hand separator in the PBN string.

  hand = handID(first, handRelFirst) ;
  // if(jgmDebug > 4 ) printf("Doing hand [%d]=%c next; ",hand, Seats[hand]);
  while ((bp < 80) && (dealBuff[bp] != '\0'))  { // Null term or max of 80
    card = IsCard(dealBuff[bp]); // Decode the char. Card, Dot, or Space
    if (card)    { // encode the card with a bit mask.
      remainCards[hand][suitInHand] |= (RankBitMap[card]);  // card = 14 (Ace) then remainCards[h][s] =
    }
    else if (dealBuff[bp] == '.')  // char is a suit marker. Bump suit;
      suitInHand++;
    else if (dealBuff[bp] == ' ')   // char is a hand separator. Bump HandRelFirst and reset the suit to spades
    {
      suitInHand = 0;       // Reset the suit to Spades
      handRelFirst++;
      hand = handID(first, handRelFirst) ;
      // if(jgmDebug > 4 ) printf("Doing hand [%d]=%c next; ",hand, Seats[hand]);
    }          /* end if-else if - else if chain for char testing */
    bp++;      //   get next char from PBN deal
  }            // end while--we reached end of pbn deal string.
    // if(jgmDebug > 4 ) printf("\n");
  return RETURN_NO_FAULT;
} // end convert routine.

int IsCard(const char cardChar){
  switch (cardChar)  {
    case '2':      return 2;
    case '3':      return 3;
    case '4':      return 4;
    case '5':      return 5;
    case '6':      return 6;
    case '7':      return 7;
    case '8':      return 8;
    case '9':      return 9;
    case 'T':    case 't':      return 10;
    case 'J':    case 'j':      return 11;
    case 'Q':    case 'q':      return 12;
    case 'K':    case 'k':      return 13;
    case 'A':    case 'a':      return 14;
    default:                    return 0;
  }
} /* end IsCard */
void dump_Deal(struct deal_st dl) { // deal_st uses [hands][suits] coding
    int  pnum , cardcount;
    char *pname="NESW";
    char *sname="SHDC";     /* In DDS Spades is suit 0, Clubs is suit 4 */
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
    /* this is already a debugging routine; no need to use DBGPRT */
    // if (jgmDebug > 4) fprintf(stderr, "Debugging Deal in dump_DDSdeal\n");
    for (pnum = COMPASS_NORTH; pnum <= COMPASS_WEST; pnum++) {
        cardcount = 0;
        pn = pname[pnum];
        fprintf(stdout, "\ndump_DDS deal Looping for Player number %d : %c ",pnum, pname[pnum]);
        fprintf (stdout,  " Player[%c ] \n", pname[pnum]);
        for (s = 0; s <= 3; s++) {  /* Spades are suit 0 in DDS binary*/
          sid = sname[s];
          fprintf(stdout, "Suit %2d=[%c] { ", s, sid);
          for (r = 12; r >= 0; r--) {     /* r goes from Ace to Deuce. r must be a signed int, not an unsigned */
              rn = rname[r];
              if ( dl.remainCards[pnum][s] & DDS_BITMASK[r] ) {        /* player p has this card .. */
                     fprintf(stdout, "%c%c ",sid, rn );
                     cardcount++ ;
              } /* end if DDS_BITMASK[r] */
            } /* end for r = 12 downto 2 */
            fprintf (stdout, "}\n");
        } /* end for s suit */
        fprintf(stdout, "Done for player%d=%c, found=%d\n", pnum, pn, cardcount);
  } /* end for player pnum */
  fprintf(stdout, "----------------dump DDS deal done ----------------\n");
} /* end dump_DDSdeal */







