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
// 2023/03/07  2.5.3  Fixes to printcompass, and documentation
// 2023/03/20  2.6.0  Added Library mode, to use Richard Pavlicek's database of 10 Million plus solved deals.
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
     void finalize_options ( struct options_st *opts ) ;
     void setup_runtime    ( struct options_st *opts ) ;

#define FATAL_OPTS_ERR -10
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

    /* initialization functions from the file dealinit_subs.c and others. Used only in main so not in protos.h*/
    void  SetResources(  int maxMemoryMB,  int maxThreads); /* for DDS */
    void init_globals ( struct options_st *opts ) ;
    void finalize_options ( struct options_st *opt_ptr) ;
    void init_runtime(struct options_st *opts) ;
    void init_cards() ;


    /* opts collects all cmd line parms in one place. Vars from previous versions of Dealer kept also so some duplication */
    struct options_st *opts ;
    struct param_st   *scr_varp ; /* pointer to script variables struct */

    int keephand ;

    struct timeval tvstart, tvstop;
    gettimeofday (&tvstart, (void *) 0); /* Start clock before setup done. Is this fair? */

    scr_varp = &parm ;        /* parm is a global scr_varp is a pointer to script variable*/
    opts = &options ;         /* struct should have been init to all zeroes or null strings in dealglobals.c */
    init_globals(opts) ;      /* give globals not set at compile time sane values before we start */

 /* process cmd line options. Save them for later after parsing done */
 /* Note that get_options returns immediately after it has found a -V or -h switch
  * so put any other options such as -D or -T that you want to affect the display, BEFORE
  * the said switch. e.g. dealerv2 -D3 -T "Test Run" -M2 -C w:/tmp/MyDataFile.csv -0 12 - 1 14 -V
  * This will show the options struct contents and the script vars before the version info and exiting.
  */

  /* get options from cmd line before anything else in case we just want help or Version */
  /* Also can detect errors such as missing files before we do a bunch of un-necessary work */
  errflg = get_options( argc, argv, opts) ;

   #ifdef JGMDBG
        JGMDPRT(1, "JGMDBG DEFINED= %d in main version %s \n",JGMDBG, VERSION );
        #ifdef YYDEBUG
            JGMDPRT(1, "YYDEBUG DEFINED yydebug== %d BISON DBG Active.  \n",yydebug );
        #else
            JGMDPRT(1, "YYDEBUG NOT Defined.  BISON DBG IN_Active.  \n" );
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
  if (errflg <= FATAL_OPTS_ERR ) {
     fprintf(stderr, "FATAL Error In GetOpts[%d]. Ending Program \n",errflg );
     exit (-1) ;
   }

  if (argc - optind > 2 || errflg) {
    fprintf (stderr, "Usage: %s %s\n", argv[0], UsageMessage );
    exit (-1);
  }
  if (optind < argc && freopen (input_file = argv[optind], "r", stdin) == NULL) {
    perror (argv[optind]);
    exit (-1);
  }

  /* init the stuff that yyparse or flex code might need */
  initdistr ();    /* create the 4D distribution array for use by the shape statement */
  init_cards();    /* setup full pack and NO_CARD in the stacked_pack and small_pack */

  /*
   * build the list of conditions to evaluate and the list of actions to do
   * ===============> YYPARSE HERE <===============
   */
  JGMDPRT(3, "Calling yyparse \n");

  yyparse ();

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


  /* ----------- Parsing of User Specs done. Decision Tree built. Action list built. cmd_line opts saved ---------*/

     finalize_options ( opts ) ;   /* resolve diffs between yyparse and cmd line. alloc runtime RAM. set RNG and DDS parms. start server etc. */
     init_runtime     ( opts ) ;

 /* ----------- Begin the Main Loop ------------*/
  JGMDPRT(2,"^^^^^^ Begin Main Loop ^^^^^^ libMode=%d wrap_cnt=%d\n",opts->rplib_mode, wrap_cnt) ;

 if (progressmeter) { fprintf (stderr, "Calculating...  0%% complete\r"); }// \r CR not \n since want same line ..

 for (ngen = 1, nprod = 0; ngen <= maxgenerate && nprod < maxproduce; ngen++) { /* start ngen at 1; simplifies counting */
      if (opts->rplib_mode == 0 ) { /* normal deal random hands mode */
         JGMDPRT(8,"In Main Generating Hands: ngen=%d Calling deal_cards\n",ngen );
         deal_cards(curdeal) ; /* NEW 2023-01-06 -- Shuffle, Swap, or Predeal then Shuffle as required */
      }
      else { /* RP lib mode */
         /* get a deal from the RP database. Format as if it were 'dealt' by this program then continue as usual */
         get_rpdeal(opts, curdeal) ; /*sets curdeal and also the dds_res_bin 20 results struct */
         JGMDPRT(8,"get_rpdeal returned to main. ngen=%d \n",ngen );
         if (wrap_cnt >= 2 ) {
         /* we are on our second pass thru the library file.
          * There is no point so show err message and quit
          */
            fprintf(stderr, " All records in Library file scanned. Cannot produce %d hands. Ending Early. \n",opts->max_produce );
            break ;
         }
      } /* end else RP lib mode */
   #ifdef JGMDBG
      if (jgmDebug >= 9) {  sr_deal_show(curdeal); }
      if (jgmDebug >= 8 ) {
            struct handstat *hsp = hs;
            JGMDPRT(8, " Calling Analyze for ngen=%d \n", ngen);
            JGMDPRT(8, "hs_size=%lx, handstat[0]=>%p, handstat[1]=>%p,handstat[2]=>%p, handstat[3]=>%p\n",
               sizeof(struct handstat), (void *)hsp, (void *)(hsp+1), (void *)(hsp+2), (void *)(hsp+3) );
         }
   #endif

      analyze (curdeal, hs);  // Collect and save all info that will be needed by eval_tree() aka interesting() and server */

      JGMDPRT(7, " Calling Interesting for ngen=%d ", ngen);

      keephand = interesting() ;
      JGMDPRT(7, " Interesting aka evaltree Returns %d\n", keephand);
      // showtree = 0 ;      /* no longer done here. done right after yyparse */
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

  cleanup_action();         /* This will do all the end of run actions such as: printcompass,Average, Frequency, Frequency2D */

  if (verbose) {
    if (strlen(title) > 0 ) { printf("\n%s\n",title); }
    printf ("Generated %d hands\n", ngen);
    printf ("Produced  %d hands\n", nprod);
    if (0 == opts->rplib_mode )  {                    /* normal run. Not reading from Library file */
       printf ("Initial random seed %lu\n", seed);
    }
    else {
       printf ("Library file records read %d, Wrap-arounds=%d\n", rp_cnt, wrap_cnt);
       printf ("Library file starting seed %lu\n", opts->rplib_seed);
    }
    printf ("Time needed %8.3f sec%s",
             (tvstop.tv_sec + tvstop.tv_usec / 1000000.0 -
             (tvstart.tv_sec + tvstart.tv_usec / 1000000.0)), crlf);
    if (jgmDebug >= 2 ) { /* print some additional info for user even if JGMDBG Code not included */
      fprintf(stdout, "Tot Calls to DDS =%6d, DDS Solve  Calls=%6d DDS_Par Calls=%d\n", dbg_dds_res_calls,  dbg_dds_lib_calls, dbg_parscore_calls );
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


