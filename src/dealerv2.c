// dealerv2.c  version 2.5.x by JGM 2023-Jan-07
// Version 2.0.1  remove MSC stuff ... & Francois Stuff ... & GIB Library file
//             2.0.2  Add LTC, Title, Printside, and other minor enhancements
//             2.0.3  Add DDS calculation of Tricks and Par.
//             2.0.4  Added -D switch on cmd line. Testing why DDS is so slow.
//             2.0.5  Complete redo of DDS to use binary not PBN deals. restructured dirs. added more cmd line switches
//             2.0.6  Introduced code to run DOP perl pgm and save results in dealer struct
//             2.0.7  Export code added. -X option processing.
//             2.0.8  CSVRPT code added. -C option processing.
//             2.0.9  seed in input file code added.
//             2.1.0  Francois Dellacherie shapes added to Dealer input file. Use external program to expand
// 2022/09/19  2.2.0  Fixed some bugs in redefining the altcount and pointcount arrays. and in use of dotnums.
// 2022/10/01  2.4.0  Added bktfreq functionality so can do frequency plots of dotnums.
// 2022/10/18  2.5.0  Fixed some bugs in C4.c; begin user_eval functionality
// 2022/11/07  2.5.0  usereval functionality coded; testing to do.
// 2023/01/07  2.5.2a Merged in changes from V4 to fix predeal; Shuffle, dealcards_subs.c and globals, etc.
//
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
#include "../include/std_hdrs.h"  /* all the stdio.h stdlib.h string.h assert.h etc. */

#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/dealprotos.h"
#include "../include/dealexterns.h"   /* was dealglobals but testing if I can compile globals separately and link it. */
#include "../include/pointcount.h"
#include "../src/UsageMsg.h"
#include "../include/dealdebug_subs.h"
#include "../include/dbgprt_macros.h"

/* next file for some subs that main uses to setup runtime and debug statements */
#include "mainsubs.c"

/* This next one is in case we want to use 64 bit ints to hold the distribution bits.
 * To allow for 64 shape statements instead of 'only' 32 shape statements.
 * then we can define MAXDISTR as 8*sizeof(long int) or thereabouts
 */
#ifndef MAXDISTR
 #define MAXDISTR 8*sizeof(int)
#endif

int main (int argc, char **argv) {
#ifdef JGMDBG
    jgmDebug = 1 ; // Assume we want some debug output. User can always set to zero from cmd line ..
#endif
    extern int yyparse  (void) ;
    void  SetResources(  int maxMemoryMB,  int maxThreads); /* for DDS */
    /* opts collects all cmd line parms in one place. Vars from previous versions of Dealer kept also so some duplication */
    struct options_st *opts ;
    struct param_st   *scr_varp ; /* pointer to script variables struct */
    int i ;
    int keephand ;

    scr_varp = &parm ;  /* parm is a global scr_varp is a pointer to script variable*/
    memset( &parm, 0 , sizeof(parm) ) ;   /* filling with zeros should both terminate the 'strings' and set len to 0 */

    opts = &options ;         /* struct should have been init to all zeroes or null strings in dealglobals.c */

    /* The non opt versions of the above will have been compile time init in dealglobals.c */
    /* None of the following are set in the input file and so should be replaced by the options.xxxx variable */
    errflg = 0;
    progressmeter = 0;
    swapping = 0;
    swapindex = 0;
    verbose = 1;
    quiet = 0 ;
    uppercase = 0 ;
    input_file = '\0' ;
    /* set these next 3 so that yyparse/flex will fill them in if reqd */
    maxdealer = -1;
    maxvuln = -1;
    dds_dealnum = -1;

    /* These globals cannot be a compile time init, since the user may re-direct stdin and stdout when calling dealer */
  fexp = stdout ; // FILE *fexp Will be opened by getopts if there is a -X option
  fcsv = stdout ; // FILE *fcsv Will be opened by getopts if there is a -C option

  struct timeval tvstart, tvstop;
  gettimeofday (&tvstart, (void *) 0);

  initdistr ();    /* create the 4D distribution array for use by the shape statement */

 /* process cmd line options. Save them for later after parsing done */
 /* Note that get_options returns immediately after it has found a -V or -h switch
  * so put any other options such as -D or -T that you want to affect the display, BEFORE
  * the said switch. e.g. dealerv2 -D3 -T "Test Run" -M2 -C w:/tmp/MyDataFile.csv -0 12 - 1 14 -V
  * This will show the options struct contents and the script vars before the version info and exiting.
  */
  get_options( argc, argv, opts) ;

   #ifdef JGMDBG
        if(jgmDebug > 0 ) {  fprintf(stderr, "JGMDBG DEFINED= %d in main version %s \n",JGMDBG, VERSION ); }
        #ifdef YYDEBUG
             if(jgmDebug > 0 ) {  fprintf(stderr, "YYDEBUG DEFINED yydebug== %d BISON DBG Active.  \n",yydebug ); }
        #else
            if(jgmDebug > 0 ) {  fprintf(stderr, "YYDEBUG NOT Defined.  BISON DBG IN_Active.  \n" ); }
        #endif
    #endif

  /* Even if compiled without JGMDBG defined, we still give user some minimal Debug info if he asks for it */
  extern void show_options     ( struct options_st *opts , int v ) ;
  extern void show_script_vars ( struct param_st   *p    , int v ) ;
  if(jgmDebug > 0) {  show_options(opts, 1) ; }
  if(jgmDebug > 0) {  show_script_vars( scr_varp, 1 ) ; }

  if (3 == options.options_error || 4 == options.options_error  ) { /* Print Version Info(3) or Help/Usage(4) and exit */
      return 0 ;
  }

  if (argc - optind > 2 || errflg) {
    fprintf (stderr, "Usage: %s %s\n", argv[0], UsageMessage );
    exit (-1);
  }
  if (optind < argc && freopen (input_file = argv[optind], "r", stdin) == NULL) {
    perror (argv[optind]);
    exit (-1);
  }
  newpack(fullpack);   /* fill fullpack with cards in order from North SA downto West C2 */
  memset(stacked_pack, NO_CARD, 52) ;  /* stacked pack used for preDeals */
  memset(curdeal, NO_CARD, 52 ) ;
  stacked_size = 0 ;
  memcpy(small_pack, fullpack, 52 ) ;  /* Shuffle works from small_pack not fullpack */
  small_size = 52 ;

  /*
   * build the list of conditions to evaluate and the list of actions to do
   * ===============> YYPARSE HERE <===============
   */
  JGMDPRT(3, "Calling yyparse \n");

  yyparse ();

   /* V4.5 ensure the two tables are in sync yyparse may have changed one but not the other*/
   for (i=0 ; i< 13 ; i++ ) { points[i] = tblPointcount[idxHcp][i] ; }

#ifdef JGMDBG   /* print several results from the parsing phase */
  if (jgmDebug >= 3) {
    fprintf(stderr, " After yyparse:: maxproduce=%d, maxgenerate=%d, maxdealer=%d, maxvuln=%d, Opener=%c, Title=%s\n",
                     maxproduce,    maxgenerate,    maxdealer,      maxvuln,   opc_opener, title );
    fprintf(stderr, "Showing yyparse generated Decision tree, varlist, actionlist  \n");
    fprintf(stderr, "Decision Tree starts at %p with type %d \n", (void *)decisiontree, decisiontree->tr_type);
    /* Expression List is only used by the printes action. Prob Not necessary to show it. */
    showdecisiontree(decisiontree);
    showvarlist(vars);  fprintf(stderr, "\nVARLIST DONE \n");
    showactionlist(actionlist); fprintf(stderr, "\nACTION List DONE \n");
    if (jgmDebug >= 4) {
        //showdistrbits(distrbitmaps) ;
        //fprintf(stderr, "\nDistr Bit Maps DONE \n");
        showAltCounts() ;
        fprintf (stderr, "Show ALT Counts Done \n");
        fprintf(stderr, "Post Parsing:: stacked_size=%d, small_size=%d, Fullpack,  Stacked pack, and Small_pack  \n",
                              stacked_size, small_size);
    }
    fprintf(stderr, "\n-------MAIN PARSING DONE @ %s :: %d ----------------\n",__FILE__, __LINE__);
  } /* end if jgmDebug */
#endif


  /* ----------------- Parsing of User Specs done. Decision Tree built. Action list built. -------------------*/
  /* Now initialize the rest of the program, taking into account the command line parms
   * Some of which may over ride the input file
   */

  initprogram(opts);  /* here we will over-ride yyparse if reqd from cmd line and do other stuff. */

    /*
     * Initialize the Deck with predealt cards from the stacked_pack, and leftover cards from small_pack
     * Wait till now because program opts may over-ride the parser
     */
  setup_deal();

   /*
   * Using glibc standard lib routines rand48 and srand48.
   * If no seed provided, use function init_rand48 which uses kernel as seed source
   */
  if (jgmDebug >= 3 ) { JGMDPRT(3,"Initializing RNG. Seed_provided=%ld, seed=%ld\n",seed_provided,seed); }
  if (!seed_provided) {
    seed = SRANDOM(0) ; /* use init_rand48() to init the RNG with a seed that the kernel provides */
  }
  else {
    seed = SRANDOM(seed) ; /* use init_rand48() to init the RNG with the users seed */
  }
  JGMDPRT(3,"RNG initialized with Seed val=%ld\n",seed );
   /* If user has set TABLEMODE (-M 2) but neglected to set Threads (-R n), then we use the TblModeDefault value */
  if ( (opts->dds_mode == 2 ) && (opts->nThreads < 2) ) {
     opts->nThreads = TblModeThreads ;
     nThreads = TblModeThreads ;
     opts->maxRamMB = 160 * TblModeThreads ;
     MaxRamMB = opts->maxRamMB ;
  } /* else dds_mode is 1 OR user has spec'd both Mode and Threads values */

  SetResources(opts->maxRamMB, opts->nThreads) ; /* 160MB/Thread max; 9,12,16 all equal and 25% faster than 6 Threads for Mode2*/
  if (maxgenerate == 0) maxgenerate = 10000000;
  if (maxproduce == 0)  maxproduce = ((actionlist == &defaultaction) || will_print) ? 40 : maxgenerate;
  JGMDPRT(2, "Maxgenerate=%d, Maxproduce=%d, DDS Threads=%d, UserServerReqd=%d\n",
               maxgenerate, maxproduce, opts->nThreads, userserver_reqd );
   /*
    * Start the user server if there was a usereval statement in the input file
    */
    if (userserver_reqd != 0 )  {
       userserver_pid = setup_userserver( server_path ) ;
       if (userserver_pid == -1 ) {
          perror(" Creating the server process returns failed PID. Aborting... ");
          assert(0) ;
       }
       JGMDPRT(3,"UserServer daemon started with pid=%d\n",userserver_pid);
    } /* user server daemon started */

  /* Walk the action list initializing any actions that need it */
  setup_action();

 if (srvDebug > 0 ) {
      // sleep(1) ;
      usleep(5000); /* 5 msec Give the child time to tell us his logfile/errmsg file name. before we start our own output */
      printf("\f") ; /* issue a form feed to start the Dealer output */
      fsync(1);  /* clear out stdout before starting real info */
 }

 /* Initialize the seat fields in handstat array for later use by the Server */
 for (i=0 ; i < 4 ; i++ ) { hs[i].hs_seat = i ; }

 /* ----------- Begin the Main Loop ------------*/
  JGMDPRT(4,"^^^^^^ Begin Main Loop ^^^^^^\n") ;

 if (progressmeter) { fprintf (stderr, "Calculating...  0%% complete\r"); }// \r CR not \n since want same line ..
 for (ngen = 1, nprod = 0; ngen <= maxgenerate && nprod < maxproduce; ngen++) { /* start ngen at 1; simplifies counting */
      treedepth = 0;
      JGMDPRT(8,"In Main Generating Hands: ngen=%d Calling deal_cards\n",ngen );
      deal_cards(curdeal) ; /* NEW 2023-01-06 -- Shuffle, Swap, or Predeal then Shuffle as required */
      #ifdef JGMDBG
        if (jgmDebug >= 9) {  sr_deal_show(curdeal); }
        if (jgmDebug >= 8 ) {
            struct handstat *hsp = hs;
            JGMDPRT(8, " Calling Analyze for ngen=%d \n", ngen);
            JGMDPRT(8, "hs_size=%lx, handstat[0]=>%p, handstat[1]=>%p,handstat[2]=>%p, handstat[3]=>%p\n",
               sizeof(struct handstat), (void *)hsp, (void *)(hsp+1), (void *)(hsp+2), (void *)(hsp+3) );
            }
      #endif

      analyze (curdeal, hs);  // Collect and save all info that will be needed by eval_tree() aka interesting() */

      JGMDPRT(7, " Calling Interesting for ngen=%d ", ngen);

      keephand = interesting() ;  /* will showtree if in DBG mode */
      JGMDPRT(7, " Interesting aka evaltree Returns %d\n", keephand);
      showtree = 0 ;              /*in debug mode only need to show the tree once; its the same for all hands */
      if (keephand) {             /* evaltree returns TRUE for the condition user specified */
                        JGMDPRT(9,"Interesting returns true Calling Action() now.");
          action();                        /* Do action list */
          nprod++;
          if (progressmeter) {
            if ((100 * nprod / maxproduce) > 100 * (nprod - 1) / maxproduce)
              fprintf (stderr, "Calculating... %2d%% complete\r",
               100 * nprod / maxproduce);
          } /* end progress meter */
      }   /* end keephand */
   #ifdef JGMDBG
      else { if ( jgmDebug >= 9) { fprintf(stderr, "Generated Hand %d is NOT interesting .. skipping actions \n",ngen) ; }
      }
   #endif
      JGMDPRT(7,"----------Done with ngen=%d ---------\n",ngen);
}    /* end for ngen */
   if ( ngen >= maxgenerate ) { ngen = maxgenerate ; } /* need this for end of run stats ... for loop leaves it at max+1 or one over */
   else if (ngen >= 2 ) {ngen-- ; }
    /* finished generating deals, either maxgenerate or maxproduce was reached */
  if (progressmeter) { fprintf (stderr, "                                      \r"); }
  gettimeofday (&tvstop, (void *) 0);

  cleanup_action();                /* This will do all the end of run actions such as: Average, Frequency, Frequency2D */

  if (verbose) {
    if (strlen(title) > 0 ) { printf("\n%s\n",title); }
    printf ("Generated %d hands\n", ngen);
    printf ("Produced  %d hands\n", nprod);
    printf ("Initial random seed %lu\n", seed);
    printf ("Time needed %8.3f sec%s",
             (tvstop.tv_sec + tvstop.tv_usec / 1000000.0 -
             (tvstart.tv_sec + tvstart.tv_usec / 1000000.0)), crlf);
    if (jgmDebug >= 2 ) { /* print some additional info for user even if JGMDBG Code not included */
      fprintf(stdout, "Tot Calls to DDS =%6d, DDS Solve  Calls=%6d\n", dbg_dds_res_calls,  dbg_dds_lib_calls );
      fprintf(stdout, "Tot Calls to GIB =%6d, GIB Solve  Calls=%6d\n", dbg_dd_calls, dbg_tdd_calls );
      fprintf(stdout, "Tot Calls to OPC =%6d, OPC Calc   Calls=%6d\n", dbg_opc_calls, dbg_opc_cmd_calls );
      fprintf(stdout, "Tot Ask Query    =%6d, UserServer Calls=%6d\n", dbg_userserver_askquery, dbg_userserver_extcalls );

    } /* end if jgmDebug */
  } /* end if verbose */

   /*
    * If there was a user server started, then
    * terminate the process, unmap the shared region, unlink the semaphores and send quit request to Server
    */
  if (userserver_pid > 0 ) {
      JGMDPRT(4,"Calling cleanup_userserver for pid=%d \n",userserver_pid ) ;
      cleanup_userserver( userserver_pid ) ;
      JGMDPRT(4,"Cleanup All Done. Normal EOJ for Server and Dealer \n");
  }

  return 0;
} /* end main */


