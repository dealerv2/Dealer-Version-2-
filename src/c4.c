/*

This code has been submitted by Danil Suits <DSuits@silknet.com>, Mar 10, 1999.

It implements the cccc() and quality() functions.  Both quality and cccc
use the algorithms described in _The Bridge World_, October 1982, with the
single exception that the values are multiplied by 100 (so that we can use
integers for them).  Thus, a minimum opening bid is about 1200, rather
than 12.00 as expressed in the text.

In the original algorithm, everything was done with fractions.  Floating
point rounding being what it is, I've decided to implement this instead as
integer math, until the last step.

As it happens, it is currently more convenient to use integers for the
return value from these functions as well So for the moment, Rescale is
basically a no-op.
*
* JGM Update 2022/02/15: The user now has the option of specifying the desired
* CCCC and Suit quality values using 'dotnums' aka numbers with Decimal pts
* for example 12.75 or 6.5 or 0.5; as in cccc(north) >= 13.25
* But the dotnums are converted to ints by x100 so this code works exactly
* the same as before. The user is spared the inconvenience of x100
* and the Dealer Input File looks 'nicer'.
*
* JGM 2022/10/18 fixed several bugs in the original code; Qx, Qxx, AQ KQ stiff Q, first doubleton, and ded for 4333.
* JGM 2023/01/22 Allowed 1st dblton to count for +1 if there is also another shortness. 6-4-2-1 counts 3 pts for shortness.
*
*/
#define _GNU_SOURCE 1
#include <assert.h>
#include "../include/std_hdrs.h"
#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/dealexterns.h"
#include "../include/dealprotos.h"
#include "../include/dbgprt_macros.h"
#include "../include/c4.h"

#define C4_TYPE int
C4_TYPE Rescale (int nValue) {
  return nValue;
}

C4_TYPE cccc (int seat) {
  return Rescale (eval_cccc (seat));
}

C4_TYPE quality (int seat, int suit) {
  return Rescale (suit_quality (seat, suit));
}
#undef C4_TYPE

int eval_cccc (int seat) {
  int toteval = 0 ;  /* Total for the hand */
  int eval = 0;      /* total for the suit */
  int pakq = 0 ;  /* A=3, K=2, Q=1 before adjustments */
  int pjt  = 0 ;  /* lower honors; J and Ten */
  int ShapePoints = 0;
  int QualityPoints = 0;

  int HigherHonors = 0;  /* Counts the honors as we go thru the suit; eventually includes even the Jack */
  int Dbltons = 0 ;
  int Shorts = 0 ; /* JGM 2023. Count voids and stiffs */

  int suit;
  JGMDPRT(7,"C4 START for SEAT=%d \n", seat) ;
  /* For each suit.... */
  for (suit = SUIT_CLUB; suit <= SUIT_SPADE; ++suit) {
    eval = 0;   pakq = 0 ;  pjt = 0 ;
    ShapePoints = 0 ; /* Shortness points in this suit */
    HigherHonors = 0; /* A, K, or Q in this suit */
    QualityPoints = 0 ; /* QualityPoints in this suit */

    int Length = hs[seat].hs_length[suit];
    JGMDPRT(8,"C4 for seat=%d, suit=%d : %c, length=%d \n", seat, suit, "CDHS"[suit], Length) ; //4

    int HasAce   = HAS_CARD2 (suit, RK_ACE);
    int HasKing  = HAS_CARD2 (suit, RK_KING);
    int HasQueen = HAS_CARD2 (suit, RK_QUEEN);
    int HasJack  = HAS_CARD2 (suit, RK_JACK);
    int HasTen   = HAS_CARD2 (suit, RK_TEN);
    int HasNine  = HAS_CARD2 (suit, RK_NINE);
    JGMDPRT(8,"A=%d,K=%d,Q=%d,J=%d,T=%d,Nine=%d\n",HasAce,HasKing,HasQueen,HasJack,HasTen,HasNine ) ;

    /* Honors Points. A=3, K=2, Q=1 ; J, T, 9 depend on other higher cards */
    if (HasAce) {
      pakq += 300;
      HigherHonors++;
    } /* end HasAce*/
    JGMDPRT(7,"Has_ACE=%d, pakq_321Pts=%d High_Honors=%d\n", HasAce, pakq,HigherHonors) ;

    if (HasKing) {
      pakq += 200;
      if (Length == 1) pakq -= 150;  /* Stiff K worth only 0.5 plus 2 pts for the stiff added later*/
      HigherHonors++;               // HigherHonors counts A and K at this point.
    } /* end HasKing */
    JGMDPRT(8,"Has_KING=%d, pakq_321Pts=%d High_Honors=%d\n", HasKing, pakq,HigherHonors) ;

    if (HasQueen) {  // Stiff Q=0; AQ or KQ, -> Q=0.5 ; QJ/QT/Qx ->  Q = 0.25
         /* Stiff Q gets zero 321 points */
        if (Length == 2 && HigherHonors == 0)  pakq +=  25 ;    // QJ, QT, Qx => Queen is worth 0.25
        if (Length == 2 && HigherHonors >  0)  pakq +=  50 ;    // AQ or KQ =>  Queen is worth 0.5
        if (Length >= 3 && HigherHonors == 0)  pakq +=  75 ;    // Qxx, QJx, QTx etc. Q is worth 0.75
        if (Length >= 3 && HigherHonors >  0)  pakq += 100 ;    // AQx(x), KQx(x), AKQ(x) Q is worth 1.0
        HigherHonors++;                                        // HigherHonors now includes the Q.
    } /* end HasQueen */
    JGMDPRT(8,"Has_Queen=%d, pakq_321Pts=%d High_Honors=%d\n", HasQueen, pakq, HigherHonors) ;
    eval += pakq ;

    if (HasJack) {
        if (HigherHonors == 2) pjt += 50;   // J with precisely two (but not 3) of A,K,Q
        if (HigherHonors == 1) pjt += 25;
        HigherHonors++;                                        // HigherHonors now includes the J.
    } /* end HasJack */
    JGMDPRT(8,"Has_Jack=%d, pjt_H_Pts=%d High_Honors=%d\n", HasJack, pjt,HigherHonors) ;

    if (HasTen ) { /* Count all Tens even in long suits for this part  */
        if ( HigherHonors == 2)             pjt += 25;
        if ((HigherHonors == 1) && HasNine) pjt += 25;
    } /* end HasTen */
    JGMDPRT(8,"Has_Ten=%d, pjt_H_Pts=%d High_Honors=%d\n", HasTen, pjt,HigherHonors) ;
    eval += pjt ;
    JGMDPRT(8,"High Card suit pts=%d from AKQ=%d,+ JT=%d \n", eval , pakq, pjt ) ;
   /*
    * Calculate suit Quality for this suit
    */
    if (Length > 0 ) {
        QualityPoints = suit_quality (seat, suit);
    }
    else { QualityPoints = 0 ; }
    eval += QualityPoints;

    JGMDPRT(7,"Qual=%d,  New suitEval=%d \n", QualityPoints ,eval ) ;

    switch (Length) {
       case 0 : ShapePoints = 300 ; Shorts++; break ;
       case 1 : ShapePoints = 200 ; Shorts++; break ;
       case 2 : ShapePoints = 100 ; Dbltons++ ; break ; /* Will subtract later if only 1 Dblton and no other shortness */
      default : ShapePoints = 0 ; break ;
    }
    eval += ShapePoints ;  /* the total points for this suit so far */
    toteval += eval ;      /* total points for the hand so far */
    JGMDPRT(3,"This suit ShapePoints =%d,  New suitEval=%d NewHand Total=%d \n", ShapePoints ,eval, toteval ) ;
  }      /* end for (suit;...) */

         /* All Dbltons given +100; deduct the 1st one  */ /* Leave no other shortness test out for this vers. more std. */
  if (Dbltons > 0) { toteval -= 100  ; }

    /* Test for 4333 */
  if ( hs[seat].square_hand == 1 )  { toteval -= 50 ; }
  JGMDPRT(7,"C4 END SEAT[%d] TotEval=%d, square=%d\n\n",seat, toteval, hs[seat].square_hand ) ;

  assert ((toteval % 5) == 0); /* KnR pts go in steps of 0.05 always */
  return (toteval);
}

int suit_quality (int seat, int suit) {
    /* called from cccc above or directly from the input file */
  int qhcp = 0 ;  /* the value of the A,K,Q,J,T and adj before *len/10 */
  int Quality = 0; /* suit qual pts (hcp + adj) * len / 10 ; hcp: A=4,K=3,Q=2,J=1,T=0.5; adj for T with 9, missing QJ in 7+ etc.*/
  int HigherHonors = 0;

  int Length = hs[seat].hs_length[suit];
  JGMDPRT(8,"Quality :: suit=%d, Length=%d, seat=%d \n", suit, Length, seat);  //4

  int HasAce   = HAS_CARD2 (suit, RK_ACE);      /* JGM Mod; HAS_CARD2 now just accesses hs[seat].Has_card[s][r] */
  int HasKing  = HAS_CARD2 (suit, RK_KING);
  int HasQueen = HAS_CARD2 (suit, RK_QUEEN);
  int HasJack  = HAS_CARD2 (suit, RK_JACK);
  int HasTen   = HAS_CARD2 (suit, RK_TEN);
  int HasNine  = HAS_CARD2 (suit, RK_NINE);
  int HasEight = HAS_CARD2 (suit, RK_EIGHT);

  /*ACE*/
  if (HasAce) {
      qhcp += 400 ;
      HigherHonors++;
      JGMDPRT(8,"Quality ::Ace qhcp=%d HighHonors=%d\n", qhcp,HigherHonors) ;
   }

  /*KING*/
  if (HasKing) {
      qhcp += 300;
      HigherHonors++;
      JGMDPRT(3,"Quality ::King qhcp=%d HighHonors=%d\n", qhcp,HigherHonors) ;
   }

  /*QUEEN*/
  if (HasQueen) {
      qhcp += 200;
      HigherHonors++;
      JGMDPRT(3,"Quality ::Queen qhcp=%d HighHonors=%d\n", qhcp,HigherHonors) ;
   }

  /*JACK*/
  if (HasJack) {
      qhcp += 100;
      HigherHonors++;
      JGMDPRT(3,"Quality ::Jack qhcp=%d HighHonors=%d\n", qhcp,HigherHonors) ;
   }
   /* Ten */
     if (HasTen) {      /* All Tens count for at least 0.5; some tens count more see later */
      qhcp += 50;
      JGMDPRT(8,"Quality ::Ten qhcp=%d HighHonors=%d\n", qhcp,HigherHonors) ;
   }

/* This next bit adds Q and/or J equivalent in 7+ suits even if the honor is not present. ReDone to suit me JGM */
  if ( Length == 7 && (HasJack == 0 || HasQueen == 0 ) ) {qhcp += 100 ; } /* +1 if either honor is missing in 7 card suit */
  else if ( Length == 8 ) {                    /* +2 for missing Q; if Q present +1 for missing Jack */
      if      (HasQueen == 0 ) { qhcp += 200 ; } /* if missing Q, Jack is irrelevant */
      else if (HasJack  == 0 ) { qhcp += 100 ; } /* if HasQueen but not Jack, then +1 */
   }
   else if (Length >= 9 ) { /* a 9+ suit can get 2 extra for missing Q AND 1 extra for missing Jack */
      if ( HasQueen == 0 ) { qhcp += 200 ; }
      if ( HasJack  == 0 ) { qhcp += 100 ; }
   } /* end if-else-if testing for Q or J in 7+ suits */

     JGMDPRT(7,"qhcp :: with Q & J adjustment= %d suit len = %d \n",qhcp, Length ) ;
   /* Now check for presence of working Ten in suits less than 6 in len */
   if (Length <= 6 ) {
     if (HasTen) {
        if ( (HigherHonors > 1) || HasJack ) { qhcp += 50 ; }       // Ten with J or with 2+ is worth 1 else worth 0.5
     }
     if (HasNine && ( HigherHonors == 2 || HasEight || HasTen ) ) { qhcp += 50 ; }
      JGMDPRT(8,"qhcp :: with Ten & 9 adjustment= %d suit len = %d \n",qhcp, Length ) ;
   } /* end Length <= 6 */
   Quality = qhcp * Length/10 ;
   JGMDPRT(7,"Suit[%d:%c] Final Quality Pts=%d Length=%d, qhcp=%d\n", suit, "CDHS"[suit],  Quality, Length,qhcp) ;
  assert ((Quality % 5) == 0);  /* Q must be divisible by 5 */
  return Quality;
} /* end suit_quality() */

