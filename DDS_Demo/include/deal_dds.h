#ifndef DEAL_DDS_H
#define DEAL_DDS_H
/* This file makes available some Dealer definitions that DDS oriented routines would need */


typedef unsigned char card52_k ;
typedef card52_k deal52_k[52] ;

#define CODEDCARD(s,r)   ( (card52_k)( ( ((s)<<4)&0xF0 ) | ( (r)&0xF ) ) )
#define CARD_SUIT(c)     ( (card52_k)((c)>>4)&0xF )
#define CARD_RANK(c)     ( (card52_k)((c)&0xF)    )
#define MAKECARD(s,r) ( (card52_k)((s)<<4) | ((r)&0xF) )
#define C_SUIT(c)       ((c)>>4)
#define C_RANK(c)       ((c)&0x0F)
#define NO_CARD     0xFF
/* This next macro is used several times in the dealaction_subs.c file. BTW hascard is a somewhat inefficient routine */
#define HAS_CARD(d,p,c) hascard(d,p,c)

/* ENUMS need dummy spot & no suit = -1 to force enum type to signed int instead of unsigned int */
enum dl_rank_ek {TWO=0, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE, SPOT=-1 } ;
enum dl_suit_ek {dl_CLUB=0, dl_DIAMOND, dl_HEART, dl_SPADE, nosuit=-1 } ;

#define SIDE_NS         0
#define SIDE_EW         1
#define COMPASS_NORTH   0
#define COMPASS_EAST    1
#define COMPASS_SOUTH   2
#define COMPASS_WEST    3
#define COMPASS_NS      4
#define COMPASS_EW      5

#define NSEATS          4

#define DL_HANDS   4
#define DL_SUITS   4
#define DL_STRAINS 5

#define MAXTITLE        100
#define LTC_VOID_WEIGHT 64
#define SUIT_CLUB       0
#define SUIT_DIAMOND    1
#define SUIT_HEART      2
#define SUIT_SPADE      3
#define SUIT_NT         4

#define DEALSTRSIZE      76

#endif
