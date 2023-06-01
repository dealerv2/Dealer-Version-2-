/* File include/SS_NT_Values.c --
 * The Net values of Stiff Honors, and Honors Dblton after Deductions. Metric dependent.
 */
#ifndef SS_VALUES_H
#define SS_VALUES_H
#ifndef USEREVAL_TYPE_H
#include "UserEval_types.h"
#endif
/* Table of Short Suit Values in NT by Metric ID --
 * For Short suit methods (Goren, Pav, Sheiwold we assume +2 or +1 will be added later always; the values here reflect that
 * E.G. a Goren stiff King is 1 (+2 later for a net of 3) same as Pavlicek
 *      a Bergen stiff King is 2 ( 3 - 1 )
 *      a Sheinwold stiff King is 2, with +2 later gives 4.
 */

 /* The Various Metrics for some this array may be irrelevant as the values are built into the algorithm e.g. KnR
  * BERGEN=0, BISSEL,  DKP, GOREN, JGM1, KAPLAN, KARPIN, KnR, LAR, MORSE, PAV, R22, SHEINW, ZAR } ;
  */
 /* The array; [Metric][22 ShortSuitItemsList: 6 stiff and 16 Dbltons ]*/
 float ss_NT_vals[metricEND][ss_END] = {
    //            <------------No Trump Value ----------------------->
    //             A         x  AK     Ax KQ     Kx  QJ     JT   Tx,xx
  {  /* Bergen  */ 4,2,1,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,1,1, 1,0, 0,0 },
  {  /* Bissel  */ 3,2,1,0,0,0, 6,5,4,3,3, 4,3,2,2,  2,1,1, 0,0, 0,0 },
  {  /* DKP N/A */ 12,8,4,1,0,0, 21,17,14,13,13, 13,10,9,9, 6,4,4, 1,1, 0,0}, /*13-9-5-2 Minus 1 for no low card or unguarded Q,J*/

  {  /* Goren   */ 4,1,0,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,1,1, 0,0, 0,0 },
  {  /* JGM1    */ 3.5,2,0.5,0,0,0, 6.5,5.5,5,4.5,4.5, 4.5,4,3.5,3, 2,1.5,1, 0.5,0,0,0 },
  {  /* Kaplan  */ 4,2,1,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,1,1, 0,0, 0,0 },
  {  /* Karpin  */ 4,2,1,0,0,0, 7,6,5,4,4, 4,3,3,3,  2,1,1, 0,0, 0,0 },
  {  /* KnR-N/A */ 3,0.5,0,0,0, 5,4,4,4,4, 3,2,2,2,  1,1,1, 0,0, 0,0 },
  {  /* Larsson */ 4,2,1,0,0,0, 7,6,5,4,4, 4,4,3,3,  2,2,2, 1,1, 0,0 },
  {  /* Morse   */ 4.5,2,0.5,0,0,0, 7.5,6,5,4.75,4.5, 4.5,3.75,3.25,3, 2,1.5,1, .5,0, 0,0 },
  {  /* Pavlicek*/ 4,1,0,0,0,0, 7,6,5,4,4, 4,3,3,3,  2,1,1, 0,0, 0,0 },
  {  /*R22-Berg */ 4,2,1,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,1,1, 1,0, 0,0 },
  {  /*Sheinwold*/ 4,2,1,0,0,0, 7,6,5,4,4, 5,4,3,3,  2,1,1, 0,0, 0,0 }, // Book 'usual' K,Q,J, QJ, Qx, Jx deduct -1
  {  /*ZAR:NoCtl*/ 3,2,1,0,0,0, 7,6,4,4,4, 4,3,3,3,  2,1,1, 0,0, 0,0 },
// ZAR Subtract 1  * * * *          *      * *       * * *  * * -- mostly per PAV except added AJ stiff = -1 per Binder?
} ; /* end 3 D array */
    //            <------------No Trump Value ----------------------->
    //             A         x   AK      Ax  KQ    Kx QJ     JT   Tx


/* Notes:
 * Bissel Honor values (downto the T) are 3 minus Number of missing higher honors. so KQ = 2+2, AK = 3+3 etc.
 *              Nice way to incorporate synergy of touching honors
 * DKP is Using Little Jack Points (13-9-5-2). Array N/A. DK -1 for any suit whose lowest card is higher than a T. +1 if with A or K
 * JGM1 based on OPC -- Q(1.5 or 2), J(0,0.5,1), and T(0,.5,1) are variable depending on higher honors.
 * Morse based on BumWrap fixed values: A=4.5, K=3, Q=1.5, J=0.75, T=0.25  with ad-hoc deductions
 * Goren short honor deductions: Stiff K, Q -2, Stiff J -1 ; Q-J, Q-x or J-x -1
 * Pavlicek NT values are deducted from the suit values:
 *      e.g. suit stiff K=3; ergo -2 for the stiff K also stiff Q like Goren ; PAV KQ, KJ, QJ, Qx, Jx also deduct -1
 * R22 same as Bergen since he 'invented' it.
 * Sheinwold Guessing based on, Quote: "Qx, Jx not worth full value" and "Stiff K = 4 and hope".
 *        Also says Q or J only worth full value if with A or K. -0.5 for Qxx or Jxx ?
 * ZAR -- Using PAV description. Start with 4-3-2-1; Any stiff -1; Dblton KQ, KJ, QJ, Qx, Jx, -1;
 * Extra for controls added later since ZAR uses raw HCP w/o controls for other things. Larsson Style Synergy.
 */
  #if 0
  // Following is just for documentation for now to keep all the short suit specs in one file for reference.
  // Cleanup redundancies later

   /* BERGEN: Varies. Usually counts for length as Opener, and for Trump Length AND shortness once a fit is found. lots of uniqueness*/
   /* PAV: says count for EITHER the stiff OR the honor; not both; so stiff Q or J = 2pts for the stiff; Stiff K = 3 pts for honor*/
   /* GOREN: Per the PAV website */
   /* SHEINW: Per the book, '5 Weeks to winning bridge and some inferences from the text */
   /* KAPLAN: Kaplan counts for length, so assume he deducts the same as Bergen as starting point. */
   /* R22: Unless we have evidence to contrary assume others like R22, etc. count like Kaplan. */
   /* DKP: These counts are BEFORE div by 3; DK deducts for any suit where lowest card > Ten */
   /* MORSE: MORSE and JGM1 are whatever tweaks JGM is trying out. */
   /* KnR: As per the text. Placeholder only since the code does it all, these values never referenced */
   /* BISSEL: Deduct per the PAV website. Pav explanation of Bissell complicated.
    *    Per Wikipedia Honors(Ace to Ten) are worth 3 minus the number of missing higher honors, Max of 15 pts in a suit.
    *    JTxxx is worth zero AKQJT the JT is worth 6 for a total of 15.
    *    Nicely captures the Synergy of honors together and the priority of Aces and Kings
    *
    * ZAR deductions are as per the PAV website with AJ also deducted. The values here do not include the extras for controls
    * since ZAR also uses HCP w/o the controls included for other things.
    */
} ;
/* Morse using BumWrap[4.5,3,1.5,.75,.25] for HCP and Ded to reflect H and HH in OPC */
/* JGM1  using BUmWrap with Ded similar to standard Deductions */
/*                A   K   Q   J   T   AK, AQ, AJ, AT, Ax,  KQ, KJ, KT, Kx,  QJ, QT, Qx,  JT, Jx, Tx,   xx */
float short_Hon_adj[metricEND][ss_END] = {
   /* Bergen */ {-.5, -1, -1, -1, -0., -0, -0,-.5, -0, -0.,-.5,-.5, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0},  /* Book gives -.5*/
   /* Bissell*/ { -0, -1, -1, -1, -0., -0, -0, -0, -0, -0.,  0,  0, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Bissell per PAV */
   /* DKP    */ { -0, -3, -3, -2, -0., -0, -0, -0, -0, -0., -1, -0,  0, -0., -3, -3, -3., -2, -2, -0,  -0.0}, /* unguarded H; but only 1/3 of shwn */
   /* Goren  */ { -0, -2, -2, -1, -0., -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Ded to give NT pts */
   /* JGM1   */ { -0, -1,-1,-.75,-.25, -1, -1,-.5, -0, -0., -1, -0, -0, -0., -1, -1, -1., -1,-.75,-.25,-0.0}, /* Ded for BumWrap? 4.5,3,1.5,0.75,0.25 */
   /* Kaplan */ { -0, -1, -1, -1, -0., -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Same as Berg for now ???? */
   /* Karpin */ { -0, -1, -1, -1, -0., -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Per PAV web Page.  */
   /* KnR    */ { -0,-2.5,-1, -0, -0., -0, -0, -0, -0, -0., -0, -0, -0, -0., -0, -0, -0., -0, -0, -0,  -0.0}, /* KnR Dont deduct for dblton Honors */
   /* LAR    */ { -0, -1, -1, -1, -0., -0, -0, -0, -0, -0., -1, -0, -0, -0., -1, -0, -0., -0, -0, -0,  -0.0}, /* based on book */
   /* Morse  */ { -0, -1, -1,-.75,-.25,-0, -.5,-.25,-0,-0., -0, -0, -0,-0.,-.25,-.25,-.5,-.5,-.75,-.25,-0.0}, /* reflects opc values for HH dblton*/
   /* PAV    */ { -0, -1, -1, -1, -0., -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Applies to NT values. Suit N/A */
   /* R22    */ { -0, -1, -1, -1, -0., -0, -0, -0, -0, -0., -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* Simple Bergen */
   /* SHEINW */ { -0, -1, -1, -1, -0., -0, -0, -0, -0, -0., -0, -0, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /* assume same as Goren for now ??? */
   /* ZAR    */ { -0, -0, -0, -0, -0., -0, -0, -0, -0., -0, -1, -1, -0, -0., -1, -1, -1., -1, -1, -0,  -0.0}, /*Assume nothing for now. ZAR not impl yet */
} ;
/* OPC    { -1, -1,-.5, -0, -0.,-.5, -0, -0, -0., -1, -1,-.5,-1.5,-.5, -0,  -0.0}, No OPC since all done in Perl Keep for Ref */
//                  0          1    2    3     4      5       6      7    8    9     10   11    12     13    14        20    88    -1
//enum metric_ek { BERGEN=0,BISSEL,DKP,GOREN, JGM1, KAPLAN, KARPIN, KnR, LAR, MORSE, PAV, R22, SHEINW, ZAR, metricEND,EXCP=20,SET=88,Quit=-1} ;
  #endif // end if 0
#endif
