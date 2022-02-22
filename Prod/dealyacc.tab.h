/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_DEALYACC_TAB_H_INCLUDED
# define YY_YY_DEALYACC_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 22 "../src/dealyacc.y"

    /* :JGM: Entries here go to both the dealyacc.tab.h and the dealyacc.tab.c files */
    /* :JGM: these definitions are required by the %union declaration */
  extern int yylex( void );             /* yylex comes from the flex object file  at link time */
  void yyerror( char *s ) ;             /* yyerror defined later in this file in code provides section */
#include "../include/dealdefs.h"       /* symbolic constants for everything */
#include "../include/dealtypes.h"      /* types; some needed by union aka YYSTYPE so put them here */
#include "../include/dealexterns.h"    /* extern statements for the various global vars. */


#line 59 "dealyacc.tab.h"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    QUERY = 258,
    COLON = 259,
    OR2 = 260,
    AND2 = 261,
    CMPEQ = 262,
    CMPNE = 263,
    CMPLT = 264,
    CMPLE = 265,
    CMPGT = 266,
    CMPGE = 267,
    ARPLUS = 268,
    ARMINUS = 269,
    ARTIMES = 270,
    ARDIVIDE = 271,
    ARMOD = 272,
    NOT = 273,
    HCP = 274,
    SHAPE = 275,
    ANY = 276,
    EXCEPT = 277,
    CONDITION = 278,
    ACTION = 279,
    PRINT = 280,
    PRINTALL = 281,
    PRINTEW = 282,
    PRINTNS = 283,
    PRINTSIDE = 284,
    PRINTPBN = 285,
    PRINTCOMPACT = 286,
    PRINTONELINE = 287,
    AVERAGE = 288,
    HASCARD = 289,
    FREQUENCY = 290,
    PREDEAL = 291,
    POINTCOUNT = 292,
    ALTCOUNT = 293,
    CONTROL = 294,
    LOSER = 295,
    QUALITY = 296,
    CCCC = 297,
    TRICKS = 298,
    NOTRUMPS = 299,
    EVALCONTRACT = 300,
    ALL = 301,
    NONE = 302,
    SCORE = 303,
    IMPS = 304,
    RND = 305,
    PT0 = 306,
    PT1 = 307,
    PT2 = 308,
    PT3 = 309,
    PT4 = 310,
    PT5 = 311,
    PT6 = 312,
    PT7 = 313,
    PT8 = 314,
    PT9 = 315,
    PRINTES = 316,
    OPC = 317,
    LTC = 318,
    DDS = 319,
    PAR = 320,
    EXPORT = 321,
    DEAL = 322,
    CSVRPT = 323,
    TRIX = 324,
    NUMBER = 325,
    HOLDING = 326,
    STRING = 327,
    IDENT = 328,
    COMPASS = 329,
    VULN = 330,
    SUIT = 331,
    CARD = 332,
    CONTRACT = 333,
    DISTR = 334,
    DISTR_OR_NUMBER = 335,
    DECNUM = 336,
    SIDE = 337
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 42 "../src/dealyacc.y"

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

#line 169 "dealyacc.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);
/* "%code provides" blocks.  */
#line 168 "../src/dealyacc.y"
   /* code that is needed by the parser action clauses It also goes to tab.h !! */

 #include "../include/dealprotos.h"  /* prototypes for procedures incl some called in parse action clauses */
  extern int yylex() ;  /* yylex will be provided by the scanner produced file */


#line 203 "dealyacc.tab.h"

#endif /* !YY_YY_DEALYACC_TAB_H_INCLUDED  */
