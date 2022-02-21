/* Pre --  Stuff here ignored by FLEX */
/* JGM 2021-11-17   Defined the USER ACTION macro in flex so that flex can update the yylloc with yylineno for bison */
/* JGM 2021-11-17   Eliminated the need for lino; using %option yylineno instead. Also other Mods */
/* JGM 2021-11-10   %x states so produce,generate,title,opener,dealer,vulnerable, now handled in flex not yacc */
/* JGM 2021-11-04   Added a pattern for DECNUM -- returns 100 * the value so 99.99 becomes an int 9999 */
/* JGM 2022-02-03   Changed the pattern for vulnerable to be case insensitive (?i: ... ) */
/* JGM 2022-02-04   Fixed Exclusion states to handle whitespace and newlines. Prob should handle 'other' chars also. TBD */
/* JGM 2022-02-04   Adding in the YY_STATE_BUFF and yy_parse_string stuff, for script var text substitution  Scriptvar*/
/* JGM 2022-02-07   Adding the csvlist and CSVRPT mods */
/* JGM 2022-02-12   Adding the trix function token processing. */
 #ifndef _GNU_SOURCE
   #define _GNU_SOURCE
 #endif
%{
  /* Prolog: This code copied to the output file created by Flex */
  /* Need some include statements here for e.g. y.tab.h etc. */
 float flexvers = 4.0 ; /* for debugging and version tracking */
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "../include/dealdefs.h"
 #include "../include/dealtypes.h"
 #include "../include/dealexterns.h"  /* for maxgenerate, maxproduce, maxdealer, and maxvuln, title etc. */
 #include "../include/dealprotos.h"  /* for my_strcpy, ... */
 #include "dealyacc.tab.h"          /* for all the token definitions */

 #define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;

   /* Following added for script variable ($0 - $9 ) processing */
  YY_BUFFER_STATE yy_script_buff ;  /* opaque struct var to get input from a string */
  YY_BUFFER_STATE yy_cur_buff;      /* opaque struct var to save Input File buffer  */
  int in_str_buff = 0 ;             /* Flag to us that EOF condition was on string buff, not Input File buff */
  int scr_vnum;                     /* Subscript into the script_parm array */

  #define BUFFLEN 255
  char buff[BUFFLEN + 1] = {'\0'};                  /* to copy yytext for local processing */
  char *c_ptr;
  long int dotpos ;
  int dotnumval ;

  /* modified yywrap to handle EOF on a script string buffer as well as the original input file buffer */
  /* Calling yywrap is the default <<EOF>> rule */

  int yywrap () {
   if (!in_str_buff ) {  /* End of Input File, means yyparse() is done */

      #ifdef JGMDBG
        if(jgmDebug >=3) { fprintf(stderr, "FLEX::Processed %d lines in Input File. Exit to main now. \n", yylineno ); }
       #endif

       return 1 ;      /* yylex is alldone. No more input */
   } /* end if Not in_str_buff */

   /* EOF was not on Input File, but rather on a script variable string buffer */
   /* Reclaim RAM, pop state, and continue with Input File */
   yy_delete_buffer( yy_script_buff );
   yypop_buffer_state();
   in_str_buff--;
   yy_switch_to_buffer(yy_cur_buff);
      #ifdef JGMDBG
        if(jgmDebug >=3) {fprintf( stderr, "Script var ended. Back to reading Input File.  in_str_buff=[%d]\n", in_str_buff );}
      #endif
   return 0 ; /* not all done yet. */
} /* end yywrap */

  /* routines to set global vars for PBN printing for dealer and Vulnerable */
 int compass2int( char *s ) {    /* checking for north , east , south , west */
  #ifdef JGMDBG
     if(jgmDebug >= 3) { fprintf(stderr, "FLEX::In compass2int with string [%s]\n", s) ; }
  #endif
    if      (strchr(s, 'n' ) || strchr(s, 'N') ) { return COMPASS_NORTH ; }
    else if (strchr(s, 'a' ) || strchr(s, 'A') ) { return COMPASS_EAST  ; }
    else if (strchr(s, 'u' ) || strchr(s, 'U') ) { return COMPASS_SOUTH ; }
    else if (strchr(s, 'w' ) || strchr(s, 'W') ) { return COMPASS_WEST  ; }
    return 0 ;  /* default to north */
 } /* end compass2int */

 int vul2int(char *s ) {         /* checking for none, NS, EW , all, both */
    #ifdef JGMDBG
       if(jgmDebug >= 3) { fprintf(stderr,"FLEX::In vul2int with side=[%s] at yylineno[%d]\n", s, yylineno) ; }
    #endif
    if      (strchr(s, 'o' ) || strchr(s, 'O') ) { return VULNERABLE_NONE ; }
    else if (strchr(s, 's' ) || strchr(s, 'S') ) { return VULNERABLE_NS   ; }
    else if (strchr(s, 'w' ) || strchr(s, 'W') ) { return VULNERABLE_EW   ; }
    else if (strchr(s, 'a' ) || strchr(s, 'A') ) { return VULNERABLE_ALL  ; }
    else if (strchr(s, 'b' ) || strchr(s, 'B') ) { return VULNERABLE_NS   ; }
    return 0 ; /* default to none */
} /* end vul2int*/

%}
        /* Flex Declarations section: States, aliases etc. */
    /* Ask Flex to keep track of the input line number -- the user action macro (line 15) will also pass yylineno to bison*/
%option yylineno

 /* Define Exclusive states to handle text processed within Flex */
%x COMMENT
%x MAXPROD
%x MAXGEN
%x MAXDEAL
%x MAXVUL
%x TITLE
%x OPENER
        /* These aliases are to make the patterns in Flex easier to write and to read */
        /* You cant put comments on lines that define aliases */
ws [ \t]+
nl \n
comment   #.*
qstring   \"[^\"\n]*[\"\n]
shape     [0-9x]{4}
    /* Note that shape will override digits, even in the case of 4432 5332 etc since it comes BEFORE digits in the rules */
id        [a-zA-Z][a-zA-Z0-9_]*
   /* JGM added underscore to above definition of an id */
digits    [0-9]+
  /*  This next one should be useful to get numbers of the form nn.mm for LTC, CCCC, Suit Quality OPC etc. */
  /* Handles for example:  .5 , .25 , 0.1 , 0.77, 12. , 12.0, 12.05, 12.76 , 2., 2.0, 2.1 , 2.05 , 2.95, etc. */
  /* Since there are no other words in dealer that use a DOT any time we see digits next to a dot it is a dotnum aka DECNUM */
  /* Note that dotnum will override digits when appropriate since n. and nn. are both longer than n or nn */

dotnum    \.[0-9]{1,2}
decnum    [0-9]{1,2}\.[0-9]{0,2}
scriptvar [$][0-9]


 /* Enter Matching Rules section  {bol}pattern   { C code to execute when pattern matches }  */
%%
"||"            return(OR2);
or              return(OR2);
"&&"            return(AND2);
and             return(AND2);
"!"             return(NOT);
not             return(NOT);
"=="            return(CMPEQ);
"!="            return(CMPNE);
"<"             return(CMPLT);
"<="            return(CMPLE);
">"             return(CMPGT);
">="            return(CMPGE);
"+"             return(ARPLUS);
"-"             return(ARMINUS);
"*"             return(ARTIMES);
"/"             return(ARDIVIDE);
"%"             return(ARMOD);
"?"             return(QUERY);
":"             return(COLON);
"//".*\n        ;                       /* do nothing if C style one line comment */
"/*"            BEGIN(COMMENT);
<COMMENT>[^*\n<<EOF>>]*                 /*In Comment State:  eat up non-star, non EOL and non EOF .. */
<COMMENT>"*"+[^*/\n<<EOF>]*             /*                   eat one or more * if followed by non *, non / non EOL non EOF */
<COMMENT>\n     ;                       /* do nothing if {nl} found. flex will track yylineno */
<COMMENT><<EOF>> yyerror("EOF found in comment");       /*yyerror prints and then does exit(-1) */
<COMMENT>"*"+"/" BEGIN(INITIAL);                        /* found one or more * with a slash next to. Exit state COMMENT */

predeal         return(PREDEAL);
pointcount      return(POINTCOUNT);
altcount        return(ALTCOUNT);
condition       return(CONDITION);
action          return(ACTION);
print           return(PRINT);
printall        return(PRINTALL);
printpbn        return(PRINTPBN);
printew         return(PRINTEW);
printns         return(PRINTNS);
printside       return(PRINTSIDE);
printcompact    return(PRINTCOMPACT);
printoneline    return(PRINTONELINE);
printes         return(PRINTES);
average         return(AVERAGE);
frequency       return(FREQUENCY);
deal            return(DEAL);
csvrpt          return(CSVRPT);

   /* now do some direct handling of the cases where yyparse not needed. It simplifies the yyparse() and the grammar */
   /* We do have to allow for the fact that the user might enter the keyword
    * e.g. generate on a different line from the value e.g. 1000000
    * Rather than define a pattern that can span multiple lines we use the exclusive states feature of Flex
    */
vulnerable({ws}|{nl}) { BEGIN(MAXVUL);   }
<MAXVUL>{nl}  ;                 /* do nothing if {nl} found. flex will track yylineno */
<MAXVUL>{ws}  ;
<MAXVUL>(?i:none|all|both|NS|EW)   { strncpy(buff, yytext, yyleng) ; buff[yyleng]='\0';
                                    maxvuln = vul2int(buff) ; yylval.y_int = maxvuln ;
                                    BEGIN(INITIAL) ;
                                 }

generate({ws}|{nl}) { BEGIN(MAXGEN);  }
<MAXGEN>{nl}  ;                /* do nothing if {nl} found. flex will track yylineno */
<MAXGEN>{ws}  ;                  /* just eat up ws */
<MAXGEN>{digits}         { strncpy(buff, yytext, yyleng) ; buff[yyleng]='\0';
                            if (!maxgenerate) maxgenerate = atoi(buff)   ;
                            yylval.y_int = maxgenerate ;
                            BEGIN(INITIAL) ;
                  }

produce({ws}|{nl})   { BEGIN(MAXPROD);   }
<MAXPROD>{nl}  ;                /* do nothing if {nl} found. flex will track yylineno */
<MAXPROD>{ws}  ;
<MAXPROD>{digits} { strncpy(buff, yytext, yyleng) ; buff[yyleng]='\0';
                        if (!maxproduce ) maxproduce  = atoi(buff)   ;
                        yylval.y_int = maxproduce ;
                        BEGIN(INITIAL) ;
                  }
title({ws}|{nl}) { BEGIN(TITLE);  }
<TITLE>{nl}   ;                /* do nothing if {nl} found. flex will track yylineno */
<TITLE>{ws}   ;
<TITLE>{qstring}  {
                    strncpy(title, yytext+1, (yyleng-2) ) ;
                    title[yyleng-2] = '\0';
                    title_len = strlen(title) ;
                    BEGIN(INITIAL) ;
                  }

dealer({ws}|{nl})  { BEGIN(MAXDEAL); }
<MAXDEAL>{nl}  ;                /* do nothing if {nl} found. flex will track yylineno */
<MAXDEAL>{ws}  ;
<MAXDEAL>(north|south|east|west)   { strncpy(buff, yytext, yyleng) ; buff[yyleng] = '\0';
                                     maxdealer = compass2int(buff); yylval.y_int = maxdealer ;
                                     BEGIN(INITIAL) ;
                                    }

opener({ws}|{nl})  { BEGIN(OPENER); }
<OPENER>{nl}  ;                /* do nothing if {nl} found. flex will track yylineno */
<OPENER>{ws}  ;
<OPENER>(north|south|east|west)   { strncpy(buff, yytext, yyleng) ; buff[yyleng] = '\0';
                                     Opener = compass2int(buff); yylval.y_int = Opener ;
                                     BEGIN(INITIAL) ;
                                    }
   /* evalcontract is an action that prints the average score for the given contract */
   /* User can specify any contract from 1C to 7N re-doubled,  nv and vul, for either side  */
evalcontract    { return(EVALCONTRACT); }
nv              { yylval.y_int = NON_VUL; return VULN; }
vul             { yylval.y_int = VUL; return VULN; }

north           { yylval.y_int = COMPASS_NORTH; return COMPASS; }
east            { yylval.y_int = COMPASS_EAST; return COMPASS; }
south           { yylval.y_int = COMPASS_SOUTH; return COMPASS; }
west            { yylval.y_int = COMPASS_WEST; return COMPASS; }
NS              { yylval.y_int = SIDE_NS ; return SIDE ; }
EW              { yylval.y_int = SIDE_EW ; return SIDE ; }

  /* These next rules match both singular and plural form, ie. club and clubs or hcp and hcps the 's' is optional */
clubs?          { yylval.y_int = SUIT_CLUB; return SUIT; }
diamonds?       { yylval.y_int = SUIT_DIAMOND; return SUIT; }
hearts?         { yylval.y_int = SUIT_HEART; return SUIT; }
spades?         { yylval.y_int = SUIT_SPADE; return SUIT; }

notrumps?   return NOTRUMPS;
hcps?       return(HCP);            /* Note you can have variables hcpn, hcpe, hcpw but NOT hcps */
pt0         return(PT0);
tens?       return(PT0);
pt1         return(PT1);
jacks?      return(PT1);
pt2         return(PT2);
queens?     return(PT2);
pt3         return(PT3);
kings?      return(PT3);
pt4         return(PT4);
aces?       return(PT4);
pt5         return(PT5);
top2        return(PT5);
pt6         return(PT6);
top3        return(PT6);
pt7         return(PT7);
top4        return(PT7);
pt8         return(PT8);
top5        return(PT8);
pt9         return(PT9);
c13         return(PT9);
losers?     return(LOSER);
tricks?     return(TRICKS);
controls?   return(CONTROL);
quality     return(QUALITY);
cccc        return(CCCC);
score       return(SCORE);
imps?       return(IMPS);
hascard     return(HASCARD);
shape       return(SHAPE);
any         return(ANY);
rnd         return(RND);
ltc         return(LTC);
dds         return(DDS);
par         return(PAR);
opc         return(OPC);
export      return(EXPORT);
trix        return(TRIX);


[CDHS]A?K?Q?J?T?9?8?7?6?5?4?3?2?        { yylval.y_str = mystrcpy(yytext); return HOLDING; }
[2-9TJQKA][CDHS] { yylval.y_int = make_card(yytext[0], yytext[1]);         return(CARD);   }

[xz][1-7][CDHSN][x]{0,2} { yylval.y_int = make_contract(&yytext[1]) ; // encode the contract as a number (5..39) + (40*UND|DBL|RDBL)
                            return(CONTRACT);
                        }

[0-9x]{4}       { strncpy(yylval.y_distr, yytext, 8);
                  if (strchr(yytext, 'x') )  { return DISTR; }
                  else                       { return DISTR_OR_NUMBER; }
                }


[0-9]+          { yylval.y_int = atoi(yytext); return NUMBER; }
[a-zA-Z][a-zA-Z0-9_]*   { yylval.y_str = mystrcpy(yytext); return IDENT; }

{dotnum} |          /* numbers of the form .[0-9]{1,2} or [0-9]{1,2}\.[0-9]{0,2} */
{decnum}        { yylval.y_decnum = (int)(100. * atof(yytext) ) ; /* will handle minus signs */
                  return DECNUM ;
                }

{qstring}      { yylval.y_str = strdup(yytext+1); /* skip open quote */
                  if (yylval.y_str[yyleng-2] != '"') {
                    fprintf(stderr,"Unterminated character string\n");
                    return STRING ;
                  }
                  else {
                    yylval.y_str[yyleng-2] = '\0'; /* remove close quote */
                    /* printf("Found string at line[%d]\n", yylineno );   // shows that yylineno works */
                    return STRING;
                  }
                }
{scriptvar}  {
               /* Save current input buffer context, open new buffer based on a string, and make it the current one */
             yy_cur_buff = YY_CURRENT_BUFFER;
             scr_vnum = *(yytext+1) - '0' ; /* cheap atoi conversion */
             yy_script_buff = yy_scan_string( parm.script_var[scr_vnum] ) ; /* a global var filled by the getopts() function */
             yypush_buffer_state(yy_script_buff) ;
             in_str_buff++ ;  /* maybe not req'd but don't know how to avoid it */
             #ifdef JGMDBG
               if(jgmDebug >= 3 ) {
                  fprintf(stderr, " A Dollar sign Script variable : %s\n", yytext ) ;
                  fprintf(stderr, "scan_string state pushed. in_str_buff = [%d], scr_vnum=[%d]\n", in_str_buff, scr_vnum);
                  fprintf(stderr, "script_var[%d]=%s\n", scr_vnum, parm.script_var[scr_vnum] ) ;
               }
             #endif
          }

\\n             { yylval.y_str = mystrcpy("\n"); return STRING; }  /* need this one for printes LF */
[ \t]           ;
\r*\n           { ; }                 /* do nothing if {nl} found. flex will track yylineno */
^\#.*\n         { ; }                 /* # in col1 is a comment. Do nothing. flex will track yylineno */
.               return(yytext[0]);    /* unknown char; handles stuff like , ; may trigger error in yyparse if unexpected */

%%
/* epilog section. More C-Code text just written to the ouput file goes here*/
