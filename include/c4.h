#ifndef PNQ_CARD
#define PNQ_CARD

#ifdef __cplusplus
extern "C"
{
#endif

#define HAS_CARD2(s,r) HAS_CARD(curdeal, seat, (card) MAKECARD(s,r))

#ifdef __cplusplus
} /* -- extern "C" */
#endif

/* This macro is just an implementation detail - in c++ it would be
   an inline function. */

int suit_quality(  int , int  ) ;
int quality (int, int);
int eval_cccc( int ) ;
int cccc (int);

#endif /* PNQ_CARD */

