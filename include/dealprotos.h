/* File dealprotos.h -- define function prototypes both those used in actions from Flex and Yacc and also ones used by main */
/* 2023/01/07 -- Merged in changes from V4 to fix predeal; dealcards_subs.c and globals, etc. */
#ifndef DEALPROTOS_H
#define DEALPROTOS_H
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include "dealtypes.h"

/* JGM?+ routine to track trees for later study and debug Later add VarTree, ActionList etc ?*/
extern void showdecisiontree(struct tree *t);
extern void showvarlist(struct var *v ) ;
extern void showactionlist(struct action *a );
extern void showAltCounts( void ) ;
extern void showdistrbits( int ***distrbitmaps[14] ) ;


/* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
        /* routines in, or called by, yyparse action clauses */
        /* Some of these functions are in the dealyacc.y file, some are in the dealparse_subs.c file    */
extern int              d2n(char shape[4]) ;   /* 4char dist string like 4315 to an int -- also non numeric dist strings */
extern struct tree      *var_lookup(char *s, int mustbethere) ;                  /* yyparse action clause */
extern struct action    *newaction(int type, struct tree *p1, char *s1, int, struct tree * ) ; /* yyparse action clause */
extern struct tree      *newtree (int, struct tree*, struct tree*, int, int);    /* yyparse action clause */
extern struct expr      *newexpr(struct tree* tr1, char* ch1, struct expr* ex1); /* yyparse action clause */
extern void              bias_deal(int suit, int compass, int length) ;         /* yyparse action clause */
extern void              predeal_holding(int compass, char *holding) ;          /* yyparse action clause */
extern void              predeal (int player, card onecard);                    /* called by predeal_holding */
extern void              insertshape(char s[4], int any, int neg_shape) ;       /* yyparse action clause */
extern void              new_var(char *s, struct tree *t) ;                     /* yyparse action clause */
extern void              setshapebit (int cl, int di, int ht, int sp, int msk, int excepted); /* called by action clause */
extern void              clearpointcount ();                 /* yyparse action clause set 13 elem array to zero*/
extern void              clearpointcount_alt (int cin);      /* yyparse action clause affects alternate counts */
extern void              pointcount (int index, int value);  /* yyparse action clause affects HCPs*/
extern void              zerocount (int points[]);           /* called by clearpointcount etc. */
extern struct csvterm_st *new_csvterm (struct tree *tr1, char *str1, int hand_mask, int trix_mask, struct csvterm_st *csv1); /* yyparse action clause */
extern struct tree      *newquery(int tag, int side, int compass, int suit, int idx); /* yyparse action clause */
/* Evaluation and condition stuff called by main, after the parsing during the evaluation phase */
#define interesting() ((int)evaltree(decisiontree))
extern void          analyze (deal d, struct handstat *hsbase);
extern int           evaltree (struct tree *t);
extern int hascard   (deal d, int player, card thiscard) ;
extern void sortHand(card *hnd_ptr, int size) ;
extern void sortDeal(deal dl ) ;
    /* routines called during the post parse, evaluation phase */
extern int           imps (int scorediff) ;
extern int           score(int vul, int coded_contract, int tricks );
extern int           undbled_score (int vuln, int suit, int level, int tricks);
extern CONTRACT_k    decode_contract(int coded_c) ;
extern void          fmt_contract_str(char *str, int level, int strain, int dbl, int vul );
extern int           get_tricks (int pn, int dn);
extern int           dd (deal d, int l, int c);     /* get cached results of true_dd */
extern int           true_dd (deal d, int l, int c); /* call the GIB DD executable */
extern int           gib_tricks (char);  /* Convert GIB trix char [0-9A-D] to an integer */
/* from c4.h */
extern int           eval_cccc (int seat) ;
extern int           suit_quality (int seat, int suit);
extern int           quality (int seat, int suit);
extern int           cccc (int seat);
/* from dealmain_subs.h */
extern long int      init_rand48( long int seed ) ; /* seed the RNG with user's seed or random seed from kernel */
extern int           gen_rand_slot ( int topval ) ;  /* gen a random int between [0 .. (topval - 1) ] */

/* from dealdds_subs.c */
int dds_tricks   (int compass, int strain ) ; /* reads the DDSRES_k struct and returns the number of tricks asked for */
int dds_parscore (int compass ) ;             /* reads the DDSRES_k struct and returns the par score asked for */
int csv_trix     ( char *buff, int h_mask ) ; /* fmts buff with a list of trick counts for the hands asked for */

/* from UserServer_subs.c */
extern int ask_query(int qtag, int side, int qcoded ) ;

/* print and other action stuff Called by main if the deal is 'interesting' ie meets the condition */
extern void action ();
extern void cleanup_action () ;  /* maybe mod this one to clear out some cache's etc? ; it seems to happen at the very end of each deal */
extern void evalcontract (struct action *acp) ;
extern void showevalcontract ( struct action *acp, int nh);
extern void printdeal (deal d);
extern void printside (deal d, int side); /*JGM replaced printew with this one. */
extern void printhands (int boardno, deal *dealp, int player, int nhands);
extern int  printpbn (int, deal);
extern char *Hand52_to_pbnbuff (int p, char *dl, char *buff ) ;
void printhands_pbn(FILE *fp, int mask, deal curdeal ) ;
extern void fprintcompact (FILE * f, deal d, int oneline); /* used for both GIB input and oneline printout */
#define printoneline(d) (fprintcompact(stdout, d, 1))
#define printcompact(d) (fprintcompact(stdout, d, 0))


        /* Utility stuff */
        /*     Initializations Delete the extern on some of these if never accessed outside ?*/
extern void          initdistr ();
extern void          initevalcontract ();
extern void          setup_action ();
extern void          initprogram( struct options_st *opt_ptr) ;

extern void          newpack (deal d);
extern void          setup_deal ();
extern void          deal_cards (deal d) ;
extern void          Shuffle(deal d, int size) ;
//extern int           shuffle (deal d) ; /* replaced by Shuffle */

extern pid_t         setup_userserver( char *pathname ) ;
extern int           cleanup_userserver ( pid_t pid ) ;


        /* low level functions and macros These may be useful in more than just the dealer.c file */
extern int           hascard (deal d, int player, card onecard) ;

extern card          make_card (char rankchar, char suitchar);       /* used in yylex() */
extern int           make_contract (char *c_str ) ;  /* Flex passes yytext[1]... = [1-7][CDHSN][x]{0,2} */
extern char         *mycalloc (size_t nel, size_t siz);   /* alloc RAM num_elem * sizeof(elem) */
extern char         *mystrcpy( char *s) ;                /* re-implement strdup - Windows? */
extern void          swap2 (deal d, int p1, int p2);     /* swap d[p1] with d[p2] (shuffle) */
extern long int      init_rand48( long int seed );          /* called by main */
extern int           gen_rand_slot ( int topval );          /* called by main */

        /* Yacc, Lex, and error stuff */
extern void          error   (char *);
extern void          yyerror (char *);
extern int           yyparse ( void );
extern int           yywrap  ( void );

/* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */

#endif /* end ifndef DEALPROTOS_H */
















