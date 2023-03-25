#ifndef DDS_DLL_H
#define DDS_DLL_H
/* File DDS_DLL_H  --
 * This file includes the minimum amount of the DDS dll.h file. Nothing from the Dealer side of the house
 */
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
// A bunch of constant #defines
#if 1
#ifndef DDS_HANDS
#define DDS_HANDS 4
#endif
#ifndef DDS_SUITS
#define DDS_SUITS 4
#endif
#ifndef DDS_STRAINS
#define DDS_STRAINS 5
#endif
#ifndef MAXNOOFTABLES
#define MAXNOOFTABLES 40
#endif
#ifndef MAXNOOFBOARDS
#define MAXNOOFBOARDS 5
#endif
#define R2  0x0004
#define R3  0x0008
#define R4  0x0010
#define R5  0x0020
#define R6  0x0040
#define R7  0x0080
#define R8  0x0100
#define R9  0x0200
#define RT  0x0400
#define RJ  0x0800
#define RQ  0x1000
#define RK  0x2000
#define RA  0x4000

#define K2       2
#define K3       3
#define K4       4
#define K5       5
#define K6       6
#define K7       7
#define K8       8
#define K9       9
#define KT      10
#define KJ      11
#define KQ      12
#define KK      13
#define KA      14

#ifdef __cplusplus
   #include <iostream>     /* C++ standard IO */
   #include <cstdio>       /* C++ headers for C io functions ? Note NO .h extension */
   #define EXTERN_C  extern "C"
 #else
   #include <stdio.h>       /* C standard IO */
   #define EXTERN_C
   #define TRUE  1
   #define FALSE 0
   typedef unsigned int bool ;   // not part of standard C, but defined in C++
 #endif
#endif

// A bunch of error message defines  -- all look C compatible
#if 1
// KEEP THEM ALL. I don't know which ones ddCalcTablePBN and SidesPar return
// Leave these in for now. I may be using SolveBoard next --- JGM
// Error codes. See interface document for more detail.
// Call ErrorMessage(code, line[]) to get the text form in line[].

// Success.
#define RETURN_NO_FAULT 1
#define TEXT_NO_FAULT "Success"

// Currently happens when fopen() fails or when AnalyseAllPlaysBin()
// get a different number of boards in its first two arguments.
#define RETURN_UNKNOWN_FAULT -1
#define TEXT_UNKNOWN_FAULT "General error"

// SolveBoard()
#define RETURN_ZERO_CARDS -2
#define TEXT_ZERO_CARDS "Zero cards"

// SolveBoard()
#define RETURN_TARGET_TOO_HIGH -3
#define TEXT_TARGET_TOO_HIGH "Target exceeds number of tricks"

// SolveBoard()
#define RETURN_DUPLICATE_CARDS -4
#define TEXT_DUPLICATE_CARDS "Cards duplicated"

// SolveBoard()
#define RETURN_TARGET_WRONG_LO -5
#define TEXT_TARGET_WRONG_LO "Target is less than -1"

// SolveBoard()
#define RETURN_TARGET_WRONG_HI -7
#define TEXT_TARGET_WRONG_HI "Target is higher than 13"

// SolveBoard()
#define RETURN_SOLNS_WRONG_LO -8
#define TEXT_SOLNS_WRONG_LO "Solutions parameter is less than 1"

// SolveBoard()
#define RETURN_SOLNS_WRONG_HI -9
#define TEXT_SOLNS_WRONG_HI "Solutions parameter is higher than 3"

// SolveBoard(), self-explanatory.
#define RETURN_TOO_MANY_CARDS -10
#define TEXT_TOO_MANY_CARDS "Too many cards"

// SolveBoard()
#define RETURN_SUIT_OR_RANK -12
#define TEXT_SUIT_OR_RANK \
  "currentTrickSuit or currentTrickRank has wrong data"

// SolveBoard
#define RETURN_PLAYED_CARD -13
#define TEXT_PLAYED_CARD "Played card also remains in a hand"

// SolveBoard()
#define RETURN_CARD_COUNT -14
#define TEXT_CARD_COUNT "Wrong number of remaining cards in a hand"

// SolveBoard()
#define RETURN_THREAD_INDEX -15
#define TEXT_THREAD_INDEX "Thread index is not 0 .. maximum"

// SolveBoard()
#define RETURN_MODE_WRONG_LO -16
#define TEXT_MODE_WRONG_LO "Mode parameter is less than 0"

// SolveBoard()
#define RETURN_MODE_WRONG_HI -17
#define TEXT_MODE_WRONG_HI "Mode parameter is higher than 2"

// SolveBoard()
#define RETURN_TRUMP_WRONG -18
#define TEXT_TRUMP_WRONG "Trump is not in 0 .. 4"

// SolveBoard()
#define RETURN_FIRST_WRONG -19
#define TEXT_FIRST_WRONG "First is not in 0 .. 2"

// AnalysePlay*() family of functions.
// (a) Less than 0 or more than 52 cards supplied.
// (b) Invalid suit or rank supplied.
// (c) A played card is not held by the right player.
#define RETURN_PLAY_FAULT -98
#define TEXT_PLAY_FAULT "AnalysePlay input error"

// Returned from a number of places if a PBN string is faulty.
#define RETURN_PBN_FAULT -99
#define TEXT_PBN_FAULT "PBN string error"

// SolveBoard() and AnalysePlay*()
#define RETURN_TOO_MANY_BOARDS -101
#define TEXT_TOO_MANY_BOARDS "Too many boards requested"

// Returned from multi-threading functions.
#define RETURN_THREAD_CREATE -102
#define TEXT_THREAD_CREATE "Could not create threads"

// Returned from multi-threading functions when something went
// wrong while waiting for all threads to complete.
#define RETURN_THREAD_WAIT -103
#define TEXT_THREAD_WAIT "Something failed waiting for thread to end"

// Tried to set a multi-threading system that is not present in DLL.
#define RETURN_THREAD_MISSING -104
#define TEXT_THREAD_MISSING "Multi-threading system not present"

// CalcAllTables*()
#define RETURN_NO_SUIT -201
#define TEXT_NO_SUIT "Denomination filter vector has no entries"

// CalcAllTables*()
#define RETURN_TOO_MANY_TABLES -202
#define TEXT_TOO_MANY_TABLES "Too many DD tables requested"

// SolveAllChunks*()
#define RETURN_CHUNK_SIZE -301
#define TEXT_CHUNK_SIZE "Chunk size is less than 1"

#endif
// end of error code definitions inside #if 1

// A bunch of DDS structure definitions  Only those for functions we use or are likely to use in future
#if 1
/* The singular version of the structs typedefs  for solving one deal at a time */
// a deal in DDS 'binary' format. Each suit int is a bit mask saying if the card is present or not*/
struct futureTricks {
  int nodes;
  int cards;
  int suit[13];
  int rank[13];
  int equals[13];
  int score[13];   // either the # of tricks (in context) or -1 (failed to reach target) or zero (no tricks can be taken)
};
// this next one changed from struct deal, bec it was conflicting with dealer's typedef deal. Might not even need this one anyway
struct deal_st {
  int trump;
  int first;
  int currentTrickSuit[3];  // up to three cards may have already been played to this trick
  int currentTrickRank[3];
  unsigned int remainCards[DDS_HANDS][DDS_SUITS];  // Note the ORDER it is NOT the same as a Res Table[s][h]
};
struct dealPBN {
  int trump;
  int first;
  int currentTrickSuit[3];
  int currentTrickRank[3];
  char remainCards[80];
};
struct ddTableDeal{
  unsigned int cards[DDS_HANDS][DDS_SUITS]; // these could be 16 bit ints, since only 13 bits in each is used.
};
struct ddTableDealPBN{                      // a deal in pbn format. each line should only be 69-70 bytes long.
  char cards[80];
};
struct ddTableResults {                      // a set of 20 results for one board
  int resTable[DDS_STRAINS][DDS_HANDS];      // Note the ORDER ; it is [Suit][Hands] here.
};

struct contractType{
  int underTricks; /* 0 = make 1-13 = sacrifice */
  int overTricks; /* 0-3, e.g. 1 for 4S + 1. */
  int level; /* 1-7 */
  int denom; /* 0 = No Trumps, 1 = trump Spades, 2 = trump Hearts,
                  3 = trump Diamonds, 4 = trump Clubs */
  int seats; /* One of the cases N, E, W, S, NS, EW;
                   0 = N 1 = E, 2 = S, 3 = W, 4 = NS, 5 = EW */
};

struct parResultsMaster {
  int score; /* Sign according to the NS view */
  int number; /* Number of contracts giving the par score */
  struct contractType contracts[10]; /* 10 Par contracts expressed as 5 integers each*/
};

 // Dont think I use this next one. For Ref Only */
struct parResults {
  /* index = 0 is NS view and index = 1
     is EW view. By 'view' is here meant
     which side that starts the bidding. */
  char parScore[2][16];
  char parContractsString[2][128];
};

// ----------- The Plural version of some of the above for solving several hands at once ------ Not Used At this time.
/* Note the 's' plural form of Deals.
Essentially an array of the singular form, with the number of entries used in the noOfTables int.
*/
struct ddTableDeals{
  int noOfTables;
  struct ddTableDeal deals[MAXNOOFTABLES * DDS_STRAINS];
};

struct ddTableDealsPBN{
  int noOfTables;
  struct ddTableDealPBN deals[MAXNOOFTABLES * DDS_STRAINS];
};

//JGM Added
struct ddHolding_st {
    unsigned int cards[DDS_HANDS][DDS_SUITS] ;
};

#endif
//----------Prototypes for the Functions used for tricks and Par calcs - Linux version ------------------------
// ---- These are all 'guarded' by EXTERN_C clauses which are null in C. So if the DDS C++ code was honest these
// ---  can all be called from C.
#if 1
EXTERN_C  int  CalcDDtable(
  struct ddTableDeal tableDeal,
  struct ddTableResults * tablep );

EXTERN_C  int  CalcDDtablePBN(
  struct ddTableDealPBN tableDealPBN,
  struct ddTableResults * tablep );

EXTERN_C  int  SidesParBin (             // takes only VUL, returns parResultsMaster
  struct ddTableResults * tablep,
  struct parResultsMaster sidesRes[2],
  int vulnerable );

EXTERN_C  int   SolveBoardPBN(
  struct dealPBN dlpbn,
  int target,
  int solutions,
  int mode,
  struct futureTricks * futp,
  int threadId);

EXTERN_C  int  SolveBoard (
  struct deal_st dl,
  int target,
  int solutions,
  int mode,
  struct futureTricks * futp,
  int threadIndex  );

EXTERN_C  void  ErrorMessage(  int code,  char line[80]);
EXTERN_C  void  SetResources(  int maxMemoryMB,  int maxThreads);
#endif
// end of Prototype declarations for the DDS library

#endif
//  end of ifndef DDS_DLL_H

