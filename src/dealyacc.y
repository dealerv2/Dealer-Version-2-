  // :JGM:  File dealyacc.y the original defs.y with re-organization esp various %code sections
  // :JGM:  MODS by JGM 2021-10-20
  // :JGM:  2021-11-14  -- Removed vulnerable, dealer, generate, and produce from the parser. Now done in flex direct.
  // :JGM:  2021-12-14  -- Added ltc counting; title; printside/printns/printew; deal52_to_pbn for dds etc.
  // :JGM:  2021-12-20  -- Started mods for DDS tricks function.
  // :JGM:  2021-12-23  -- Extensive changes to EVALCONTRACT grammar and action clauses
  // :JGM:  2022-02-10  -- Completed first version of csvreport
  // :JGM:  2022-02-11  -- Started adding TRIX functionality to csvreport


%code top {
    /* Entries here will not be put to the dealyacc.tab.h file, but will go to the dealyacc.tab.c file */
    /* we don't want the definition of yyerror to go to flex, just  the prototype */
    // The _GNU_SOURCE def will affect somewhat what bison generates from macros etc. So put it very first
    #ifndef _GNU_SOURCE
       #define _GNU_SOURCE
    #endif
    float bisonvers = 3.0 ;
  #include "../include/std_hdrs.h"
}

%code requires {
    /* :JGM: Entries here go to both the dealyacc.tab.h and the dealyacc.tab.c files */
    /* :JGM: these definitions are required by the %union declaration */
  extern int yylex( void );             /* yylex comes from the flex object file  at link time */
  void yyerror( char *s ) ;             /* yyerror defined later in this file in code provides section */
#include "../include/dealdefs.h"       /* symbolic constants for everything */
#include "../include/dealtypes.h"      /* types; some needed by union aka YYSTYPE so put them here */
#include "../include/dealexterns.h"    /* extern statements for the various global vars. */

} /* end code requires */


  /*
  # :JGM: added y_flt to allow fractional values such as 2.5 losers or Aces = 4.5 hcp etc. Also OPC of 44.25 e.g. is possible
  # :JGM: added y_arr2[2] y_arr4[4] y_arr8[8] just in case I need it for something
  # :JGM: Note that this Union is 64 bits wide on a 64 bit machine because it contains pointers.
  # :JGM: thus circa 2021 making y_arr2 (which is 2*32 bits) and y_arr4 (which is 4*16 bits) should not increase its size
  # :JGM: circa 2022 Linux has largely taken over the Unix world. Microsoft WSL v2.0 will run almost any Linux app.
  #       Therefore I am writing using any linux or gnu feature that is useful without worrying about portability
  */
%union {
        int            y_int;       /* 4 bytes */
        char          *y_str;       /* 8 bytes */
        struct tree   *y_tree;      /* 8 bytes */
        struct action *y_action;    /* 8 bytes */
        struct expr   *y_expr;      /* 8 bytes */
        char           y_distr[8];  /* 8 bytes was 4 bytes when ints and ptrs were 4. We need 5 to hold distr + NULL */
        float          y_flt;       /* 4 bytes */
        int            y_arr2[2];   /* 8 bytes */
        short int      y_arr4[4];   /* 8 bytes */
        char           y_arr8[8];   /* 8 bytes */
        decimal_k      y_decnum ;   /* 4 bytes int containing float * 100 */
        numx100_k      y_numx100;   /* 4 bytes all the numbers of the form 99.99 will be x100 */
        struct csvterm_st *y_csv ;  /* 8 bytes */
}  /* end union */
  /*
   these next specs give both the associativity and the precedence. The precedence goes from low to high. Query is lowest.
   Bison warns that sometimes %left or %nonassoc is not needed. Just use %precedence instead.
   */

%left QUERY
%left COLON
%left OR2
%left AND2
%left CMPEQ
%left CMPNE
%left CMPLT
%left CMPLE
%left CMPGT
%left CMPGE
%left ARPLUS
%left ARMINUS
%left ARTIMES
%left ARDIVIDE
%left ARMOD
%nonassoc NOT

%token HCP
%token SHAPE
%token ANY
%token EXCEPT
%token CONDITION
%token ACTION
%token PRINT
%token PRINTALL
%token PRINTEW
%token PRINTNS
%token PRINTSIDE
%token PRINTPBN
%token PRINTCOMPACT
%token PRINTONELINE
%token AVERAGE
%token HASCARD
%token FREQUENCY
%token PREDEAL
%token POINTCOUNT
%token ALTCOUNT
%token CONTROL
%token LOSER
%token QUALITY
%token CCCC
%token TRICKS
%token NOTRUMPS
%token EVALCONTRACT
%token ALL
%token NONE
%token SCORE
%token IMPS
%token RND
%token PT0
%token PT1
%token PT2
%token PT3
%token PT4
%token PT5
%token PT6
%token PT7
%token PT8
%token PT9
%token PRINTES
%token  OPC
%token  LTC
%token  DDS
%token  PAR
%token  EXPORT
%token  DEAL
%token  CSVRPT
%token  TRIX
%token  PRINTRPT

%token <y_int> NUMBER
%token <y_str> HOLDING
%token <y_str> STRING
%token <y_str> IDENT
%token <y_int> COMPASS
%token <y_int> VULN
%token <y_int> SUIT
%token <y_int> CARD
%token <y_int> CONTRACT
%token <y_distr> DISTR
%token <y_distr> DISTR_OR_NUMBER

  // :JGM: added next 2 lines: DECNUM= nn.mm; side = NS or EW
%token <y_numx100> DECNUM
%token <y_int> SIDE

%type <y_tree>  expr
%type <y_int> number
%type <y_int> compass
%type <y_int> printlist
%type <y_int> shlprefix
%type <y_int> any
%type <y_distr> shape
%type <y_action> actionlist action
%type <y_expr> exprlist
%type <y_str> optstring
  /* Next by JGM 2021-09-20 */
%type <y_int> side
%type <y_numx100> decnum
%type <y_csv> csvlist

%printer { fprintf(yyo, "[%d]", $$); } <y_int> <y_numx100>
%printer { fprintf(yyo, "[%s]", $$); } <y_str> <y_distr>
    /* %printer { fprintf(yyo, "[%g]", $$); } <y_flt>  No floats in this grammar */

%start defs

%code provides {   /* code that is needed by the parser action clauses It also goes to tab.h !! */

 #include "../include/dealprotos.h"  /* prototypes for procedures incl some called in parse action clauses */
  extern int yylex() ;  /* yylex will be provided by the scanner produced file */

}  /* end code provides */

%%

defs
        : /* empty */
        | defs def
        ;

def     : PREDEAL predealargs
        | POINTCOUNT { clearpointcount(); pointcount_index=12;} pointcountargs
        | ALTCOUNT number
                { clearpointcount_alt($2); pointcount_index=12;} pointcountargs
        | CONDITION expr
                {  decisiontree = $2; }
        | expr
                {  decisiontree = $1; }
        | IDENT '=' expr
                { new_var($1, $3); }
        | ACTION actionlist
                { actionlist = $2; }
        ;

predealargs
        : predealarg
        | predealargs predealarg
        ;

predealarg
        :  COMPASS { predeal_compass = $1;} holdings
        |  SUIT '(' COMPASS ')' CMPEQ NUMBER {bias_deal($1,$3,$6);}
        ;

holdings
        : HOLDING
                { predeal_holding(predeal_compass, $1); }
        | holdings ',' HOLDING
                { predeal_holding(predeal_compass, $3); }
        ;

pointcountargs
        : /* empty */
        | number
                { pointcount(pointcount_index, $1);
                  pointcount_index--;
                }
          pointcountargs
        ;

compass
        : COMPASS
                { use_compass[$1] = 1; $$= $1;  }
        ;

side
        : SIDE
                { use_side[$1] = 1 ; $$ = $1; } /* need to fill handstat struct for side if use_side[i]=1*/
        ;

shlprefix
        : ','
                { $$ = 0; }
        | ARPLUS
                { $$ = 0; }
        | /* empty */
                { $$ = 0; }
        | ARMINUS
                { $$ = 1; }
        ;

any
        : /* empty */
                { $$ = 0; }
        | ANY
                { $$ = 1; }
        ;

/* AM990705: extra production to fix unary-minus syntax glitch */
number
        : NUMBER
        | ARMINUS NUMBER
                { $$ = - $2; }
        | DISTR_OR_NUMBER
                { $$ = d2n($1); }
        ;

decnum  : DECNUM { $$ = $1 ; }
        ;

shape
        : DISTR
        | DISTR_OR_NUMBER
        ;

shapelistel
        : shlprefix any shape
                { insertshape($3, $2, $1); }
        ;

shapelist
        : shapelistel
        | shapelist shapelistel
        ;

expr
        : number
                { $$ = newtree(TRT_NUMBER, NIL, NIL, $1, 0); }
        | decnum
                { $$ = newtree(TRT_DECNUM, NIL, NIL, $1, 100); }
        | IDENT
                { $$ = var_lookup($1, 1); }
        | SUIT '(' compass ')'
                { $$ = newtree(TRT_LENGTH, NIL, NIL, $1, $3);  }
        | HCP '(' compass ')'
                { $$ = newtree(TRT_HCPTOTAL, NIL, NIL, $3, 0);  }
        | HCP '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_HCP, NIL, NIL, $3, $5);  }
        | PT0 '(' compass ')'
                { $$ = newtree(TRT_PT0TOTAL, NIL, NIL, $3, 0);  }
        | PT0 '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_PT0, NIL, NIL, $3, $5);  }
        | PT1 '(' compass ')'
                { $$ = newtree(TRT_PT1TOTAL, NIL, NIL, $3, 0);  }
        | PT1 '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_PT1, NIL, NIL, $3, $5);  }
        | PT2 '(' compass ')'
                { $$ = newtree(TRT_PT2TOTAL, NIL, NIL, $3, 0); }
        | PT2 '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_PT2, NIL, NIL, $3, $5); }
        | PT3 '(' compass ')'
                { $$ = newtree(TRT_PT3TOTAL, NIL, NIL, $3, 0); }
        | PT3 '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_PT3, NIL, NIL, $3, $5);  }
        | PT4 '(' compass ')'
                { $$ = newtree(TRT_PT4TOTAL, NIL, NIL, $3, 0); }
        | PT4 '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_PT4, NIL, NIL, $3, $5);  }
        | PT5 '(' compass ')'
                { $$ = newtree(TRT_PT5TOTAL, NIL, NIL, $3, 0);  }
        | PT5 '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_PT5, NIL, NIL, $3, $5);  }
        | PT6 '(' compass ')'
                { $$ = newtree(TRT_PT6TOTAL, NIL, NIL, $3, 0);  }
        | PT6 '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_PT6, NIL, NIL, $3, $5);  }
        | PT7 '(' compass ')'
                { $$ = newtree(TRT_PT7TOTAL, NIL, NIL, $3, 0);  }
        | PT7 '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_PT7, NIL, NIL, $3, $5);  }
        | PT8 '(' compass ')'
                { $$ = newtree(TRT_PT8TOTAL, NIL, NIL, $3, 0);  }
        | PT8 '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_PT8, NIL, NIL, $3, $5);  }
        | PT9 '(' compass ')'
                { $$ = newtree(TRT_PT9TOTAL, NIL, NIL, $3, 0);  }
        | PT9 '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_PT9, NIL, NIL, $3, $5);  }
        | LOSER '(' compass ')'
                { $$ = newtree(TRT_LOSERTOTAL, NIL, NIL, $3, 0);  }
        | LOSER '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_LOSER, NIL, NIL, $3, $5);  }
        | CONTROL '(' compass ')'
                { $$ = newtree(TRT_CONTROLTOTAL, NIL, NIL, $3, 0);  }
        | CONTROL '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_CONTROL, NIL, NIL, $3, $5);  }
        | CCCC '(' compass ')'
                { $$ = newtree(TRT_CCCC, NIL, NIL, $3, 0); }
        | QUALITY '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_QUALITY, NIL, NIL, $3, $5); }
        | SHAPE '(' compass ',' shapelist ')'
                {
          $$ = newtree(TRT_SHAPE, NIL, NIL, $3, 1<<(shapeno++));
          if (shapeno >= 32) {
            yylloc.first_line = @3.first_line ; /* do this so bison activates yylloc et. al. */
            yyerror("Too many shapes -- only 32 allowed!\n");
            YYERROR;
          }
        }
        | HASCARD '(' COMPASS ',' CARD ')'
                { $$ = newtree(TRT_HASCARD, NIL, NIL, $3, $5); }
        | TRICKS '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_TRICKS, NIL, NIL, $3, $5); }
        | TRICKS '(' compass ',' NOTRUMPS ')'
                { $$ = newtree(TRT_TRICKS, NIL, NIL, $3, 4); }
        | SCORE '(' VULN ',' CONTRACT ',' expr ')'
                { $$ = newtree(TRT_SCORE, $7, NIL, $3, $5); } // TRT_SCORE will have to decode the contract
        | IMPS '(' expr ')'
                { $$ = newtree(TRT_IMPS, $3, NIL, 0, 0); }
        | '(' expr ')'
                { $$ = $2; }
        | expr CMPEQ expr
                { $$ = newtree(TRT_CMPEQ, $1, $3, 0, 0); }
        | expr CMPNE expr
                { $$ = newtree(TRT_CMPNE, $1, $3, 0, 0); }
        | expr CMPLT expr
                { $$ = newtree(TRT_CMPLT, $1, $3, 0, 0); }
        | expr CMPLE expr
                { $$ = newtree(TRT_CMPLE, $1, $3, 0, 0); }
        | expr CMPGT expr
                { $$ = newtree(TRT_CMPGT, $1, $3, 0, 0); }
        | expr CMPGE expr
                { $$ = newtree(TRT_CMPGE, $1, $3, 0, 0); }
        | expr AND2 expr
                { $$ = newtree(TRT_AND2, $1, $3, 0, 0); }
        | expr OR2 expr
                { $$ = newtree(TRT_OR2, $1, $3, 0, 0); }
        | expr ARPLUS expr
                { $$ = newtree(TRT_ARPLUS, $1, $3, 0, 0); }
        | expr ARMINUS expr
                { $$ = newtree(TRT_ARMINUS, $1, $3, 0, 0); }
        | expr ARTIMES expr
                { $$ = newtree(TRT_ARTIMES, $1, $3, 0, 0); }
        | expr ARDIVIDE expr
                { $$ = newtree(TRT_ARDIVIDE, $1, $3, 0, 0); }
        | expr ARMOD expr
                { $$ = newtree(TRT_ARMOD, $1, $3, 0, 0); }
        | expr QUERY expr COLON expr
                { $$ = newtree(TRT_IF, $1, newtree(TRT_THENELSE, $3, $5, 0, 0), 0, 0); }
        | NOT expr
                { $$ = newtree(TRT_NOT, $2, NIL, 0, 0); }
        | RND '(' expr ')'
                { $$ = newtree(TRT_RND, $3, NIL, 0, 0);  }

       | LTC '(' compass ')'
                { $$ = newtree(TRT_LTCTOTAL, NIL, NIL, $3,  0 ); }
       | LTC '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_LTC,      NIL, NIL, $3, $5 );  }
       | OPC '(' side ')'
                 { $$ = newtree(TRT_OPCSIDE, NIL, NIL, $3,  5);  } // 'strain' = 5 means longest fit
       | OPC '(' side ',' SUIT ')'
                 { $$ = newtree(TRT_OPCSIDE, NIL, NIL, $3,  $5);  } // opc pts if strain is the trump suit.
       | OPC '(' side ',' NOTRUMPS ')'
                 { $$ = newtree(TRT_OPCSIDE, NIL, NIL, $3,  4);  } // 'strain' = 4 means Notrump

       | DDS '(' compass ',' SUIT ')'
                { $$ = newtree(TRT_DDS, NIL, NIL, $3, $5); ;}
       | DDS '(' compass ',' NOTRUMPS ')'
                { $$ = newtree(TRT_DDS, NIL, NIL, $3,  4); }
       | PAR '(' side ')'
                { $$ = newtree(TRT_PAR, NIL, NIL, $3,  0);  /* PAR needs all 20 results. Forces mode=2. */ }
      // PAR ( side , both|none|NS|EW ) will need to resurrect the VULNERABLE token that is now just a flex action.
        ;
      // The exprlist is only used for the printes action, not for the condition decision tree.
exprlist
        : expr
                { $$ = newexpr($1, 0, 0); }
        | STRING
                { $$ = newexpr(0, $1, 0); }
        | exprlist ',' expr
                { $$ = newexpr($3, 0, $1); }  /* add a struct containing ptr to expr($3) to end of expr list $1 */
        | exprlist ',' STRING
                { $$ = newexpr(0, $3, $1); }  /* add a struct containing ptr to string($3) to end of expr list $1 */
        ;
     // The csvlist is only used for the csvrpt action. Like printes but allows hands and trix as well as expr and strings
csvlist
        : COMPASS
               { $$ = new_csvterm(0,  0,  1<<$1,   0,    0) ; } /* set bit in bit mask */
        | SIDE
               { $$ = new_csvterm(0,  0,  5<<$1,   0,    0) ; } /* side NS=0 sets compass 0 and 2; EW=1 sets 1 & 3 */
        | DEAL
               { $$ = new_csvterm(0,  0,    15,    0,    0) ; }    /* set all bits */
        | expr
               { $$ = new_csvterm($1, 0,    0,     0,    0) ; }
        | STRING
               { $$ = new_csvterm(0, $1,    0,     0,    0) ; }
        | TRIX '(' COMPASS ')'
               { $$ = new_csvterm(0,  0,    0, 1<<$3,    0) ; }
        | TRIX '(' DEAL ')'
               { $$ = new_csvterm(0,  0,    0,    15,    0) ; }
        | csvlist ',' COMPASS
               { $$ = new_csvterm(0,  0,  1<<$3,   0,   $1) ; }/* add struct with bit mask to end of list */
        | csvlist ',' SIDE
               { $$ = new_csvterm(0,  0,  5<<$3,   0,   $1) ; }
        | csvlist ',' DEAL
               { $$ = new_csvterm(0,  0,    15,    0,   $1) ; }
        | csvlist ',' expr
               { $$ = new_csvterm($3, 0,    0,     0,   $1) ; }  /* add a struct containing ptr to expr($3) to end of csvlist $1 */
        | csvlist ',' STRING
                { $$ = new_csvterm(0, $3,   0,     0,   $1) ; }  /* add a struct containing ptr to string($3) to end of csvlist $1 */
        | csvlist ',' TRIX '(' COMPASS ')'
               { $$ = new_csvterm(0,  0,    0, 1<<$5,   $1) ; }
        | csvlist ',' TRIX '(' DEAL ')'
               { $$ = new_csvterm(0,  0,    0,    15,   $1) ; }
        ;


actionlist
        : action
                { $$ = $1; }
        | action ',' actionlist
                { $$ = $1; $$->ac_next = $3; }
        | /* empty */
                { $$ = 0; }
        ;
  /* JGM:: Actions are like expressions; the most common result is to create a new tree of a certain type */


action                          /* Actions that happen during the run Do not choose more than one  usually*/
        : PRINTALL
                { will_print++;
                  $$ = newaction(ACT_PRINTALL, NIL, (char *) 0, 0, NIL);
                }
        | PRINTEW
                { will_print++;
                  $$ = newaction(ACT_PRINTSIDE, NIL, (char *) 0, 1 , NIL);
                }
        | PRINTNS
                { will_print++;
                  $$ = newaction(ACT_PRINTSIDE, NIL, (char *) 0,  0, NIL);
                }
        | PRINTSIDE '(' side ')'
                { will_print++;
                  $$ = newaction(ACT_PRINTSIDE, NIL, (char *) 0, $3, NIL);
                }
        | PRINTCOMPACT
                { will_print++;
                  $$=newaction(ACT_PRINTCOMPACT,NIL,0,0, NIL);}
        | PRINTCOMPACT '(' expr ')'
                { will_print++;
                  $$=newaction(ACT_PRINTCOMPACT,$3,0,0, NIL);}
        | PRINTONELINE
                { will_print++;
                  $$ = newaction(ACT_PRINTONELINE, NIL, 0, 0, NIL);}
        | PRINTONELINE '(' expr ')'
                { will_print++;
                  $$ = newaction(ACT_PRINTONELINE, $3,  0, 0, NIL);}
        | PRINTPBN
                { will_print++;
                  $$=newaction(ACT_PRINTPBN,NIL,0,0, NIL);}
        | PRINTES '(' exprlist ')'
                { will_print++;
                  $$=newaction(ACT_PRINTES,(struct tree*)$3,0,0, NIL); }

                    /* Actions that happen at End Of Run. Can choose as many as you like, or several of same kind */

        | PRINT '(' printlist ')'
                { will_print++;
                  $$ = newaction(ACT_PRINT, NIL, (char *) 0, $3, NIL);
                }
        | AVERAGE optstring expr
                { $$ = newaction(ACT_AVERAGE, $3, $2, 0, NIL); }

        | FREQUENCY optstring '(' expr ',' number ',' number ')'
                { $$ = newaction(ACT_FREQUENCY, $4, $2, 0, NIL);
                  $$->ac_u.acu_f.acuf_lowbnd = $6;
                  $$->ac_u.acu_f.acuf_highbnd = $8;}
        | FREQUENCY optstring '(' expr ',' number ',' number ',' expr ',' number ',' number ')'
                { $$ = newaction(ACT_FREQUENCY2D, $4, $2, 0, $10);
                  $$->ac_u.acu_f2d.acuf_lowbnd_expr1 = $6;
                  $$->ac_u.acu_f2d.acuf_highbnd_expr1 = $8;
                  $$->ac_u.acu_f2d.acuf_lowbnd_expr2 = $12;
                  $$->ac_u.acu_f2d.acuf_highbnd_expr2 = $14; }

        | EVALCONTRACT '(' side ',' CONTRACT ',' VULN ')'          /* removed will_print++. This action prints only at EOJ. */
                { $$=newaction(ACT_EVALCONTRACT,0,0,$3, NIL);
                  $$->ac_u.acucontract.coded = $5 ;           /* Flex makes coded contract from e.g z3Hxx */
                  $$->ac_u.acucontract.vul = $7 ;             /* Vul not part of flex coded contract */
                  $$->ac_u.acucontract.dbl =  $5 / 40;        /* DBL = 1, RDBL = 2 */
                  $$->ac_u.acucontract.strain = $5 % 5;       /* 0 .. 4 => C D H S N */
                  $$->ac_u.acucontract.level  = ($5 % 40 )/5; /* 5 .. 35  for 1 - 7 */
                }
        | EXPORT '(' side ')'
                { $$=newaction(ACT_EXP_SIDE_HLD, 0, 0, $3, NIL); }
        | EXPORT '(' compass ')'
                { $$=newaction(ACT_EXP_SEAT_HLD, 0, 0, $3, NIL); }
        | CSVRPT '(' csvlist ')'
                { will_print++;
                  $$=newaction(ACT_CSVRPT,(struct tree*)$3,0,0, NIL); }
        | PRINTRPT '(' csvlist ')'
                { will_print++ ;
                  $$=newaction(ACT_PRINTRPT, (struct tree*)$3,0,0, NIL); }
        ;    /* end of action chain */

optstring        /* optstring is an optional string printed with either the average or frequency action. it can be absent */
        : /* empty */
                { $$ = (char *) 0; }
        | STRING
                { $$ = $1; }
        ;
        // an end of run action to print all hands for each compass asked for on a separate pages from each other
printlist
        : COMPASS
                { $$ = (1<<$1); }
        | printlist ',' COMPASS
                { $$ = $1|(1<<$3); }
        ;
%%
  /* End of Bison/Yacc specs.
   */

 /* Blank Code section */
 /* these next ones not referred to anywhere else ; even in dealparse_subs.c */
    /* The @$ refers to the item on the top of the LOCATION stack, which tracks the semantic value stack */
    /* The scanner, aka Flex, will put the value of lineno into the location object yylloc, and bison will use it */
    /* to update the location stack much the same way it uses yylval to update the semantic stack. */

extern FILE *yyin;
extern FILE *yyo ;

/* forward declarations for use by the main code that follows */
extern int yylex(void) ;

    void yyerror( char *msg) {
        /* lets see if this works .. */
        fprintf(stderr, "line %d: %s\n", yylloc.first_line , msg);
        exit(-1);
    } /* end yyerror() */

struct tree *var_lookup(char *s, int mustbethere) {
        struct var *v;
        for(v=vars; v!=0; v = v->v_next)
                if (strcmp(s, v->v_ident)==0) return v->v_tree;
        if (mustbethere) yyerror( "unknown variable");
        return 0;
} /* end var_lookup */

void new_var(char *s, struct tree *t) {
        struct var *v;
        if (var_lookup(s, 0)!=0) yyerror( "redefined variable");
        v = (struct var *) mycalloc(1, sizeof(*v));
        v->v_next = vars;
        v->v_ident = s;
        v->v_tree = t;
        vars = v;
} /* end new_var */

/* this is all the ways you can permute 4 numbers 0,1,2,3; We use this for shape specs?? of the form "any 4432" or "any 5431" etc. */
int perm[24][4] = {
        {       0,      1,      2,      3       },
        {       0,      1,      3,      2       },
        {       0,      2,      1,      3       },
        {       0,      2,      3,      1       },
        {       0,      3,      1,      2       },
        {       0,      3,      2,      1       },
        {       1,      0,      2,      3       },
        {       1,      0,      3,      2       },
        {       1,      2,      0,      3       },
        {       1,      2,      3,      0       },
        {       1,      3,      0,      2       },
        {       1,      3,      2,      0       },
        {       2,      0,      1,      3       },
        {       2,      0,      3,      1       },
        {       2,      1,      0,      3       },
        {       2,      1,      3,      0       },
        {       2,      3,      0,      1       },
        {       2,      3,      1,      0       },
        {       3,      0,      1,      2       },
        {       3,      0,      2,      1       },
        {       3,      1,      0,      2       },
        {       3,      1,      2,      0       },
        {       3,      2,      0,      1       },
        {       3,      2,      1,      0       },
};

void insertshape(char s[4], int any, int neg_shape) {
        int i,j,p;
        int xcount=0, ccount=0;
        char copy_s[4];

        for (i=0;i<4;i++) {
        if (s[i]=='x')
                        xcount++;
                else
                        ccount += s[i]-'0';
        }
      switch(xcount) {
        case 0:
                if (ccount!=13)
                        yyerror( "wrong number of cards in shape");
                for (p=0; p<(any? 24 : 1); p++)
                   setshapebit(s[perm[p][3]]-'0', s[perm[p][2]]-'0', /* we call setshape with C,D,H,S but s is S,H,D,C */
                               s[perm[p][1]]-'0', s[perm[p][0]]-'0',
                               1<<shapeno, neg_shape);
                break;
        default:
                if (ccount>13)
                        yyerror("too many cards in ambiguous shape");
                bcopy(s, copy_s, 4);
                for(i=0; copy_s[i] != 'x'; i++)
                        ;
                if (xcount==1) {
                        copy_s[i] = 13-ccount+'0';      /* could go above '9' to : ; < = for '0' + 10, 11, 12, 13 */
                        insertshape(copy_s, any, neg_shape);
                } else {
                        for (j=0; j<=13-ccount; j++) {
                                copy_s[i] = j+'0';
                                insertshape(copy_s, any, neg_shape);
                        }
                }
                break;
        } /* end switch xcount */
} /* end insertshape() */

int d2n(char s[4]) { /* distribution (e.g. string "4432" to an integer. Not sure how it handles 55xx */
        static char copys[5];

        strncpy(copys, s, 4);
        return atoi(copys);
} /* end d2n() */

struct tree *newtree(int type, struct tree *p1, struct tree *p2, int  i1, int i2)
{
        /* char *mycalloc(); */
        struct tree *p;

        p = (struct tree *) mycalloc(1, sizeof(*p));
        p->tr_type = type;
        p->tr_leaf1 = p1;
        p->tr_leaf2 = p2;
        p->tr_int1 = i1;
        p->tr_int2 = i2;
        return p;
}   /* end new_tree() */

struct action *newaction(int type, struct tree *p1, char *s1, int i1, struct tree *p2)
{
        /* char *mycalloc(); */
        struct action *a;

        a = (struct action *) mycalloc(1, sizeof(*a));
        a->ac_type = type;
        a->ac_expr1 = p1;
        a->ac_str1 = s1;
        a->ac_int1 = i1;
        a->ac_expr2 = p2;
        return a;
} /* end newaction() */

struct expr *newexpr(struct tree* tr1, char* ch1, struct expr* ex1)
{
    struct expr* e;
    e=(struct expr*) mycalloc(1, sizeof(*e));
    e->ex_tr = tr1;
    e->ex_ch = ch1;   /* ptr to string expr for optstring in print statements and the like */
    e->next  = 0;
    if(ex1) {
        struct expr* exau = ex1;
            /* AM990705: the while's body had mysteriously disappeared, reinserted it */
            while(exau->next) exau = exau->next;
            exau->next = e;
            return ex1;  /* JGM?? ex1 not changed from input; we are always returning head of list. */
                         /* JGM?? if we return e (new end of list) then we don't need the while loop.  */
    } else {
        return e;
    }
} /* end newexpr() */

void predeal_holding(int compass, char *holding) {
        char suit;

        suit = *holding++;
        while (*holding) {
                predeal(compass, make_card(*holding, suit));
                holding++;
        }
} /* end predeal_holding() */

  // There does not seem to be any code to actually generate bias deals only to handle the parsing.

extern int biasdeal[4][4];
extern char*player_name[4];
extern char *suit_name[4];


int bias_len(int compass){
  return
    TRUNCZ(biasdeal[compass][0])+
    TRUNCZ(biasdeal[compass][1])+
    TRUNCZ(biasdeal[compass][2])+
    TRUNCZ(biasdeal[compass][3]);
} /* end bias_len() */

int bias_totsuit(int suit){
  return
    TRUNCZ(biasdeal[0][suit])+
    TRUNCZ(biasdeal[1][suit])+
    TRUNCZ(biasdeal[2][suit])+
    TRUNCZ(biasdeal[3][suit]);
} /* end bias_totsuit() */

 /* routine to handle alternate predeal specs  of form spades(north)==4 */
 /* JGM says: What? it checks for errors but there is no code in the main loop that takes this into account */
void bias_deal(int suit, int compass, int length){
  if(biasdeal[compass][suit]!=-1){
    char s[256];
    sprintf(s,"%s's %s suit has length already set to %d",
      player_name[compass],suit_name[suit],
      biasdeal[compass][suit]);
    yyerror( s);
  }
  biasdeal[compass][suit]=length;
  if(bias_len(compass)>13){
      char s[256];
    sprintf(s,"Suit lengths too long for %s",
      player_name[compass]);
    yyerror( s);
  }
  if(bias_totsuit(suit)>13){
    char s[256];
    sprintf(s,"Too many %ss",suit_name[suit]);
    yyerror( s);
  }
}  /* end bias_deal() */


/* JGM added Code for the CSV Report action */
struct csvterm_st *new_csvterm (struct tree *tr1, char *str1, int hand_mask,  int trix_mask, struct csvterm_st *csv1) {
//                                  expr tree        string    hands-to-print trick-set ask  ptr->previous csvlist item

    struct csvterm_st *csv;
    csv=(struct csvterm_st*) mycalloc(1, sizeof(*csv));
    csv->csv_tr = tr1;
    csv->csv_str = str1;
    csv->csv_hands = hand_mask ;
    csv->csv_trix  = trix_mask ;
    csv->next  = 0;   /* new terms go at the end of the list */
  #ifdef JGMDBG
    if (jgmDebug >=3 ) {
       fprintf(stderr, "In New CSVTERM. expr_ptr=[%p], str_ptr=[%p], h_mask=[%d], t_mask=[%d], csvlist_HEAD_ptr=[%p], new_term_ptr=[%p] \n",
           (void *)tr1 , (void *)str1, hand_mask, trix_mask, (void*)csv1, (void*)csv ) ;
    }
  #endif

      if(csv1) { /* if the list exists already */
        struct csvterm_st *csv2 = csv1;
            while(csv2->next) csv2 = csv2->next; /* loop until end of list */
            csv2->next = csv;                    /* update end of list with new csvterm */
            return csv1; /* prob don't need to return this, but we do need the while loop. */
    }
    else   {   return csv;   } /* handles the case of the very first term */

} /* end new_csvterm() */

#define YY_USE_PROTOS

 /* end blank code section */


