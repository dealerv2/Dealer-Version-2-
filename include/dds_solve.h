#ifndef DDS_SOLVE_H
#define DDS_SOLVE_H
// This file makes available some minimal DDS definitions that Dealer based subs might need.

#define DDS_HANDS   4
#define DDS_SUITS   4
#define DDS_STRAINS 5
#define RETURN_NO_FAULT 1

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

 void  SetResources (  int maxMemoryMB,  int maxThreads);

 int SolveBoard(
  struct deal_st dl,
  int target,
  int solutions,
  int mode,
  struct futureTricks * futp,
  int threadIndex
  );

 int SolveBoardPBN(
  struct dealPBN dlpbn,
  int target,
  int solutions,
  int mode,
  struct futureTricks * futp,
  int thrId
  );

 int CalcDDtable(
  struct ddTableDeal tableDeal,
  struct ddTableResults * tablep);

 int  CalcDDtablePBN(
  struct ddTableDealPBN tableDealPBN,
  struct ddTableResults * tablep
  );
  void ErrorMessage(  int code,  char line[80]);

#endif
// end the #ifndef DDS_DLL_H
