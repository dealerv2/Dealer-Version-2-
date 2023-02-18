#ifndef CCCC_H
#define CCCC_H

#ifdef __cplusplus
extern "C"
{
#endif

#define HAS_KARD2(s,r) HAS_CARD(curdeal, seat, (card) MAKECARD(s,r))
// Redefine the original HAS_CARD2 macro to use the Has_card array.
#define HAS_CARD2(s,r) hs[seat].Has_card[(s)][(r)]  // use global var hs and the array filled in by analyze()

#ifdef __cplusplus
} /* -- extern "C" */
#endif

/* This macro is just an implementation detail - in c++ it would be
   an inline function. */

int suit_quality(  int , int  ) ;
int quality (int, int);
int eval_cccc( int ) ;
int cccc (int);

#endif /* CCCC_H */

