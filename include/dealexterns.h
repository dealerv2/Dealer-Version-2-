 /* File dealgbls.h  2021-09-17 ;;;JGM  This file is the extern defs for the stuff in dealglbls.c */
 /*  Date      Version   Author  Description
  * 2022/01/02 1.0.0    JGM     Collect all dealer symbolic constants and macros in one place.
  * 2022-02-27 2.1.7    JGM     Mods for Francois Dellacherie enhanced shapes
  * 2022/09/15 2.2.0    JGM     Mods for Bucket Frequency Functionality
  */
#ifndef DEALEXTERNS_H
#define DEALEXTERNS_H
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif

    #include "std_hdrs.h"
    #include "dealdefs.h"
    #include "dealtypes.h"
    #include "deal_dds.h"

    /* cmd line parameter variables  some used by yyparse */
 extern struct options_st options ; /* defined and init in globals.c */
 extern int     lino ;              /* incremented in yylex() when comments etc found. Not any more ?*/
 extern int     nprod,maxproduce;
 extern int     ngen,maxgenerate;
 extern int     verbose, uppercase;
 extern int     swapping, swapindex ;
 extern long    seed, seed_provided ;
 extern int     quiet ;
 extern char    title[MAXTITLESIZE] ;   /* set from cmd line or from input file directly from dealflex.l */
 extern size_t  title_len;
 extern int     Opener;             /* Dealer uses COMPASS_NORTH etc which is an int */
 extern char    opc_opener;         /* opc uses characters W, N , E, S */
 extern int     jgmDebug ;          /* cmd line as opposed to #define Debug flag. 0 = None. 9=verbose */
 extern char   *input_file;
 extern int     dds_mode;           /* 0=GIB, 1=SolveOneBoard, 2=CalcDDtable Binary, 3=CalcDDtablePBN */
 extern int     par_vuln;
 extern int     progressmeter;
 extern int     nThreads;           /* -R 0..10 MaxRam = 160 * nThreads */
 extern int     MaxRamMB ;
 extern int     csv_firsthand;
 extern char    csv_trixbuff[64] ; // room for 20 * (2digits + comma) and a bit extra
 extern size_t  csv_trixbuff_len ;
 extern FILE   *fexp ;
 extern FILE   *fcsv ;
 extern int errflg ;
  /* used by yyparse in its action clauses or directly by Flex*/
 extern int     maxdealer;        /* Flex */
 extern int     maxvuln;          /* Flex */
 extern char   *crlf ;            /* either \r\n for Windows or \n for Linux/Unix. MacOS=? */
 extern char    card_id[13];
 extern char    strain_id[6];
 extern char    seat_id[4];
 extern char    rank_ids[];
 extern char   *player_name[4] ;
 extern char   *suit_name[4];
 extern char    suit_id[5][3];
 extern char    side_seat[2][2];
 extern int     side_hand[2][2];
 extern char    lcrep[] ;
 extern char    ucrep[] ;

 /*
 * Various handshapes can be asked for. For every shape the user is
 * interested in a number is generated. In every distribution that fits that
 * shape the corresponding bit is set in the distrbitmaps 4-dimensional array.
 * This makes looking up a shape a small constant cost.
 */
 extern int shapeno;
 extern int ***distrbitmaps[14];        /* 4 Dimensional structure for Distributions */
/* Global vars for communication between Flex and Bison */
 extern int use_vulnerable[NVULS];
 extern int use_compass[NSEATS];  /* Global var: 1 means this seat's hand needs to be analyzed */
 extern int use_side[2] ;         /* Global var: 1 means we need both hands for this side and the side also */

 extern int will_print;                 /* Is there a print action? if yes default produce = 40 */
 extern int predeal_compass;            /* global var for predeal communication */
 extern struct contract_st contract;
 extern struct param_st parm ;         /* for script parameters $0 thru $9 */
 //extern struct fd_shape_st fd_shapes ;  // Francois Dellacherie shapes
 //extern char fd_cmd[16] ;
// extern char fd_cmd_buff[256];

 extern struct tree    defaulttree ;
 extern struct tree   *decisiontree ;
 extern struct action  defaultaction ;
 extern struct action *actionlist;
 extern struct var    *vars;

 extern deal           fullpack;
 extern deal           stacked_pack;
 extern deal           curdeal;
 extern deal          *deallist;
 extern deal           small_pack;   /* 2023-01-05 cards left after predeal done */
 extern int            small_size;   /* number of cards left after predeal done  */
 extern int            stacked_size;
 extern int            biasdeal[4][4];

 extern int deal_sorted ;       /* set when a deal is sorted. In case we need to be sure */
 extern int hand_sorted[4] ;

/* Some vars for saving the results of hand analysis and DD solutions */
 extern int            results[2][5][14];  // for evalcontract
 extern struct handstat hs[4];
 extern struct sidestat ss[2];
 extern DDSRES_k dds_res_bin;
 extern int dds_dealnum ;      /* for keeping track of whats in the dds cache */
 extern int opc_dealnum ;      /* for keeping track of whats in the opc cache */

extern char opc_cmd_buff[128];
extern char opc_pgm_path[32] ;
extern int opc_pgmlen ;
extern struct opc_Vals_st opcRes ;
extern char export_buff[64] ;

 extern int     points[13] ;            /* Goren (or other) HCP values A=4, K=3 etc. */
 extern int     ltc_weights[13] ;       /* Weight that allows coding the top cards in a suit. A=32, K=16, Q=8, J=4, T=2 x=1*/
 /* the pointcount array of various point count arrays. Needs the file pointcount.h to be included with this file.*/
 extern int tblPointcount [idxEnd][13] ;
 extern int alt_tbl_idx;
 extern int pointcount_index;
 /* Some ReadOnly Attributes of various cards Currently Controls is the main one, also Kleinman pts and ltc weights(dup) */
 extern int CardAttr_RO [idxEndRO][13] ;

/* table that converts a score diff to IMPs */
 extern int imp_tbl[24] ;

 /* some debugging stuff */
extern int      treelev;                /* the level we are at in the decision tree */
extern int      showtree;               /* default to show it if in DBG mode */
extern int      treedepth;              /* in case we want to debug the tree walking */
extern struct treeptr_st tree_ptrs[];
extern int      num_trees ;
extern int dbg_dds_lib_calls;
extern int dbg_dds_res_calls;
extern int dbg_parscore_calls;
extern int dbg_tdd_calls;
extern int dbg_dd_calls;
extern int dbg_opc_calls;
extern int dbg_opc_cmd_calls;
extern int dbg_analyze_active;

#endif /* ifndef DEALEXTERNS_H */
