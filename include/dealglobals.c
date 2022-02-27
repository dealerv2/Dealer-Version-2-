/* This is file dealglobals.c -- defines global vars and allocates storage for them  */
/* Uses types, tags, and symbolic constants defined in other header files */
#ifndef DEALGLOBALS_C
#define DEALGLOBALS_C
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
#include "std_hdrs.h"
#include "dealtypes.h"          /* some type defs and struct tags needed below. Will include dealdefs.h also */

char *crlf = "\n";
char card_id[13]    = {'2','3','4','5','6','7','8','9','T','J','Q','K','A'};
char rank_ids[]     =  "23456789TJQKA";
char strain_id[6]   = {'C','D','H','S','N', 'L' };
char seat_id[4]     = {'N','E','S','W'};
char *player_name[] = {"North", "East", "South", "West" };
char *suit_name[]   = {"Club", "Diamond", "Heart", "Spade"};
char suitid[5][3]   = {"C:", "D:", "H:", "S:", "N:"};
char side_seat[2][2]= { {'N','S'} , {'E','W'} };
int  side_hand[2][2]= { { 0 , 2 } , { 1 , 3 } } ;

/* Global variables */
struct options_st options = {0};             /* Cmd line will set these. Will override values from input file if set */
struct param_st parm ;
int    csv_firsthand = COMPASS_NORTH ;
char   csv_trixbuff[64] ; // room for 20 * (2digits + comma) and a bit extra
size_t csv_trixbuff_len ;
/* for CCCC */
struct context c;
/* original cmd line switches -- many also appear in a yyparse action clause. --   would be nicer to put these all in a struct */
int swapping;                  /* -x [0|2|3] */
int swapindex;
int computing_mode;            /* e */             /* e is no longer valid option. var kept just in case */
int maxgenerate = 0 ;          /* -g: */  /* flex action clause */
int progressmeter = 0;         /* -m */   /* this is a toggle option */
int Opener = COMPASS_WEST;     /* -O: */  /* flex action clause  0=north (or east) 1=east(or north) 2=south(or west) 3=west(or south) */
char opc_opener = 'W' ;                   /* define one so that extern will be happy. Dont want to extern a struct memb*/
int maxproduce = 0     ;       /* -p: */  /* flex action clause */
int quiet;                     /* -q */
long seed  = 0 ;               /* -s: */ /* seed can now be set in Input File */
long seed_provided = 0  ;
int uppercase = 0 ;            /* -u */
int verbose = 1;               /* -v */
int errflg;

/* JGM Added the following cmd line options .. Also the Opener O flag above */
int jgmDebug = 0;       /* -D 1 .. 9; level of verbosity in fprintf(stderr, ) statements. 0 = No Debug (unless defined JGMDEBUG) */
int dds_mode = 1;       /* -M 0 Dont use DDS; 1=Solve One Board(default in main); 2= Solve CalcTable BIN; 3 = solve CalcTablePBN ; 4=both) */
int par_vuln = -1;      /* -P -1 no par calculations, 0=none, 1=both, 2=ns, 3= ew */
int nThreads = 9;       /* -R 0..10 MaxRam = 160 * nThreads */
int MaxRamMB = 1440 ;
char title[MAXTITLESIZE]= "";  /* -T title. Usually in quotes which are removed by getopt flex action clause */
size_t  title_len = 0 ;
FILE *fexp;      /* -X file for exporting to; Normally NOT left as stdout except for testing */
FILE *fcsv;      /* -C file for csvreport. Open in append mode unless user puts filename:w */

int dbg_dds_lib_calls = 0;
int dbg_dds_res_calls = 0;
int dbg_parscore_calls =0;
int dbg_tdd_calls = 0;
int dbg_dd_calls = 0;
int dbg_opc_calls = 0;
int dbg_opc_cmd_calls = 0;
int dbg_analyze_active = 0;

int dds_dealnum = -1 ;         /* -1 no DDS needed; 0 DDS needed; >0 ngen number of last fetch */
int opc_dealnum = -1 ;         /* -1 OPC not done ; >0 ngen number of the last opc call */

char* input_file = '\0';

/* Non Parsing Global vars*/
int nprod, ngen ;
struct handstat hs[4] ;
struct sidestat ss[2] ;

/* OPC Related vars. Maybe they don't need to be globals */
char opc_cmd_buff[128] = "/usr/local/bin/DOP/dop ";
char opc_pgm_path[32]  = "/usr/local/bin/DOP/dop ";
int opc_pgmlen = 23 ;
struct opc_Vals_st opcRes ;

char export_buff[64] ; /* only need about 50 chars now; prob wont use export buff for csv since re-init too often? ... */

/* CSV Report Stuff */


    /* typedef usigned char deal[52];     see typedefs.h */
deal fullpack;
deal stacked_pack;
deal curdeal;
deal *deallist;
char dds_pbndeal[80] ;    /* the deal in a format DDS likes. 69 chars; similar to printoneline but not quite */
int deal_sorted = 0 ;       /* Future use; Several print actions could benefit from knowing the hands are sorted */
int hand_sorted[4] = {0,0,0,0};

enum { STAT_MODE, EXHAUST_MODE };  /* Deleted all the Francois stuff; so only STAT_MODE relevant */
int computing_mode = DEFAULT_MODE;

char lcrep[] = "23456789tjqka";
char ucrep[] = "23456789TJQKA";
#define representation (uppercase ? ucrep : lcrep );

int biasdeal[4][4] = { {-1, -1, -1, -1}, {-1, -1, -1, -1},
                       {-1, -1, -1, -1}, {-1, -1, -1, -1}};

int imp_tbl[24] = { 10,   40,   80,  120,  160,  210,  260,  310,  360,  410,  490,  590,
                   740,  890, 1090, 1190, 1490, 1740, 1990, 2240, 2490, 2990, 3490, 3990  };

/* Various handshapes can be asked for. For every shape the user is
   interested in a number (shapeno) is generated. In every distribution that fits that
   shape the corresponding bit is set in the distrbitmaps 4-dimensional array.
   This makes looking up a shape a small constant cost.
*/

int ***distrbitmaps[14];

int results[2][5][14];  /* evalcontract: [side][strain][number of tricks] Counts number of times each happens */

struct tree    defaulttree = {TRT_NUMBER, NIL, NIL, 1, 0};
struct tree   *decisiontree = &defaulttree;
struct action  defaultaction = {   /* next-ptr     type       expr1            expr2             int   str1      */
                    (struct action *) 0,     ACT_PRINTALL, (struct tree *) 0, (struct tree *) 0 , 0 , (char *) 0 ,
            /* union.averages     count sum sumsq var   std */
                     .ac_u.acuavg={ 0,   0,   0,   0.0, 0.0 }     // init struct acuavgt
} ;
 // end defaultaction intializer list
struct action *actionlist = &defaultaction;
struct var    *vars = 0 ;

        /* Vars used by code in parser action statements */
int will_print;
int maxdealer;
int maxvuln;
int predeal_compass;         /* global variable for predeal communication */
int shapeno ;
int use_compass[NSEATS] = {0,0,0,0};
int use_vulnerable[NVULS] ;
int use_side[2] = {0,0};     /* for opc so we dont call DOP for both sides if only one side needed */


struct contract_st contract;     /* level, strain, dbled, Vul, coded, string*/

/* from deuce to Ace  -- (weight of a void is 64) */
/*                      2, 3, 4, 5, 6, 7, 8, 9, T, J, Q,  K,  A     */
int ltc_weights[13] = { 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 8, 16, 32 }; /* may be superceded by CardAttr_RO  below */
int points[13]      = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2,  3,  4 }; /* Goren HCP values */
/* the pointcount array itself */
int countindex = -1;
int pointcount_index;    /* global variable for pointcount communication */
int tblPointcount [idxEnd][13] = {
    /* tables tens to c13 MUST be in this order to make sense to the user.
     * Put HCP at very end since it has its own routines to handle it.
     * Order of other tables does not matter; they are unlikely to be changed; Controls and LTC weights are pretty fixed
     */
    /* 2  3  4  5  6  7  8  9  T  J  Q  K  A */
    {  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, /* tens = pt0 so idx must be 0 */
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, /* jacks */
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, /* queens */
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, /* kings */
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, /* aces */
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, /* top2 */
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1}, /* top3 */
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1}, /* top4 */
    {  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1}, /* top5 */
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 4, 6}, /* c13 = pt9 idx=9 */
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4}, /* HCP idx = (idxEnd - 1) ; put at end since it has its own zero etc rtn*/
} ; /* End tblPointCount */

int CardAttr_RO [idxEndRO][13] = { /* Values Not changeable by user via altcount or pointcount cmd */
    /* 2  3  4  5  6  7  8  9  T  J  Q   K   A */
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,   2},  /* controls idxControls = 0 */
    {  1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 8, 16, 32},  /* ltc weights. Will ID WHICH of the top cards we have. */
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 9,  13},  /* Kleinman Pts Need to add 1 synergy pt to a suit with A or K and 1 other*/
} ; /* End CardAttr_RO */

/* JGM Debugging and learning about Dealer use of Trees */
struct treeptr_st tree_ptrs[100] ;      /* JGM?+ an array to store pointers for tracing and debugging. */
int num_trees = 0 ;                     /* running count of  trees allocated so far */
int treelev   = 0; /* the level we are at in the decision tree */
int showtree  = 1; /* default to show it if in DBG mode */
int treedepth = 0; /* in case we want to debug the tree walking */


#endif /* ifndef DEALGLOBALS_C */



