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
/* JGM 2022-03-03   Adding SETSEED and <FR1>, <FR2>, <FR3> and buffer switching for Francois shapes -- based on V4. */
/* JGM 2022-09-11   Cleaned up altcount, pointcount, and defcount processing. Can now do any mix of ints and dotnums in list */
/* JGM 2022-09-14   Removed defcount. Added bktfreq. */
/* JGM 2022-11-07   Mods for UserEval; new token ; new keyword .. */
/* JGM 2022-12-20   Touch up for UserEval; uevtag, var name change */
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
 #define YY_USER_INIT state_stk[++buffptr] = 0 ; yybuff_stk[buffptr] = YY_CURRENT_BUFFER ; \
                     show_our_state(3, buffptr, "YY_USER_INIT setup " );
    /* YY_USER_INIT occurs too early for YY_CURRENT_BUFFER to be intialized */

   YY_BUFFER_STATE yy_cur_buff;      /* opaque struct var to save current buffer used in script var but not for restore */
 /* following added during Francois shape addition */
  YY_BUFFER_STATE yy_shape_buff ;    /* opaque struct var to get input from results of Francois parse */
  YY_BUFFER_STATE yy_file_buff;      /* opaque struct var to save Input File buffer  */
  YY_BUFFER_STATE yy_temp_buff;

         /* For script variable ($0 - $9 ) processing outside of Francois */
  YY_BUFFER_STATE yy_parm_buff ;  /* opaque struct var to get input from a string */

       /* Francois stuff for parsing */
  #define NULL_STATE 0
  #define FILE_STATE 1
  #define PARM_STATE 2
  #define FRANCOIS   3
  int buffptr = 0 ;            /* buffptr == 0 means we havent pushed anything yet */
  int state_stk[10] = { -1 };
  YY_BUFFER_STATE yybuff_stk[10] = { 0 } ;  /* array of handles to opaque structs */


  int in_str_buff = 0 ;             /* Flag to us that EOF condition was on string buff, not Input File buff */
  int scr_vnum;                     /* Subscript into the script_parm array for scriptvar code*/
  int scrv_idx = 0;                 /* Subscript into the script_parm array for Francois code*/


  #ifdef JGMDBG
      void show_our_state( int dbg_req, int stk_idx,  char *msg ) {
         if (jgmDebug >= dbg_req) {
            fprintf(stderr, "[ShowState: state[%d]=%d, BuffHandle=%p, Msg:%s\n",
                  stk_idx, state_stk[stk_idx], (void *)yybuff_stk[stk_idx], msg ) ;
         }
         return ;
      }
   #else
      void show_our_state( int dbg_req, int stk_idx,  char *msg ) { return ; }
   #endif

  #define BUFFLEN 255
  char buff[BUFFLEN + 1] = {'\0'};                  /* to copy yytext for local processing */
  char *c_ptr;
  long int dotpos ;
  int dotnumval ;

 /* Francois vars and functions    */
     /* Francois stuff for processing */
 #define CMDBUFFLEN 254
 char fd_cmd[] = "/usr/local/bin/DealerV2/lib/fdp ' ";  /* Note the single quote. Required because FD shapes contain special shell chars */
 char fd_cmd_buff[256] = "/usr/local/bin/DealerV2/lib/fdp ' ";
 char txt_buff[256] ; /*hold a copy of yytext when Francois shapes being done ? */
 char *fdbp ;   /* pointer to next avail byte in fd_cmd_buff */
 struct fd_shape_st fd_shape_buff ; /* 2K buffer to hold results of Francois expansion, and a length */
 int fd_status = 0;

 /* append text to cmd buff */
 char *fmt_cmd_buff( char *buff, char *txt ) {  // generic to just append anything we ask starting at bp.
    char *bp  ;
    bp = buff ;
   // cmd buff to say e.g. ../lib/fdp 'shape{north, 5M(431)}' with single quotes around shape to avoid shell exp
   bp = stpcpy(bp, txt) ;        // typically yytext strdup passed in.
  *bp = '\0' ;                   // and then make sure we end with a null
  #ifdef JGMDBG
      if (jgmDebug >= 4 ) { fprintf(stderr, "NEW CMD BUFF=[%s]\n",fd_cmd_buff ) ; }
  #endif
  return bp ;            /* bp points to null byte copied in from fdshape  so we can append to buff if needed */
 } /* end fmt_cmd_buff */

 /* issue the popen cmd using the previously formatted cmd_buff; */
 int parse_fd_shapes (struct fd_shape_st *fd_res, char *cmd_buff ) {
    FILE *fp;
    int status;
    char *cptr ;
    fp = popen(cmd_buff , "r" ) ;
    if (fp == NULL) {
        perror( ":: popen returned NULL" ) ;
        fprintf(stderr, "parse_fd_shapes:: popen FDP cmd failed ! \n");
        return -1 ;
    }
    cptr = fgets(fd_res->fd_shape_str, FD_SHAPE_LEN, fp ) ;  // read the output of the pipe; includes one NULL
    if ( cptr == NULL ) {
        perror( ":: fgets returned NULL" ) ;
        fprintf(stderr, "parse_fd_shapes:: fgets dpp cmd failed ! \n");
        return -1 ;
    }
    fd_res->shape_len = strlen(fd_res->fd_shape_str);
    if (fd_res->fd_shape_str[fd_res->shape_len-1] == '\n' ) {  // flex does not want a newline
                 fd_res->fd_shape_str[fd_res->shape_len-1] = '\0'; // replace newline with a null
    }
    #ifdef JGMDBG
       if (jgmDebug >= 4 ) { fprintf(stderr, "Len:%ld,FGETS*=%s\n",fd_res->shape_len, fd_res->fd_shape_str ) ;  }
    #endif
    fd_res->fd_shape_str[fd_res->shape_len+1] = '\0'; /* If no NL to clobber we need this for the dbl NULL */
    status = pclose(fp);
    if (status == -1) {
        perror("parse_fd_shapes:: pclose reported error ") ;
    }
  return 1 ;
 } /* end parse_fd_shapes */

  /* General Parsing routines */
  /* set global vars for PBN printing for dealer and Vulnerable */
 int compass2int( char *s ) {    /* checking for north , east , south , west */
  #ifdef JGMDBG
     if(jgmDebug >= 3) { fprintf(stderr, "FLEX::In compass2int with string [%s]\n", s) ; }
  #endif
    if      (strchr(s, 'n' ) || strchr(s, 'N') ) { return COMPASS_NORTH ; }
    else if (strchr(s, 'a' ) || strchr(s, 'A') ) { return COMPASS_EAST  ; }
    else if (strchr(s, 'u' ) || strchr(s, 'U') ) { return COMPASS_SOUTH ; }
    else if (strchr(s, 'w' ) || strchr(s, 'W') ) { return COMPASS_WEST  ; }
    return COMPASS_NORTH ;  /* default to north */
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
    return VULNERABLE_NONE ; /* default to none */
 } /* end vul2int*/

 void setup_my_stack( void ) {
    state_stk[++buffptr] = FILE_STATE ;
    yybuff_stk[buffptr] = YY_CURRENT_BUFFER ;
    show_our_state( 3, buffptr,  "Flex: Early Rules Result" );
    //printf( "Early Print on STDOUT \n");
 }

%}
        /* Flex Declarations section: States, aliases etc. */
    /* Ask Flex to keep track of the input line number -- the user action macro (line 15) will also pass yylineno to bison*/
%option yylineno

 /* Define Exclusive states to handle text processed within Flex */
%x COMMENT
%x SETSEED
%x MAXPROD MAXGEN MAXDEAL MAXVUL
%x OPENER
%x TITLE
%x FD1 FD2 FD3

        /* These aliases are to make the patterns in Flex easier to write and to read */
        /* You cant put comments on lines that define aliases */
ws [ \t]+
nl \n
wsnl [ \t\n]
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

 /* descriptive tags for the UserEval Statements. Also allowed numbers */
 /*   uevtag   [~][A-Za-z] */

  /* stuff for Francois Shapes  */
FTERM  [}]
FCOMPASS (north|east|south|west){ws}*[,]
FDIRVAR  [$][0-9]{ws}*[,]
                 /* -  [    ]  \n  */
FPUNC [or+:><=,() \x2d\x5b\x5d\0A]
FDIST  [cdhsMmx0-9]
    /* $n (a script var) is allowed in FDshape, but needs special handling -- might be followed by another shape or not.*/
FDISTVAR [$][0-9]{ws}*[+]?

 /* Enter Matching Rules section  {bol}pattern   { C code to execute when pattern matches }  */
%%
   /* must indent This code at the top of the Rules section works. YY_CURRENT_BUFFER returns a value*/
   static int init_done = 0 ;
   if( !init_done ) {
       setup_my_stack() ;
       init_done = 1 ;
       // printf("SETUP my stack done on STDOUT \n");
   }

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
"/*"            BEGIN(COMMENT);        // slash star comments cannot contain greater than, less than or eof in caps

<COMMENT>[^*\n<<EOF>>]*     ;          /*In Comment State:  eat up non-star, non EOL and non EOF .. */
<COMMENT>"*"+[^*/\n<<EOF>]* ;          /*                   eat one or more * if followed by non *, non / non EOL non EOF */
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
printrpt        return(PRINTRPT);
bktfreq         return(BKTFREQ) ;
usereval        { userserver_reqd++ ; return(USEREVAL); } /* set a flag to start the server */

   /* now do some direct handling of the cases where yyparse not needed. It simplifies the yyparse() and the grammar */
   /* We do have to allow for the fact that the user might enter the keyword
    * e.g. "generate" on a different line from the value e.g. 1000000
    * Rather than define a pattern that can span multiple lines we use the exclusive states feature of Flex
    */
seed({ws}|{nl}) { BEGIN(SETSEED);   }
<SETSEED>{nl}  ;                 /* do nothing if {nl} found. flex will track yylineno */
<SETSEED>{ws}  ;
<SETSEED>{digits}   { strncpy(buff, yytext, yyleng) ; buff[yyleng]='\0';
                       seed = atol(buff) ; /* no need to set yylval since not returning to yyparse() */
                       seed_provided = 1L ;
                       BEGIN(INITIAL) ;
                     }

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
title({ws}|{nl})   { BEGIN(TITLE) ; }
<TITLE>{qstring}  {
                    if( (yyleng - 2) < ( MAXTITLE ) ) {
                        strncpy(title, yytext+1, (yyleng-2) ) ;
                        title[yyleng-2] = '\0';
                     }
                     else {
                        strncpy(title, yytext+1, MAXTITLE ) ;
                        title[MAXTITLE]='\0';
                     }
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


[0-9]+                  { yylval.y_int = atoi(yytext); return NUMBER; }
[a-zA-Z][a-zA-Z0-9_]*   { yylval.y_str = mystrcpy(yytext); return IDENT; }

{dotnum} |          /* numbers of the form .[0-9]{1,2} or [0-9]{1,2}\.[0-9]{0,2} */
{decnum}        { yylval.y_int  = (int)(100. * atof(yytext) ) ; /* will handle minus signs */
         #ifdef JGMDBG
               if(jgmDebug >= 3) { fprintf(stderr, "FLEX::Found a dotnum/decnum string= [%s] value=%d\n", yytext, yylval.y_int ) ; }
         #endif
                  return NUMBER ; /*?+?*/
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
             yy_parm_buff = yy_scan_string( parm.script_var[scr_vnum] ) ; /* a global var filled by the getopts() function */
             yypush_buffer_state(yy_parm_buff) ;
             state_stk[++buffptr] = PARM_STATE ; /* push our own state on the stack. */
             yybuff_stk[buffptr]  = YY_CURRENT_BUFFER ; /* save handle to current buffer */
             show_our_state( 3 , buffptr, "In PARM State. Starting Scan of Script Buffer");
             #ifdef JGMDBG
                  if(jgmDebug >= 3 ) {
                        fprintf(stderr, " A Dollar sign Script variable : %s\n", yytext            ) ;
                        fprintf(stderr, "script_var[%d]=%s\n", scr_vnum, parm.script_var[scr_vnum] ) ;
                  }
             #endif
          }    /* end scriptvar */

          /* Rules and Exclusive States for Francois Shapes
             Note we dont mind extra spaces everywhere; we can just pass them to Francois' code.
             We don't allow newlines though; we can't pass those to Francois' code and its too much
             complexity to skip over them.
             So in contrast to most of the other clauses in an input file
             enhanced shape{ must all be on one line up to the comma after the compass */

shape{wsnl}*[{]  { BEGIN(FD1) ;
                     memset(fd_cmd_buff, ' ' , CMDBUFFLEN );
                     fdbp = fmt_cmd_buff(fd_cmd_buff, fd_cmd ) ; /* /usr/local/bin/DealerV2/lib/fdp ' */
                     fdbp = fmt_cmd_buff(fdbp , "shape{ " ) ;
                     show_our_state( 3 , buffptr, "In Francois FD1. Still reading File Buff");
               }  /* end BEGIN FD1 */

<FD1>{FDIRVAR}          {              /* found a script var $n for the direction */
                          scrv_idx = yytext[1] - '0' ;
                          fdbp = fmt_cmd_buff( fdbp, parm.script_var[scrv_idx] ) ; // compass dir to cmd_buff
                          fdbp = fmt_cmd_buff( fdbp, ", " ) ;                     // followed by comma
                          BEGIN(FD2) ;
                        }
<FD1>{FCOMPASS}         {   /* found a word for the compass direction followed by comma. copy everything*/
                          fdbp = fmt_cmd_buff( fdbp, yytext ) ;
                          BEGIN(FD2) ;
                        }
<FD2>({FDIST}|{FPUNC})+ { /* found an FD shape spec or spec list that is NOT a script var */
                           fdbp = fmt_cmd_buff( fdbp , yytext ) ;
                         }
<FD2>{FDISTVAR}          { /* found an FD shape spec that IS a script var maybe has a trailing + sign or not */
                           scrv_idx = yytext[1] - '0' ;
                           fdbp = fmt_cmd_buff(fdbp, parm.script_var[scrv_idx] ); // fd_shape to cmd buff
                           c_ptr = strchr(&yytext[1] , (int)'+' );
                           if ( c_ptr ) { fdbp = fmt_cmd_buff(fdbp, " + " ) ; } // and a + if one followed the script var
                         }
<FD2>{FTERM}          {    BEGIN(FD3); /* reached end of Francois shape spec. Run cmd to create standard shape from it */
                           fdbp = fmt_cmd_buff(fdbp, " }'" ); // terminate shape. terminate opening quote.
                           /* Now run the popen cmd. Results into fd_shape_buff */
                           fd_status=parse_fd_shapes( &fd_shape_buff, fd_cmd_buff ) ;

                           /* Now we switch contexts and redirect Flex to the string with the std shapes in it */
                           /* Save current input buffer context, open new buffer based on a string, and make it the current one */

                           if ( state_stk[buffptr] == FILE_STATE ) { /* This should always be true */
                              yy_file_buff = YY_CURRENT_BUFFER;  /* the yybuff_stk will have this already */
                              #ifdef JGMDBG
                                 if (jgmDebug >= 4 ) {
                                    fprintf(stderr, "Francois saved TO curr_file_buff ptr=[%p]\n", (void *) yy_file_buff ) ;
                                    fprintf(stderr, "Flex:raw: setting yy_scan_string to[%s]\n", fd_shape_buff.fd_shape_str );
                                 }
                              #endif
                           }
                           yy_shape_buff = yy_scan_string( fd_shape_buff.fd_shape_str ) ;
                           yypush_buffer_state(yy_shape_buff) ;
                           state_stk[++buffptr] = FRANCOIS ;
                           yybuff_stk[buffptr]  = YY_CURRENT_BUFFER ; /* save handle to current (Francois) buffer */
                           show_our_state( 4 , buffptr, "In Francois TERM found state");
                              #ifdef JGMDBG
                                 if (jgmDebug >= 3 ) {
                                    fprintf(stderr, "FD3::Result of fdp process=%s\n", fd_shape_buff.fd_shape_str ) ;
                                 }
                              #endif
                           /* at this point flex has been redirected to read from the string buff. */
                           /* return(FRANCOIS); we could do this and make it a no-op ?   */
                           BEGIN(INITIAL) ;
                              /* now that we lex'ed the Francois shape, return to normal state to lex the results of fdp*/
                     }
                  /* end FDTERM When shape_str reaches end yywrap will return us to file buffer */

<FD1,FD2,FD3>(.|[\n])           { ; } /* ignore any chars that don't match Francois shapes in these states incl \n*/

\\n             { yylval.y_str = mystrcpy("\n"); return STRING; }  /* need this one for printes LF */
[ \t]           { ; }
\r*\n           { ; }                 /* do nothing if {nl} found. flex will track yylineno */
^\#.*\n         { ; }                 /* # in col1 is a comment. Do nothing. flex will track yylineno */
.               { return(yytext[0]); }   /* unknown char; handles stuff like , ; may trigger error in yyparse if unexpected */

%%

/* epilog section. More C-Code text just written to the ouput file goes here*/
/* This code is guarranteed to come AFTER the yylex() routine generated by Flex.
 * Often we can put a main here. If we define our own yywrap it should go here
 * because it cannot go before Flex's extern int yywrap() declaration.
 */
 int yywrap () {  /* NEW VERSION WITH STACKS THAT MIGHT NOT BE NEEDED */
   if (! YY_CURRENT_BUFFER ) { /* no valid buffer defined Must be EOJ */
       show_our_state( 3 , buffptr, "yywrap:: No Current Buffer. Quitting now ... ");
       #ifdef JGMDBG
         if (jgmDebug >= 3 ) {
            fprintf(stderr, "yywrap:: yylineno=[%d] Flex Exit to main now. No valid curr buffer\n", yylineno);
         }
       #endif
       //printf( "YYWRAP returning ALL DONE -- 1 \n");
       return 1 ; /* alldone == 1 */
   }
   if (state_stk[buffptr] == FILE_STATE ) {  /* End of Input File, means yyparse() is done */
            show_our_state( 4 , buffptr, "yywrap:: End of Input File Quitting now...");
       #ifdef JGMDBG
         if (jgmDebug >= 3 ) {
            fprintf(stderr, "yywrap:: yylineno=[%d] Flex Exit to main now. state_stk[%d]=%d, curr_buff_ptr=[%p]\n",
                                       yylineno, buffptr, state_stk[buffptr], (void *)YY_CURRENT_BUFFER);
         }
       #endif
       //printf( "YYWRAP returning ALL DONE -- 2 \n");
       return 1 ;      /* yylex is alldone. No more input */
   } /* end file state */

   /* EOF was not on Input File, but rather on a string variable  buffer */
   /* Reclaim RAM, pop state, and continue with Previous buffer */

   if (state_stk[buffptr] == PARM_STATE ) {
         yy_delete_buffer(yy_parm_buff);   /* could do this via the buffer stack also */
         yypop_buffer_state();
         yybuff_stk[buffptr]  = 0 ; /* erase handle to current buffer */
         state_stk[buffptr--] = 0 ;  /* pop our own state */
         yy_switch_to_buffer(yybuff_stk[buffptr] ) ;   /* switch back to the previous buffer yy_cur_buff? */
         show_our_state( 3 , buffptr, "yywrap::End of PARM; Context Restored via Popping");
         return 0 ; /* not all done yet */
   } /* end PARM_STATE*/

   else if (state_stk[buffptr] == FRANCOIS ) {
         yy_delete_buffer(yy_shape_buff);  /* could I say YY_CURRENT_BUFFER here instead of shape_buff ? */
         yypop_buffer_state();
         yybuff_stk[buffptr]  = 0 ; /* erase handle to current buffer */
         state_stk[buffptr--] = 0 ;  /* pop our own state */
         yy_switch_to_buffer(yybuff_stk[buffptr] ) ;   /* switch back to the previous buffer Should be File Buff*/
         show_our_state( 3 , buffptr, "yywrap::End of Francois; Context Restored via Popping");
         return 0 ; /* not all done yet */
   } /* end FRANCOIS */

   if( state_stk[buffptr] <= 0 ) {
        fprintf(stderr, "Can't happen in yywrap. Exiting Flex now .... \n") ;
        return 1 ; /* All Done */
   }
   /* To get here there must be a current buffer -- but how do we get here?  */
   show_our_state( 3 , buffptr, "End of yywrap UNKNOWN; ");
   return 0 ; /* not all done yet. */
} /* end yywrap */

