/* ---- OneBoard.c ----
 * Program to read a file of deals produced by Dealer and solve them for number of tricks. One Result at a time
 * Date        Version     Comment
 * 2022/01/09   0.9a       Adapted from TablesPBN4.c
 */
 #ifndef _GNU_SOURCE
 #define _GNU_SOURCE
 #endif

#include <stdlib.h>
#include <string.h>  /* Supplies memcpy(), memset(), and the strlen() family of functions */

// from Bo's example and testing code
#include <time.h>      /* In case we want to time how quick this is */
#include <math.h>      /* Probably for calculating elapsed times. Not needed for dds itself ? */

// JGM required includes, not usually found in Bo's code.
#include <assert.h> /* For debugging. To turn off define the symbol NDEBUG on the cmd line? */
#include <errno.h>  /* The external errno variable and all the values it can take on */
#include <getopt.h> /* Supplies external optarg, opterr, optind, and getopt() function */
#include <libgen.h> /* Supplies the basename() function */
#include <unistd.h> /* Supplies EXIT_FAILURE, EXIT_SUCCESS -- POSIX */

#include "../include/dll_solve.h"
// DDS is quite fussy about PBN input format. Fmt is not exactly =  printoneline in dealer
// even blank lines cause it to return all zero results for that  chunk.
#define PBNLINE 69

// Some of these next ones are defined in C++ headers but not defined in C header files
// ---- dds definitions from dds_jgm.h -------
// NULL is for  pointers, NIL is for chars in strings ZERO for int return codes
#ifndef NULL
   #define NULL 0
#endif
#ifndef NIL
   #define NIL '\0'
#endif
#ifndef ZERO
   #define ZERO 0
#endif
#ifndef FALSE
   #define FALSE 0
#endif
#ifndef TRUE
   #define TRUE 1
#endif
// this next #if 1 surrounds a bunch of #defines for various constants they do not define structs
#if 1
// These next suit definitions are DDS standard, but they do not correspond to Dealer or DOP standard.
#define SP 0
#define HE 1
#define DI 2
#define CL 3
#define NT 4

#define SPADES   0
#define HEARTS   1
#define DIAMONDS 2
#define CLUBS    3
#define NOTRUMP  4

#define NORTH    0
#define EAST     1
#define SOUTH    2
#define WEST     3
// These next DDS VUL definitions do NOT correspond to Dealer which are: 0=None,3=both,1=NS,2=EW
#define VUL_NONE 0
#define VUL_BOTH 1
#define VUL_NS   2
#define VUL_EW   3

#define DDS_HANDS 4
#define DDS_SUITS 4
#define DDS_STRAINS 5

/* ======================================== */
/* The above finishes the #include section. handy_jgm.h and handy_dds.h files should contain this
 */

/* Defines for program options, and initialization */

#define OPTSTR "i:o:l:c:s:D:h" /* what command line switches the program will recommend */
#define USAGE_FMT  "Usage_S0: %s [-i inputfile] [-o outputfile] [-l logfile]  -c(ompass[NESW] -s(train[CDHSN]  [-D debug level] [-h] \n"

#define ERR_FOPEN_INPUT  "fopen(input, r)"
#define ERR_FOPEN_OUTPUT "fopen(output, w)"
#define ERR_FOPEN_LOG    "fopen(logfile, w)"
#define ERR_MAIN_LOOP    "ERROR MAIN LOOP BLEW UP"
#define DEFAULT_PROGNAME "SolveOne"
#define LOG_MSG_FMT "Solved %d total deals, read %d lines from infile\n"
/* ------------------ */
#endif
// above ends the #if 1 statement that surrounds the #defines for constants

typedef  unsigned       int uint32_t ;  /* Define my own since i cant find it in std hdrs */
typedef  unsigned short int uint16_t ;
typedef  unsigned char      card_t   ;
typedef  unsigned char      deal52[52] ;
typedef  unsigned int       holding_k[DDS_HANDS][DDS_SUITS] ;


struct options_st {
  FILE         *input;
  FILE         *output;
  FILE         *logfile;
  int           Debug;                 // 0 no debug output. 9 verbose debug  output
  int           target;                // the mode for SolveBoard to use. -1 is find max number of tricks
  int           solutions;             // the number of solutions to return; there may be many. 1 is any one that works.
  char          strains[8] ;           // the char string as entered on cmd line. CDHSN or subset
  int           strainslist[DDS_STRAINS] ;    // S,H,D,C,N
  char          leaders[8] ;     // the char string as entered on cmd line. NESW or subset
  int           compasslist[DDS_HANDS];  // N,E,S,W
  int           Threads ;
} ;
typedef struct options_st options_k; /* the _t ending is reserved by ISO. I use _k for 'kind' */

struct Dealer_Results_st {   // Major link from DDS to Dealer. Results in form convenient to Dealer
    int parScore_NS ;
    // int parScore_NS[4] ;     // One for each Vulnerability in case user asks.
    int tricks[4][5] ;          // [hands=n,e,s,w][strains = c,d,h,s,n]
    int errcode ; // 0 = ok, -1 = failure
    char ddsmsg[40];  // in case DDS fails get some of the errmsg. Also used for contract strings
} ;
typedef struct Dealer_Results_st DDSRES_k ;
DDSRES_k        dds_Res_bin ;  /* 20 tricks results, a par score, a text field and an error code */
DDSRES_k       *dds_Res_ptr = &dds_Res_bin;
char infilename[80]  = "stdin";
char outfilename[80] = "stdout";
char logfilename[80] = "stderr";

/* Global vars */
struct options_st  options ;
struct options_st *opts_ptr = &options;

struct futureTricks        fut1, fut2, fut3, *fut_ptr;
struct deal_st              OneDeal, dl, *dl_ptr;
//syntax dl.trump dl.first dl.remainCards[Compass][Suit] dl.currentTrickSuit[3] dl.currentTrickRank[3]=0,

// Define some global vars to put the deals and results in.
// The order of definition of these next two matters a lot. ddTableResults must come first.

struct ddTableDealPBN      pbn_deal;   // one set of 52 cards
// syntax dpbn.deals[boardnum].cards[80]

unsigned int holding[4][4] ;
unsigned int *hold_ptr = &holding[0][0];

char strains[] = "SHDCN";
char ntricks[] = "0123456789ABCD-" ; // 14 means this combo not asked for.
char compass[] = "nesw" ;
char cards[]   = "xx23456789TJQKA" ; // DDS coding; 2-14; Dealer coding is 0-12 ;

int lineno = 0 ;
int solve_cnt = 0;
int solve_err = 0;
int skip_cnt = 0;
int out_cnt = 0;
extern int PBNbuff_to_Holding(char const *dealBuff,
                            unsigned int remainCards[DDS_HANDS][DDS_SUITS]);
extern void dump_Deal(struct deal_st dl);

void Init_DDSRES ( DDSRES_k *resp ) {
  int h , s ;
  resp->errcode = 0 ; // or should it be 1?
  resp->ddsmsg[0] = '\0'; ;
  resp->parScore_NS = 0;
  for (h = 0 ; h < DDS_HANDS ; h++) {
     for (s = 0 ; s < DDS_STRAINS ; s++ ) resp->tricks[h][s] = 14 ; // This strain not asked for.
  }
}
// now that all vars and types specified, give fwd function prototypes.
void  usage(         const char *progname,  int opt);
int   getopts(        options_k *options,   int argc, char *argv[] ) ;
int   do_main_loop(   options_k *opts_ptr  ) ;  // Where  the REAL work gets done; uses global vars
int   show_inp_hand(  int lineno, char *kards ) ;
void  show_results(   char *pbnptr, DDSRES_k *ddsRes ) ;
void  output_result(FILE *outfile,  char *pbnptr, DDSRES_k *ddsRes ) ;
int   solve_and_save(options_k  *opts_ptr, char *buff, struct deal_st dl ) ; // uses globals lineno and solve_cnt
int   log_progress(   FILE      *logfile ) ;                // uses globals lineno and solve_cnt
int   validPBNline(char *pbn_buff, options_k *opts_ptr) ;

void  show_DealerResults (char *dpbn_ptr, DDSRES_k *res_ptr  ) ;
int  set_strainslist(struct options_st *opts ) ;
int  set_compasslist(struct options_st *opts ) ;
int main(int argc, char *argv[]) {
//                        pbn-deals  results  logfile     Debug  target  solutions   strains incl           seats incl         Threads
    options_k options = { stdin,     stdout,  stderr,       0    ,   -1,       1    , "CDHSN", {1,1,1,1,1}, "NESW", {1,1,1,1}, 2 }  ;


    int rc, i;
    int maxRam , nThreads ;
    fut_ptr = &fut1 ;
     for (i = 0 ; i < 3; i++) {
        dl.currentTrickSuit[i] = 0;
        dl.currentTrickRank[i] = 0;
    }
    Init_DDSRES(dds_Res_ptr);

    rc = getopts(&options, argc, argv);   // getopts should process all opts and open the specified files.
    if (!rc) {
         errno = EINVAL;
         perror("Could not set options");
         return EXIT_FAILURE;
    }

   if ( (options.input == NULL) || (options.output == NULL) || (options.logfile == NULL) ) { // getopts opens the files..
     errno = ENOENT;
     perror( "Open failed on one of the input, output, or log files");
     usage( "OneBoard", 1 );
     return EXIT_FAILURE;
   }
   if ( options.Debug != 0 ) {
      fprintf(options.output, "OUTFILE OK\n");
      fprintf(options.logfile, "LOGIFLE OK, Debug=%d, target=[%d], solutions=[%d]\n",
               options.Debug, options.target, options.solutions ) ;
   }
   //          MaxRamMB  nThreads
   nThreads = options.Threads ;
   maxRam = nThreads * 160 ;
    SetResources(maxRam,        nThreads);      // not sure if his code can handle auto calc of threads for linux.

   // getopts will have opened any files
   // now we just have to read the input, fill in the deal_st struct, and call the DDS SolveBoard function
   // We do it one line at a time
   fprintf( options.logfile , "OneBoard Test. Infile=[%s], Outfile=[%s], Logfile=[%s] \n",
        infilename, outfilename,  logfilename ) ;
   rc = do_main_loop( &options );

    // The input has been read, and the results calculated and written.

   fprintf(options.logfile, "Read %d lines, solved [%d] deals wrote [%d] lines to  [%s] Skipped %d Input lines\n ",
                                 lineno,      solve_cnt,      out_cnt,        outfilename,    skip_cnt);

   fprintf(options.logfile, "***** NORMAL END OF RUN *****\n");
   fclose(options.input);
   fclose(options.output);
   fclose(options.logfile);

   return EXIT_SUCCESS;
}
/* Where the REAL work gets done */
int   do_main_loop(   options_k *opts_ptr ) {
   #define MAXLINE 1023
   int max_n = MAXLINE;
   char inp_line[1024];
   int rc ;
   char *lf_pos ;
// The buffer is null-terminated and includes the newline character, if one was found.
   while ( fgets(inp_line, max_n, opts_ptr->input) ) {
      lf_pos = strchrnul(inp_line, '\n') ; // if no \n lf_pos is end-of-string
      *lf_pos = '\0';
      lineno++;
      if (opts_ptr->Debug > 7 ) { fprintf(stderr,"lineno[%d] %s\n",lineno, inp_line) ; }
      rc = validPBNline(inp_line, opts_ptr);
      if ( rc == FALSE ) {
          skip_cnt++;
          if (opts_ptr->Debug >= 5 ) { fprintf(stderr,"validPBN Errnum= %d; Skipping line[%d]\n",rc, lineno) ; }
          continue ;
      } /* end invalid inp_line rc */
      // ========================================
      rc = PBNbuff_to_Holding(inp_line,  dl.remainCards );  // all three of these defined ok. dl is global

      if (opts_ptr->Debug >= 5 ) { dump_Deal(dl) ; }
      solve_and_save( opts_ptr, inp_line, dl) ;  // uses global vars lineno and solve_cnt; pass the whole dl struct in
      Init_DDSRES(dds_Res_ptr);
      rc = 1 ;
   } // end while fgets input line.
   return solve_cnt ;
} // end do_main_loop

int solve_and_save(  options_k  *opts_ptr, char *pbnptr, struct deal_st dl ) {
   // uses global vars lineno and solve_cnt dds_Res_ptr
   int res_rc, s, h, so;  // s is DDS strains; so is Dealer Strains
   char line[128] ; // for when DDS converts its error codes to text
   static int thrIDX = 0;
   struct futureTricks fut ;

   for (s = 0; s < DDS_STRAINS ; s++ ) {
      if (opts_ptr->strainslist[s] == 0 ) continue;
      dl.trump = s ;
      so  = ( (3-s) >= 0 ) ? (3 - s ) : 4 ;
    for (h = 0 ; h < DDS_HANDS ; h++ ) {
       if ( opts_ptr->compasslist[h] == 0 ) continue ;
       dl.first = h ;
    if (opts_ptr->Debug > 4) {fprintf(stderr, "Calling SolveBoard [%d] for Strain=[%d], Leader=[%d]\n",
                                                                 lineno, dl.trump, dl.first ) ; }
         // ####################################################################
         res_rc = SolveBoard(dl, opts_ptr->target, opts_ptr->solutions, 0, &fut, thrIDX);

         if (res_rc != RETURN_NO_FAULT) {
            solve_err++ ;
            ErrorMessage(res_rc, line);  // convert the numeric err msg to text in the char array line[]
            fprintf(stderr, "DDS error@[%d],count=[%d] Msg= %s\n", lineno, solve_err, line);
         }
         else {
             solve_cnt++;
             dds_Res_ptr->tricks[h][so] = fut.score[0] ;
             if (opts_ptr->Debug >= 4 ) {
                 fprintf(stderr, "Deal[%d] Solve Count=%d, Returns Tricks=[%d] for Seat[%d] Strain[%d]\n",
                                    lineno, solve_cnt,        fut.score[0],     dl.first,    dl.trump    ) ;
            }
         }
       } /* end for h<DDS_HANDS */
   } /* end for s < DDS_STRAINS */
   // Done one set Hands:Strains one at a time. Ouput result
   output_result( opts_ptr->output, pbnptr, dds_Res_ptr );
   out_cnt++ ;
   log_progress( opts_ptr->logfile ) ;
   return (solve_cnt) ;
} // end solve_and_save

int log_progress( FILE *logfile ) {          // uses global vars lineno and solve_cnt
   fprintf(logfile, "Total Lines from infile[%d.], Solved %d deals,  Wrote[%d] lines, skipped[%d], SolveErrs=%d\n",
                                        lineno ,   solve_cnt,         out_cnt,         skip_cnt ,    solve_err   ) ;
   return 1;
} // end log_progress
/*
 * The output file format looks the same with the tricks added on the end marked by a colon
 * N:K84.AJT843.5.Q82 QJ2.K5.AJ92.KJ54 T653.Q96.KQ74.76 A97.72.T863.AT93:9ABCD,58339,56789,DCBA2
 *
 * The tricks are given as a 4 sets of five SHDCN one for each Compass NESW
 */

 /* we have made 20 calls to solve one board, one for each combo of Leader and Strain.
  * we have saved the 20 results in dds_Res.tricks[h][s]
  * now we will put then at the end of the pbndeal string and send to a file (or stdout)
  */
void output_result( FILE *output, char *pbn_ptr, DDSRES_k *dds_Res_ptr ) {
   // simple format suitable for later processing;
   // N:[pbn fmt hand ]:<--NT--><--Sp--><--He--><--Di--><--Cl--> in order NESW
   int s ; // dealer strain 0=C, 4=N
   int k ; // kompass 0=N, 3=W
   int t ; // number of tricks 0-13 as a single char 0-9A-D if strain not asked for then X
   char *lf_pos ;
      lf_pos = strchrnul(pbn_ptr, '\n') ; // if no \n lf_pos is end-of-string
      *lf_pos = NIL ;         // replace LF or '\0' with '\0'
      fprintf(output,"%s:", pbn_ptr ); // no linefeed. tricks coming next
         for (s = 4 ; s >= 0 ; s-- ) {          // strains NSHDC
              for ( k = 0 ; k < 4 ; k++) {     // compass directions nesw
            t = dds_Res_ptr->tricks[k][s] ;
            fprintf(output, "%c", ntricks[t]) ; /*convert an int 0..14 into a char 0123456789ABCD- */
         } // end for compass
         if ( s > 0 ) { fprintf(output, ","); }
      } // end for strains
      fprintf(output, "\n");
} // output_results

void show_DealerResults (char *dpbn_ptr, DDSRES_k *res_ptr  ) {
   // N:[pbn fmt hand ] n:ntricks[5],etricks[5],stricks[5],wtricks[5]
   int s ; // strain 0=S, 4=N
   int k ; // kompass 0=N, 3=W
   int t ; // number of tricks 0-9ABCD
   char *lf_pos ;

   // syntax:  res_all.results[board_num].resTable[strain][kompass]
   // syntax:  dpbn.deals[boardnum].cards[]
   lf_pos = strchrnul(dpbn_ptr, '\n') ; // if no \n lf_pos is end-of-string
   *lf_pos = NIL ;         // replace LF or '\0' with '\0'
   printf("[%s] n:", dpbn_ptr ); // no linefeed. tricks coming next
      for ( k = 0 ; k < 4 ; k++) {     // compass directions nesw
         for (s = 0 ; s < 5 ; s++ ) {  // strains SHDCN
            t = dds_Res_ptr->tricks[k][s] ;
            printf("%c", ntricks[t]) ;
         } // end for strains
         if ( k < 3 ) { printf(","); }
      } // end for compass
      printf("\n");
} // end show_results

int show_inp_hand(int linenum , char *kards ) {
   printf("Line[%d]:= %s\n", linenum,  kards ) ;
   return 1 ;
}

int getopts(options_k *options, int argc, char *argv[] ) {
    int opt;
    opterr = 1;  /* Initialize opterr to 0, which disables getopt from emiting a ?. */

    while ((opt = getopt(argc, argv, OPTSTR)) != EOF) {
       switch(opt) {
           case 'i':  /* the i option is followed by an argument - the filename to open - which is put in optarg by getopt */
              if (!(options->input = fopen(optarg, "r")) ){
                 perror(ERR_FOPEN_INPUT);  /* on stderr print a string followed by an error message corresponding to errno */
                 exit(EXIT_FAILURE);
                 /* NOTREACHED */ /* NOTREACHED is a lint directive that I like. */
              }
              strncpy(infilename, optarg, 80);
              break;

           case 'o': /* the o option is followed by an argument - the filename to open - which is put in optarg by getopt */
              if (!(options->output = fopen(optarg, "w")) ){
                 perror(ERR_FOPEN_OUTPUT);
                 exit(EXIT_FAILURE);
                 /* NOTREACHED */
              }
              strncpy(outfilename, optarg, 80);
              break;

           case 'l': /* the l option is followed by an argument - the filename to open - which is put in optarg by getopt */
              if (!(options->logfile = fopen(optarg, "w")) ){
                 perror(ERR_FOPEN_LOG);
                 exit(EXIT_FAILURE);
                 /* NOTREACHED */
              }
              strncpy(logfilename, optarg, 80);
              break;

            case 's': /* the s option is followed by an argument - the strains to solve for */
              strncpy( options->strains, optarg,  5 ); /* on cmd line will be CDHSN | cdhsn or a subset */
              options->strains[5] = '\0' ;
              set_strainslist(options) ;
              break;

            case 'c': /* the c option is followed by an argument - the compass directions to solve for */
              strncpy( options->leaders, optarg,  4 ); /* on cmd line will be NESW | nesw or a subset */
              options->leaders[5] = '\0' ;
              set_compasslist(options) ;
              break;

           case 'D':
              options->Debug = atoi( optarg );
              break;

            case 'R' :
               options->Threads = atoi( optarg );

            case 'h':
           default:
              usage(basename(argv[0]), opt);
              /* NOTREACHED */
              break;
       }  // end case opt
    } // end while
       return 1 ;
} // end getopts()

int set_strainslist ( struct options_st *opts ) {
   char fch , *chptr;
   int rc;
   int i ;
   for (i= 0 ; i < DDS_STRAINS ; i++ ) { opts->strainslist[i]=0; }
   chptr = opts->strains;
   while( NIL != (fch = *chptr++ ) ) {
      switch(fch) {
      case 'C' :
      case 'c' :      opts->strainslist[CLUBS]    = 1 ;     break ;
      case 'D' :
      case 'd' :      opts->strainslist[DIAMONDS] = 1 ;     break ;
      case 'H' :
      case 'h' :      opts->strainslist[HEARTS]   = 1 ;     break ;
      case 'S' :
      case 's' :      opts->strainslist[SPADES]   = 1 ;     break ;
      case 'N' :
      case 'n' :      opts->strainslist[NOTRUMP]  = 1 ;     break ;
      default:
         fprintf( stderr, "%c is Invalid! Strain characters must be one of  CDHSN\n", fch);
         rc = -1 ;
         break ;
    } // end switch
  } // end while
    if (opts->Debug > 3 ) {
        fprintf(stderr, "These strains[%s] give this list:[ ",opts->strains);
        for (i= 0 ; i < DDS_STRAINS ; i++ ) { fprintf(stderr, "%d, ",opts->strainslist[i]); }
        fprintf(stderr, "]\n");
    }
      return rc;
} // end set_strainslist

int set_compasslist ( struct options_st *opts ) {
   char fch , *chptr;
   int rc = 1;
   int i ;
   for (i= 0 ; i < DDS_HANDS ; i++ ) { opts->compasslist[i]=0; }
   chptr = opts->leaders;
   while( NIL != (fch = *chptr++ ) ) {
      switch(fch) {
      case 'N' :
      case 'n' :      opts->compasslist[NORTH]  = 1 ;     break ;
      case 'E' :
      case 'e' :      opts->compasslist[EAST]   = 1 ;     break ;
      case 'S' :
      case 's' :      opts->compasslist[SOUTH]  = 1 ;     break ;
      case 'W' :
      case 'w' :      opts->compasslist[WEST]   = 1 ;     break ;
      default:
         fprintf( stderr, "%c is Invalid! Compass characters must be one of  NESW\n", fch);
         rc = -1 ;
         break ;
    } // end switch
  } // end while
      if (opts->Debug > 3 ) {
        fprintf(stderr, "These Leaders[%s] give this list:[ ",opts->leaders);
        for (i= 0 ; i < DDS_HANDS ; i++ ) { fprintf(stderr, "%d, ",opts->compasslist[i]); }
        fprintf(stderr, "]\n");
    }
    return rc ;
} // set_tcompasslist

void  usage(         const char *progname, int opt) {
   if (opt == '?' ) { fprintf(stderr,"ERROR!! %c is an invalid option\n", optopt); }
   fprintf(stderr, USAGE_FMT, progname ? progname : DEFAULT_PROGNAME );
   exit(EXIT_FAILURE);
   /* NOTREACHED */
}

int validPBNline(char *pbn_ptr, options_k *opts_ptr ) {
    size_t bufflen;
  // FALSE defined as 0, TRUE defined as 1
    bufflen = strlen(pbn_ptr) ;
    if (opts_ptr->Debug > 6 ) {
            fprintf(stderr, "validPBN::lino[%d] len[%ld]:%s\n",lineno, bufflen, pbn_ptr);
    }
    if (*pbn_ptr != 'N' && *pbn_ptr != 'n' && *pbn_ptr != 'S' && *pbn_ptr != 's' &&
        *pbn_ptr != 'E' && *pbn_ptr != 'e' && *pbn_ptr != 'W' && *pbn_ptr != 'w' ) return FALSE ;
    if (*(pbn_ptr+1) != ' ' && *(pbn_ptr+1) != ':' ) return FALSE ; // not strictly necessary? but safer.

    if (bufflen < PBNLINE || bufflen > PBNLINE+1) return FALSE ;

    return TRUE ;
} /* end validPBNline */

