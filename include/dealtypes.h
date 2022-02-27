/* File dealtypes.h -- by :JGM:   2021 version Typedefs and Structures Templates*/
/* 2021-12-28 -- Added some types for IF between dds and dealer */
#ifndef DEALTYPES_H
#define DEALTYPES_H 1
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
#include <sys/types.h>   /* to import some std ISO types for our own use */
#include "dealdefs.h"   /* for symbolic constants */
#include "pointcount.h"  /* for the enum for the pointcounts. It defines idxEnd needed later */

#if 1   // Dealer Structs and types  _k means 'kind' aka type but _t reserved
typedef int decimal_k ; /* to allow easy switch to float if ever.*/
typedef int numx100_k ; /* Not used at present. Future may allow for different eval tree functions */
/* JGM refers to Dealer deals as deal52 since it takes 52 bytes to store one.
 * These next two for future change to legacy Dealer code to make the types _k so that card and deal don't conflict with
 * other users. card and deal are really names that are too generic to be used in this way */
typedef unsigned char card52_k ;
typedef card52_k deal52_k[52] ;
/* The legacy Dealer definitions. Too embedded in code to be changed at this point. TODO list */
typedef unsigned char card;
typedef card deal[52];


/*
 * decision tree and other expression stuff. JGM added a float to struct tree for future use. Not used at present
 */
struct tree {
        int             tr_type;
        struct tree     *tr_leaf1, *tr_leaf2;
        int             tr_int1;
        int             tr_int2;
        int             tr_int3;   // a grep for this thru all source files does not show any hits.
        float           tr_flt1;   // maybe store the dotnums as both decimal_k and float JGM?+?
};

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

struct action {
        struct action   *ac_next;
        int              ac_type;
        struct tree     *ac_expr1;
        struct tree     *ac_expr2;
        int              ac_int1;
        char            *ac_str1;
        union {
                struct acuft acu_f;
                struct acuft2d acu_f2d;
                struct acuavgt acuavg;
                struct contract_st acucontract; /* added this so could handle dbled contracts */
        } ac_u;
};

/* For cccc and quality */
struct context {
  deal *pd ; /* pointer to current deal */
  struct handstat *ps ; /* Pointer to stats of current deal */
};

struct handstat {
    int hs_length[NSUITS];      /* distribution */
    int hs_points[NSUITS];      /* 4321 HCP per suit */
    int hs_totalpoints;         /* Sum of above hs_points[NSUITS]; */
    int hs_bits;                /* Bitmap to check distribution */
    int hs_loser[NSUITS];       /* Losers in a suit  simple version of LTC. Does not allow for half losers e.g. AJT etc as def'd by Blue Team*/
    int hs_totalloser;          /* Losers in the hand */
    int hs_control[NSUITS];     /* Controls in a suit */
    int hs_totalcontrol;        /* Controls in the hand */
    int hs_counts[idxEnd][NSUITS];  /* pt0 thru pt9 + HCP  last one is a  dup of the hs_points array */
    int hs_totalcounts[idxEnd];

/*   Next fields added by JGM .... */
    decimal_k hs_ltc[NSUITS];     /* Modern  Losing trick count per suit. Not OPC related.;;;JGM */
    decimal_k hs_totalltc;        /* Total of above four ;;;JGM */
    int topcards[NSUITS][3];

};  /* end struct handstat */

struct opc_Vals_st {    /* The 8 values that the -q OPC report returns at this time. */
    /* Since all OPC values can take on either half or sometimes quarter values e.g. Ace=4.5 hcp
     * fscanf returns floating values. We can then convert these to decimal_k and store in sidestat
     */
    int opc_dealnum ;   /* For caching  */
    float DOP[5];       /* Side: opc per strain C, D, H, S, N *OR* Compass: one per compass +1 for hand total */
    float DOP_long ;    /* opc in longest fit; usually the max opc; will duplicate one of the above */
    float QL_suit ;     /* Quick Losers. Not strictly opc, but tedious to calculate and keeps us out of slams off two Aces */
    float QL_nt ;       /* Quick Losers in NT. A stiff is now two losers, not one. */
    int fields ;        /* For Debugging. should be the number of fields returned by dop -q call */
} ;
/* Fields that might be needed one day if ever I get around to building opc into dealer instead of a perl pgm */
/* change the #if 0 below to #if 1 to compile them in. */
#if 0
   // fields on a per hand basis
    int hs_fl[NSUITS];          /* FL per suit ? OPC fit length pts. +1 for F8, +2 for F9, +3 for F10 & up */
    int hs_fv[NSUITS];          /* FV per suit Hf or Df pts for honors in partner's suit */
    int hs_together[NSUITS];    /* Length together with partner */
    int hs_totalfl;             /* Sum of above hs_fl[NSUITS]; */
    int hs_totalfv;             /* Sum of above hs_fv[NSUITS]; */
/* OPC related fields. Each one should be x100 internally to preserve integer values thruout        */
/* User MUST enter either numbers with decimal pts in them -- digits after Dec pt are optional
 *      e.g. opc(NS)>=26. or Mult x 100 himself e.g opc(NS)>=2600                                   */
/* So when comments below refer to -1 or +1 the value in the hs_xxxx field will be -100 or +100 etc.*/
/* Most relevant is the total for the side depends on whether NT or suit contract and which suit ,  */
/* then for the hand, and much less often for the suit.                                             */
    decimal_k hs_opc_hcp[NSUITS];    /* opc version of hcp */
    decimal_k hs_opc_lpt[NSUITS];    /* opc pts for 5+ suits */
    decimal_k hs_opc_dpt[NSUITS];   /* opc for voids, stiffs, or doubletons Opener only unless bidding NT */
    decimal_k hs_opc_syn[NSUITS]; /* opc pts for 3 of top 4 honors in 5+ suits = +1, in 6+suit = +2  Add one more in each case if  4 of top 5 */

    decimal_k hs_opc_hcp_tot;  /* total for the hand, not for the side */
    decimal_k hs_opc_lpt_tot;  /* total for the hand, not for the side; max of 2 pts for Responder */
    decimal_k hs_opc_dpt_tot;  /* total for the hand, not for the side */
    decimal_k hs_opc_syn_tot;  /* total for the hand, not for the side */
    /* +ve Adjustments */
    decimal_k hs_opc_kings;    /* +1 for 3 Kings, +2 for 4 Kings */
    decimal_k hs_opc_queens;   /* +1 for 4 Queens */
    /* Negative Adjustments. Do not deduct more than 2 opc pts for NoAce, NoKing, NoQueen, and 4333 */
    decimal_k hs_opc_noaces ;  /* -1 Dedcution for no Aces */
    decimal_k hs_opc_nokings;  /* -1 Deduction for no Kings. +1 for 3 Kings, +2 for 4 Kings */
    decimal_k hs_opc_noqueens; /* -1 Deduction for no Queens. +1 for 4 Queens */
    decimal_k hs_opc_square;   /* -1 Deduction for 4333 shape */
    decimal_k hs_opc_adj_tot;  /* total of above 4 ; max of 2 pts deduction */
    decimal_k hs_opc_nt_ded_tot; /* When playing in NT -2 for each stiff, -3 for each void */

    /* These arrays need both hands in order to fill them in */
    decimal_k hs_opc_hf[NSUITS];       /* Bonus for honors in partner's 5+ suit */
    decimal_k hs_opc_misfit[NSUITS];   /* Deduction for shortness in partner's 5+ suits */
    decimal_k hs_opc_waste[NSUITS];    /* Deduction for wasted honors vs partner's stiff or void */
    decimal_k hs_opc_nowaste[NSUITS];  /* Add pts for no wasted honors vs partner's stiff or void */
    decimal_k hs_opc_dfit[NSUITS];     /* Support points in hand when this suit is trumps */

    decimal_k hs_opc_hf_tot;           /* total for the hand, not for the side */
    decimal_k hs_opc_misfit_tot;       /* total for the hand, not for the side */
    decimal_k hs_opc_waste_tot;        /* total for the hand, not for the side */
    decimal_k hs_opc_nowaste_tot;      /* total for the hand, not for the side */

   /* fields on a per side basis */
    decimal_k ss_opc_hcp_tot;     /* total for the side */
    decimal_k ss_opc_lpt_tot;     /* total for the side */
    decimal_k ss_opc_dpt_tot;     /* total for the side */
    decimal_k ss_opc_syn_tot;     /* total for the side */
    decimal_k ss_opc_adj_tot;     /* total for the side */
    decimal_k ss_opc_nt_ded_tot;  /* When playing in NT -2 for each stiff, -3 for each void. one fewer if dummy */

    /* These arrays need both hands in order to fill them in */
    decimal_k ss_opc_fitlen[NSUITS];    /* Length of fit with partner in this suit */
    decimal_k ss_opc_flpts[NSUITS];     /* Bonus opc pts for  8, 9, or 10+ fit in each suit */
    decimal_k ss_opc_dfpts[NSUITS];     /* Shortness Points when this suit is trumps; Total for the side */

    decimal_k ss_opc_hf_tot;      /* total for the side */
    decimal_k ss_opc_misfit_tot;  /* total for the side */
    decimal_k ss_opc_waste_tot;   /* total for the side */
    decimal_k ss_opc_nowaste_tot; /* total for the side */
    decimal_k ss_opc_mirror_tot;  /* total for the side */
#endif

struct sidestat {          /* OPC related numbers that need both hands of a side to be calculated */
                           /* Since all OPC values can take on either half or sometimes quarter values e.g. Ace=4.5 hcp */
                           /* We store everything internally as x100 integers e.g. Ace=4.5=>450, 15.5 OPC->15500 etc. */
/* opc related fields. I may never fill these in individually; all I really need for now is the total for the side for NT and for the Longest fit. */
    int       ss_cached ;         /* 0 Values for this side are not current; 1 values for this side are current */


/* 2022/01/15 -- These are the only ones returned by the OPC call at this time. */
    decimal_k ss_opc_hlf_nt;        /* the total opc for the side if playing in NT */
    decimal_k ss_opc_hldf_l;        /*      ditto                            in their longest fit. */
    decimal_k ss_opc_hldf[NSUITS] ; /*      ditto  when each suit is trumps. one of these will dup the longest fit. */
    decimal_k ss_quick_losers_suit; /* Quick losers for suit play  Total for the side */
    decimal_k ss_quick_losers_nt;   /* Quick losers for nt   play  Total for the side */
}; /* end struct sidestat */


struct options_st {         /* struct for storing cmd line option switches */
  int       options_error;          //  0 = none. 3= Version Info. 1= Invalid option. 2=Fixed Thread. */
  int       swapping;               // -x 0|2|3 (eXchange) JGM needs 0-9 for parameter passing
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
  char      title[MAXTITLESIZE];    // -T:   descriptive title for the set of hands.(MAXTITLE=100)
  size_t    title_len ;
  char      preDeal[4][32] ;        // Predeal Holdings in Dealer fmt Sxxxx,Hxxxxx, etc. -N, -E, -S, -W options
  int       preDeal_len[4] ;        // length of preDeal strings on cmd line.
  int       dbg_lvl;                // -D:  run program with this value of debug verbosity.
  int       dds_mode ;              // -M  1= PBN, 2= Binary, Default = 1
  char      opc_opener ;            // -O  mostly for OPC; N/E  or W/S; assume W if not set.
  int       opener;                 /*     an int for this pgm since it wants ints for Compass'es */
  int       par_vuln ;              // -P  Set Vuln for Par calcs. 0=None, 1=NS, 2=EW, 3=Both. Then converted to DDS coding.
  int       nThreads ;              // -R  Resource set nThreads 0..9; maxRamMB = nThreads*160MB
  int       maxRamMB ;              //     Calc'ed by pgm once nThreads are set.
  char      ex_fname[128] ;         // -X  Filename used for exporting ; opens FILE *fexp global var
  char      csv_fname[128];         // -C  Filename used for CSV report; if w:<fname> opens for write. else for append
  char      csv_fmode[8];
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

 #endif   // dealer structs and types

 #if 0 // JGM temp maybe unused structs and types Change to #if 1 to add them to code
// Next one just for fun by JGM. Not sure what I would use it for, ...  */
struct res_st {
        u_int16_t res_typ;
        union  {
              char    res_buff[32];
              int     res_ivals[8] ;
              double  res_dvals[4];
        } res_u;
} ;
typedef struct res_st RESULT_k ;
/* a JGM tree ... for what we don't know */
struct gtree_st {                       /* spec members in order that leads to best packing 8bytes first then 4 then 1 */
    struct gtree_st *gt_left, *gt_right ;
    //int         gt_type ;
    u_int16_t   gt_type ;
    int         gt_iv[6] ;      /* 24 bytes */
    decimal_k   gt_dv[6] ;
    float       gt_fv[6] ;
    char        gt_str[24] ; /* enough for one hand string in GIB fmt(16 chars) + 6 chars for trix + 1 char for null; */
    struct res_st *gt_resptr ; /* the struct will need to be malloc'ed if needed */
};
typedef struct gtree_st GTREE_k ;
typedef struct gtree_st *GTREEPTR_k ;
#endif  //JGM temp types



#endif /* ifndef DEALTYPES_H */


