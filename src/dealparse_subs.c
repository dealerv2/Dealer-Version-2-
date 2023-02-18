/* File dealparse_subs.c -- JGM 2022-Feb-15 */
/* Contains code mostly called from yyparse(). Separate file to limit size of the dealyacc.y file
 * 2023/01/07 -- Merged in changes from V4 to fix predeal; dealcards_subs.c and globals, etc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/dealexterns.h"
#include "../include/dealprotos.h"
#include "../include/dbgprt_macros.h"

char *mycalloc (size_t nel, size_t siz) {
  char *p;

  p = calloc (nel, siz);
  if (p) return p;
  fprintf (stderr, "Out of memory\n");
  exit (-1); /*NOTREACHED */
}
/* this next one is referred to in dealflex.l to copy holdings and place the ptr in yylval.y_str */
char *mystrcpy(char *s) {
        char *cs;
        cs = mycalloc(strlen(s)+1, sizeof(char));
        strcpy(cs, s);
        return cs;
} /* end mystrcpy() */

void error (char *s) {
  fprintf (stderr, "%s%s", s, crlf);
  exit (10);
}

/* JGM has modified how the altcounts are tracked so that the value cin in the altcount cin statement matches ptn */
/* Can't really see a need to keep TWO arrays that track std 4-3-2-1 HCP; but kept them both anyway */
/* so we have points[13] and tblPointCount[idxEnd][13] which both do the same thing. */
void zerocount (int points[13]) {  /* generic zero out of any pointcount array: HCP or ptn */
  int i;
  for (i = 0; i < 13 ; i++) points[i] = 0; /* JGM change to incr loop */
}

void clearpointcount () {   /* zero out the HCP array 'points' and the copy tblPointCount[idxHCP] */
  zerocount (tblPointcount[idxHcp]);
  zerocount ( points );
  alt_tbl_idx = -1 ;         /* this global var keeps track of which pt cnt table we are updating. -1 flags HCP tbl */
 }

void clearpointcount_alt (int cin) {
               /* cin comes from the altcount tblnum <number_list> or from defcount tblnum <decnum_list> statements*/
  JGMDPRT(4,"Clear Alt Count cin=%d\n",cin);  /* as of Sep cin should be 0 .. 10 to allow defcount to set the hcp values to dotnums */
  zerocount (tblPointcount[cin]);
  alt_tbl_idx = cin;                      /* global var alt_tbl_idx keeps track of which pt cnt table we are updating. */
}

void pointcount (int rank, int value) { /* set the value for an entry in the tblPointCount[idxHcp] tbl */
            /* There is a global var alt_tbl_idx set by YACC code that tracks which alt count is being changed */
            /* there is a global var pointcount_index set by YACC code that keeps track of where rank is right now */
  assert (rank <= 12);
  if (rank < 0) {                       /* we have counted down from 12 too far. */
      yyerror ("too many pointcount values");
  }
  if (alt_tbl_idx < 0 )  {                  /* alt_tbl_idx selects which table to affect; < 0 defaults to the hcp one */
    tblPointcount[idxHcp][rank] = value;
    points[ rank ] = value ;               /* keep the points and tblPointcount[idxHCP] arrays in sync */
  }
  else {
    tblPointcount[alt_tbl_idx][rank] = value;
  } /* end if else countindex */
#ifdef JGMDBG
  if (jgmDebug >= 3 ) {
      JGMDPRT(3,"In pointcount:: Setting TBL# %d Rank=%d to Value=%d -> %d\n", alt_tbl_idx, rank, value, tblPointcount[alt_tbl_idx][rank] );
  }
#endif
}  /* end set point count pointcount */



void setshapebit (int cl, int di, int ht, int sp, int msk, int excepted) {
  if (excepted)
    distrbitmaps[cl][di][ht][sp] &= ~msk; /*4D array set to zero by calloc */
  else
    distrbitmaps[cl][di][ht][sp] |= msk;
}

card make_card (char rankchar, char suitchar) {
  int rank, suit = 0;

  for (rank = 0; rank < 13 && ucrep[rank] != rankchar; rank++) ; /* incr until we find the card rank */
  assert (rank < 13);
  switch (suitchar) {
    case 'C':      suit = 0;      break;
    case 'D':      suit = 1;      break;
    case 'H':      suit = 2;      break;
    case 'S':      suit = 3;      break;
    default:
      assert (0);
    }
  return MAKECARD (suit, rank);
} /* end make_card (rank, suit) */


/* In Flex we define a contract as [z][1-7][CDHSN][x]{0,2} so we can have z3N, z3Nx, and z3Nxx
 * Flex passes us the string minus the leading z
 * Then we count the x's in the string,
 * and encode the contract as an int with (5*level + strain) + (40 * xCount) Gives a number between 5(1C) and 119(7Nxx)
 *  When we later factor in the Vulerability by setting bit 7 we get a number between 5 and 247 )
 */
int make_contract (char *c_str ) {  /* yytext[1]... = [1-7][CDHSN][x]{0,2} */
  int level, strain, dbl_flag, coded_contract;
  level = (int) c_str[0] - '0';
  switch (c_str[1]) {
    case 'C':      strain = 0;      break;
    case 'D':      strain = 1;      break;
    case 'H':      strain = 2;      break;
    case 'S':      strain = 3;      break;
    case 'N':      strain = 4;      break;
    default:
      strain = 0;      printf ("%c", c_str[1]);
      assert (0);
  } /* end switch on strain char */
  dbl_flag = 0;
  while (*c_str != '\0' ) { /* count the x's in the contract string starting at yytext[1], skipping yytext[0] */
      if (*c_str == 'x' ) dbl_flag++ ;  // should be 0,1,2
      c_str++ ;
  }
  assert( dbl_flag >= 0 && dbl_flag <= 2 );
  coded_contract = level*5 + strain; /* a number between 5 (1C) and 39 (7NT) -- undoubled contract */
  coded_contract += 40*dbl_flag ; /* dbld contract 45 - 79 ; redbled contract 85 - 119 */
  return coded_contract ;
}

void fmt_contract_str(char *str, int level, int strain, int dbl, int vul ) {
    char dbl_str[3] = {'\0'};
    char vulch = ' ';
    if      (dbl == 0 ) { dbl_str[0] = ' ' ; }
    else if (dbl == 1 ) { dbl_str[0] = 'x' ; }
    else if (dbl == 2 ) { dbl_str[0] = 'x'; dbl_str[1] = 'x' ; }
    if      (vul >  0 ) { vulch = 'V' ; }
    sprintf(str, "%d%c%s %c%c",level,"CDHSN"[strain],dbl_str,vulch,'\0') ;
    return ;
} /* end fmt_contract_str */

/* Fill a structure with the individual fields from a coded contract, a printable string and the original coded value */
struct contract_st decode_contract(int coded_contract ) {
    struct contract_st c_res ;
    int coded ;
    coded = coded_contract & 0x7F ;
    c_res.vul = (coded_contract >> 8 ) & 0x01 ;
    c_res.dbl = coded / 40 ;
    c_res.strain = coded % 5 ;
    c_res.level  = (coded % 40 ) / 5 ;
    fmt_contract_str(c_res.c_str, c_res.level, c_res.strain, c_res.dbl, c_res.vul ) ;
    c_res.coded= coded_contract ;
    return c_res ;
} /* end decode contract */

void predeal (int player, card onecard) {  /* this moves a card from fullpack to stacked_pack, replacing it with NO_CARD */
  int i, j;

  for (i = 0; i < 52; i++) {
    if (fullpack[i] == onecard) {
      fullpack[i] = NO_CARD;
      --full_size;
      for (j = player * 13; j < (player + 1) * 13; j++)
        if (stacked_pack[j] == NO_CARD) {
        stacked_pack[j] = onecard;
        stacked_size++ ;
        return;
        }
      yyerror ("More than 13 cards for one player");
    }  /* end fullpack == onecard */
  }
  yyerror ("Card predealt twice");
} /* end  predeal -- */



