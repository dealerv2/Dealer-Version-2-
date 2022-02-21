/* File dealmain_subs.c JGM 2022-Feb-17  */
// 2022-02-07 Expanding Options with -X and -0 thru -9
/* This file should not be compiled independently. #inlcude it in dealjgm.c */
#if 1                            /* change to  when not testing */
#include "../include/std_hdrs.h"  /* all the stdio.h stdlib.h string.h assert.h etc. */

#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/dealprotos.h"
#include "../include/dealexterns.h"
#endif
#define OPTSTR "hmquvVg:p:s:x:C:D:M:O:P:R:T:N:E:S:W:X:0:1:2:3:4:5:6:7:8:9:"

long int init_rand48( long int seed ) {
    union {
        unsigned short int sss[3] ;
                      char sbuff[6];
    } su;
    size_t numbytes;
    long int u_seed48, two16, two32 ;
    two16 = 1UL << 16 ;
    two32 = two16 * two16 ;

    if (seed != 0L ) { /* user has provided his own seed so just use it */
        srand48(seed) ;
        return seed ;
    }
    /* user has not specified a seed, so get one from the kernel and use seed48 instead of srand48*/
    numbytes = getrandom(su.sbuff, sizeof(su.sbuff), 0) ;
    if ( !numbytes ) {
       perror("getrandom FAILED. Cannot seed rand48") ;
       exit (-1) ;
    }
    assert( numbytes == 6 ) ;
    seed48(su.sss) ;   /* ignore seed48 return value of pointer to internal buffer */
    u_seed48 = (long int)su.sss[0] + (long int)su.sss[1]*two16 + (long int)su.sss[2]*two32;
#ifdef JGMDBG
     if (jgmDebug > 0 ) {
         fprintf(stderr, "No Seed Provided. DONE Initializing RNG init_rand48, %d, %d, %d, %ld\n",
                su.sss[0], su.sss[1], su.sss[2], u_seed48);
     }
#endif
    return u_seed48;
} /* end init_rand48 */

int gen_rand_slot ( int topval ) { /* return a random number in the range [0 .. (topval - 1)] */
        /* mrand48 returns a number between in range -2^31 .. +2^31 i.e. a 32 bit range. */
        return ( abs( (int) (mrand48() % (long int) topval) ) ) ;
}

/* Debugging Functions  */
int bias_handdbg(int compass){
  return
    TRUNCZ(biasdeal[compass][0])+
    TRUNCZ(biasdeal[compass][1])+
    TRUNCZ(biasdeal[compass][2])+
    TRUNCZ(biasdeal[compass][3]);
} /* end bias_len() */

int bias_suitdbg(int suit){
  return
    TRUNCZ(biasdeal[0][suit])+
    TRUNCZ(biasdeal[1][suit])+
    TRUNCZ(biasdeal[2][suit])+
    TRUNCZ(biasdeal[3][suit]);
} /* end bias_totsuit() */
void show_options ( struct options_st *opts , int v ) {
    fprintf(stderr, "Showing Options with Verbosity = %d\n",v);
    fprintf(stderr, "\t %s=[%d]\n", "g:Maxgenerate", opts->max_generate ) ;
    fprintf(stderr, "\t %s=[%d]\n", "m:ProgressMeter", opts->progress_meter ) ;
    fprintf(stderr, "\t %s=[%d]\n", "p:Maxproduce", opts->max_produce ) ;
    fprintf(stderr, "\t %s=[%d]\n", "q:Quiet", opts->quiet ) ;
    fprintf(stderr, "\t %s=[%ld]\n","s:Seed", opts->seed ) ;
    fprintf(stderr, "\t %s=[%d]\n", "u:UpperCase", opts->upper_case ) ;
    fprintf(stderr, "\t %s=[%d ; %d]\n", "v:Verbose", opts->verbose, verbose ) ;
    fprintf(stderr, "\t %s=[%d]\n", "x:eXchange aka Swapping", opts->swapping ) ;
    fprintf(stderr, "\t %s=[%s] mode=[%s]\n", "C:Fname",   opts->csv_fname, opts->csv_fmode   ) ;
    fprintf(stderr, "\t %s=[%d] set to %d\n", "D:Debug Verbosity", opts->dbg_lvl, jgmDebug ) ;
    fprintf(stderr, "\t %s=[%d] set to %d\n", "M:DDS Mode", opts->dds_mode, dds_mode ) ;
    fprintf(stderr, "\t %s=[%c, %d]\n", "O:Opener", opts->opc_opener, opts->opener  ) ;
    fprintf(stderr, "\t %s=[%d]\n", "P:Par Vuln", opts->par_vuln  ) ;
    fprintf(stderr, "\t %s=[%d]\n", "R:MaxThreads", opts->nThreads ) ;
    fprintf(stderr, "\t %s=[%d]\n", "R:MaxRamMB", opts->maxRamMB ) ;
    fprintf(stderr, "\t %s=[%s],len=%ld\n", "T:Title", opts->title,opts->title_len ) ;
    fprintf(stderr, "\t %s=[%s]\n", "N:PreDeal", opts->preDeal[0] ) ;
    fprintf(stderr, "\t %s=[%s]\n", "S:PreDeal", opts->preDeal[2] ) ;
    fprintf(stderr, "\t %s=[%s]\n", "E:PreDeal", opts->preDeal[1] ) ;
    fprintf(stderr, "\t %s=[%s]\n", "W:PreDeal", opts->preDeal[3] ) ;
    fprintf(stderr, "\t %s=[%s]\n", "X:Fname",   opts->ex_fname   ) ;
   return ;
} /* end show opts */
void showargs(int argc, char *argv[]) {
    int argnum = 0 ;
    while (argnum < argc ) {
        printf("Argnum:%d = %s \n",argnum, *(argv+argnum) );
        argnum++;
    }
    return ;
}
  /* These will only show the ones actually set on the cmd line */
void show_script_vars ( struct param_st *p , int v ) {
   int i ;
   if( !p->scripting ) { return ;  } /* no script vars set */
   fprintf(stderr, "Showing Script Vars with Verbosity = %d\n",v);
   for (i = 0 ; i < 10 ; i ++ ) {
      if (p->script_var_len[i] > 0 ) {  fprintf(stderr, "\t [$%d]=%s\n", i, p->script_var[i] ); }
   }
   return ;
} /* end show script_vars */

/* Command Line Parsing via the main function and a while loop returns 0 no errors, 1, 2, 3,  errors*/
int str_UC(char *dst, char *src , int maxn) {
   int p = 0;
   while ( *src != '\0'  && p < maxn ) {
      *dst++ = toupper((unsigned char) *src++ ) ;
      p++;
   }
   *dst='\0';
   return p ;
}

/* this code will set the opts in the options struct and also the related global variable(s) for backwards compatitbility */
int get_options (int argc, char *argv[], struct options_st *opts) {
    int opt_c;
    opterr = 0;
    opts->options_error = 0 ;  /* assume success */
    /* init some options to the same init state as their global variable to keep them in sync in case not set on cmd line */
    opts->swapping = 0;
    opts->progress_meter = 0;
    opts->quiet = 0 ;
    opts->upper_case = 1;
    opts->verbose = 1 ;
    opts->max_generate = 100000 ;
    opts->max_produce = 40 ;
    opts->seed = 0 ;
    opts->seed_provided = 0 ;
    opts->dbg_lvl = 0 ;
    opts->opc_opener = 'N' ;
    opts->opener = 1 ;
    opts->dds_mode = 1 ;
    opts->par_vuln = -1 ;
    opts->nThreads = 1 ;
    opts->maxRamMB = 160 ;
    opts->csv_fmode[0] = '\0' ; // if null no csvfile specified.

    while ((opt_c = getopt(argc, argv, OPTSTR)) != EOF) {
      switch(opt_c) {
        case 'h': {
            fprintf(stdout, "--- HELP COMING --- \n");
            fprintf(stdout, "%s Usage: -[options] [input_filename | stdin] [>output_file]\n", argv[0]);
            fprintf(stdout,"%s\n", UsageMessage);   /* UsageMessage in src/UsageMsg.h */
            fprintf(stdout, "--- HELP DONE --- \n");
            opts->options_error = 4 ;
            break;
        }
      case 'g':
        opts->max_generate = atoi( optarg );    maxgenerate = opts->max_generate;
        break;
      case 'm':
        opts->progress_meter = 1;              progressmeter = opts->progress_meter;
        break;
      case 'p':
        opts->max_produce = atoi( optarg );     maxproduce = opts->max_produce;
        break;
      case 'q':
        opts->quiet ^= 1;                       quiet = opts->quiet;
        break;
      case 's':
        opts->seed_provided = 1;                seed_provided = opts->seed_provided;
        opts->seed = atol( optarg );            seed          = opts->seed;
        if (seed == LONG_MIN || seed == LONG_MAX) {
            fprintf (stderr, "Seed overflow: seed must be between %ld and %ld\n",
              LONG_MIN, LONG_MAX);
            exit (-1);
        }
        break;
      case 'u':
        uppercase ^= 1;                         opts->upper_case = uppercase ;
        break;
      case 'v':
        verbose ^= 1 ;                          opts->verbose = verbose;
        break;
      case 'x':                  // replaced the three switches 0,2,3 with one switch that takes a value
         opts->swapping  = atoi( optarg ) ;     swapping = opts->swapping ;
         break;

      case 'V':
        if (opts->title_len > 0  ) { printf("Title: %s\n", opts->title); }
        printf ("Version info....\n");
        printf ("Revision: %s \n", VERSION );
        printf ("Date: 2022/02/17 \n");
        printf ("$Author: Hans, Henk, JGM $\n");
        #ifdef JGMDBG
          printf("JGMDBG is defined. Debugging printing to stderr is active\n");
        #endif
        opts->options_error = 3 ; // Version Info
        return 3 ;
        break ;

/* options added by JGM: -C, -D,  -M, -O, -P, -R, -T, -N, -E, -S, -W, -X and -0 thru -9  */
      case 'C':         /* Filename for CSV report. Normally opened with append unless preceded by w: */
         if ( optarg[0] == 'w' && optarg[1] == ':' ) {
            strncpy(opts->csv_fname, &optarg[2], 127 ) ;
            opts->csv_fmode[0] = 'w' ; opts->csv_fmode[1] = '\0' ;
         }
         else {
            strncpy(opts->csv_fname, optarg, 127 ) ;
            opts->csv_fmode[0] = 'a' ;  opts->csv_fmode[1] = '\0' ;
         }
         fcsv = fopen(opts->csv_fname , opts->csv_fmode) ;
         if (fcsv == NULL ) {
            perror("ERROR!! Open CSV Report file FAILED");
            fprintf(stderr, "ERROR!! Cant open [%s] for %s \n",opts->csv_fname, opts->csv_fmode );
            opts->options_error = -2 ;
         }
        #ifdef JGMDBG
               if (jgmDebug >= 4) {fprintf(stderr, "CVSRPT File %s opened in %s Mode\n",opts->csv_fname, opts->csv_fmode ); }
         #endif
          break ;
      case 'D':
        opts->dbg_lvl = atoi( optarg ) ;      jgmDebug = opts->dbg_lvl;
        break ;
     case 'M':
        opts->dds_mode = atoi( optarg ) ;
        if (0 <= opts->dds_mode && opts->dds_mode <= 2 ) { dds_mode = opts->dds_mode ; }
        else opts->dds_mode = dds_mode ; // if invalid use the  compile time value
        break ;
     case 'O':
        opts->opc_opener = *optarg ;  // now convert the char to an compass direction
        opts->opener = (*optarg == 'N' || *optarg == 'n' ) ? COMPASS_NORTH :
                       (*optarg == 'E' || *optarg == 'e' ) ? COMPASS_EAST  :
                       (*optarg == 'S' || *optarg == 's' ) ? COMPASS_SOUTH :
                       (*optarg == 'W' || *optarg == 'w' ) ? COMPASS_WEST  : COMPASS_WEST ;
        if (opts->opener == COMPASS_WEST ) opts->opc_opener = 'W' ; // default in case invalid entry
        break ;
      case 'P':  /* set the vulnerability for Par calculations */
        opts->par_vuln = atoi( optarg ) ; // Will Need to translate from Dealer coding to DDS coding when calling Par function
        par_vuln =  (opts->par_vuln == 0 ) ? 0 :      // None
                    (opts->par_vuln == 1 ) ? 2 :      // NS
                    (opts->par_vuln == 2 ) ? 3 :      // EW
                    (opts->par_vuln == 3 ) ? 1 : -1 ; // Both. Assume no Par Calcs if invalid entry.
        break ;
      case 'R':
        opts->nThreads=atoi( optarg ) ;
        if (opts->nThreads > 10 ) {
           fprintf(stderr, "Invalid value[%d] for R opt(0..10) setting to 9.\n",opts->nThreads);
           opts->nThreads = 9 ;
           opts->options_error = 2 ; // invalid thread value
        }
        opts->maxRamMB = 160*opts->nThreads ;
        break ;
      case 'T':
        strncpy( opts->title, optarg, 79);       strncpy(title, optarg, 79);
        opts->title_len = strlen( opts->title ); title_len = opts->title_len;
        break;
      case 'W':
        opts->preDeal_len[3] = str_UC( opts->preDeal[3], optarg, 31) ;
        break;
      case 'E':
        opts->preDeal_len[1] = str_UC( opts->preDeal[1], optarg, 31) ;
        break;
      case 'S':
        opts->preDeal_len[2] = str_UC( opts->preDeal[2], optarg, 31) ;
        break;
      case 'N':
        opts->preDeal_len[0] = str_UC( opts->preDeal[0], optarg, 31) ;
        break;
      case 'X':
         strncpy(opts->ex_fname, optarg, 127 ) ;
         fexp = fopen(optarg , "w" ) ;
         if (fexp == NULL ) {
            perror("ERROR!! Open eXport file for writing FAILED");
            fprintf(stderr, "ERROR!! Cant open [%s] for write \n",optarg );
            opts->options_error = -2 ;
         }
         break ;


      /* Next ten options set the scripting variables stored in the global struct 'parm' */
      case '0' :
         strncpy(parm.script_var[0], optarg, PARAM_SIZE) ;
         parm.script_var_len[0] = strlen(parm.script_var[0]) ;
         parm.scripting = 1 ;
         break ;
      case '1' :
         strncpy(parm.script_var[1], optarg, PARAM_SIZE) ;
         parm.script_var_len[1] = strlen(parm.script_var[1]) ;
         parm.scripting = 1 ;
         break ;
      case '2' :
         strncpy(parm.script_var[2], optarg, PARAM_SIZE) ;
         parm.script_var_len[2] = strlen(parm.script_var[2]) ;
         parm.scripting = 1 ;
         break ;
      case '3' :
         strncpy(parm.script_var[3], optarg, PARAM_SIZE) ;
         parm.script_var_len[3] = strlen(parm.script_var[3]) ;
         parm.scripting = 1 ;
         break ;
      case '4' :
         strncpy(parm.script_var[4], optarg, PARAM_SIZE) ;
         parm.script_var_len[4] = strlen(parm.script_var[4]) ;
         parm.scripting = 1 ;
         break ;
      case '5' :
         strncpy(parm.script_var[5], optarg, PARAM_SIZE) ;
         parm.script_var_len[5] = strlen(parm.script_var[5]) ;
         parm.scripting = 1 ;
         break ;
      case '6' :
         strncpy(parm.script_var[6], optarg, PARAM_SIZE) ;
         parm.script_var_len[6] = strlen(parm.script_var[6]) ;
         parm.scripting = 1 ;
         break ;
      case '7' :
         strncpy(parm.script_var[7], optarg, PARAM_SIZE) ;
         parm.script_var_len[7] = strlen(parm.script_var[7]) ;
         parm.scripting = 1 ;
         break ;
      case '8' :
         strncpy(parm.script_var[8], optarg, PARAM_SIZE) ;
         parm.script_var_len[8] = strlen(parm.script_var[8]) ;
         parm.scripting = 1 ;
         break ;
      case '9' :
         strncpy(parm.script_var[9], optarg, PARAM_SIZE) ;
         parm.script_var_len[9] = strlen(parm.script_var[9]) ;
         parm.scripting = 1 ;
         break ;
/*  -- end of JGM added cmd line switches */

      default :
            fprintf(stdout, "DEFAULT CASE FALL THROUGH\n");
      case '?' : // there was an invalid option invalid char is in optopt
            fprintf(stdout, "%s Usage: -[options] [input_filename | stdin] [>output_file]\n", argv[0]);
            fprintf(stdout, "Valid Options == [%s]\n", OPTSTR );
            fprintf(stdout, "\t\t\t[%c] is an invalid option\n", optopt ) ;
            opts->options_error = 1 ;  // invalid option found
            break;
      } /* end switch(opt) */
    } /* end while getopt*/
/* +++++++++++++++++++++++++++++++ main program continues here +++++++++++++++++++ */
    if (opts->dbg_lvl >= 4 ) { fprintf(stderr, "Command Line switches done proceeding with main program \n") ; }
        return opts->options_error ;

} /* end getopts */


