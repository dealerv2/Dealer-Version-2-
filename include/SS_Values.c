#ifndef SS_VALUES_H
#define SS_VALUES_H
#ifndef USEREVAL_TYPE_H
#include "UserEval_types.h"
#endif

/* Table of Short Suit Values in NT and Suits by Metric ID
 * We give the NET value since it is too complicated to give a corrected value
 * then correct the corrected value depending on what flavor of short suit evaluation the metric does
 * For metrics that evaluate length not shortness, the initial valuation for NT and suit, will be the same.
 * When we do not know the method used, eg ?? then we assume one of the 'standard' deductions, Goren, Kaplan, Bergen ??
 */
 /* The 22 items in question from Stiff Ace down to dblton spots */

 /* The Various Metrics for some this array may be irrelevant as the values are built into the algorithm e.g. KnR
  * BERGEN=0, BISSEL,  DKP, GOREN, JGM1, KAPLAN, KARPIN, KnR, LAR, MORSE, PAV, R22, SHEINW, ZAR } ;
  */
 /* The 3D array; [Metric][NT,Suit][22 ShortSuitItemsList]*/
 float ss_vals[metricEND][2][ss_END] = {
    //             A         x  AK     Ax KQ     Kx  QJ    JT    Tx      A         x AK      Ax KQ     Kx  QJ    JT    Tx
  {  /* Bergen  */ {4,2,1,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,1,1, 1,0, 0,0 }, {4,2,1,0,0,0, 7,6,5,4,4, 5,4,3,3, 2,1,1, 1,0, 0,0 }},
  {  /* Bissel  */ {3,2,1,0,0,0, 6,5,4,3,3, 4,3,2,2,  2,1,1, 0,0, 0,0 }, {3,2,1,0,0,0, 6,5,4,3,3, 4,3,2,2, 2,1,1, 0,0, 0,0 }},
  {  /* DKP N/A */ {12,8,4,1,0,0, 21,17,14,13,13, 13,10,9,9, 6,4,4, 1,1, 0,0}, /*13-9-5-2 Minus 1 for no low card or unguarded Q,J*/
                   {12,8,4,1,0,0, 21,17,14,13,13, 13,10,9,9, 6,4,4, 1,1, 0,0}},
  {  /* Goren   */ {4,1,0,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,1,1, 0,0, 0,0 }, {6,3,2,2,2,2, 8,7,6,5,5, 6,5,4,4, 4,2,2, 1,1, 1,1 }},
  {  /* JGM1    */ {3.5,2,0.5,0,0,0, 6.5,5.5,5,4.5,4.5, 4.5,4,3.5,3, 2,1.5,1, 0.5,0,0,0 },
                  {3.5,2,0.5,0,0,0, 6.5,5.5,5,4.5,4.5, 4.5,4,3.5,3, 2,1.5,1, 0.5,0,0,0 }},
  {  /* Kaplan  */ {4,3,0,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,2,2, 1,1, 0,0 }, {4,3,0,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,2,2, 1,1, 0,0 }},
  {  /* Karpin  */ {4,2,1,0,0,0, 7,6,5,4,4, 4,3,3,3,  2,1,1, 0,0, 0,0 }, {4,2,1,0,0,0, 7,6,5,4,4, 4,3,3,3,  2,1,1, 0,0, 0,0 }},
  {  /* KnR-N/A */ {3,0.5,0,0,0, 5,4,4,4,4, 3,2,2,2,  1,1,1, 0,0, 0,0 }, {3,0.5,0,0,0, 5,4,4,4,4, 3,2,2,2,  1,1,1, 0,0, 0,0 }},
  {  /* Larsson */ {4,2,1,0,0,0, 7,6,5,4,4, 4,4,3,3,  2,2,2, 1,1, 0,0 }, {4,2,1,0,0,0, 7,6,5,4,4, 4,4,3,3,  2,2,2, 1,1, 0,0 }},
  {  /* Morse   */ {4.5,2,0.5,0,0,0, 7.5,6,5,4.75,4.5, 4.5,3.75,3.25,3, 2,1.5,1, .5,0, 0,0 },
                  {4.5,2,0.5,0,0,0, 7.5,6,5,4.75,4.5, 4.5,3.75,3.25,3, 2,1.5,1, .5,0, 0,0 }},
  {  /* Pavlicek*/ {4,1,0,0,0,0, 7,6,5,4,4, 4,3,3,3,  2,1,1, 0,0, 0,0 }, {6,3,2,2,2,2, 8,7,6,5,5, 5,4,4,4,  3,2,2, 1,1, 1,1 }},
  {  /*R22-Berg */ {4,2,1,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,1,1, 1,0, 0,0 }, {4,2,1,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,1,1, 1,0, 0,0 }},
  {  /*Sheinwold*/ {4,2,0,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,1,1, 0,0, 0,0 }, {6,4,2,2,2,2, 8,7,6,5,5, 6,5,4,4,  3,2,2, 1,1, 1,1 }},
  {  /* ZAR     */ {5,3,1,0,0,0, 10,8,7,6,6, 5,4,4,4, 2,1,1, 0,0, 0,0 }, {5,3,1,0,0,0, 10,8,7,6,6, 5,4,4,4, 2,1,1, 0,0, 0,0 }},
} ; /* end 3 D array */

/* Notes:
 * Bissel Honor values (downto the T) are 3 minus Number of missing higher honors. so KQ = 2+2, AK = 3+3 etc.
 *              Nice way to incorporate synergy of touching honors
 * DKP is Using Little Jack Points (13-9-5-2). Array N/A. DK -1 for any suit whose lowest card is higher than a T. +1 if with A or K
 * JGM1 based on OPC -- Q(1.5 or 2), J(0,0.5,1), and T(0,.5,1) are variable depending on higher honors.
 * Morse based on BumWrap fixed values: A=4.5, K=3, Q=1.5, J=0.75, T=0.25  with ad-hoc deductions
 * Pavlicek NT values are deduced from the suit values: e.g. suit stiff K=3; ergo in NT it must be worth only 1
 * R22 same as Bergen since he 'invented' it.
 * Sheiwold Guessing based on, Quote: "Qx, Jx not worth full value" and "Stiff K = 4 and hope"
 * ZAR -- Using PAV description. Start with 6-4-2-1; Any stiff -1; Dblton KQ, KJ, QJ, Qx, Jx, -1
 */
#endif
