/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
#line 11 "../src/dealyacc.y"

    /* Entries here will not be put to the dealyacc.tab.h file, but will go to the dealyacc.tab.c file */
    /* we don't want the definition of yyerror to go to flex, just  the prototype */
    // The _GNU_SOURCE def will affect somewhat what bison generates from macros etc. So put it very first
    #ifndef _GNU_SOURCE
       #define _GNU_SOURCE
    #endif
    float bisonvers = 3.0 ;
  #include "../include/std_hdrs.h"

#line 78 "dealyacc.tab.c"




# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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


#line 135 "dealyacc.tab.c"

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

#line 245 "dealyacc.tab.c"

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


#line 279 "dealyacc.tab.c"

#endif /* !YY_YY_DEALYACC_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   766

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  87
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  143
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  354

#define YYUNDEFTOK  2
#define YYMAXUTOK   337


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      84,    85,     2,     2,    86,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    83,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   178,   178,   179,   182,   183,   183,   185,   184,   186,
     188,   190,   192,   197,   198,   202,   202,   203,   207,   209,
     214,   216,   215,   223,   228,   233,   235,   238,   239,   245,
     246,   252,   253,   255,   259,   263,   264,   268,   273,   274,
     278,   280,   282,   284,   286,   288,   290,   292,   294,   296,
     298,   300,   302,   304,   306,   308,   310,   312,   314,   316,
     318,   320,   322,   324,   326,   328,   330,   332,   334,   336,
     338,   340,   342,   351,   353,   355,   357,   359,   361,   363,
     365,   367,   369,   371,   373,   375,   377,   379,   381,   383,
     385,   387,   389,   391,   393,   396,   398,   400,   402,   404,
     406,   412,   414,   416,   418,   423,   425,   427,   429,   431,
     433,   435,   437,   439,   441,   443,   445,   447,   449,   455,
     457,   460,   466,   470,   474,   478,   482,   485,   488,   491,
     494,   497,   503,   507,   510,   514,   521,   529,   531,   533,
     539,   540,   545,   547
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "QUERY", "COLON", "OR2", "AND2", "CMPEQ",
  "CMPNE", "CMPLT", "CMPLE", "CMPGT", "CMPGE", "ARPLUS", "ARMINUS",
  "ARTIMES", "ARDIVIDE", "ARMOD", "NOT", "HCP", "SHAPE", "ANY", "EXCEPT",
  "CONDITION", "ACTION", "PRINT", "PRINTALL", "PRINTEW", "PRINTNS",
  "PRINTSIDE", "PRINTPBN", "PRINTCOMPACT", "PRINTONELINE", "AVERAGE",
  "HASCARD", "FREQUENCY", "PREDEAL", "POINTCOUNT", "ALTCOUNT", "CONTROL",
  "LOSER", "QUALITY", "CCCC", "TRICKS", "NOTRUMPS", "EVALCONTRACT", "ALL",
  "NONE", "SCORE", "IMPS", "RND", "PT0", "PT1", "PT2", "PT3", "PT4", "PT5",
  "PT6", "PT7", "PT8", "PT9", "PRINTES", "OPC", "LTC", "DDS", "PAR",
  "EXPORT", "DEAL", "CSVRPT", "TRIX", "NUMBER", "HOLDING", "STRING",
  "IDENT", "COMPASS", "VULN", "SUIT", "CARD", "CONTRACT", "DISTR",
  "DISTR_OR_NUMBER", "DECNUM", "SIDE", "'='", "'('", "')'", "','",
  "$accept", "defs", "def", "$@1", "$@2", "predealargs", "predealarg",
  "$@3", "holdings", "pointcountargs", "$@4", "compass", "side",
  "shlprefix", "any", "number", "decnum", "shape", "shapelistel",
  "shapelist", "expr", "exprlist", "csvlist", "actionlist", "action",
  "optstring", "printlist", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,    61,    40,    41,    44
};
# endif

#define YYPACT_NINF (-116)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -116,   113,  -116,   -18,   625,   -27,   -25,   625,   685,    -9,
     -23,  -116,    11,     0,    10,    35,    37,    42,    46,    64,
      90,   111,   128,   172,   197,   241,   257,   258,   265,   266,
     267,   278,   282,   284,   288,  -116,    33,   298,  -116,  -116,
     625,  -116,  -116,  -116,   749,  -116,  -116,  -116,     6,     6,
     749,   299,  -116,  -116,  -116,   302,  -116,   303,   304,   107,
     107,   305,   312,   313,   314,  -116,   287,   310,  -116,   328,
     -23,  -116,    11,  -116,     6,     6,     6,     6,     6,   342,
     625,   625,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   337,     6,     6,   337,   625,     6,    57,   625,
     625,   625,   625,   625,   625,   625,   625,   625,   625,   625,
     625,   625,   625,  -116,    -7,   336,   352,   337,   625,   625,
    -116,   625,   344,   337,   489,   -50,   351,   685,   343,   359,
     362,  -116,  -116,  -116,    11,    -3,     1,   369,   371,   372,
     378,   193,   208,    39,    43,    49,    53,    55,    58,    60,
      72,    74,    96,  -116,   380,    99,   381,   396,   749,   401,
    -116,   728,   504,   320,   218,   274,   300,   364,   260,   307,
      95,    14,   171,   440,  -116,  -116,   361,     7,  -116,   105,
     403,   233,   254,   749,   625,   405,  -116,   749,   166,   410,
     412,  -116,   382,  -116,  -116,  -116,   749,   169,  -116,   421,
    -116,   413,   437,    11,  -116,  -116,   429,  -116,   430,   448,
    -116,   -22,   447,  -116,  -116,  -116,   450,  -116,   451,  -116,
     457,  -116,   458,  -116,   459,  -116,   460,  -116,   474,  -116,
     479,  -116,   480,  -116,   481,  -116,  -116,   482,   -21,  -116,
    -116,   625,   475,  -116,  -116,  -116,   542,  -116,     4,  -116,
     490,  -116,  -116,  -116,    -1,   488,  -116,   557,  -116,  -116,
     -41,  -116,   420,   483,   496,   565,  -116,   493,   494,   495,
     497,   498,   499,   501,   502,   503,   505,   507,   508,   509,
     510,   516,   517,   518,   519,   533,   436,  -116,  -116,   200,
    -116,  -116,  -116,    11,   537,  -116,   749,   539,   540,  -116,
     500,  -116,  -116,  -116,   749,  -116,  -116,   511,  -116,  -116,
    -116,  -116,  -116,   625,  -116,  -116,  -116,  -116,  -116,  -116,
    -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,
     545,   514,  -116,  -116,   -40,  -116,   289,    11,   541,   543,
     547,  -116,   222,  -116,  -116,  -116,  -116,   625,    32,    11,
     548,    11,   550,  -116
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,   121,     0,
       0,     5,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    42,     0,    33,    34,
       0,     3,    40,    41,    10,    32,    42,    93,     0,     0,
       9,     0,   122,   123,   124,     0,   130,   126,   128,   140,
     140,     0,     0,     0,     0,    12,   119,     0,    15,     0,
       4,    13,    20,     7,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,     0,     0,     0,     0,
     141,     0,     0,     0,     0,     0,     0,   121,     0,     0,
       0,    14,     6,    21,    20,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    24,     0,     0,     0,     0,    11,     0,
      78,     0,    86,    85,    79,    80,    81,    82,    83,    84,
      87,    88,    89,    90,    91,    44,     0,    27,   142,     0,
       0,     0,     0,   133,     0,     0,   102,   101,     0,     0,
       0,   107,     0,   109,   105,   106,   108,     0,   120,     0,
      18,    16,     0,    20,     8,    68,     0,    66,     0,     0,
      70,     0,     0,    77,    94,    46,     0,    48,     0,    50,
       0,    52,     0,    54,     0,    56,     0,    58,     0,    60,
       0,    62,     0,    64,     0,    97,    95,     0,     0,   100,
      43,     0,     0,    26,    28,    25,    29,    38,    27,   132,
       0,   125,   127,   129,     0,     0,   131,     0,   138,   137,
       0,   139,     0,     0,     0,     0,    22,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    92,    45,    30,     0,
      72,    39,   143,     0,     0,   104,   103,     0,     0,   114,
       0,   116,   112,   113,   115,    73,    19,     0,    69,    67,
      71,    75,    74,     0,    47,    49,    51,    53,    55,    57,
      59,    61,    63,    65,    96,    99,    98,    35,    36,    37,
       0,     0,   111,   110,     0,    17,     0,     0,     0,     0,
       0,    76,     0,   136,   118,   117,   134,     0,     0,     0,
       0,     0,     0,   135
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -116,  -116,  -116,  -116,  -116,  -116,   566,  -116,  -116,  -115,
    -116,   270,   -67,  -116,  -116,   -11,  -116,  -116,   326,  -116,
      -4,  -116,  -116,   513,  -116,   582,  -116
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    41,    72,   134,    70,    71,   129,   201,   132,
     203,   114,   154,   246,   289,    42,    43,   329,   247,   248,
      44,   188,   197,    65,    66,   121,   179
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      47,    73,    99,    50,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   243,   244,   204,
     243,   244,   270,   284,   113,     3,   297,   339,   157,   110,
     111,   112,   153,   298,   340,    99,    98,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     180,    68,    45,    69,   271,   285,   185,    48,   190,    49,
      99,   133,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,    67,   141,   142,   175,   176,
     113,    35,   205,   206,    74,   293,   207,   208,   266,   290,
     245,    38,   158,   245,    75,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   109,
     110,   111,   112,     2,   181,   182,    96,   183,   349,    76,
     187,    77,   196,   133,   215,   216,    78,     3,   217,   218,
      79,     4,     5,     6,   219,   220,     7,     8,   221,   222,
     223,   224,   160,   225,   226,   227,   228,     9,    80,    10,
      11,    12,    13,    14,    15,    16,    17,   229,   230,   231,
     232,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    81,    31,    32,    33,    34,   120,
     254,   233,   234,    35,   236,   237,    36,   111,   112,    37,
     249,   250,   133,    38,    39,    82,    99,    40,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,    99,    83,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,    99,   286,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   256,   257,   296,   261,   262,    84,    99,   304,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   107,   108,   109,   110,   111,   112,   213,   327,
     328,    85,   330,   104,   105,   106,   107,   108,   109,   110,
     111,   112,    99,   214,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   346,   347,   336,
     105,   106,   107,   108,   109,   110,   111,   112,   252,   115,
     108,   109,   110,   111,   112,    86,   342,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   350,   253,
     352,    87,    88,   348,   135,   136,   137,   138,   139,    89,
      90,    91,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,    92,   155,   156,     3,    93,   159,    94,     4,
       5,     6,    95,   127,   341,   106,   107,   108,   109,   110,
     111,   112,    97,   116,   128,     9,   117,   118,   119,   123,
      13,    14,    15,    16,    17,   189,   124,   125,   126,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,   130,    31,    32,    33,    34,   140,   191,   153,
     192,    35,   177,   193,    46,   194,   178,    37,   184,   199,
     200,    38,    39,   195,     3,    40,   202,   242,     4,     5,
       6,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,     9,   209,   210,   112,   211,    13,
      14,    15,    16,    17,   212,   235,   260,   238,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,   239,    31,    32,    33,    34,   240,   299,   251,   300,
      35,   255,   301,    46,   302,   258,    37,   259,   263,   264,
      38,    39,   303,     3,    40,   267,   268,     4,     5,     6,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   265,     9,   269,   272,   273,   274,    13,    14,
      15,    16,    17,   275,   276,   277,   278,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
     279,    31,    32,    33,    34,   280,   281,   282,   283,    35,
     287,   186,    46,   288,   292,    37,   294,   306,   305,    38,
      39,     3,   307,    40,   291,     4,     5,     6,   308,   309,
     310,   335,   311,   312,   334,   313,   314,   315,   316,   338,
     317,     9,   318,   319,   320,   321,    13,    14,    15,    16,
      17,   322,   323,   324,   325,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,   326,    31,
      32,    33,    34,   331,   332,   333,   343,    35,   344,   295,
      46,   337,   345,    37,   351,   353,   131,    38,    39,     3,
     198,    40,   122,     4,     5,     6,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     9,
       0,     0,     0,     0,    13,    14,    15,    16,    17,     0,
       0,     0,     0,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,     0,    31,    32,    33,
      34,     0,     0,     0,     0,    35,     0,     0,    46,     0,
       0,    37,     0,     0,     0,    38,    39,     0,     0,    40,
      51,    52,    53,    54,    55,    56,    57,    58,    59,     0,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      61,    99,   241,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,    62,     0,     0,     0,
       0,    63,    99,    64,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112
};

static const yytype_int16 yycheck[] =
{
       4,    12,     3,     7,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    13,    14,   134,
      13,    14,    44,    44,    74,    14,    67,    67,    95,    15,
      16,    17,    82,    74,    74,     3,    40,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
     117,    74,    70,    76,    76,    76,   123,    84,   125,    84,
       3,    72,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    84,    80,    81,    85,    86,
      74,    70,    85,    86,    84,    86,    85,    86,   203,    85,
      86,    80,    96,    86,    84,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,    14,
      15,    16,    17,     0,   118,   119,    83,   121,    86,    84,
     124,    84,   126,   134,    85,    86,    84,    14,    85,    86,
      84,    18,    19,    20,    85,    86,    23,    24,    85,    86,
      85,    86,    85,    85,    86,    85,    86,    34,    84,    36,
      37,    38,    39,    40,    41,    42,    43,    85,    86,    85,
      86,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    84,    62,    63,    64,    65,    72,
     184,    85,    86,    70,    85,    86,    73,    16,    17,    76,
      85,    86,   203,    80,    81,    84,     3,    84,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,     3,    84,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,     3,   241,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    85,    86,   257,    85,    86,    84,     3,   262,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    12,    13,    14,    15,    16,    17,    85,    79,
      80,    84,   293,     9,    10,    11,    12,    13,    14,    15,
      16,    17,     3,    85,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    85,    86,   313,
      10,    11,    12,    13,    14,    15,    16,    17,    85,    49,
      13,    14,    15,    16,    17,    84,   337,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,   349,    85,
     351,    84,    84,   347,    74,    75,    76,    77,    78,    84,
      84,    84,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    84,    93,    94,    14,    84,    97,    84,    18,
      19,    20,    84,    86,    85,    11,    12,    13,    14,    15,
      16,    17,    84,    84,    74,    34,    84,    84,    84,    84,
      39,    40,    41,    42,    43,   125,    84,    84,    84,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    84,    62,    63,    64,    65,    75,    67,    82,
      69,    70,    86,    72,    73,    74,    74,    76,    84,    86,
      71,    80,    81,    82,    14,    84,    74,    76,    18,    19,
      20,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    34,    86,    85,    17,    86,    39,
      40,    41,    42,    43,    86,    85,    84,    86,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    85,    62,    63,    64,    65,    85,    67,    85,    69,
      70,    86,    72,    73,    74,    85,    76,    85,    77,    86,
      80,    81,    82,    14,    84,    76,    76,    18,    19,    20,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    85,    34,    76,    78,    76,    76,    39,    40,
      41,    42,    43,    76,    76,    76,    76,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      76,    62,    63,    64,    65,    76,    76,    76,    76,    70,
      85,    72,    73,    21,    74,    76,    78,    71,    85,    80,
      81,    14,     7,    84,   248,    18,    19,    20,    85,    85,
      85,    70,    85,    85,    84,    86,    85,    85,    85,    75,
      85,    34,    85,    85,    85,    85,    39,    40,    41,    42,
      43,    85,    85,    85,    85,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    85,    62,
      63,    64,    65,    86,    85,    85,    85,    70,    85,    72,
      73,    86,    85,    76,    86,    85,    70,    80,    81,    14,
     127,    84,    60,    18,    19,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    -1,    -1,    -1,    39,    40,    41,    42,    43,    -1,
      -1,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    62,    63,    64,
      65,    -1,    -1,    -1,    -1,    70,    -1,    -1,    73,    -1,
      -1,    76,    -1,    -1,    -1,    80,    81,    -1,    -1,    84,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    -1,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    61,    -1,    -1,    -1,
      -1,    66,     3,    68,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    88,     0,    14,    18,    19,    20,    23,    24,    34,
      36,    37,    38,    39,    40,    41,    42,    43,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    62,    63,    64,    65,    70,    73,    76,    80,    81,
      84,    89,   102,   103,   107,    70,    73,   107,    84,    84,
     107,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      35,    45,    61,    66,    68,   110,   111,    84,    74,    76,
      92,    93,    90,   102,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    83,    84,   107,     3,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    74,    98,    98,    84,    84,    84,    84,
      72,   112,   112,    84,    84,    84,    84,    86,    74,    94,
      84,    93,    96,   102,    91,    98,    98,    98,    98,    98,
      75,   107,   107,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    82,    99,    98,    98,    99,   107,    98,
      85,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,    85,    86,    86,    74,   113,
      99,   107,   107,   107,    84,    99,    72,   107,   108,    98,
      99,    67,    69,    72,    74,    82,   107,   109,   110,    86,
      71,    95,    74,    97,    96,    85,    86,    85,    86,    86,
      85,    86,    86,    85,    85,    85,    86,    85,    86,    85,
      86,    85,    86,    85,    86,    85,    86,    85,    86,    85,
      86,    85,    86,    85,    86,    85,    85,    86,    86,    85,
      85,     4,    76,    13,    14,    86,   100,   105,   106,    85,
      86,    85,    85,    85,   107,    86,    85,    86,    85,    85,
      84,    85,    86,    77,    86,    85,    96,    76,    76,    76,
      44,    76,    78,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    44,    76,   107,    85,    21,   101,
      85,   105,    74,    86,    78,    72,   107,    67,    74,    67,
      69,    72,    74,    82,   107,    85,    71,     7,    85,    85,
      85,    85,    85,    86,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    79,    80,   104,
     102,    86,    85,    85,    84,    70,   107,    86,    75,    67,
      74,    85,   102,    85,    85,    85,    85,    86,   107,    86,
     102,    86,   102,    85
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    87,    88,    88,    89,    90,    89,    91,    89,    89,
      89,    89,    89,    92,    92,    94,    93,    93,    95,    95,
      96,    97,    96,    98,    99,   100,   100,   100,   100,   101,
     101,   102,   102,   102,   103,   104,   104,   105,   106,   106,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   108,   108,   108,   108,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   110,
     110,   110,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     112,   112,   113,   113
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     0,     3,     0,     4,     2,
       1,     3,     2,     1,     2,     0,     3,     6,     1,     3,
       0,     0,     3,     1,     1,     1,     1,     0,     1,     0,
       1,     1,     2,     1,     1,     1,     1,     3,     1,     2,
       1,     1,     1,     4,     4,     6,     4,     6,     4,     6,
       4,     6,     4,     6,     4,     6,     4,     6,     4,     6,
       4,     6,     4,     6,     4,     6,     4,     6,     4,     6,
       4,     6,     6,     6,     6,     6,     8,     4,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     5,     2,     4,     4,     6,     4,     6,     6,
       4,     1,     1,     3,     3,     1,     1,     1,     1,     1,
       4,     4,     3,     3,     3,     3,     3,     6,     6,     1,
       3,     0,     1,     1,     1,     4,     1,     4,     1,     4,
       1,     4,     4,     3,     9,    15,     8,     4,     4,     4,
       0,     1,     1,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
    case 70: /* NUMBER  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1227 "dealyacc.tab.c"
        break;

    case 71: /* HOLDING  */
#line 163 "../src/dealyacc.y"
         { fprintf(yyo, "[%s]", ((*yyvaluep).y_str)); }
#line 1233 "dealyacc.tab.c"
        break;

    case 72: /* STRING  */
#line 163 "../src/dealyacc.y"
         { fprintf(yyo, "[%s]", ((*yyvaluep).y_str)); }
#line 1239 "dealyacc.tab.c"
        break;

    case 73: /* IDENT  */
#line 163 "../src/dealyacc.y"
         { fprintf(yyo, "[%s]", ((*yyvaluep).y_str)); }
#line 1245 "dealyacc.tab.c"
        break;

    case 74: /* COMPASS  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1251 "dealyacc.tab.c"
        break;

    case 75: /* VULN  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1257 "dealyacc.tab.c"
        break;

    case 76: /* SUIT  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1263 "dealyacc.tab.c"
        break;

    case 77: /* CARD  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1269 "dealyacc.tab.c"
        break;

    case 78: /* CONTRACT  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1275 "dealyacc.tab.c"
        break;

    case 79: /* DISTR  */
#line 163 "../src/dealyacc.y"
         { fprintf(yyo, "[%s]", ((*yyvaluep).y_distr)); }
#line 1281 "dealyacc.tab.c"
        break;

    case 80: /* DISTR_OR_NUMBER  */
#line 163 "../src/dealyacc.y"
         { fprintf(yyo, "[%s]", ((*yyvaluep).y_distr)); }
#line 1287 "dealyacc.tab.c"
        break;

    case 81: /* DECNUM  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_numx100)); }
#line 1293 "dealyacc.tab.c"
        break;

    case 82: /* SIDE  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1299 "dealyacc.tab.c"
        break;

    case 98: /* compass  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1305 "dealyacc.tab.c"
        break;

    case 99: /* side  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1311 "dealyacc.tab.c"
        break;

    case 100: /* shlprefix  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1317 "dealyacc.tab.c"
        break;

    case 101: /* any  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1323 "dealyacc.tab.c"
        break;

    case 102: /* number  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1329 "dealyacc.tab.c"
        break;

    case 103: /* decnum  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_numx100)); }
#line 1335 "dealyacc.tab.c"
        break;

    case 104: /* shape  */
#line 163 "../src/dealyacc.y"
         { fprintf(yyo, "[%s]", ((*yyvaluep).y_distr)); }
#line 1341 "dealyacc.tab.c"
        break;

    case 112: /* optstring  */
#line 163 "../src/dealyacc.y"
         { fprintf(yyo, "[%s]", ((*yyvaluep).y_str)); }
#line 1347 "dealyacc.tab.c"
        break;

    case 113: /* printlist  */
#line 162 "../src/dealyacc.y"
         { fprintf(yyo, "[%d]", ((*yyvaluep).y_int)); }
#line 1353 "dealyacc.tab.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 5:
#line 183 "../src/dealyacc.y"
                     { clearpointcount(); pointcount_index=12;}
#line 1998 "dealyacc.tab.c"
    break;

  case 7:
#line 185 "../src/dealyacc.y"
                { clearpointcount_alt((yyvsp[0].y_int)); pointcount_index=12;}
#line 2004 "dealyacc.tab.c"
    break;

  case 9:
#line 187 "../src/dealyacc.y"
                {  decisiontree = (yyvsp[0].y_tree); }
#line 2010 "dealyacc.tab.c"
    break;

  case 10:
#line 189 "../src/dealyacc.y"
                {  decisiontree = (yyvsp[0].y_tree); }
#line 2016 "dealyacc.tab.c"
    break;

  case 11:
#line 191 "../src/dealyacc.y"
                { new_var((yyvsp[-2].y_str), (yyvsp[0].y_tree)); }
#line 2022 "dealyacc.tab.c"
    break;

  case 12:
#line 193 "../src/dealyacc.y"
                { actionlist = (yyvsp[0].y_action); }
#line 2028 "dealyacc.tab.c"
    break;

  case 15:
#line 202 "../src/dealyacc.y"
                   { predeal_compass = (yyvsp[0].y_int);}
#line 2034 "dealyacc.tab.c"
    break;

  case 17:
#line 203 "../src/dealyacc.y"
                                             {bias_deal((yyvsp[-5].y_int),(yyvsp[-3].y_int),(yyvsp[0].y_int));}
#line 2040 "dealyacc.tab.c"
    break;

  case 18:
#line 208 "../src/dealyacc.y"
                { predeal_holding(predeal_compass, (yyvsp[0].y_str)); }
#line 2046 "dealyacc.tab.c"
    break;

  case 19:
#line 210 "../src/dealyacc.y"
                { predeal_holding(predeal_compass, (yyvsp[0].y_str)); }
#line 2052 "dealyacc.tab.c"
    break;

  case 21:
#line 216 "../src/dealyacc.y"
                { pointcount(pointcount_index, (yyvsp[0].y_int));
                  pointcount_index--;
                }
#line 2060 "dealyacc.tab.c"
    break;

  case 23:
#line 224 "../src/dealyacc.y"
                { use_compass[(yyvsp[0].y_int)] = 1; (yyval.y_int)= (yyvsp[0].y_int);  }
#line 2066 "dealyacc.tab.c"
    break;

  case 24:
#line 229 "../src/dealyacc.y"
                { use_side[(yyvsp[0].y_int)] = 1 ; (yyval.y_int) = (yyvsp[0].y_int); }
#line 2072 "dealyacc.tab.c"
    break;

  case 25:
#line 234 "../src/dealyacc.y"
                { (yyval.y_int) = 0; }
#line 2078 "dealyacc.tab.c"
    break;

  case 26:
#line 236 "../src/dealyacc.y"
                { (yyval.y_int) = 0; }
#line 2084 "dealyacc.tab.c"
    break;

  case 27:
#line 238 "../src/dealyacc.y"
                { (yyval.y_int) = 0; }
#line 2090 "dealyacc.tab.c"
    break;

  case 28:
#line 240 "../src/dealyacc.y"
                { (yyval.y_int) = 1; }
#line 2096 "dealyacc.tab.c"
    break;

  case 29:
#line 245 "../src/dealyacc.y"
                { (yyval.y_int) = 0; }
#line 2102 "dealyacc.tab.c"
    break;

  case 30:
#line 247 "../src/dealyacc.y"
                { (yyval.y_int) = 1; }
#line 2108 "dealyacc.tab.c"
    break;

  case 32:
#line 254 "../src/dealyacc.y"
                { (yyval.y_int) = - (yyvsp[0].y_int); }
#line 2114 "dealyacc.tab.c"
    break;

  case 33:
#line 256 "../src/dealyacc.y"
                { (yyval.y_int) = d2n((yyvsp[0].y_distr)); }
#line 2120 "dealyacc.tab.c"
    break;

  case 34:
#line 259 "../src/dealyacc.y"
                 { (yyval.y_numx100) = (yyvsp[0].y_numx100) ; }
#line 2126 "dealyacc.tab.c"
    break;

  case 37:
#line 269 "../src/dealyacc.y"
                { insertshape((yyvsp[0].y_distr), (yyvsp[-1].y_int), (yyvsp[-2].y_int)); }
#line 2132 "dealyacc.tab.c"
    break;

  case 40:
#line 279 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_NUMBER, NIL, NIL, (yyvsp[0].y_int), 0); }
#line 2138 "dealyacc.tab.c"
    break;

  case 41:
#line 281 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_DECNUM, NIL, NIL, (yyvsp[0].y_numx100), 100); }
#line 2144 "dealyacc.tab.c"
    break;

  case 42:
#line 283 "../src/dealyacc.y"
                { (yyval.y_tree) = var_lookup((yyvsp[0].y_str), 1); }
#line 2150 "dealyacc.tab.c"
    break;

  case 43:
#line 285 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_LENGTH, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2156 "dealyacc.tab.c"
    break;

  case 44:
#line 287 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_HCPTOTAL, NIL, NIL, (yyvsp[-1].y_int), 0);  }
#line 2162 "dealyacc.tab.c"
    break;

  case 45:
#line 289 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_HCP, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2168 "dealyacc.tab.c"
    break;

  case 46:
#line 291 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT0TOTAL, NIL, NIL, (yyvsp[-1].y_int), 0);  }
#line 2174 "dealyacc.tab.c"
    break;

  case 47:
#line 293 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT0, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2180 "dealyacc.tab.c"
    break;

  case 48:
#line 295 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT1TOTAL, NIL, NIL, (yyvsp[-1].y_int), 0);  }
#line 2186 "dealyacc.tab.c"
    break;

  case 49:
#line 297 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT1, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2192 "dealyacc.tab.c"
    break;

  case 50:
#line 299 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT2TOTAL, NIL, NIL, (yyvsp[-1].y_int), 0); }
#line 2198 "dealyacc.tab.c"
    break;

  case 51:
#line 301 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT2, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int)); }
#line 2204 "dealyacc.tab.c"
    break;

  case 52:
#line 303 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT3TOTAL, NIL, NIL, (yyvsp[-1].y_int), 0); }
#line 2210 "dealyacc.tab.c"
    break;

  case 53:
#line 305 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT3, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2216 "dealyacc.tab.c"
    break;

  case 54:
#line 307 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT4TOTAL, NIL, NIL, (yyvsp[-1].y_int), 0); }
#line 2222 "dealyacc.tab.c"
    break;

  case 55:
#line 309 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT4, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2228 "dealyacc.tab.c"
    break;

  case 56:
#line 311 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT5TOTAL, NIL, NIL, (yyvsp[-1].y_int), 0);  }
#line 2234 "dealyacc.tab.c"
    break;

  case 57:
#line 313 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT5, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2240 "dealyacc.tab.c"
    break;

  case 58:
#line 315 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT6TOTAL, NIL, NIL, (yyvsp[-1].y_int), 0);  }
#line 2246 "dealyacc.tab.c"
    break;

  case 59:
#line 317 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT6, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2252 "dealyacc.tab.c"
    break;

  case 60:
#line 319 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT7TOTAL, NIL, NIL, (yyvsp[-1].y_int), 0);  }
#line 2258 "dealyacc.tab.c"
    break;

  case 61:
#line 321 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT7, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2264 "dealyacc.tab.c"
    break;

  case 62:
#line 323 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT8TOTAL, NIL, NIL, (yyvsp[-1].y_int), 0);  }
#line 2270 "dealyacc.tab.c"
    break;

  case 63:
#line 325 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT8, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2276 "dealyacc.tab.c"
    break;

  case 64:
#line 327 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT9TOTAL, NIL, NIL, (yyvsp[-1].y_int), 0);  }
#line 2282 "dealyacc.tab.c"
    break;

  case 65:
#line 329 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PT9, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2288 "dealyacc.tab.c"
    break;

  case 66:
#line 331 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_LOSERTOTAL, NIL, NIL, (yyvsp[-1].y_int), 0);  }
#line 2294 "dealyacc.tab.c"
    break;

  case 67:
#line 333 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_LOSER, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2300 "dealyacc.tab.c"
    break;

  case 68:
#line 335 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_CONTROLTOTAL, NIL, NIL, (yyvsp[-1].y_int), 0);  }
#line 2306 "dealyacc.tab.c"
    break;

  case 69:
#line 337 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_CONTROL, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int));  }
#line 2312 "dealyacc.tab.c"
    break;

  case 70:
#line 339 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_CCCC, NIL, NIL, (yyvsp[-1].y_int), 0); }
#line 2318 "dealyacc.tab.c"
    break;

  case 71:
#line 341 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_QUALITY, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int)); }
#line 2324 "dealyacc.tab.c"
    break;

  case 72:
#line 343 "../src/dealyacc.y"
                {
          (yyval.y_tree) = newtree(TRT_SHAPE, NIL, NIL, (yyvsp[-3].y_int), 1<<(shapeno++));
          if (shapeno >= 32) {
            yylloc.first_line = (yylsp[-3]).first_line ; /* do this so bison activates yylloc et. al. */
            yyerror("Too many shapes -- only 32 allowed!\n");
            YYERROR;
          }
        }
#line 2337 "dealyacc.tab.c"
    break;

  case 73:
#line 352 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_HASCARD, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int)); }
#line 2343 "dealyacc.tab.c"
    break;

  case 74:
#line 354 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_TRICKS, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int)); }
#line 2349 "dealyacc.tab.c"
    break;

  case 75:
#line 356 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_TRICKS, NIL, NIL, (yyvsp[-3].y_int), 4); }
#line 2355 "dealyacc.tab.c"
    break;

  case 76:
#line 358 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_SCORE, (yyvsp[-1].y_tree), NIL, (yyvsp[-5].y_int), (yyvsp[-3].y_int)); }
#line 2361 "dealyacc.tab.c"
    break;

  case 77:
#line 360 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_IMPS, (yyvsp[-1].y_tree), NIL, 0, 0); }
#line 2367 "dealyacc.tab.c"
    break;

  case 78:
#line 362 "../src/dealyacc.y"
                { (yyval.y_tree) = (yyvsp[-1].y_tree); }
#line 2373 "dealyacc.tab.c"
    break;

  case 79:
#line 364 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_CMPEQ, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2379 "dealyacc.tab.c"
    break;

  case 80:
#line 366 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_CMPNE, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2385 "dealyacc.tab.c"
    break;

  case 81:
#line 368 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_CMPLT, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2391 "dealyacc.tab.c"
    break;

  case 82:
#line 370 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_CMPLE, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2397 "dealyacc.tab.c"
    break;

  case 83:
#line 372 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_CMPGT, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2403 "dealyacc.tab.c"
    break;

  case 84:
#line 374 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_CMPGE, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2409 "dealyacc.tab.c"
    break;

  case 85:
#line 376 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_AND2, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2415 "dealyacc.tab.c"
    break;

  case 86:
#line 378 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_OR2, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2421 "dealyacc.tab.c"
    break;

  case 87:
#line 380 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_ARPLUS, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2427 "dealyacc.tab.c"
    break;

  case 88:
#line 382 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_ARMINUS, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2433 "dealyacc.tab.c"
    break;

  case 89:
#line 384 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_ARTIMES, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2439 "dealyacc.tab.c"
    break;

  case 90:
#line 386 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_ARDIVIDE, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2445 "dealyacc.tab.c"
    break;

  case 91:
#line 388 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_ARMOD, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0); }
#line 2451 "dealyacc.tab.c"
    break;

  case 92:
#line 390 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_IF, (yyvsp[-4].y_tree), newtree(TRT_THENELSE, (yyvsp[-2].y_tree), (yyvsp[0].y_tree), 0, 0), 0, 0); }
#line 2457 "dealyacc.tab.c"
    break;

  case 93:
#line 392 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_NOT, (yyvsp[0].y_tree), NIL, 0, 0); }
#line 2463 "dealyacc.tab.c"
    break;

  case 94:
#line 394 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_RND, (yyvsp[-1].y_tree), NIL, 0, 0);  }
#line 2469 "dealyacc.tab.c"
    break;

  case 95:
#line 397 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_LTCTOTAL, NIL, NIL, (yyvsp[-1].y_int),  0 ); }
#line 2475 "dealyacc.tab.c"
    break;

  case 96:
#line 399 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_LTC,      NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int) );  }
#line 2481 "dealyacc.tab.c"
    break;

  case 97:
#line 401 "../src/dealyacc.y"
                 { (yyval.y_tree) = newtree(TRT_OPCSIDE, NIL, NIL, (yyvsp[-1].y_int),  5);  }
#line 2487 "dealyacc.tab.c"
    break;

  case 98:
#line 403 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_DDS, NIL, NIL, (yyvsp[-3].y_int), (yyvsp[-1].y_int)); ;}
#line 2493 "dealyacc.tab.c"
    break;

  case 99:
#line 405 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_DDS, NIL, NIL, (yyvsp[-3].y_int),  4); }
#line 2499 "dealyacc.tab.c"
    break;

  case 100:
#line 407 "../src/dealyacc.y"
                { (yyval.y_tree) = newtree(TRT_PAR, NIL, NIL, (yyvsp[-1].y_int),  0);  /* PAR needs all 20 results. Forces mode=2. */ }
#line 2505 "dealyacc.tab.c"
    break;

  case 101:
#line 413 "../src/dealyacc.y"
                { (yyval.y_expr) = newexpr((yyvsp[0].y_tree), 0, 0); }
#line 2511 "dealyacc.tab.c"
    break;

  case 102:
#line 415 "../src/dealyacc.y"
                { (yyval.y_expr) = newexpr(0, (yyvsp[0].y_str), 0); }
#line 2517 "dealyacc.tab.c"
    break;

  case 103:
#line 417 "../src/dealyacc.y"
                { (yyval.y_expr) = newexpr((yyvsp[0].y_tree), 0, (yyvsp[-2].y_expr)); }
#line 2523 "dealyacc.tab.c"
    break;

  case 104:
#line 419 "../src/dealyacc.y"
                { (yyval.y_expr) = newexpr(0, (yyvsp[0].y_str), (yyvsp[-2].y_expr)); }
#line 2529 "dealyacc.tab.c"
    break;

  case 105:
#line 424 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm(0,  0,  1<<(yyvsp[0].y_int),   0,    0) ; }
#line 2535 "dealyacc.tab.c"
    break;

  case 106:
#line 426 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm(0,  0,  5<<(yyvsp[0].y_int),   0,    0) ; }
#line 2541 "dealyacc.tab.c"
    break;

  case 107:
#line 428 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm(0,  0,    15,    0,    0) ; }
#line 2547 "dealyacc.tab.c"
    break;

  case 108:
#line 430 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm((yyvsp[0].y_tree), 0,    0,     0,    0) ; }
#line 2553 "dealyacc.tab.c"
    break;

  case 109:
#line 432 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm(0, (yyvsp[0].y_str),    0,     0,    0) ; }
#line 2559 "dealyacc.tab.c"
    break;

  case 110:
#line 434 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm(0,  0,    0, 1<<(yyvsp[-1].y_int),    0) ; }
#line 2565 "dealyacc.tab.c"
    break;

  case 111:
#line 436 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm(0,  0,    0,    15,    0) ; }
#line 2571 "dealyacc.tab.c"
    break;

  case 112:
#line 438 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm(0,  0,  1<<(yyvsp[0].y_int),   0,   (yyvsp[-2].y_csv)) ; }
#line 2577 "dealyacc.tab.c"
    break;

  case 113:
#line 440 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm(0,  0,  5<<(yyvsp[0].y_int),   0,   (yyvsp[-2].y_csv)) ; }
#line 2583 "dealyacc.tab.c"
    break;

  case 114:
#line 442 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm(0,  0,    15,    0,   (yyvsp[-2].y_csv)) ; }
#line 2589 "dealyacc.tab.c"
    break;

  case 115:
#line 444 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm((yyvsp[0].y_tree), 0,    0,     0,   (yyvsp[-2].y_csv)) ; }
#line 2595 "dealyacc.tab.c"
    break;

  case 116:
#line 446 "../src/dealyacc.y"
                { (yyval.y_csv) = new_csvterm(0, (yyvsp[0].y_str),   0,     0,   (yyvsp[-2].y_csv)) ; }
#line 2601 "dealyacc.tab.c"
    break;

  case 117:
#line 448 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm(0,  0,    0, 1<<(yyvsp[-1].y_int),   (yyvsp[-5].y_csv)) ; }
#line 2607 "dealyacc.tab.c"
    break;

  case 118:
#line 450 "../src/dealyacc.y"
               { (yyval.y_csv) = new_csvterm(0,  0,    0,    15,   (yyvsp[-5].y_csv)) ; }
#line 2613 "dealyacc.tab.c"
    break;

  case 119:
#line 456 "../src/dealyacc.y"
                { (yyval.y_action) = (yyvsp[0].y_action); }
#line 2619 "dealyacc.tab.c"
    break;

  case 120:
#line 458 "../src/dealyacc.y"
                { (yyval.y_action) = (yyvsp[-2].y_action); (yyval.y_action)->ac_next = (yyvsp[0].y_action); }
#line 2625 "dealyacc.tab.c"
    break;

  case 121:
#line 460 "../src/dealyacc.y"
                { (yyval.y_action) = 0; }
#line 2631 "dealyacc.tab.c"
    break;

  case 122:
#line 467 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action) = newaction(ACT_PRINTALL, NIL, (char *) 0, 0, NIL);
                }
#line 2639 "dealyacc.tab.c"
    break;

  case 123:
#line 471 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action) = newaction(ACT_PRINTSIDE, NIL, (char *) 0, 1 , NIL);
                }
#line 2647 "dealyacc.tab.c"
    break;

  case 124:
#line 475 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action) = newaction(ACT_PRINTSIDE, NIL, (char *) 0,  0, NIL);
                }
#line 2655 "dealyacc.tab.c"
    break;

  case 125:
#line 479 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action) = newaction(ACT_PRINTSIDE, NIL, (char *) 0, (yyvsp[-1].y_int), NIL);
                }
#line 2663 "dealyacc.tab.c"
    break;

  case 126:
#line 483 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action)=newaction(ACT_PRINTCOMPACT,NIL,0,0, NIL);}
#line 2670 "dealyacc.tab.c"
    break;

  case 127:
#line 486 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action)=newaction(ACT_PRINTCOMPACT,(yyvsp[-1].y_tree),0,0, NIL);}
#line 2677 "dealyacc.tab.c"
    break;

  case 128:
#line 489 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action) = newaction(ACT_PRINTONELINE, NIL, 0, 0, NIL);}
#line 2684 "dealyacc.tab.c"
    break;

  case 129:
#line 492 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action) = newaction(ACT_PRINTONELINE, (yyvsp[-1].y_tree),  0, 0, NIL);}
#line 2691 "dealyacc.tab.c"
    break;

  case 130:
#line 495 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action)=newaction(ACT_PRINTPBN,NIL,0,0, NIL);}
#line 2698 "dealyacc.tab.c"
    break;

  case 131:
#line 498 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action)=newaction(ACT_PRINTES,(struct tree*)(yyvsp[-1].y_expr),0,0, NIL); }
#line 2705 "dealyacc.tab.c"
    break;

  case 132:
#line 504 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action) = newaction(ACT_PRINT, NIL, (char *) 0, (yyvsp[-1].y_int), NIL);
                }
#line 2713 "dealyacc.tab.c"
    break;

  case 133:
#line 508 "../src/dealyacc.y"
                { (yyval.y_action) = newaction(ACT_AVERAGE, (yyvsp[0].y_tree), (yyvsp[-1].y_str), 0, NIL); }
#line 2719 "dealyacc.tab.c"
    break;

  case 134:
#line 511 "../src/dealyacc.y"
                { (yyval.y_action) = newaction(ACT_FREQUENCY, (yyvsp[-5].y_tree), (yyvsp[-7].y_str), 0, NIL);
                  (yyval.y_action)->ac_u.acu_f.acuf_lowbnd = (yyvsp[-3].y_int);
                  (yyval.y_action)->ac_u.acu_f.acuf_highbnd = (yyvsp[-1].y_int);}
#line 2727 "dealyacc.tab.c"
    break;

  case 135:
#line 515 "../src/dealyacc.y"
                { (yyval.y_action) = newaction(ACT_FREQUENCY2D, (yyvsp[-11].y_tree), (yyvsp[-13].y_str), 0, (yyvsp[-5].y_tree));
                  (yyval.y_action)->ac_u.acu_f2d.acuf_lowbnd_expr1 = (yyvsp[-9].y_int);
                  (yyval.y_action)->ac_u.acu_f2d.acuf_highbnd_expr1 = (yyvsp[-7].y_int);
                  (yyval.y_action)->ac_u.acu_f2d.acuf_lowbnd_expr2 = (yyvsp[-3].y_int);
                  (yyval.y_action)->ac_u.acu_f2d.acuf_highbnd_expr2 = (yyvsp[-1].y_int); }
#line 2737 "dealyacc.tab.c"
    break;

  case 136:
#line 522 "../src/dealyacc.y"
                { (yyval.y_action)=newaction(ACT_EVALCONTRACT,0,0,(yyvsp[-5].y_int), NIL);
                  (yyval.y_action)->ac_u.acucontract.coded = (yyvsp[-3].y_int) ;           /* Flex makes coded contract from e.g z3Hxx */
                  (yyval.y_action)->ac_u.acucontract.vul = (yyvsp[-1].y_int) ;             /* Vul not part of flex coded contract */
                  (yyval.y_action)->ac_u.acucontract.dbl =  (yyvsp[-3].y_int) / 40;        /* DBL = 1, RDBL = 2 */
                  (yyval.y_action)->ac_u.acucontract.strain = (yyvsp[-3].y_int) % 5;       /* 0 .. 4 => C D H S N */
                  (yyval.y_action)->ac_u.acucontract.level  = ((yyvsp[-3].y_int) % 40 )/5; /* 5 .. 35  for 1 - 7 */
                }
#line 2749 "dealyacc.tab.c"
    break;

  case 137:
#line 530 "../src/dealyacc.y"
                { (yyval.y_action)=newaction(ACT_EXP_SIDE_HLD, 0, 0, (yyvsp[-1].y_int), NIL); }
#line 2755 "dealyacc.tab.c"
    break;

  case 138:
#line 532 "../src/dealyacc.y"
                { (yyval.y_action)=newaction(ACT_EXP_SEAT_HLD, 0, 0, (yyvsp[-1].y_int), NIL); }
#line 2761 "dealyacc.tab.c"
    break;

  case 139:
#line 534 "../src/dealyacc.y"
                { will_print++;
                  (yyval.y_action)=newaction(ACT_CSVRPT,(struct tree*)(yyvsp[-1].y_csv),0,0, NIL); }
#line 2768 "dealyacc.tab.c"
    break;

  case 140:
#line 539 "../src/dealyacc.y"
                { (yyval.y_str) = (char *) 0; }
#line 2774 "dealyacc.tab.c"
    break;

  case 141:
#line 541 "../src/dealyacc.y"
                { (yyval.y_str) = (yyvsp[0].y_str); }
#line 2780 "dealyacc.tab.c"
    break;

  case 142:
#line 546 "../src/dealyacc.y"
                { (yyval.y_int) = (1<<(yyvsp[0].y_int)); }
#line 2786 "dealyacc.tab.c"
    break;

  case 143:
#line 548 "../src/dealyacc.y"
                { (yyval.y_int) = (yyvsp[-2].y_int)|(1<<(yyvsp[0].y_int)); }
#line 2792 "dealyacc.tab.c"
    break;


#line 2796 "dealyacc.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 550 "../src/dealyacc.y"

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


