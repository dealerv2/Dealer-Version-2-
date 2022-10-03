  /* Dummy Grammar file for testing what Dealer does ... */
 
%code top {
    /* Code top section. Is NOT copied to  .tab.h file */
   #ifndef _GNU_SOURCE
       #define _GNU_SOURCE
   #endif
   #define FILEVERS "YaccTest 1.0"
   #include "../include/std_hdrs.h"
   int yylex (void);
 } /* end code top */
 
%code requires {
    /* :JGM: Entries here go to both the dealyacc.tab.h and the dealyacc.tab.c files */
    /* :JGM: these definitions are required by the %union declaration */
  extern int yylex( void );             /* yylex comes from the flex object file  at link time */
  void yyerror( char *s ) ;             /* yyerror defined later in this file in code provides section */
#include "../include/dealdefs.h"       /* symbolic constants for everything */
#include "../include/dealtypes.h"      /* types; some needed by union aka YYSTYPE so put them here */
#include "../include/dealexterns.h"    /* extern statements for the various global vars. */

} /* end code requires */ 

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

  /* Entering code provides section */
%code provides {   /* Just before rules section. Code that is needed by the parser action clauses Copied to tab.h !!  */

  /* Code Provides: prototypes for procedures incl some called in parse action clauses */
  /* For example we DO need math.h to be included at this point or it will not compile */
  /* But we do not need stdlib.h (for exit) or ctype.h (for isdigit) since these are only called by routines
   * later in the main procedure
   */
  #include <stdio.h>
  #include <math.h>     /* for double arithmetic pow() etc. */
  #include <string.h>   /* for strdup etc. needed by lexer may not be needed here */
  extern int yylex() ;  /* yylex will be provided by the scanner produced file */

}  /* end code provides */
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

%printer { fprintf(yyo, "[%d]", $$); } <y_int> <y_numx100> <y_decnum>
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

def     : CONDITION expr
                {  decisiontree = $2; }
        | expr
                {  decisiontree = $1; }
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
        | SHAPE '(' compass ',' shapelist ')'
                {
          $$ = newtree(TRT_SHAPE, NIL, NIL, $3, 1<<(shapeno++));
          if (shapeno >= 32) {
            yylloc.first_line = @3.first_line ; /* do this so bison activates yylloc et. al. */
            yyerror("Too many shapes -- only 32 allowed!\n");
            YYERROR;
          }
        }
        

%%  /* End of Bison/Yacc Rules Section */


 /* end blank code section  and end of .y file */
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
                        setshapebit(s[perm[p][3]]-'0', s[perm[p][2]]-'0',
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
int ***distrbitmaps[14];

void setshapebit (int cl, int di, int ht, int sp, int msk, int excepted) {
  if (excepted)
    distrbitmaps[cl][di][ht][sp] &= ~msk;
  else
    distrbitmaps[cl][di][ht][sp] |= msk;
}
/* Various handshapes can be asked for. For every shape the user is
   interested in a number (shapeno) is generated. In every distribution that fits that
   shape the corresponding bit is set in the distrbitmaps 4-dimensional array.
   This makes looking up a shape a small constant cost.
*/

void initdistr () {
  int ***p4, **p3, *p2;
  int clubs, diamonds, hearts;

  /* Allocate the four dimensional pointer array */

  for (clubs = 0; clubs <= 13; clubs++) {
    p4 = (int ***) mycalloc ((unsigned) 14 - clubs, sizeof (*p4));
    distrbitmaps[clubs] = p4;
    for (diamonds = 0; diamonds <= 13 - clubs; diamonds++) {
      p3 = (int **) mycalloc ((unsigned) 14 - clubs - diamonds, sizeof (*p3));
      p4[diamonds] = p3;
      for (hearts = 0; hearts <= 13 - clubs - diamonds; hearts++) {
        p2 = (int *) mycalloc ((unsigned) 14 - clubs - diamonds - hearts, sizeof (*p2));
        p3[hearts] = p2;
      }
    }
  }
} /* end initdistr */

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


/* this next bit just so it compiles */
  int main (int argc, char *argv[] ) {
  
     printf("Starting Stub version:[%s]; UnionSize=[%ld] \n", FILEVERS, sizeof(yylval) );
     /* yyerror("Early Exit"); // this just tests that it's our yyerror that is active here.. */
     yyparse() ;
     return 0;
  }
