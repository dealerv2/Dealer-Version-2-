/* ---- OneBoard.c ----
 * Program to read a file of deals produced by Dealer and solve them for number of tricks. One Result at a time
 * Useful example of mixing Dealer code and defs with DDS code and defs
 * Date        Version     Comment
 * 2022/01/09   0.9a       Adapted from TablesPBN4.c
 * 2022/01/09   1.0        Adapted from OneBoard.c
 *                         Generate some random deal52 deals, convert them to DDS_BIN deals, and solve them one strain/leader
 *                         at a time. No input file, not much in the way of output.
 */
 #ifndef _GNU_SOURCE
 #define _GNU_SOURCE
 #endif
 #ifdef __cplusplus
  #define EXTERN_C extern "C"
#else
  #define EXTERN_C
#endif

#include <stdio.h>   /* Can't do without it */
#include <stdlib.h>  /* for srand48, mrand48 and friends */
#include <sys/random.h>   /* for getrandom() */
#include <string.h>  /* Supplies memcpy(), memset(), and the strlen() family of functions */

// from Bo's example and testing code
#include <time.h>      /* In case we want to time how quick this is */
#include <sys/time.h>  /* Needed for gettimeofday function */
#include <math.h>      /* Probably for calculating elapsed times. and atof */

// JGM required includes, not usually found in Bo's code.
#include <assert.h> /* For debugging. To turn off define the symbol NDEBUG on the cmd line? */
#include <errno.h>  /* The external errno variable and all the values it can take on */
#include <getopt.h> /* Supplies external optarg, opterr, optind, and getopt() function */
#include <libgen.h> /* Supplies the basename() function */
#include <unistd.h> /* Supplies EXIT_FAILURE, EXIT_SUCCESS -- POSIX */

#include "../include/dds_solve.h"
#include "../include/deal_dds.h"     //  defines some Dealer types and constants DDS might want

#if 1
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
// These next VUL definitions do not correspond to Dealer which is: 0,3,1,2
#define VUL_NONE 0
#define VUL_BOTH 1
#define VUL_NS   2
#define VUL_EW   3

#define DDS_HANDS 4
#define DDS_SUITS 4
#define DDS_STRAINS 5
#endif

int ddsDebug = 8;

/* ======================================== */
/* The above finishes the #include section. handy_jgm.h and handy_dds.h files should contain this
 */

/* Defines for program options, and initialization */

#define OPTSTR "c:n:o:s:D:R:S:h" /* what command line switches the program will recommend */
#define USAGE_FMT  "Usage_S0: %s [-c Compass list] [-n Number of Deals] [-o outputfile] -[s Strains list]\n" \
                   " [-D debug level] [-h] -R threads -S seed for RNG\n"

#define ERR_FOPEN_INPUT  "fopen(input, r)"
#define ERR_FOPEN_OUTPUT "fopen(output, w)"
#define ERR_FOPEN_LOG    "fopen(logfile, w)"
#define ERR_MAIN_LOOP    "ERROR MAIN LOOP BLEW UP"
#define DEFAULT_PROGNAME "TimeOne"
#define LOG_MSG_FMT "Solved %d total deals\n"
/* ------------------ */
#endif
// above ends the #if 1 statement that surrounds the #defines for constants

typedef  unsigned int       holding_k[DDS_HANDS][DDS_SUITS] ;

struct options_st {
  char          leaders[8] ;              // c: Compass the char string as entered on cmd line. NESW or subset
  int           compasslist[DDS_HANDS];   //       N,E,S,W
  int           nDeals ;                  // n: will solve this many deals x 20 for each number of threads
  FILE         *output;                   // o: default to NULL no output wanted
  char          ofilename[80] ;           //       Default to /dev/null
  char          strains[8] ;              // s: the char string as entered on cmd line. CDHSN or subset
  int           strainslist[DDS_STRAINS]; //       S,H,D,C,N
  int           Debug;                    // D: 0 no debug output. 9 verbose debug  output
  int           maxThreads ;              // R: Resources; max # of threads
  int           maxRamMB  ;               //       160 * MaxThreads
  long int      seed ;                    // S: Seed for RNG
// these next ones no longer on cmd line. Fixed
  int           target;                   // the mode for SolveBoard to use. -1 is find max number of tricks
  int           solutions;                // the number of solutions to return; there may be many. 1 is any one that works.


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
struct deal_st             dl, *dl_ptr;
//syntax     dl.trump,  dl.first,  dl.remainCards[Compass][Suit] dl.currentTrickSuit[3] dl.currentTrickRank[3]=0,
struct ddTableDeal         dds_deal, dbg_deal ;
//syntax     dds_deal.cards[H][S]

// Define some global vars to put the deals and results in.
// The order of definition of these next two matters a lot. ddTableResults must come first.

holding_k holding[4][4] ;
holding_k *hold_ptr = &holding[0][0];

char strains[] = "SHDCN";
char ntricks[] = "0123456789ABCD-" ; // 14 means this combo not asked for.
char compass[] = "nesw" ;
char cards[]   = "xx23456789TJQKA" ; // DDS coding; 2-14; Dealer coding is 0-12 ;

int solve_cnt = 0;
int solve_err = 0;

void  copy_holding( holding_k dst, holding_k src ) {
 int h, s ;
 for (h=0; h< DDS_HANDS ; h++ ) {
     for (s=0 ; s < DDS_SUITS ; s++ ) {
         dst[h][s] = src[h][s] ;
      }
   }
} /* end copy_holding */
void Init_DDSRES ( DDSRES_k *resp ) {
  int h , s ;
  resp->errcode = 0 ; // or should it be 1?
  resp->ddsmsg[0] = '\0'; ;
  resp->parScore_NS = 0;
  for (h = 0 ; h < DDS_HANDS ; h++) {
     for (s = 0 ; s < DDS_STRAINS ; s++ ) resp->tricks[h][s] = 14 ; // This strain not asked for.
  }
}

void  usage(         const char *progname,  int opt);
int   getopts(        options_k *options,   int argc, char *argv[] ) ;
int   do_main_loop(   options_k *opts_ptr  ) ;  // Where  the REAL work gets done; uses global vars
int   solve_set(       DDSRES_k *dds_Res_ptr, options_k  *opts_ptr, struct deal_st dl ) ;
int  set_strainslist(struct options_st *opts ) ;
int  set_compasslist(struct options_st *opts ) ;
void show_options ( struct options_st *opts , int v ) ;
/* The routines in DealingSubs.c */
EXTERN_C void dump_Deal(struct deal_st dl);
EXTERN_C struct ddTableDeal Deal52_to_DDSBIN( deal52_k d);
EXTERN_C int Shuffle ( deal52_k d) ;
EXTERN_C void freshpack( deal52_k d ) ;
EXTERN_C long int init_rand48( long int seed ) ;
EXTERN_C void dump_DDSdeal(struct ddTableDeal dl) ;
EXTERN_C void sr_deal_show( deal52_k dl ) ;
EXTERN_C void sortDeal( deal52_k dl ) ;

int main(int argc, char *argv[]) {
    options_k options = {
//                Leaders, c_list,  nDeals, ofile, ofilename, strains, s_list,
                   "NESW",{1,1,1,1}, 1000,  NULL, "/dev/null", "CDHSN", {1,1,1,1,1},
//               Debug, maxThrd, maxRam, seed, target, solns
                   2,      2,     320,     0,    -1,     1   };
    int rc, i;
    int maxRam , nThreads, totalSolutions ;
    fut_ptr = &fut1 ;
     for (i = 0 ; i < 3; i++) {
        dl.currentTrickSuit[i] = 0;
        dl.currentTrickRank[i] = 0;
    }
    Init_DDSRES(dds_Res_ptr);

    rc = getopts(&options, argc, argv);   // getopts should process all opts and open the specified files.
    if (options.Debug >=1 ) show_options(&options, 1 ) ; /* Debug = 0 should suppress all debug msgs */
    if (!rc) {
         errno = EINVAL;
         perror("Could not set options");
         return EXIT_FAILURE;
    }

    //          MaxRamMB  nThreads : for One board at a time nThreads has no effect
   nThreads = options.maxThreads ;
   maxRam = nThreads * 160 ;
   SetResources(maxRam,   options.maxThreads);
   init_rand48(options.seed) ;

  struct timeval tvstart, tvstop;
   gettimeofday (&tvstart, (void *) 0);
   printf("Version II Calling DO MAIN LOOP start seconds= [%ld]\n",(long int) tvstart.tv_sec ) ;
   totalSolutions = do_main_loop( &options );
   gettimeofday (&tvstop, (void *) 0);
    // The input has been read, and the results calculated and written.
   printf ("Done: %d Deals %d Oneboard solutions with Threads=%d   Time needed %8.3f sec\n",
            options.nDeals, totalSolutions, options.maxThreads,
            (tvstop.tv_sec + tvstop.tv_usec / 1000000.0 -
            (tvstart.tv_sec + tvstart.tv_usec / 1000000.0) ) );
   return EXIT_SUCCESS;
}
/* Where the REAL work gets done */
int   do_main_loop(   options_k *opts_ptr ) {
   #define MAXLINE 1023
   int ndl ;
   solve_cnt = 0 ;
   deal52_k curdeal ;
   freshpack(curdeal);
   fprintf (stderr, "Calculating...  0%% complete\r");
   for (ndl = 1 ; ndl <= opts_ptr->nDeals ; ndl++ ) {
      if ( (100 * ndl / opts_ptr->nDeals) > 100 * (ndl - 1) / opts_ptr->nDeals ) {
              fprintf (stderr, "Calculating... %2d%% complete\r",
               100 * ndl / opts_ptr->nDeals );
      }
      Shuffle(curdeal);
      sortDeal(curdeal);
      //sr_deal_show(curdeal);

      dds_deal = Deal52_to_DDSBIN(curdeal) ; // Useful Routine. Convert Dealer deal to a binary DDS deal
      // dump_DDSdeal(dds_deal);

      copy_holding(dl.remainCards, dds_deal.cards ) ;
      // copy_holding(dbg_deal.cards, dl.remainCards ) ;
      solve_cnt += solve_set( dds_Res_ptr, opts_ptr, dl) ;
      // fprintf(stderr, "solve set returns. solve_cnt = %d\n", solve_cnt );
      // fprintf(opts_ptr->output, "A couple of solutions: H=North, s=spades, tricks=%d West NT tricks=%d\n",
      //         dds_Res_ptr->tricks[0][3],dds_Res_ptr->tricks[3][4]);
      Init_DDSRES(dds_Res_ptr);
   } // end for nDeals
   return solve_cnt ;
} // end do_main_loop()

int solve_set(  DDSRES_k *dds_Res_ptr, options_k  *opts_ptr, struct deal_st dl ) {
   // struct deal_st dl is a DDS structure. The remaincards[H][S} array has been filled
   int res_rc, s, h, so;  // s is DDS strains; so is Dealer Strains
   char line[128] ; // for when DDS converts its error codes to text
   int thrIDX = 0;
   struct futureTricks fut ;
   int solved;
   solved = 0 ;

   for (s = 0; s < DDS_STRAINS ; s++ ) {
      if (opts_ptr->strainslist[s] == 0 ) continue;
      dl.trump = s ;
      so  = ( (3-s) >= 0 ) ? (3 - s ) : 4 ;  //so = suit output aka dealer suit clubs=0 etc.
    for (h = 0 ; h < DDS_HANDS ; h++ ) {
       if ( opts_ptr->compasslist[h] == 0 ) continue ;
       dl.first = h ;
    if (opts_ptr->Debug > 4) {fprintf(stderr, "Calling SolveBoard for Strain=[%d], Leader=[%d]\n",dl.trump, dl.first ) ; }
         // ####################################################################
         res_rc = SolveBoard(dl, opts_ptr->target, opts_ptr->solutions, 0, &fut, thrIDX);
         //thrIDX++ ;
         //if (thrIDX >= opts_ptr->maxThreads ) thrIDX = 0 ;

         if (res_rc != RETURN_NO_FAULT) {
            solve_err++ ;
            ErrorMessage(res_rc, line);  // convert the numeric err msg to text in the char array line[]
            fprintf(stderr, "DDS error@ count=[%d] Msg= %s\n", solve_err, line);
         }
         else {
             solved++;
             dds_Res_ptr->tricks[h][so] = fut.score[0] ;
             if (opts_ptr->Debug >= 4 ) {
                 fprintf(stderr, "Solve Count=%d, Returns Tricks=[%d] for Seat[%d] Strain[%d]\n",
                                    solve_cnt,        fut.score[0],     dl.first,    dl.trump    ) ;
            }
            if (opts_ptr->output ) { // user wants to see the details, not just the time taken
                 fprintf(opts_ptr->output, "Solve Count=%d, Returns Tricks=[%d] for Seat[%d] Strain[%d]\n",
                        solve_cnt+solved,   fut.score[0],  dl.first,    dl.trump    ) ;
            }

         }
       } /* end for h<DDS_HANDS */
   } /* end for s < DDS_STRAINS */
   // done one set of 20 results; return
  return (solved ) ;
} /* end solve_set */

/*
 * The output file format looks the same with the tricks added on the end marked by a colon
 * N:K84.AJT843.5.Q82 QJ2.K5.AJ92.KJ54 T653.Q96.KQ74.76 A97.72.T863.AT93:9ABCD,58339,56789,DCBA2
 *
 * The tricks are given as a 4 sets of five SHDCN one for each Compass NESW
 */

int getopts(options_k *options, int argc, char *argv[] ) {
    int opt;
    opterr = 1;  /* Initialize opterr to 0, which disables getopt from emiting a ?. */
    options->output = stdout ;
// OPTSTR "c:n:o:s:D:R:S:h"
    while ((opt = getopt(argc, argv, OPTSTR)) != EOF) {
       // fprintf(stderr, "Get Opts Begins for opt %c:%s \n", opt, optarg);

       switch(opt) {
            case 'c': /* the c option is followed by an argument - the compass directions to solve for */
              strncpy( options->leaders, optarg,  4 ); /* on cmd line will be NESW | nesw or a subset */
              options->leaders[5] = '\0' ;
              set_compasslist(options) ;
              break;
            case 'n':
              options->nDeals = atoi( optarg );
              break;

            case 'o': /* the o option is followed by an argument - the filename to open - which is put in optarg by getopt */
              if (!(options->output = fopen(optarg, "w")) ){
                 perror(ERR_FOPEN_OUTPUT);
                 exit(EXIT_FAILURE);
                 /* NOTREACHED */
              }
              strncpy(outfilename, optarg, 80);
              strncpy(options->ofilename, optarg, 80 ) ;
              break;

            case 's': /* the s option is followed by an argument - the strains to solve for */
              strncpy( options->strains, optarg,  5 ); /* on cmd line will be CDHSN | cdhsn or a subset */
              options->strains[5] = '\0' ;
              set_strainslist(options) ;
              break;
            case 'D':
              options->Debug = atoi( optarg );
              break;

            case 'R' :
               options->maxThreads = atoi( optarg );
               options->maxRamMB = 160 * options->maxThreads;
               break ;

            case 'S':
              options->seed = (long int) atoi( optarg );
              break;

            case 'h':
           default:
              usage(basename(argv[0]), opt);
              /* NOTREACHED */
              break;
       }  // end case opt
    } // end while
       return 1 ;
} // end getopts()

void show_options ( struct options_st *opts , int v ) {
    fprintf(stderr, "Showing Options with Verbosity = %d\n",v);
    fprintf(stderr, "\t %s=[%s]\n", "c:compass list", opts->leaders ) ;
    fprintf(stderr, "\t %s=[%d]\n", "n:number of deals", opts->nDeals ) ;
    fprintf(stderr, "\t %s=[%s]\n", "o:output filename", opts->ofilename ) ;
    fprintf(stderr, "\t %s=[%s]\n", "s:srains list", opts->strains ) ;
    fprintf(stderr, "\t %s=[%d]\n", "D:Debug Level ", opts->Debug ) ;
    fprintf(stderr, "\t %s=[%d:%d]\n", "R:Number of Threads:RamMB", opts->maxThreads, opts->maxRamMB ) ;
    fprintf(stderr, "\t %s=[%ld]\n","S:Seed for RNG ", opts->seed ) ;

   return ;
} /* end show opts */
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

