  /* File dealdefs.h -- VERSION 2.5  -- Major deletions
   * by ;;JGM   2022 version Collect all dealer symbolic constants and macros in one place.
   * Last Modified: 2022/02/19
   */
  /* Does not do function prototypes, or global vars. or externs */
  /* Deleted many defs no longer needed since the code is modified. */

  /* Make the header file guard .. */
#ifndef DEALDEFS_H
#define DEALDEFS_H 1
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif

#define VERSION "2.1.0"

#ifndef UNUSED
  #define UNUSED(x) ((void)(true ? 0 : ((x), void(), 0)))
                                // was x++
#endif

#ifndef NOTUSED
   #define NOTUSED(v) ( (void)(v) )
#endif
  /* this next one used in the  defs.y file in bias deal code converts -ve numbers to zero, leaves +ve numbers as is*/
#ifndef TRUNCZ
 #define TRUNCZ(x) ((x)<0?0:(x))
#endif

/* this next one added because Linux has a BRIDGE utility that refers to ethernet cards */
/* will have to make sure there is an ln /usr/games/gibcli to /usr/games/bridge */
#define DD_PGM "/usr/games/gibcli"
#define OPC_PGM "/usr/local/bin/DOP/dop"
#define FDP_PGM "/usr/local/bin/Dealer/fdp"
#define SUCCESS 1
#define FAILED  0
#define NIL ((struct tree *) 0)

/* ENUMS need spot, no suit = -1 to force enum type to signed int instead of unsigned int */
enum rank_ek {TWO=0, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE, SPOT=-1 } ;
enum suit_ek {CLUBS=0, DIAMONDS, HEARTS, SPADES, nosuit=-1 } ;

 /* these next ones mostly by JGM */
 /* These next three numbers do NOT include room for a terminating NULL char */
 #define DEALSTRSIZE 76
 #define COMPACT_DEAL_SIZE 96
 #define PBNLINE 69
 #define MAXLINE 256

#define MAXTITLE        255
#define MAXTITLESIZE    256
#define LTC_VOID_WEIGHT 64
#define SUIT_CLUB       0
#define SUIT_DIAMOND    1
#define SUIT_HEART      2
#define SUIT_SPADE      3
#define SUIT_NT         4
#define NSUITS          4
#define NSTRAINS        5

  /* following macros changed by ;;; JGM  */
  /* was shift <<6, >>6 and mask 0x3F  */
  /* but card is unsigned char, so <<6 will use high bit. */
#define MAKECARD(suit,rank) ( (card)((suit)<<4) | ((rank)&0xF) )
#define CARD_RANK(c) (  (card)(c)&0xF     )
#define CARD_SUIT(c) ( ((card)(c)>>4)&0xF )
#define C_SUIT(c)       ((c)>>4)
#define C_RANK(c)       ((c)&0x0F)
#define NO_CARD     0xFF
/* This next macro is used several times in the dealaction_subs.c file. BTW hascard is a somewhat inefficient routine */
#define HAS_CARD(d,p,c) hascard(d,p,c)
/* JGM Removed MAKECONTRACT since it was no longer used in the JGM revised eval contract routines */
/* #define MAKECONTRACT(suit, tricks, dbl) ( (tricks*5 + suit) + 40*dbl ) -- No longer used even with dbl */

#define NSEATS          4
#define SIDE_NS         0
#define SIDE_EW         1
#define COMPASS_NORTH   0
#define COMPASS_EAST    1
#define COMPASS_SOUTH   2
#define COMPASS_WEST    3
#define COMPASS_NS      4
#define COMPASS_EW      5

  /* These next ones were defined in tree.h */
#define VULNERABLE_NONE 0
#define VULNERABLE_NS   1
#define VULNERABLE_EW   2
#define VULNERABLE_ALL  3
#define NVULS           4

#define NON_VUL 0
#define VUL     1

  /* this looks like it could be in an enum but never mind */
#define TRT_NUMBER      0
#define TRT_AND2        1
#define TRT_OR2         2
#define TRT_CMPEQ       3
#define TRT_CMPNE       4
#define TRT_CMPLT       5
#define TRT_CMPLE       6
#define TRT_CMPGT       7
#define TRT_CMPGE       8
#define TRT_LENGTH      9
#define TRT_ARPLUS      10
#define TRT_ARMINUS     11
#define TRT_ARTIMES     12
#define TRT_ARDIVIDE    13
#define TRT_ARMOD       14
#define TRT_HCPTOTAL    15
#define TRT_HCP         16
#define TRT_SHAPE       17
#define TRT_NOT         18
#define TRT_HASCARD     19
#define TRT_IF          20
#define TRT_THENELSE    21
#define TRT_LOSERTOTAL  22
#define TRT_LOSER       23
#define TRT_TRICKS      24
#define TRT_RND         25
#define TRT_CONTROL     26
#define TRT_CONTROLTOTAL        27
#define TRT_SCORE       28
#define TRT_IMPS        29
#define TRT_CCCC        30
#define TRT_QUALITY     31
#define TRT_PT0TOTAL    32
#define TRT_PT0         33
#define TRT_PT1TOTAL    34
#define TRT_PT1         35
#define TRT_PT2TOTAL    36
#define TRT_PT2         37
#define TRT_PT3TOTAL    38
#define TRT_PT3         39
#define TRT_PT4TOTAL    40
#define TRT_PT4         41
#define TRT_PT5TOTAL    42
#define TRT_PT5         43
#define TRT_PT6TOTAL    44
#define TRT_PT6         45
#define TRT_PT7TOTAL    46
#define TRT_PT7         47
#define TRT_PT8TOTAL    48
#define TRT_PT8         49
#define TRT_PT9TOTAL    50
#define TRT_PT9         51
/* Next added by JGM  -- leave a gap and start at 60? */
#define TRT_DECNUM      52
#define TRT_LTC         53
#define TRT_LTCTOTAL    54
#define TRT_DDS         55
#define TRT_PAR         56
#define TRT_OPCCOMPASS  57
#define TRT_OPCSIDE     58

/* ---Action Types another possible enum --- */

#define ACT_PRINTALL     0
#define ACT_PRINT        1
#define ACT_AVERAGE      2
#define ACT_FREQUENCY    3
#define ACT_PRINTCOMPACT 4
#define ACT_PRINTONELINE 5
#define ACT_PRINTPBN     6
#define ACT_PRINTEW      7
#define ACT_PRINTNS      8
#define ACT_FREQUENCY2D  9
#define ACT_PRINTES      10
#define ACT_EVALCONTRACT 11
#define ACT_PRINTSIDE    12
#define ACT_EXP_SIDE_HLD 13
#define ACT_EXP_SEAT_HLD 14
#define ACT_CSVRPT       15
#define ACT_PRINTRPT     16

/* Constants for CCCC and Quality */
#define C4_TYPE         int
#define RK_ACE          12
#define RK_KING         11
#define RK_QUEEN        10
#define RK_JACK          9
#define RK_TEN           8
#define RK_NINE          7
#define RK_EIGHT         6

#define DEFAULT_MODE STAT_MODE
#define TWO_TO_THE_13 (1<<13)
// since int is 4 bytes get 32 different shapes. On 64 bit system could get 64 shapes if were to use long ints.
#define MAXDISTR    8*sizeof(int)

/* --------------------JGM LATER ADD ONS ------------------------------- */
  // #define RANDOM drand48
#define RANDOM(top)  gen_rand_slot( (top) )
#define SRANDOM(s)   init_rand48((s))

 #ifdef JGMDBG
    /* a print statement with a description, some sort of int value, and an explanation */
   #define DBGPRT(d,iv,ex) fprintf(stderr,"%s[%ld] at [%s]\n",(d),(long int)(iv),(ex));
   /* a print statement with a descriptrion, a value, the format to use, and a term (often new line or space) char */
   #define JGMPRT(d,v,fmt,tc) fprintf(stderr,"%s[%(fmt)](tc)",(m),(v) ) ;
 #else
   #define DBGPRT(m,i,l)
 #endif

#endif /*ifndef DEALDEFS_H */




