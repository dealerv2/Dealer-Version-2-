/* File dll_Solve.h in Drafts folder -- Bunch of stuff removed for easier reading by JGM 2021-09-24 */
#ifndef DLL_SOLVE_H
#define DLL_SOLVE_H
 #ifdef __cplusplus
   #include <iostream>     /* C++ standard IO */
   #include <cstdio>       /* C++ headers for C io functions ? Note NO .h extension */
   #define EXTERN_C  extern "C"
 #else
   #include <stdio.h>       /* C standard IO */
   #define EXTERN_C
   typedef unsigned int bool ;   // not part of standard C, but defined in C++
 #endif
// Linux only.
  #define DLLEXPORT
  #define STDCALL
  #define INT8 char

/* Version 2.9.0. Allowing for 2 digit minor versions */
#define DDS_VERSION 20900

#define DDS_HANDS 4
#define DDS_SUITS 4
#define DDS_STRAINS 5

// The DDS library, libdds.a, was compiled with these hard limits defined.
// These next two are related: MAXNOOFTABLES = ( MAXNOOFBOARDS / DDS_STRAINS )
#define MAXNOOFBOARDS 200
#define MAXNOOFTABLES 40

#if 1 // error codes definitions
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
// Now the Defs for the DDS Data Structs
struct futureTricks {
  int nodes;
  int cards;
  int suit[13];
  int rank[13];
  int equals[13];
  int score[13];
};

struct deal_st {
  int trump;
  int first;
  int currentTrickSuit[3];
  int currentTrickRank[3];
  unsigned int remainCards[DDS_HANDS][DDS_SUITS];
};


struct dealPBN {
  int trump;
  int first;
  int currentTrickSuit[3];
  int currentTrickRank[3];
  char remainCards[80];
};

struct ddTableDeal{
  unsigned int cards[DDS_HANDS][DDS_SUITS];
};

struct ddTableDealPBN{
  char cards[80];
};

struct ddTableResults{
  int resTable[DDS_STRAINS][DDS_HANDS];
};
 // Now the Prototype definitions for the functions in the library
// DLLEXPORT  is the empty string on Linux
// STDCALL is empty for C __stdcall for C++ ?
// EXTERN_C is empty if using C compiler, = extern "C" if using C++ ; it means do not mangle the symbol Defs

EXTERN_C DLLEXPORT void STDCALL SetResources (  int maxMemoryMB,  int maxThreads);

EXTERN_C DLLEXPORT int STDCALL SolveBoard(
  struct deal_st dl,
  int target,
  int solutions,
  int mode,
  struct futureTricks * futp,
  int threadIndex
  );

EXTERN_C DLLEXPORT int STDCALL SolveBoardPBN(
  struct dealPBN dlpbn,
  int target,
  int solutions,
  int mode,
  struct futureTricks * futp,
  int thrId
  );

EXTERN_C DLLEXPORT int STDCALL CalcDDtable(
  struct ddTableDeal tableDeal,
  struct ddTableResults * tablep);

EXTERN_C DLLEXPORT int STDCALL CalcDDtablePBN(
  struct ddTableDealPBN tableDealPBN,
  struct ddTableResults * tablep
  );
EXTERN_C DLLEXPORT void STDCALL ErrorMessage(  int code,  char line[80]);

#endif
// end the #ifndef DDS_DLL_H


