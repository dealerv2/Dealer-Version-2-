/* This is file dealglobals.c -- defines global vars and allocates storage for them  */
/* Uses types, tags, and symbolic constants defined in other header files */
/* 2022-02-27 -- Mods for Francois Dellacherie enhanced shapes */
/* 2022-10-18 Mods for Has_card and future user_eval functionlity */
/* 2023/01/07 -- Merged in changes from V4 to fix predeal; dealcards_subs.c and globals, etc. */
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

/* JGM Added Global variables */
 /* Cmd line may set options Will override values from input file if set */
struct options_st options = {0};        /* C99 supposed to set according to type?*/
struct options_st *p_opts = &options ;
struct param_st parm = {0};
int    csv_firsthand = COMPASS_NORTH ;
char   csv_trixbuff[64] = {0} ; // room for 20 * (2digits + comma) and a bit extra
size_t csv_trixbuff_len = 0 ;

/* original cmd line switches -- many also appear in a yyparse action clause. --   would be nicer to put these all in a struct */

int maxgenerate = 0 ;          /* -g: */  /* flex action clause Must be zero for Flex to process the input file value*/
int progressmeter = 0;         /* -m */   /* this is a toggle option */
int Opener = COMPASS_WEST;     /* -O: */  /* flex action clause  0=north (or east) 1=east(or north) 2=south(or west) 3=west(or south) */
char opc_opener = 'W' ;                   /* define one so that extern will be happy. Dont want to extern a struct memb*/
int maxproduce = 0     ;       /* -p: */  /* flex action clause Init value MUST be zero for Flex to process the input file value*/
int quiet = 0 ;                /* -q */   /* option for pbn printout */
long seed  = 0 ;               /* -s: */ /* seed can now be set in Input File */
long seed_provided = -1  ;
int uppercase = 0 ;            /* -u */
int verbose = 1;               /* -v */   /* end of run stats. Default is to print them */
int swapping = 0 ;             /* -x [0|2|3] Zero turns off swapping*/
int swapindex = 0;
int errflg = 0;

/* JGM Added the following cmd line options .. Also the Opener O flag above */
int jgmDebug = 0;       /* -D 1 .. 9; level of verbosity in fprintf(stderr, ) statements. 0 = No Debug (unless defined JGMDEBUG) */
int srvDebug = 0;       /* -D has a decimal digit 0 ..9 e.g. -D 1.6 (dealer=1, server=6), or -D .9 (dealer = 0, server = 9) */
int dds_mode = 1;       /* -M 1 use Board Mode fastest for 1-5 results; 2 Use Table Mode, fastest for 5-20 results*/
int par_vuln = -1;      /* -P -1 no par calculations, 0=noneVul, 1=bothVul, 2=nsVul, 3= ewVul */
int nThreads = 1;       /* -R 1..9 MaxRam = 160 * nThreads On an 8 core box, more than 9 threads is no benefit*/
int MaxRamMB = 160 ;
int TblModeThreads = 9;
/* If we need to force Table Mode on DDS for e.g. Par Calcs, or just via -M switch, then also force extra threads. */

char title[MAXTITLESIZE]= "";  /* -T title. Usually in quotes which are removed by getopt flex action clause */
size_t  title_len = 0 ;
FILE *fexp;      /* -X file for exporting to; Normally NOT left as stdout except for testing */
FILE *fcsv;      /* -C file for csvreport. Open in append mode unless user puts w:filename */
FILE *rp_file;   /* -L rpdd.zrd file. Default is ../rpdd.zrd */
char rplib_default[64] = "../rpdd.zrd"; /* parent dir means works from either Debug or Prod */

int    rplib_mode= 0 ;
int    rp_cnt    = 0 ;
int    wrap_cnt  = 0 ;
int    rp_recnum = 0 ;
int    rplib_blk_sz = RP_BLOCKSIZE ; /* will be adjusted based on DB file size */
int    rplib_recs   = MAX_RPDD_RECS; /* will be calculated at run time */
int    rplib_max_seed=MAX_RP_SEED ;

/* -U Path name for the UserEval binary. Default is UserServer in the current directory. Can be set by -U cmd line parm
 * -U Must be full pathname. ../src/MyUserPgm will NOT work.
 * Instead: /usr/local/bin/DealerV2/UserEval/DealerServer  or /home/MyUser/MyDir/MySubDir/MyUserPgm
 */
char server_dir[SERVER_PATH_SIZE+1]  = "/home/greg19/Programming/Bridge_SW/JGMDealer/deal_v5/UserEval";
//char server_dir[SERVER_PATH_SIZE+1]  = "/usr/local/bin/DealerV2/UserEval"; // The user's current directory
char server_pgm[64]   = "DealerServer"; /* In the current directory. or user sets path name via -U switch */
char server_path[SERVER_PATH_SIZE+1] = "/usr/local/bin/DealerV2/UserEval/DealerServer";

pid_t userserver_pid = 0 ;

int dbg_dds_lib_calls = 0;
int dbg_dds_res_calls = 0;
int dbg_parscore_calls =0;
int dbg_tdd_calls = 0;
int dbg_dd_calls = 0;
int dbg_opc_calls = 0;
int dbg_opc_cmd_calls = 0;

int dbg_userserver_extcalls = 0 ;
int dbg_userserver_askquery = 0 ;

int dds_dealnum = -1 ;         /* -1 no DDS needed; 0 DDS needed; >0 ngen number of last fetch */
int opc_dealnum = -1 ;         /* -1 OPC not done ; >0 ngen number of the last opc call */

char* input_file = '\0';

/* Non Parsing Global vars*/
int nprod, ngen ;
struct handstat    hs[4] ;
struct sidestat_st ss[2] ;

/* OPC Related vars. Maybe they don't need to be globals */
char opc_cmd_buff[128] = "/usr/local/bin/DOP/dop ";
char opc_pgm_path[32]  = "/usr/local/bin/DOP/dop ";
int opc_pgmlen = 23 ;
struct opc_Vals_st opcRes ;

char export_buff[64] ; /* Room for two or three Predeal hands. */

    /* typedef char deal[52];     see typedefs.h Changed from unsigned to straight char 2022-10-24*/
deal fullpack;
deal stacked_pack;
deal curdeal;
deal *deallist;
deal small_pack;   /* 2023-01-05 cards left after predeal done */
int  small_size   = 52 ;   /* number of cards left after predeal done  */
int  stacked_size = 0  ;
int  full_size = 52 ;

/*
 * these next ones are probably no longer needed:
 *    dds_pbndeal no longer used. dealer now calls dds with the dds binary format not PBN format as originally coded
 *    deal sorted, and hand sorted no longer needed as we now sort deal by default
 */
int deal_sorted = 0 ;       /* Future use; Several print actions could benefit from knowing the hands are sorted */
int hand_sorted[4] = {0,0,0,0};

enum { STAT_MODE, EXHAUST_MODE };  /* Deleted all the Francois stuff; so only STAT_MODE relevant */

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
struct action  defaultaction = {   /* next-ptr     type       expr1            expr2             int1   str1      */
                    (struct action *) 0,     ACT_PRINTALL, (struct tree *) 0, (struct tree *) 0 , 0 , (char *) 0
} ;
 // end defaultaction intializer list
struct action *actionlist = &defaultaction;
struct var    *vars = 0 ;

        /* Vars used by code in parser action statements init them for safety */
int will_print = 0;
int maxdealer = -1;           /* set to a value to force yyparse to fill it in if needed */
int maxvuln = -1;             /* Ditto. Both Strictly descriptive in printpbn routine. Seldom used */
int userserver_reqd = 0 ;       /* will set this if we see a usereval statement in the input file */
int predeal_compass = -1 ;    /* global variable for predeal communication */
int shapeno = 0;              /* Count number of shape statements. 0-31. Defines bit in bit mask */
int use_compass[NSEATS] = {0,0,0,0};  /* skip analysis if compass never used */
int use_side[2] = {0,0};     /* opc and usereval use this. will also cause related use_compass'es to be set */


struct contract_st contract;     /* level, strain, dbled, Vul, coded, string*/

/* from deuce to Ace  -- (weight of a void is 64) */
/*                      2, 3, 4, 5, 6, 7, 8, 9, T, J, Q,  K,  A     */
int ltc_weights[13] = { 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 8, 16, 32 }; /* may be superceded by CardAttr_RO  below */
int points[13]      = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2,  3,  4 }; /* Goren HCP values */
/* the pointcount array itself */
int alt_tbl_idx = -1 ;       /* Global var set by Yacc file code to track which altcount is being modified */
int pointcount_index;        /* global var set by Yacc file code to track which rank is being modified */
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
    /* 2  3  4  5  6  7  8  9  T  J  Q  K    A */
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



