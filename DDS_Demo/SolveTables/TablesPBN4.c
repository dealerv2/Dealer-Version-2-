/* ---- TablesPBN4.c ----
 * Simple driver program to read a file of deals produced by Dealer and solve them for number of tricks.
 * Date        Version     Comment
 * 2021/12/11   0.9        adapted from TablesPBN3.cpp -- want to se if I can compile it with C not C++
 *                          I can compile to .o in C, but I can't link to library using gcc; must use g++
 * 2021/12/12   1.0         Begin incorporating PAR results into the pgm. This may be what dealer user wants anyway
 */
 #define _GNU_SOURCE
 #ifdef UNUSED
   #undef UNUSED
   #define UNUSED(x) ( (void)(1 ? 0 : (x++)) )
 #endif
#include <stdlib.h>
#include <string.h>  /* Supplies memcpy(), memset(), and the strlen() family of functions */

// from Bo's example and testing code
#include <time.h>      /* In case we want to time how quick this is */
#include <math.h>      /* Probably for calculating elapsed times. Not needed for dds itself ? */

// JGM required includes, not usually found in Bo's code.
#include <assert.h> /* For debugging. To turn off define the symbol NDEBUG on the cmd line? */
#include <errno.h>  /* Defines the external errno variable and all the values it can take on */
#include <getopt.h> /* Supplies external optarg, opterr, optind, and getopt() function */
#include <libgen.h> /* Supplies the basename() function */
#include <unistd.h> /* Supplies EXIT_FAILURE, EXIT_SUCCESS -- POSIX */

#include "../include/dll4.h"
// DDS is quite fussy about PBN input format. Not much error checking in converting to internal DDS binary fmt.
// even blank lines cause it to return all zero results for that  chunk.
#define PBNLINE 69
#define MYCHUNKSIZ  10

// Some of these next ones are defined in C++ headers but not defined in C header files
// ---- dds definitions from dds_jgm.h -------
// NULL is for testing pointers, NIL is for testing chars in strings ZERO for testing int return codes
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
// These next VUL definitions do not correspond to Dealer which is: 0,3,1,2
#define VUL_NONE 0
#define VUL_BOTH 1
#define VUL_NS   2
#define VUL_EW   3


/* ------------------ */
/* The above finishes the #include section. I have included in this source file the handy_jgm.h and handy_dds.h files
 * in an effort to try to track down these funny incomplete definition errors.
 */

/* Defines for program options, and initialization */
/* Use all capital letters when naming a #define  to distinguish them from variables */
#define OPTSTR "i:o:l:p:f:v:D:h" /* what command line switches the program will recommend */
#define USAGE_FMT  "Usage_V4: %s [-i inputfile] [-o outputfile] [-l logfile] [-p ParResults file] \n" \
                    "[-f trump_filter SHDCN ] [-v vulnerability for par results] [-D debug level] [-h]\n"
#define ERR_FOPEN_INPUT  "fopen(input, r)"
#define ERR_FOPEN_OUTPUT "fopen(output, w)"
#define ERR_FOPEN_LOG    "fopen(logfile, w)"
#define ERR_FOPEN_PAR    "fopen(parResfile, w)"
#define ERR_MAIN_LOOP    "ERROR MAIN LOOP BLEW UP"
#define DEFAULT_PROGNAME "TablesPBN"
#define DEFAULT_PARFILE  "/tmp/TablesPBN.par"
#define LOG_MSG_FMT "Solved %d total deals, read %d lines from infile\n"
/* ------------------ */
#endif
// above ends the #if 1 statement that surrounds the #defines for constants

typedef  unsigned int uint32_t ;

/* Note that making a typedef struct has plus and minus. */
typedef struct options_k {   /* the _t ending is reserved for future standard types. I use _k for 'kind' */
  FILE         *input;
  FILE         *output;
  FILE         *logfile;
  FILE         *parfile;
  int           parmode;
  char          filter[8];
  int           Debug;        // 0 no debug output. 9 verbose debug  output
} options_k;
char infilename[80]  = "stdin";
char outfilename[80] = "stdout";
char logfilename[80] = "stderr";
char parfilename[80] = DEFAULT_PARFILE ;
char trumpfilter[16] = "None Given";

struct ddTableResults      res_20  ;   // one set of 20 results


// Define some global vars to put the deals and results in.
// The order of definition of these next two matters a lot. ddTableResults must come first.
struct allParResults       res_par;    // an array[MAXNOOFTABLES] of structs of type parResults
struct ddTableDealPBN      pbn_deal;   // one set of 52 cards
struct ddTableDealsPBN     dpbn ;      // short name because frequently used. 200 entries; up to 40 deals.
// syntax dpbn.deals[boardnum].cards[80]
struct ddTablesRes         res_all ;   // array of ddTableResults max of 200 (5*40)
// syntax res_all.results[board_num].resTable[strain][kompass]

struct DDSInfo info ;                  // just in case I can figure out how to use this.

char defParfile[]="/tmp/DDSparResults.txt";
char strains[] = "SHDCN";
char ntricks[] = "0123456789ABCD" ;
char compass[] = "nesw" ;
char cards[]   = "xx23456789TJQKA" ;
int trumpFilter[DDS_STRAINS] = {0,0,0,0,0}; // filter none; solve all strains. S,H,D,C,N
int lineno = 0 ;
int solve_cnt = 0;
int par_cnt = 0;
int skip_cnt = 0;

// now that all vars and types specified, give fwd function prototypes.
void  usage(         const char *progname,  int opt);
int   getopts(        options_k *options,   int argc, char *argv[] ) ;
int   do_main_loop(   options_k *opts_ptr,  int chunk_sz ) ;  // Where  the REAL work gets done
void  show_results(   struct     ddTableDealsPBN *dpbn_ptr, struct ddTablesRes *res_all_ptr ) ;
int   show_inp_hand(  int        lineno, int boardnum, char *cards ) ;
int   output_results( FILE      *outfile, struct ddTableDealsPBN *dpbn_ptr, struct ddTablesRes *res_all_ptr );
int   set_trumpFilter(char      *filter ) ;
int   solve_and_save(options_k  *opts_ptr, int boardnum ) ; // uses globals lineno and solve_cnt
int   log_progress(   FILE      *logfile ) ;                // uses globals lineno and solve_cnt
void  PrintPar(       FILE      *parfile, int brdnum,   struct parResults *par);
int   output_par_results( FILE *parfile,  int max_bnum, struct allParResults *res_all_ptr );
int   validPBNline(char *pbn_buff, options_k *opts_ptr) ;

int main(int argc, char *argv[]) {
//                        pbn-deals  results  logfile   paroutput  parmode    filter  Debug
    options_k options = { stdin,     stdout,  stderr,   NULL  ,      -1,        "",     0    };
    int rc ;
    int chunk_sz = MYCHUNKSIZ ; // use say 10 for testing, MAXNOOFTABLES for production

    rc = getopts(&options, argc, argv);   // getopts should process all opts and open the specified files.
    if (!rc) {
         errno = EINVAL;
         perror("Could not set options");
         return EXIT_FAILURE;
    }

   if ( (options.input == NULL) || (options.output == NULL) || (options.logfile == NULL) ) { // getopts opens the files..
     errno = ENOENT;
     perror( "Open failed on one of the input, output, or log files");
     usage( "TablesPBN", 1 );
     return EXIT_FAILURE;
   }
   if ( options.parmode != -1 && options.parfile == NULL ) { // user wants Par, but either no filename given or open failed
      if (!( options.parfile = fopen(defParfile, "w") )) {   // try to open the default par report file
        errno = ENOENT;
        perror("Could Not open Default Par file");
        return EXIT_FAILURE;
      }
    } /* end if parmode != -1 */
    if (options.parfile != NULL && options.parmode < 0 ) { /* user set a parfile but not a parmode. Assume mode 0 (Nobody Vul) */
        options.parmode = 0;
    }

   if ( options.Debug != 0 ) {
      fprintf(options.output, "OUTFILE OK\n");
      fprintf(options.logfile, "LOGIFLE OK, Debug=%d, parmode=%d, filter=%s \n",
               options.Debug, options.parmode, options.filter ) ;
   }
    /* SetMaxThreads(6);  */    // not sure if his code can handle auto calc of threads for linux.
    SetResources(1440,9); // For my machine 9 threads is best. 9*160MB = 1440 for Ram.

   // getopts will have opened any files and set any trumpFilter values.
   // now we just have to read the input, fill in the dpbn array, and call the DDS solve routine.
   // We can't do more than 40 lines of input at once, so we need to do it in chunks
   fprintf( options.logfile , "PBN4 Test. Infile=[%s], Outfile=[%s], Parfile=[%s], Logfile=[%s] TrumpFilter=[%s]\n",
         infilename, outfilename, parfilename, logfilename, trumpfilter ) ;
   rc = do_main_loop( &options, chunk_sz );

    // The input has been read, and the results calculated and written.

   fprintf(options.logfile, "Read %d lines, Wrote %d solved deals to [%s] wrote %d Par Results to [%s] Skipped %d Input lines\n ",
                                                        lineno, solve_cnt,outfilename, par_cnt, parfilename, skip_cnt);

   fprintf(options.logfile, "***** NORMAL END OF RUN *****\n");
   fclose(options.input);
   fclose(options.output);
   fclose(options.logfile);
   if (options.parfile != NULL ) fclose(options.parfile) ;

   return EXIT_SUCCESS;
}
/* Where the REAL work gets done */
int   do_main_loop(   options_k *opts_ptr,  int chunk_sz ) {
   #define MAXLINE 1023
   int max_n = MAXLINE;
   char inp_line[1024];
   int rc ;
   int boardnum = 0 ;
   char *lf_pos ;
// The buffer is null-terminated and includes the newline character, if one was found.
   while ( fgets(inp_line, max_n, opts_ptr->input) ) {
      lf_pos = strchrnul(inp_line, '\n') ; // if no \n lf_pos is end-of-string
      *lf_pos = '\0';
      lineno++;
      if (opts_ptr->Debug >= 5 ) { fprintf(stderr,"lineno[%d] %s\n",lineno, inp_line) ; }
      rc = validPBNline(inp_line, opts_ptr);
      if ( rc == FALSE ) {
          skip_cnt++;
          if (opts_ptr->Debug >= 5 ) { fprintf(stderr,"RC = %d; Skipping line[%d]\n",rc, lineno) ; }
          continue ;
      } /* end invalid inp_line rc */
      strcpy( dpbn.deals[boardnum].cards, inp_line );
      if (opts_ptr->Debug >= 5 ) {
         show_inp_hand(lineno, boardnum, dpbn.deals[boardnum].cards) ;
      }
      boardnum++ ;
      if ( boardnum >= chunk_sz ) {
         solve_and_save( opts_ptr,  boardnum ) ;  // uses global vars lineno and solve_cnt
         boardnum = 0 ;
      }
   } // end while fgets input line.
  if ( boardnum > 0 ) {  /* the file did not contain an even multiple of chunk_size deals. Some left to do */
     solve_and_save( opts_ptr,  boardnum ) ; // uses global vars lineno and solve_cnt
  }
   return solve_cnt ;
} // end do_main_loop

int solve_and_save(  options_k  *opts_ptr, int boardnum ) { // uses global vars lineno and solve_cnt
   int res_rc;
   int mode = 0;    // Vulnerability for Par calculations. -1 No Par calcs, 0=None Vul, 1=Both, 2= NS only 3= EW only.
   char line[128] ; // for when DDS converts its error codes to text
   dpbn.noOfTables = boardnum ;
   res_rc = CalcAllTablesPBN(&dpbn, mode, trumpFilter, &res_all, &res_par);

   if (res_rc != RETURN_NO_FAULT) {
      ErrorMessage(res_rc, line);  // convert the numeric err msg to text in the char array line[]
      fprintf(stderr, "DDS error: %s\n", line);
    }

   if (opts_ptr->Debug >= 4 ) {
      show_results(&dpbn, &res_all) ;
   }
   output_results( opts_ptr->output, &dpbn, &res_all );
   if (opts_ptr->parmode != -1 ) {
          output_par_results(opts_ptr->parfile, dpbn.noOfTables, &res_par) ;
   }
   log_progress( opts_ptr->logfile ) ;
   return (solve_cnt) ;
} // end solve_and_save

int log_progress( FILE *logfile ) {          // uses global vars lineno and solve_cnt
   fprintf(logfile, "Total Lines from infile[%d.], Solved %d total deals, wrote %d Par Results\n", lineno , solve_cnt, par_cnt ) ;
   return 1;
} // end log_progress
/*
 * The output file format looks the same with the tricks added on the end marked by a colon
 * N:K84.AJT843.5.Q82 QJ2.K5.AJ92.KJ54 T653.Q96.KQ74.76 A97.72.T863.AT93:9ABCD,58339,56789,DCBA2
 *
 * The tricks are given as a 4 sets of five SHDCN one for each Compass NESW
 */
int output_results( FILE *output, struct ddTableDealsPBN *dpbn_ptr, struct ddTablesRes *res_all_ptr ) {
   // simple format suitable for later processing;
   // N:[pbn fmt hand ]:ntricks[5],etricks[5],stricks[5],wtricks[5]
   int bnum ;
   int max_bnum ;
   int s ; // strain 0=S, 4=N
   int k ; // kompass 0=N, 3=W
   int t ; // number of tricks 0-9ABCD
   char *lf_pos ;

   // syntax:  res_all.results[board_num].resTable[strain][kompass]
   // syntax:  dpbn.deals[boardnum].cards[]
   max_bnum = dpbn_ptr->noOfTables ;
   for (bnum = 0; bnum < max_bnum ; bnum++) {
      lf_pos = strchrnul(dpbn_ptr->deals[bnum].cards, '\n') ; // if no \n lf_pos is end-of-string
      *lf_pos = NIL ;         // replace LF or '\0' with '\0'
      fprintf(output,"%s:", dpbn_ptr->deals[bnum].cards ); // no linefeed. tricks coming next
      for ( k = 0 ; k < 4 ; k++) {     // compass directions nesw
         for (s = 0 ; s < 5 ; s++ ) {  // strains SHDCN
            t = res_all_ptr->results[bnum].resTable[s][k] ;
            fprintf(output, "%c", ntricks[t]) ; /*convert an int 0..13 into a char 0123456789ABCD */
         } // end for strains
         if ( k < 3 ) { fprintf(output, ","); }
      } // end for compass
      fprintf(output, "\n");
   } // end for bnum
   solve_cnt += max_bnum ;       // update global variable
   return max_bnum;
} // output_results
void PrintPar(FILE *parfile, int brdnum, struct parResults *par) {
  fprintf(parfile,   "%5d. NS: %s:: list:%s\n", brdnum, par->parScore[0], par->parContractsString[0]);
  fprintf(parfile, "       EW: %s:: list:%s\n",         par->parScore[1], par->parContractsString[1]);
}
int output_par_results( FILE *parfile, int max_bnum, struct allParResults *res_all_ptr ) {
   int bnum ;
   struct parResults *onePar ;
   for (bnum = 0; bnum < max_bnum ; bnum++) {
      onePar = &res_all_ptr->presults[bnum];
      PrintPar(parfile, ++par_cnt, onePar) ;
   }
   return par_cnt;
} // output_par_results

void show_results(struct ddTableDealsPBN *dpbn_ptr, struct ddTablesRes *res_all_ptr ) {
   // N:[pbn fmt hand ] n:ntricks[5],etricks[5],stricks[5],wtricks[5]
   int bnum ;
   int max_bnum ;
   int s ; // strain 0=S, 4=N
   int k ; // kompass 0=N, 3=W
   int t ; // number of tricks 0-9ABCD
   char *lf_pos ;

   // syntax:  res_all.results[board_num].resTable[strain][kompass]
   // syntax:  dpbn.deals[boardnum].cards[]
   max_bnum = dpbn_ptr->noOfTables ;
   for (bnum = 0; bnum < max_bnum ; bnum++) {
      lf_pos = strchrnul(dpbn_ptr->deals[bnum].cards, '\n') ; // if no \n lf_pos is end-of-string
      *lf_pos = NIL ;         // replace LF or '\0' with '\0'
      printf("[%s] n:", dpbn_ptr->deals[bnum].cards ); // no linefeed. tricks coming next
      for ( k = 0 ; k < 4 ; k++) {     // compass directions nesw
         for (s = 0 ; s < 5 ; s++ ) {  // strains SHDCN
            t = res_all_ptr->results[bnum].resTable[s][k] ;
            printf("%c", ntricks[t]) ;
         } // end for strains
         if ( k < 3 ) { printf(","); }
      } // end for compass
      printf("\n");
   } // end for bnum
} // end show_results

int show_inp_hand(int linenum , int boardnum , char *kards ) {
   printf("Line[%d]:BD#[%d]= %s\n", linenum, boardnum, kards ) ;
   return 1 ;
}

int getopts(options_k *options, int argc, char *argv[] ) {
    int opt;
    opterr = 0;  /* Initialize opterr to 0, which disables getopt from emiting a ?. */

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

          case 'p' : /* User is specifying the file in which to write the options */
             if (!(options->parfile = fopen(optarg, "w")) ){
                 perror(ERR_FOPEN_PAR);
                 exit(EXIT_FAILURE);
                 /* NOTREACHED */
             }
             strncpy(parfilename, optarg, 80);
             if (options->parmode == -1 ) { options->parmode=1 ; } /* choose both VUL if user has not specified */
             break;

            case 'f': /* the f option is followed by an argument - the strains NOT to solve for */
              strncpy( options->filter, optarg,  5 ); /* on cmd line will be CDHSN | cdhsn or a subset */
              options->filter[5] = '\0' ;
              strncpy(trumpfilter, optarg, 8);
              set_trumpFilter(options->filter) ;
              break;

          case 'v':
              options->parmode = atoi( optarg );
              break;

           case 'D':
              options->Debug = atoi( optarg );
              break;

            case 'h':
           default:
              usage(basename(argv[0]), opt);
              /* NOTREACHED */
              break;
       }  // end case opt
       set_trumpFilter(options->filter) ;   // setup the default trump filter.
       if ( options->parmode != -1 && options->parfile == NULL ) {
            if (!(options->parfile = fopen(DEFAULT_PARFILE, "w")) ){
                 perror(ERR_FOPEN_PAR);
                 exit(EXIT_FAILURE);
                 /* NOTREACHED */
             }
        }

   } // end while
       return 1 ;
} // end getopts()

int set_trumpFilter ( char *filter ) {
   char fch ;
   int rc = 1 ;
   while( NIL != (fch = *filter++ ) ) {
      switch(fch) {
      case 'C' :
      case 'c' :
         trumpFilter[CLUBS] = 1 ;
         break ;
      case 'D' :
      case 'd' :
         trumpFilter[DIAMONDS] = 1 ;
         break ;
      case 'H' :
      case 'h' :
         trumpFilter[HEARTS] = 1 ;
         break ;
      case 'S' :
      case 's' :
         trumpFilter[SPADES] = 1 ;
         break ;
      case 'N' :
      case 'n' :
         trumpFilter[NOTRUMP] = 1 ;
         break ;
      default:
         fprintf( stderr, "%c is Invalid! Filter characters must be one of  CDHSN\n", fch);
         rc = -1 ;
         break ;
    } // end switch
  } // end for
      return( rc ) ;
} // set_trumpFilter

void  usage(         const char *progname, int opt) {
   if (opt == '?' ) { fprintf(stderr,"ERROR!! %c is an invalid option\n", optopt); }
   fprintf(stderr, USAGE_FMT, progname?progname:DEFAULT_PROGNAME);
   exit(EXIT_FAILURE);
   /* NOTREACHED */
}

int validPBNline(char *pbn_ptr, options_k *opts_ptr ) {
    size_t bufflen;
  // FALSE defined as 0, TRUE defined as 1
    bufflen = strlen(pbn_ptr) ;
    if (opts_ptr->Debug > 6 ) {
            fprintf(stderr, "VALID::[%c][%c][%ld]Checking:%s\n",*pbn_ptr, *(pbn_ptr+1), bufflen, pbn_ptr);
    }
    if (*pbn_ptr != 'N' && *pbn_ptr != 'n' && *pbn_ptr != 'S' && *pbn_ptr != 's' &&
        *pbn_ptr != 'E' && *pbn_ptr != 'e' && *pbn_ptr != 'W' && *pbn_ptr != 'w' ) return FALSE ;
    if (*(pbn_ptr+1) != ' ' && *(pbn_ptr+1) != ':' ) return FALSE ; // not strictly necessary? but safer.

    if (bufflen < PBNLINE || bufflen > PBNLINE+1) return FALSE ;

    return TRUE ;
} /* end validPBNline */
