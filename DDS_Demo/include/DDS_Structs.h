/* DDS_Structs.h  --- the DDS data structures -- */
struct futureTricks
{
  int nodes;
  int cards;
  int suit[13];
  int rank[13];
  int equals[13];
  int score[13];
};

struct deal
{
  int trump;
  int first;
  int currentTrickSuit[3];
  int currentTrickRank[3];
  unsigned int remainCards[DDS_HANDS][DDS_SUITS];
};


struct dealPBN
{
  int trump;
  int first;
  int currentTrickSuit[3];
  int currentTrickRank[3];
  char remainCards[80];
};


struct boards
{
  int noOfBoards;
  struct deal deals[MAXNOOFBOARDS];
  int target[MAXNOOFBOARDS];
  int solutions[MAXNOOFBOARDS];
  int mode[MAXNOOFBOARDS];
};

struct boardsPBN
{
  int noOfBoards;
  struct dealPBN deals[MAXNOOFBOARDS];
  int target[MAXNOOFBOARDS];
  int solutions[MAXNOOFBOARDS];
  int mode[MAXNOOFBOARDS];
};

struct solvedBoards
{
  int noOfBoards;
  struct futureTricks solvedBoard[MAXNOOFBOARDS];
};

struct ddTableDeal
{
  unsigned int cards[DDS_HANDS][DDS_SUITS];
};

struct ddTableDeals
{
  int noOfTables;
  struct ddTableDeal deals[MAXNOOFTABLES * DDS_STRAINS];
};

struct ddTableDealPBN
{
  char cards[80];
};

struct ddTableDealsPBN
{
  int noOfTables;
  struct ddTableDealPBN deals[MAXNOOFTABLES * DDS_STRAINS];
};

struct ddTableResults
{
  int resTable[DDS_STRAINS][DDS_HANDS];
};

struct ddTablesRes
{
  int noOfBoards;
  struct ddTableResults results[MAXNOOFTABLES * DDS_STRAINS];
};

struct parResults
{
  /* index = 0 is NS view and index = 1
     is EW view. By 'view' is here meant
     which side that starts the bidding. */
  char parScore[2][16];
  char parContractsString[2][128];
};


struct allParResults
{
  struct parResults presults[MAXNOOFTABLES];
};

struct parResultsDealer
{
  /* number: Number of contracts yielding the par score.
     score: Par score for the specified dealer hand.
     contracts:  Par contract text strings.  The first contract
       is in contracts[0], the last one in contracts[number-1].
       The detailed text format is is given in the DLL interface
       document.
  */
  int number;
  int score;
  char contracts[10][10];
};

struct contractType
{
  int underTricks; /* 0 = make 1-13 = sacrifice */
  int overTricks; /* 0-3, e.g. 1 for 4S + 1. */
  int level; /* 1-7 */
  int denom; /* 0 = No Trumps, 1 = trump Spades, 2 = trump Hearts,
                  3 = trump Diamonds, 4 = trump Clubs */
  int seats; /* One of the cases N, E, W, S, NS, EW;
                   0 = N 1 = E, 2 = S, 3 = W, 4 = NS, 5 = EW */
};

struct parResultsMaster
{
  int score; /* Sign according to the NS view */
  int number; /* Number of contracts giving the par score */
  struct contractType contracts[10]; /* Par contracts */
};

struct parTextResults
{
  char parText[2][128]; /* Short text for par information, e.g.
                Par -110: EW 2S EW 2D+1 */
  bool equal; /* true in the normal case when it does not matter who
            starts the bidding. Otherwise, false. */
};


struct playTraceBin
{
  int number;
  int suit[52];
  int rank[52];
};

struct playTracePBN
{
  int number;
  char cards[106];
};

struct solvedPlay
{
  int number;
  int tricks[53];
};

struct playTracesBin
{
  int noOfBoards;
  struct playTraceBin plays[MAXNOOFBOARDS];
};

struct playTracesPBN
{
  int noOfBoards;
  struct playTracePBN plays[MAXNOOFBOARDS];
};

struct solvedPlays
{
  int noOfBoards;
  struct solvedPlay solved[MAXNOOFBOARDS];
};

