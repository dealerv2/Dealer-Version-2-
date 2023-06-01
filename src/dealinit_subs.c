/* File dealinit_subs.c  JGM 2023--MAR-15
 * Version 2.5.5a -- first version of this file. Collect all the init routines in one place.
 */
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

#define FATAL_OPTS_ERR -10
#ifndef DDS_TABLE_MODE
   #define DDS_TABLE_MODE 2      // import from the DDS namespace
#endif
int isCard (char crd) ;
void setup_deal() ;

void init_globals ( struct options_st *opts ) {
      /* give sane values to globals not initialized at compile time */
    memset( &parm, 0 , sizeof(parm) ) ;   /* filling with zeros should both terminate the 'strings' and set len to 0 */
    /* The non opt versions of the above will have been compile time init in dealglobals.c */

    /* None of the following are set in the input file and so should be replaced by the options.xxxx variable */
    errflg = 0;     progressmeter = 0;     swapping = 0;     swapindex = 0;     verbose = 1;     quiet = 0 ;   uppercase = 0 ;

    /* set these next 3 so that yyparse/flex will fill them in if reqd */
    maxdealer = -1;     maxvuln = -1;     dds_dealnum = -1;

    /* These globals cannot be a compile time init, since the user may re-direct stdin and stdout when calling dealer */
   fexp = stdout ; // FILE *fexp Will be opened by getopts if there is a -X option
   fcsv = stdout ; // FILE *fcsv Will be opened by getopts if there is a -C option

   input_file = '\0' ;    /* NULL pointer */

   /* set some options_st defaults. All set to zero at compile time.
    * Set non zero ones to match the corresponding compile time global
    */
  opts->verbose      = verbose;
  opts->opc_opener   = opc_opener; opts->opener = Opener;
  opts->max_produce  = maxproduce;
  opts->max_generate = maxgenerate;
  opts->opc_opener   = opc_opener;   opts->opener = Opener;
  opts->dds_mode     = dds_mode ;
  opts->par_vuln     = par_vuln ;
  opts->nThreads     = nThreads ;
  opts->maxRamMB     = MaxRamMB ;
  opts->seed_provided = seed_provided ;

   return ;
} /* end init_globals */

void predeal_cmdparms(int compass, char *holding) {
/* yyparse may have filled in the Predeals if called for in input file
 * But cmd line Predeals will over-ride those
 * aside: predeal function is in file: dealparse_subs.c
 */
         char suit, ch;
        size_t holding_len ;
        int  l  ;
        int cnt = 0 ;
        holding_len = strlen(holding) ;
        JGMDPRT(4, "Predeal_cmdparms:: Compass=%d, holding=[%s]\n", compass, holding) ;
        suit = ' ';
        for (l = 0 ; l < holding_len; l++ ) {
            ch = *(holding + l) ;

            if (ch == 'C' || ch == 'D' || ch == 'H' || ch == 'S' ) { suit = ch ; }
            else if (isCard(ch) >= 0 ) { predeal(compass, make_card(ch, suit)  );  cnt++ ;}
            /* if not a suit or  card skip it; probably a comma or other suit separator */
        }
        JGMDPRT(4,"Predeal_cmdparms:: Holding len=%ld, cards predealt = %d\n", holding_len, cnt ) ;
} /* end predeal_cmdparms() */

void initdistr () {
   /* Various handshapes can be asked for. For every shape the user is
   interested in a number is generated. In every distribution that fits that
   shape the corresponding bit is set in the distrbitmaps 4-dimensional array.
   This makes looking up a shape a small constant cost.
*/
  int ***p4, **p3, *p2;
  int clubs, diamonds, hearts;

  /* Allocate the four dimensional pointer array. calloc will set it to zero. */

  for (clubs = 0; clubs <= 13; clubs++) {
    p4 = (int ***) mycalloc ((unsigned) 14 - clubs, sizeof (*p4));
    distrbitmaps[clubs] = p4;
    for (diamonds = 0; diamonds <= 13 - clubs; diamonds++) {
      p3 = (int **) mycalloc ((unsigned) 14 - clubs - diamonds, sizeof (*p3));
      p4[diamonds] = p3;
      for (hearts = 0; hearts <= 13 - clubs - diamonds; hearts++) {
        p2 = (int *) mycalloc ((unsigned) 14 - clubs - diamonds - hearts, sizeof (*p2));
        p3[hearts] = p2;
      }
    }
  }
} /* end initdistr */

void init_cards() {
  newpack(fullpack);   /* fill fullpack with cards in order from North SA downto West C2 */
  memset(stacked_pack, NO_CARD, 52) ;  /* stacked pack used for preDeals */
  memset(curdeal, NO_CARD, 52 ) ;
  stacked_size = 0 ;
  memcpy(small_pack, fullpack, 52 ) ;  /* Shuffle works from small_pack not fullpack */
  small_size = 52 ;
}

void setup_rng( struct options_st *opts ) {
   /*
   * Using glibc standard lib routines rand48 and srand48.
   * If no seed provided, use function init_rand48 which uses kernel as seed source
   */
  if (jgmDebug >= 3 ) { JGMDPRT(3,"Initializing RNG. Seed_provided=%ld, seed=%ld\n",seed_provided,seed); }
  if (!opts->seed_provided) {
    opts->seed = SRANDOM(0) ; /* use init_rand48() to init the RNG with a seed that the kernel provides */
  }
  else {
    opts->seed = SRANDOM(opts->seed) ; /* use init_rand48() to init the RNG with the users seed */
  }
  JGMDPRT(3,"RNG initialized with Seed val=%ld\n",opts->seed );
} /* end setup RNG */

void setup_dds_mode ( struct options_st *opts ) {
  void ZeroCache( DDSRES_k *Results) ;
  void  SetResources(  int maxMemoryMB,  int maxThreads); /* for DDS */
/* If user has set TABLEMODE (-M 2) but neglected to set Threads (-R n), then we use the TblModeDefault value */
  if ( (opts->dds_mode == 2 ) && (opts->nThreads < 2) ) {
     opts->nThreads = TblModeThreads ;
     nThreads = TblModeThreads ;
     opts->maxRamMB = 160 * TblModeThreads ;
     MaxRamMB = opts->maxRamMB ;
  } /* else dds_mode is 1 OR user has spec'd both Mode and Threads values */

  SetResources(opts->maxRamMB, opts->nThreads) ; /* 160MB/Thread max; 9,12,16 all equal and 25% faster than 6 Threads for Mode2*/
  ZeroCache(&dds_res_bin) ; /* dds_res_bin is a global struct holding tricks for 20 combos of leader and strain */
   JGMDPRT(3,"DDS_mode Set. Mode=%d, Threads=%d, Ram=%d MB \n",opts->dds_mode, opts->nThreads, opts->maxRamMB ) ;
   return ;
} /* end setup_dds_mode */

void finalize_options ( struct options_st *opt_ptr) {
   /* Over-ride what was in input file with what was entered on the cmd line via switches
    * If the opts value was not set on the cmd line, and there was one set in the input file copy input value to opts struct
    * also does some rudimentary consistency checking
    */
  int set_rpdd_lib_vars(FILE *rpdd_file, struct options_st *opts);
  int i, lib_recs, myseed;
  long rpdd_pos ;

    /* if the use_side flag is set, we need to set the use_compass flags for both seats regardless of what the parser did*/
    if (use_side[0] == 1 ) {use_compass[0] = 1; use_compass[2]= 1;  } /* NS */
    if (use_side[1] == 1 ) {use_compass[1] = 1; use_compass[3]= 1;  } /* EW */
    JGMDPRT(3,"Finalize Options. use_side=[%d,%d] \n", use_side[0], use_side[1] );
    if (opt_ptr->opc_opener != '\0' ) { /* cmd line entry (if any) has highest priority*/
        Opener = opt_ptr->opener;       /* Get opts function will have set both */
        opc_opener = opt_ptr->opc_opener ;
    }
    else {  /* Keep opts struct in sync with the default or anything that Flex has set */
         opt_ptr->opener = Opener ;
         opt_ptr->opc_opener = seat_id[Opener] ;
         opc_opener = opt_ptr->opc_opener;
    }
     /* need to check the title stuff here, since yyparse() may have set it also */
    if( opt_ptr->title_len > 0 ) {
       JGMDPRT(4,"Setting Title to CmdlineTitle=[%s],CmdLineTitleLen=[%ld]\n",
                  opt_ptr->title, opt_ptr->title_len ) ;
       title_len = opt_ptr->title_len  ;
       strncpy( title, opt_ptr->title, MAXTITLESIZE - 1 );
    }
    else if (title_len > 0 ) {
       JGMDPRT(4,"Setting Title to input file Title=[%s],title_len=[%ld]\n",title, title_len );
       opt_ptr->title_len = title_len ;
       strncpy(opt_ptr->title, title, title_len+1) ;
    }
    JGMDPRT(4,"Title=[%s], NewTitleLen=[%ld]\nCmdlineTitle=[%s],CmdLineTitleLen=[%ld]\n",
                  title, title_len, opt_ptr->title, opt_ptr->title_len ) ;

    if (opt_ptr->max_generate > 0 ) { maxgenerate = opt_ptr->max_generate ; }
    else if (maxgenerate > 0 )      { opt_ptr->max_generate = maxgenerate ; }

     /* need to check the seed stuff here, since yyparse() may have set it also */
    if( opt_ptr->seed_provided > 0 ) {  /* getopts will set this to 1 if there is a -s switch on cmd line */
       seed_provided = opt_ptr->seed_provided  ;
       seed = opt_ptr->seed; opt_ptr->rplib_seed = seed ;
       JGMDPRT(4, "Cmd Line  Seed[%ld] sets global seed\n", opt_ptr->seed ) ;
    }
    else if (seed_provided) {  /* flex will set this to 1 if there is a seed in the input file */
       opt_ptr->seed_provided = seed_provided  ;
       opt_ptr->seed = seed ; opt_ptr->rplib_seed = seed ;
    JGMDPRT(4, "Input file  Seed[%ld] sets opts_st seed\n", seed ) ;
    }
    else { JGMDPRT(4, "No Seed provided. Will use kernel entropy\n"); }

    /* If not in Library mode allow user to over-ride any predeal statements in the input file via cmd line switches */
    /* Undo any predealing yyparse has done.   */
     if ( opt_ptr->rplib_mode == 0 && (opt_ptr->preDeal_len[0] > 0 || opt_ptr->preDeal_len[1] > 0 ||
         opt_ptr->preDeal_len[2] > 0 || opt_ptr->preDeal_len[3] > 0) ) {
        JGMDPRT(4, "Setting up predeal from the cmdline \n");
        /* Reset everything relating to deal initialization to reverse whatever yyparse might have done */
        init_cards() ;
        for (i =0 ; i < 4 ; i++ ) {
           if (opt_ptr->preDeal_len[i] > 0 ) {
              JGMDPRT(4, "Calling Predeal hand = %c [%s] \n", "NESW"[i], opt_ptr->preDeal[i] );
              predeal_cmdparms(i, opt_ptr->preDeal[i] ) ;
           }
        }
      } /* end if checking the lengths for predeal */


    /* These next ones are never set in the Input file */
    /* These next two are set by main before calling getopts, but may have been overridden */
    if ( opt_ptr->par_vuln != -1 ) { /* user has said he will be doing Par calcs */
        par_vuln = opt_ptr->par_vuln ;
        if (dds_mode != DDS_TABLE_MODE ) {
           dds_mode = DDS_TABLE_MODE ;
           opt_ptr->dds_mode = DDS_TABLE_MODE ;
           setup_dds_mode(opt_ptr) ;
           JGMDPRT(4, "CmdLine sets par_vuln = [%d -> %d] and dds_mode=%d \n",opt_ptr->par_vuln, par_vuln, opt_ptr->dds_mode );
        } /* Must do full TableCalc to get par scores */
    }
    else if (par_vuln != -1 ) { /* par spec in input file */
       opt_ptr->par_vuln = par_vuln ;
       if (dds_mode != DDS_TABLE_MODE ) {
           dds_mode = DDS_TABLE_MODE ;
           opt_ptr->dds_mode = DDS_TABLE_MODE ;
           setup_dds_mode(opt_ptr) ;
           JGMDPRT(4, "Infile sets par_vuln = [%d -> %d] and dds_mode=%d \n",par_vuln, opt_ptr->par_vuln, opt_ptr->dds_mode );
        } /* Must do full TableCalc to get par scores */
    } /* end check par_vuln */


    /* Check for consistency when using LIB mode: Lib mode overrides everything else.
     * No predeal, no Par, no swapping, dds_mode set to '2' since 20 results returned,
     * and seed must be valid and the seek position of the lib file set accordingly
     */
    if ( opt_ptr->rplib_mode == 1 ) {
       rplib_mode = opt_ptr->rplib_mode ;
       JGMDPRT(3, "Lib Mode Specified. Ignoring impossible options \n");
       predeal_compass = -1 ;    /* reset predeal_compass in case yyparse found predeal spec in input file */
       if (stacked_size > 0 ) {  /* there was predealing in the yacc file */
          fprintf(stderr, "Cannot pre Deal hands from input file if using Library mode. Ignoring. \n");
          init_cards() ; /* undo any previous deck stacking. */
                         /* Not really needed since Lib mode just overwrites curdeal but a good safety net. */
       }
       if (opt_ptr->swapping > 0 ) {
          fprintf(stderr, "Swapping not possible if using Library mode. Ignoring. \n");
       }

       /* Repurpose the seed spec if any to mean an offset into the Library file */
       lib_recs = set_rpdd_lib_vars(rp_file, opt_ptr) ; /* calculate DB size, then calc blksize and  max seed accordingly */
       if( opt_ptr->rplib_seed > 0 ) { /* in Lib mode treat seed as an offset into the lib file */
            myseed = (opt_ptr->rplib_seed < rplib_max_seed) ? opt_ptr->rplib_seed : rplib_max_seed ;
            rpdd_pos = seek_rpdd_pos(rp_file, myseed) ;
            if( rpdd_pos == 0 ) {
               fprintf(stderr, "Seeking to position of myseed=%d failed. Will start at beginning \n", myseed);
               rpdd_pos = seek_rpdd_pos(rp_file, 0) ;
            }
       }
       JGMDPRT(2,"finalize_options Seeking to RP_Record # %u \n",myseed * rplib_blk_sz + 1 );

       if (opt_ptr->max_generate > lib_recs ) {
          fprintf(stderr, "dealinit.c::Finalize Options::ERR maxgenerate too big. Library does not contain %d records. Setting to %d \n",
                                    maxgenerate,lib_recs ) ;
          opt_ptr->max_generate = lib_recs ; maxgenerate =  lib_recs ;
          if (opt_ptr->max_produce > opt_ptr->max_generate) {
             opt_ptr->max_produce = opt_ptr->max_generate ;
             maxproduce = opt_ptr->max_produce ;
          }
       }
       /* now make sure to zap all impossible option flags */
       opt_ptr->dds_mode = DDS_TABLE_MODE ;  dds_mode = DDS_TABLE_MODE ; /* dummy; bec Lib mode returns all 20 results */
       opt_ptr->swapping = 0;                swapping = 0 ;
       memset(opt_ptr->preDeal,     0 , sizeof(opt_ptr->preDeal)     ) ;
       memset(opt_ptr->preDeal_len, 0 , sizeof(opt_ptr->preDeal_len) ) ;
       JGMDPRT(4,"*---------Finalize Options all Done----------* \n");
    } /* end rplib_mode == 1 */
    #ifdef JGMDBG
      if (jgmDebug >=4 ) {
         fprintf(stderr, "%s.%d:: Predeal check, stacked_size=%d Next: FullPack, StackedPack, curdeal\n",
                         __FILE__,__LINE__,stacked_size);
         sr_deal_show(fullpack);
         sr_deal_show(stacked_pack);
         sr_deal_show(curdeal) ;
      }
    #endif
    JGMDPRT(4, "Done initprogram dds_mode=%d, Dbg_Verbosity=%d, vers=%s\n", dds_mode, jgmDebug, VERSION ) ;
} /* end finalize_options */

void init_runtime(struct options_st *opts) {
    if (opts->rplib_mode != 1 ) {
      setup_rng(opts) ;
      setup_dds_mode(opts) ;
      /*
       * Initialize the Deck with predealt cards from the stacked_pack, and leftover cards from small_pack
       * Wait till now because program opts may over-ride the parser
       * might skip this if Library mode
       */
      setup_deal();
    }

   if (maxgenerate == 0) {
       maxgenerate = 10000000 ;
       if (opts->rplib_mode == 1 && maxgenerate > rplib_recs ) {
          maxgenerate = rplib_recs; opts->max_generate = rplib_recs;
       }
   }
   if (maxproduce == 0)  maxproduce = ((actionlist == &defaultaction) || will_print) ? 40 : maxgenerate;
   JGMDPRT(2, "Maxgenerate=%d, Maxproduce=%d, DDS Threads=%d, UserServerReqd=%d\n",
               maxgenerate, maxproduce, opts->nThreads, userserver_reqd );
   /* V4.5 ensure the two tables are in sync yyparse may have changed one but not the other*/
   for (int i=0 ; i<13 ; i++ ) { points[i] = tblPointcount[idxHcp][i] ; }

   /*
    * Start the user server if there was a usereval statement in the input file
    */
   if (userserver_reqd != 0 )  {
       userserver_pid = setup_userserver( server_path ) ;
       if (userserver_pid == -1 ) {
          perror(" Creating the server process returns failed PID. Aborting... ");
          assert(0) ;
       }
       JGMDPRT(3,"UserServer daemon started with pid=%d \nfrom path=[%s]\n",userserver_pid,server_path);
    } /* user server daemon started */

   if (srvDebug > 0 ) {
      // sleep(1) ;
      usleep(5000); /* 5 msec Give the child time to tell us his logfile/errmsg file name. before we start our own output */
      printf("\f") ; /* issue a form feed to start the Dealer output */
      fsync(1);  /* clear out stdout before starting real info */
   }
   setup_rng( opts ) ;         /* used seed value to init the rng */
   seed = opts->seed ;         /* set the global to match what setup_rng has done */

      /* Initialize the seat fields in handstat array for later use by the Server */
   for (int i=0 ; i < 4 ; i++ ) { hs[i].hs_seat = i ; }

   /* Walk the action list initializing any actions that need it */
   setup_action();
   return ;
}  /* end init runtime */


