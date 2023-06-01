/* File dealtypes.h -- by :JGM Dealer Version 2 Types and Templates. */

/* Date      Version  Author  Description
* 2021/12/28 1.0.0    JGM     Collect all dealer symbolic constants and macros in one place.
* 2022/02/09 2.1.5    JGM     FD shapes, and printrpt ported from deal_v3
* 2022/09/25 2.2.0    JGM     Added constants for Bucket Frequency functionality
* 2022/10/18 2.5.0    JGM     Changed typedef for card; added Has_cards array; other changes for user_eval functionality.
*/
#ifndef DEALTYPES_H
#define DEALTYPES_H 1
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
#include <sys/types.h>   /* to import some std ISO types for our own use */
#ifndef DEALDEFS_H
   #include "dealdefs.h"   /* for symbolic constants */
#endif
#include "pointcount.h"  /* for the enum for the pointcounts. It defines idxEnd needed later */

typedef int decimal_k ; /* to allow easy switch to float if ever.*/
typedef int numx100_k ; /* Not used at present. Future may allow for different eval tree functions */
/*
 * JGM refers to Dealer deals as deal52 since it takes 52 bytes to store one.
 * These next two for future change to legacy Dealer code to make the types _k so that card and deal don't conflict with
 * other users. card and deal are really names that are too generic to be used in this way
 */
typedef unsigned char BYTE_k ;
typedef char     card52_k ;                  /* not used anywhere */
typedef card52_k DEAL52_k[52] ;
/* The legacy Dealer definitions. Too embedded in code to be changed at this point. TODO list */
/* 2022-10-18. Removed unsigned Since JGM changed the macros to shift 4 from shift 6,
 * unsigned causes warnings and requires casting to avoid them
 */
//typedef unsigned char card;
typedef char card;
typedef card deal[52];

/*
 * decision tree and other expression stuff. JGM added a float to struct tree for future use. Not used at present
 */
struct tree {
        int             tr_type;
        struct tree     *tr_leaf1, *tr_leaf2;
        int             tr_int1;
        int             tr_int2;
        int             tr_int3;   // JGM used for USEREVAL coded hand/suit/idx 2022-11-07
        float           tr_flt1;   // maybe store the dotnums as both decimal_k and float JGM?+?
};
typedef struct tree TREE_ST ;

struct expr {                    // for the printes action. a printes term is either a char string or an expr tree ptr */
        struct tree    *ex_tr;
        char           *ex_ch;
        struct expr    *next;
};

struct var {
        struct var *v_next;
        char *v_ident;
        struct tree *v_tree;
} ;
/* JGM added this next one so we can have a decode / encode contract function and return all pieces of it */
struct contract_st { // we encode the contract so it will fit into an int and we can store in a tree node int.
    int level ;     // 1-7; encode with contract = level*5; decode with level = (contract%40)/5
    int strain;     // 0 = C, 1 = D, 2 = H, 3 = S, 4 = NT; encode with += strain; decode with contract %5 ;
    int dbl ;       // 0 = undbl, 1 = dbled, 2= redbled; encode with += 40*dbl; decode with contract/40 ;
    int vul ;       // 0 = nv, 1 = vul ; encode with |= (vul << 8 ); decode with (contract >> 8) & 0x1 ;
    int coded;      // results of all of above if used. A number less than 248
    char c_str[8];  // space for the contract string such as z3Hxx for e.g. 3 Hearts ReDoubled
    int  trix[14];  // 2023-01-17 incr trix[i] each time dds(compass,strain) returns value "i"
 } ;
typedef struct contract_st CONTRACT_k ;  // use _k suffix for 'kind' since _t for 'type' is POSIX reserved.

/* struct to describe a csvterm to be put in csv report. Only one of csv_tr, csv_str, csv_hands is set at any one time */
struct csvterm_st {
      struct tree           *csv_tr;    /* traverse this tree to evaluate the expression */
      char                  *csv_str;   /* a label, col heading, or possibly a script var contents */
      int                    csv_hands; /* a bit mask showing which hands to print at this point */
      int                    csv_trix ; /* Mask for hands. Either 1 bit set 1,2,4,8 or all bits set 15 */
      struct csvterm_st     *next;      /* Pointer to next csv_term */
}; /* end csv_term_st */

#define FD_SHAPE_LEN 3070  /* FD shapes can expand to over 1300 chars frequently. An example file gives 2378 */
struct fd_shape_st {
   size_t shape_len ;
   char fd_shape_str[3072];  // two more than FD_SHAPE_LEN because Flex needs TWO nulls at end of string buffer.
                             // testing: shape{north, 5+Mxxx:h>=c,h>=d } gives an output of 1173 chars 167 distributions
                             //  shape{west, (xxxx):d>c or h>s} gives 2378 chars and 338 distributions

} ;

struct qcoded_st {
        unsigned int idx   : 8 ;  // low bits
        unsigned int suit  : 8 ;
        unsigned int hand  : 8 ;
        unsigned int sflag : 4 ;
        unsigned int hflag : 4 ; // high bits
    } ;

union qcoded_ut {
      unsigned int coded_all ;
      struct qcoded_st ucbits ;
} ;


/*
 * Actions to be taken
 */
 /* This next struct, added by JGM to the union in the action struct to allow better average calcualtions
  * will allow us to calc on the fly at any time the average, and std dev of the quantity in question
  * The formula for the variance is:  (sqsum*count - sum*sum)/(count*(count-1)).
  * we keep variance not sd to avoid the sqroot calculations on every sample. save sd sqroot for CleanupAction at EOJ
  */
struct acuavgt{
    long count ; // should = nprod (or nprod+1?)
    long sum   ; // should be the same as ac_int1 above
    long sqsum ; // the only new piece of data
    double avg;  // current value of average
    double vary; // current value of the SAMPLE variance
};

struct acuft{
  long acuf_lowbnd;
  long acuf_highbnd;
  long acuf_uflow;
  long acuf_oflow;
  long*acuf_freqs;
};

struct acuft2d{
  long acuf_lowbnd_expr1;
  long acuf_highbnd_expr1;
  long acuf_lowbnd_expr2;
  long acuf_highbnd_expr2;
  long*acuf_freqs;
};
 /* JGM Add. 2022-09-14;
  * Added new functionality to allow freq reporting for quantities that use DOTNUMs such as cccc, quality, dop, and ltc
  * and hence have ranges typically between 0 and 4000 (i.e between 0 and 40.00  in steps of 50 or 100 typically. )
  * a bucket size of 1 used by the normal frequency functions is not necessary and uses up a LOT of room for quantities
  * that are  always zero anyway.
  * Replaced longs with ints, since ints are now 32 bits on 64 bit machines and we don't need longs to count occurrences */
struct bucket_st {
   int Lo, Hi, Sz, Num ;
   int *Names ;         /* ptr to list of ints giving the lowest value in the bucket.  Used in printout */
   char Dir   ;         /* put at end so not to mess up alighment */
} ;
struct acubf_st {
  struct bucket_st bkt ; /* struct to hold all the parms that describe a bucket */
  int *bkt_freqs;       /* ptr to 1D table of counters */
};
struct acubf2d_st {
  struct bucket_st bktd ;   /* Parms describing the buckets in the Down direction   aka the '1' dir in the orig frequency2D funcs*/
  struct bucket_st bkta ;   /* Parms describing the buckets in the Across direction aka the '2' dir */
  int *bkt_freqs;  /* ptr to 2D table of counters */
};
 /* end of JGM add  for bucket frequency  */
struct action {
        struct action   *ac_next;
        int              ac_type;
        struct tree     *ac_expr1;
        struct tree     *ac_expr2;
        int              ac_int1;
        char            *ac_str1;
        union ac_ut {
                struct acuft acu_f;
                struct acuft2d acu_f2d;
                struct acuavgt acuavg;
                struct contract_st acucontract; /* JGM added this so could handle dbled contracts */
                struct acubf_st acu_bf ;        /* JGM added this for 1D bucket frequency */
                struct acubf2d_st acu_bf2d ;    /* JGM added this for 2D bucket frequency */
        } ac_u;
};

/* For cccc and quality --- Does not seem to be used by either c4.c or c4.h*/
/* might be useful for the upcoming user_eval functionality */
struct context_st {
  deal *pd ; /* pointer to current deal */
  struct handstat *ps ; /* Pointer to stats of current deal */
};

struct handstat {
    int hs_seat ;               /* make it easy to deduce the player given the pointer */
    int hs_length[NSUITS];      /* distribution */
    int hs_points[NSUITS];      /* 4321 HCP per suit */
    int hs_totalpoints;         /* Sum of above hs_points[NSUITS]; */
    int hs_bits;                /* Bitmap to check distribution */
    int hs_loser[NSUITS];       /* Losers in a suit  simple version of LTC. Does not allow for half losers e.g. AJT etc as def'd by Blue Team*/
    int hs_totalloser;          /* Losers in the hand */
    int hs_control[NSUITS];     /* Controls in a suit */
    int hs_totalcontrol;        /* Controls in the hand */
    int hs_counts[idxEnd][NSUITS];  /* pt0 thru pt9 & HCP which is a dup of the hs_points array */
    int hs_totalcounts[idxEnd];     /* pt0 thru pt9 & HCP total of all 4 suits */

/*   Next fields added by JGM .... */
    int square_hand ;              /* == 1 for 4333 zero otherwise */
    int hs_ltc[NSUITS];     /* Modern  Losing trick count per suit. Counts in half losers. Values x 100 */
    int hs_totalltc;        /* Total ltc in the hand Counts in half losers. Values x 100 */
    int topcards[NSUITS][3];      /* These are bit masks that allow easy calc of LTC via a switch statement */
    int Has_card[NSUITS][13];    /* CCCC uses cards down to the 8; printoneline and others use hascard a lot. */

};  /* end struct handstat */
typedef struct handstat HANDSTAT_k ;


/*
 * Minimal set of opc values. Only ones returned by perl program for now; as returned as floats, and as used in pgm
 */
struct opc_Vals_st {          /* The 8 values that the -q OPC report returns as floats. struct filled by scanf */
    /* Since all OPC values can take on either half or sometimes quarter values e.g. Ace=4.5 hcp
     * fscanf returns floating values. We can then convert these to decimal_k (aka int x100) and store in sidestat
     */
    int opc_dealnum ;   /* For caching  */
    float DOP[5];       /* Side: opc per strain C, D, H, S, N *OR* Compass: one per compass +1 for hand total */
    float DOP_long ;    /* opc in longest fit; usually the max opc; will duplicate one of the above */
    float QL_suit ;     /* Quick Losers. Not strictly opc, but tedious to calculate and keeps us out of slams off two Aces */
    float QL_nt ;       /* Quick Losers in NT. A stiff is now two losers, not one. */
    int fields ;        /* For Debugging. should be the number of fields returned by dop -q call */
} ;

typedef struct sidestat_st {  /* the floats converted to int x100 for use by this pgm */
    int       ss_cached ;         /* 0 Values for this side are not current; 1 values for this side are current */
    decimal_k ss_opc_hlf_nt;        /* the total opc for the side if playing in NT */
    decimal_k ss_opc_hldf_l;        /*      ditto                            in their longest fit. */
    decimal_k ss_opc_hldf[NSUITS] ; /*      ditto  when each suit is trumps. one of these will dup the longest fit. */
    decimal_k ss_quick_losers_suit; /* Quick losers for suit play  Total for the side */
    decimal_k ss_quick_losers_nt;   /* Quick losers for nt   play  Total for the side */
} SIDESTAT_k ; /* end struct sidestat */


/* struct for storing cmd line option switches */
struct options_st {
  int       options_error;          //  0 = none. 3= Version Info. 1= Invalid option. 2=Fixed Thread. 4=Usage msg*/
  int       swapping;               // -x 0|2|3 (eXchange) JGM needs 0-9 for scripting parameter passing
  int       progress_meter;         // -m
  int       quiet;                  // -q
  int       upper_case;             // -u default? toggle?
  int       verbose;                // -v   print stats re seed and time taken at end of run
  int       show_version;           // -V
  /* These next options all need  a value */
  int       max_generate;           // -g:
  int       max_produce;            // -p:
  long int  seed;                   // -s:
  long int  seed_provided;
  /* these next ones are by JGM.  */
  char      title[MAXTITLESIZE];    // -T:   descriptive title for the set of hands.(MAXTITLE=100 in docs, 256 in defs)
  size_t    title_len ;
  char      preDeal[4][32] ;        // Predeal Holdings in Dealer fmt Sxxxx,Hxxxxx, etc. -N, -E, -S, -W options
  int       preDeal_len[4] ;        // length of preDeal strings on cmd line.
  int       dbg_lvl;                // -D:  run program with this value of debug verbosity.
  int       srv_dbg_lvl;            //      If usereval forks a server, run server with this value of debug verbosity
  char      rplib_fname[128];       // -L  path to rpdd.zrd db file. ../rpdd.zrd will work.
  long int  rplib_seed;             //     offset in 1000 record incr into the DB. re-use of -s switch.
  int       rplib_mode;             //      zero if not in lib mode; 1 if -L switch
  int       dds_mode ;              // -M  1 Single result mode; 2 all 20 strain-compass combinations; used for par etc.
  char      opc_opener ;            // -O  mostly for OPC; N/E  or W/S; assume W if not set.
  int       opener;                 /*     an int for this pgm since it wants ints for Compass'es */
  int       par_vuln ;              // -P  Set Vuln for Par calcs. 0=None, 1=NS, 2=EW, 3=Both. Then converted to DDS coding.
  int       nThreads ;              // -R  Resource set nThreads 0..9; maxRamMB = nThreads*160MB
  int       maxRamMB ;              //     Calc'ed by pgm once nThreads are set.
  char      ex_fname[128] ;         // -X  Filename used for exporting ; opens FILE *fexp global var
  char      csv_fname[128];         // -C  Filename used for CSV report; if w:<fname> opens for write. else for append
  char      csv_fmode[8];
  char      userpgm[256]  ;         // -U   Path of the User provided external server executable. Default=UserServer ROOT Dir.
} ;   /* end options_st */
#define PARAM_SIZE 126
struct param_st {
   int  scripting ;
   char script_var[10][128] ;         // Two more than PARAM_SIZE bec Flex needs two string terminators.
   size_t script_var_len[10];         //
} ;

typedef struct tree *TREE_PTR ;         /* JGM?+ to keep track of trees for debug and learn */
struct treeptr_st {
    char tree_class ;
    int tree_type ;
    TREE_PTR tree_ptr ;
} ;

#endif /* ifndef DEALTYPES_H */


