/*  JGM -- File dealdds_subs.c 2022-feb-15
 *     Borrows some defs and types from Dealer .h files without importing everything as there are name conflicts
 */
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif

#define CACHE_INV 0
#define CACHE_UPD 1
#define CACHE_OK  2
#define DDS_TABLE_MODE 2
#define DDS_BOARD_MODE 1
#define INV_TRIX -1
/* These next two may never get used as there is really no reason to call DDS from Dealer with PBN mode.
 * unless you were not generating Dealer deals (Deal52) and just reading from a file
 * but why use Dealer for that ?
 */
#define DDS_INP_DEALER = 4
#define DDS_INP_PBN    = 8

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "../include/dealdefs.h"
#include "../include/dds_dll.h"
#include "../include/deal_dds.h"

extern int dds_dealnum;          /* keeps track of whether the dds cache needs refreshing */
extern int par_vuln;             /* -1 no par calculations, 0=none, 1=ew, 2=ns, 3= both? */
extern int dbg_dds_lib_calls;    /* print dds counts at end of run for debugging and timing */
extern int dbg_dds_res_calls;
extern int dbg_parscore_calls;
extern int MaxRamMB;
extern int nThreads;
extern int csv_firsthand;        /* not used was intending to allow user to print deal in arbitrary seq; but too confusing */

/*API we export to the Dealer side */

typedef unsigned char deal[52];
extern int dds_mode ; /* 1 = solveOneBoard, 2=CalcDDtable */
extern int ngen, dds_dealnum, par_vuln, jgmDebug ;
extern deal curdeal;

DDSRES_k dds_res_bin;
DDSRES_k dds_res ;

/*    Prototypes From Dealer Side */
    /* for Debugging .. To Be Deleted or in an #ifdef */
extern void showRawResults(struct ddTableResults *dd );
extern void showReturns(DDSRES_k *dd ) ;
extern void dump_Deal(struct deal_st dl);
    /* Real Protos */
extern int Deal52_to_Holding(deal d , unsigned int kards[DDS_HANDS][DDS_SUITS] ) ;
extern struct ddTableDeal Deal52_to_DDSBIN(deal d);  // ddTableDeal    uses [hands][suits] index order.
DDSRES_k true_CalcTable(deal dl, int vul, int dds_mode ) ; // ddTableResults uses [suits][hands] index order.
int      true_SolveBoard(deal curdeal, int compass, int strain ) ;

void ZeroCache( DDSRES_k *Results) {
    memset(Results, 0 ,  sizeof(DDSRES_k) ) ;
    memset(Results->tricks, INV_TRIX , sizeof(Results->tricks) ) ;
}

/* return 0 = Cache invalid, 1 = entry [h][s] needs update, 2 = Cache OK */
int CheckCache ( DDSRES_k *Results, int ngen, int ddsnum , int h, int s ) {
  #ifdef JGMDBG
     if ( jgmDebug >= 8 ) {
       fprintf(stderr, "CheckCache.66 ngen=%d,ddsnum=%d, h=%d, s=%d,Res.Tricks[h][s]=%d, INVTRIX=%d \n",
               ngen,ddsnum,h,s,Results->tricks[h][s], INV_TRIX );
      }
  #endif
    if( ngen != ddsnum ) return CACHE_INV ; // call dds library either SolveBoard or CalcDDtable depending on mode
    if ( Results->tricks[h][s] == INV_TRIX ) return CACHE_UPD ; // need to call DDS lib to update this entry at least
    assert(0<=Results->tricks[h][s] && 13 >= Results->tricks[h][s] );
    return CACHE_OK ;   // No need to call DDS lib.
}

int SetDDSmode(int mode) {       /* Default is mode 1; but Par, csv_trix, and the user -M2 will set to mode 2 */
    if ( mode == DDS_BOARD_MODE ) {
        dds_mode = DDS_BOARD_MODE ;
        SetResources(320 , 2 ) ;     // We probably can only use 1 thread in this mode, but give it 2 just in case.
        MaxRamMB = 320 ;            // update global vars for consistency
        nThreads = 2 ;
        return 1 ;
    }
    if (mode == DDS_TABLE_MODE ) {
        dds_mode = DDS_TABLE_MODE ;
        SetResources(MaxRamMB, nThreads) ;  // Could be insufficient but user must have a reason for setting this.
        return 1 ;
    }
    else  {
        fprintf(stderr, "SetDDSmode:: dds mode[%d] is INVALID!!. Setting to TableMode with 9 Threads and 1440 MB \n", mode );
        dds_mode = DDS_TABLE_MODE ;
        SetResources(9*160, 9) ; // 9 Threads seems the sweet spot on my machine
        MaxRamMB = 9*160 ;       // update global vars for consistency
        nThreads = 9 ;
        return 1 ;
    }
} /* end SetDDSmode */

/* compass is the hand we want the tricks for. If mode=2, no change needed
 * but for mode 1 we need to set the op leader to be the hand to the right of compass like true_dd does
 */

int dds_tricks(int compass, int strain ) {  //ngen, dds_dealnum, dds_res_bin, dds_mode are globals
    int t0, t1;
    int CacheState ;
    dbg_dds_res_calls++;


 #ifdef JGMDBG
    if (jgmDebug >= 5 ) {
       fprintf(stderr, "IN DDS_TRICKS.112 res_calls=%d ngen=%d dds_dealnum=%d,hand[%c],strain[%c], dds_mode=%d \n",
                           dbg_dds_res_calls, ngen, dds_dealnum, "neswSW"[compass],"cdhsN"[strain], dds_mode ); }
 #endif
    CacheState = CheckCache(&dds_res_bin, ngen, dds_dealnum, compass, strain ) ;
 #ifdef JGMDBG
    if (jgmDebug >= 5 ) {
       fprintf(stderr, "DDS_TRICKS.118: CheckCache returns[%d], ngen=%d,dds_dealnum=%d, trix=%d \n",
                        CacheState, ngen, dds_dealnum, dds_res_bin.tricks[compass][strain] );
    }
 #endif

    if    (CacheState == CACHE_INV ) { /* ngen is on a new deal */
        ZeroCache( &dds_res_bin);
        dds_res_bin.CacheStatus = CACHE_INV ;
    }
    else if (CacheState == CACHE_UPD ) {
        dds_res_bin.CacheStatus = CACHE_UPD ; /* The cache is partially OK, but need to call lib anyway for new h/s combo */
    }
    if ( CacheState == CACHE_UPD || CacheState == CACHE_INV ) { /* Decide what kind of lib call to make then make it. */
           dbg_dds_lib_calls++;

#ifdef JGMDBG
         if (jgmDebug >= 5 ) {fprintf(stderr, "IN DDS_TRICKS.134 CacheInvalid lib_calls=%d, mode=%d, dds_dealnum=[%d]  \n",
                           dbg_dds_lib_calls, dds_mode, dds_dealnum ); }
#endif

        if (dds_mode == DDS_TABLE_MODE ) {
            dds_res_bin = true_CalcTable (curdeal, par_vuln, DDS_TABLE_MODE) ;
        }
        else if (dds_mode == DDS_BOARD_MODE) {

           dds_res_bin.tricks[compass][strain] = true_SolveBoard(curdeal, compass, strain ) ;
           t1 = dds_res_bin.tricks[compass][strain];
   #ifdef JGMDBG
            if (jgmDebug >= 5 ) {
               fprintf(stderr, "SOLVEBOARD for compass=%c strain=%c, returns %d trix \n",
                           "neswSW"[compass], "cdhsN"[strain], t1 );
            }
   #endif
        }
        else {
            fprintf(stderr, "Cant Happen in dds_tricks. dds_mode=[%d] is Invalid!! Continuing with TableMode\n", dds_mode );
            SetDDSmode(DDS_TABLE_MODE) ;
            dds_res_bin = true_CalcTable (curdeal, par_vuln, DDS_TABLE_MODE) ;
        }
    }  /* end if CacheState */
 #ifdef JGMDBG
    if (jgmDebug >= 6 ) {
       fprintf(stderr, "IN DDS_TRICKS.155 res_calls=%d ngen=%d dds_dealnum=%d, trix[%d,%d]=%d \n",
            dbg_dds_res_calls, ngen, dds_dealnum, compass, strain, dds_res_bin.tricks[compass][strain] );
   }
 #endif

    // assert The cache entry for dds_res_bin.tricks[compass][strain] is valid

    /* Just a normal compass, north,east,south,west. */
    /* Then return the relevant result from the cache */
    t0 = dds_res_bin.tricks[compass][strain];
    t1 = dds_res_bin.parScore_NS ;
#ifdef JGMDBG
    if (jgmDebug >= 6 ) {
       fprintf(stderr, "Leaving dds_tricks.172 ngen=%d  lib_calls=%d, res_calls=%d tricks=%d, par=%d\n",
                           ngen, dbg_dds_lib_calls, dbg_dds_res_calls, t0, t1 );

       fprintf(stderr,"DDS tricks.174 RETURNS dds_res_bin[%d][%d] = %d ******* \n",
                        compass, strain, dds_res_bin.tricks[compass][strain] );
    }
#endif
       return dds_res_bin.tricks[compass][strain];
} /* end dds_tricks */

DDSRES_k true_CalcTable(deal dl, int vul, int dds_mode ) {  // Mode should always be DDS_TABLE_MODE here...
    int dds_rc = 1; /* OK*/
    int si, so, h, t;
    struct ddTableResults    Res_20;    /* 20 Results */
    struct ddTableDeal       dds_BIN_deal; /* Deal in DDS binary suitable for input to CalcDDtable */
    struct parResultsMaster  sidesRes[2] ;
    DDSRES_k DealerRes ;
    char Denom[]   ="NSHDC";  // Different order yet again ... for the parResultsMaster structure
    char seatNames[6][3] = { {"N"},{"E"},{"S"},{"W"},{"NS"},{"EW"} };
    char line[128] ; // for DDS error messages

    // dbg_dds_lib_calls and dbg_dds_res_calls updated by dds_tricks before calling this function


#ifdef JGMDBG
        if (jgmDebug >= 7 ) {
           fprintf(stderr, "IN trueCalcTable.203 dbg_dds_lib_calls=%d, ngen=%d, dealnum=%d --  jgmDebug=%d, dds_mode=%d\n",
                           dbg_dds_lib_calls, ngen, dds_dealnum, jgmDebug, dds_mode );
        }
#endif
        dds_BIN_deal =  Deal52_to_DDSBIN (dl);
        dds_rc = CalcDDtable(dds_BIN_deal, &Res_20 );
#ifdef JGMDBG
        if (jgmDebug >= 7 ) { fprintf(stderr, "  CalcDDtable returned with %d \n", dds_rc); }
#endif
        // check return code; print error msg if not OK  NO_FAULT is 1
        if (dds_rc != RETURN_NO_FAULT)  {
            ErrorMessage(dds_rc, line);
            fprintf(stderr, "Table Mode DDS error: %s\n", line);
            strncpy(DealerRes.ddsmsg, line, 40);
            DealerRes.errcode = -1 ;
        }
        dds_rc = SidesParBin(&Res_20, sidesRes, par_vuln);  /* calculate Par result */
            // check return code; print error msg if not OK
        if (dds_rc != RETURN_NO_FAULT)  {
            ErrorMessage(dds_rc, line);
            fprintf(stderr, "Table Mode DDS error: %s\n", line);
            strncpy(DealerRes.ddsmsg, line, 40);
            DealerRes.errcode = -1 ;
        }
#ifdef JGMDBG
        if (jgmDebug > 7) { showRawResults(&Res_20) ; }
#endif
        // successful DDS call. Fill the Dealer Results struct and return it
        DealerRes.errcode = RETURN_NO_FAULT ; // success
            // fill the tricks results array
        for (si=0; si < DDS_STRAINS ; si++ ) {
            for (h=0 ; h < DDS_HANDS ; h++ ) {
                t = Res_20.resTable[si][h]; // Tricks
                so =(si == 0 ) ?  3 :     // Spades
                    (si == 1 ) ?  2 :     // Hearts
                    (si == 2 ) ?  1 :     // Diamonds
                    (si == 3 ) ?  0 :     // Clubs
                                  4 ;     // si must equal 4 aka No Trump
                DealerRes.tricks[h][so] = t ;
            } /* end for h < DDS_HANDS */
        } /* end for si < DDS_STRAINS */

#ifdef JGMDBG
        if (jgmDebug > 7) { showReturns(&DealerRes) ;  }
#endif
        // report the par score and (one of) the corresponding contract(s)
        DealerRes.parScore_NS = sidesRes[0].score;
        sprintf( DealerRes.ddsmsg,"%2d%c by %s  ", sidesRes[0].contracts[0].level,
                        Denom[sidesRes[0].contracts[0].denom], seatNames[sidesRes[0].contracts[0].seats] );
        DealerRes.CacheStatus = CACHE_OK ;
        dds_dealnum = ngen ; /* mark the cache as up to date. */
         int t1 = DealerRes.parScore_NS ;
         int t0 =  DealerRes.tricks[0][3] ; /* North Spades Just for Debug */

#ifdef JGMDBG
        if (jgmDebug >= 7) {
            fprintf(stderr, "Done trueCalcTable.259 : North Spades Tricks=%d, parScore_NS=%d \n ", t0, t1 );
        }
#endif
        return DealerRes ;
} /* end true_CalcTable*/

/* end true_CalcTable */
void init_deal_st (struct deal_st *dl_ptr) { /* deal_st is what is passed to the DDS solveOneBoard routine */
    int h, s ;
    dl_ptr->first = 0 ;
    dl_ptr->trump = 0 ;
    for (h=0 ; h<3 ; h++ ) {
        dl_ptr->currentTrickSuit[h] = 0;
        dl_ptr->currentTrickRank[h] = 0;
    }
    for (h=0 ; h<4 ; h++ ) {
        for (s = 0; s < 4 ; s++ ) {
            dl_ptr->remainCards[h][s] = 0 ;
        }
    }
} /* end init_deal_st */

// true_SolveBoard called with Dealer encoding for strain where 0=clubs and 3=spades
int true_SolveBoard (deal curdeal, int h, int s ) {
struct futureTricks        fut;
struct deal_st              dl;
int rc, trix ;
int lho[4]={1,2,3,0} ; // The lho[compass] is the opening leader we pass to SolveOneBoard
int dds_strain[5]={3,2,1,0,4} ; /* translate from Dealer stain to DDS strain; Clubs=3, Spades=0 NT = 4*/
char line[120] ;
// dbg_dds_lib_calls and dbg_dds_res_calls updated by dds_tricks before calling this function
// memset( &dl, 0 , sizeof(dl) ); /* set the whole struct to zeroes */
  init_deal_st ( &dl ) ;

 #ifdef JGMDBG
    if (jgmDebug >= 7) {
         fprintf( stderr, "true_SolveBoard.295 dl struct set to zero.ngen=%d, trump=[%d], first=[%d], dds_mode=%d\n",
                     ngen, dl.trump, dl.first, dds_mode ) ;
    }
#endif
  rc =  Deal52_to_Holding(curdeal, dl.remainCards);
 #ifdef JGMDBG
    if (jgmDebug >= 8) {
        fprintf(stderr, "true_SolveBoard.302 Deal52_To_Holding Done. Calling dump_Deal\n");
        dump_Deal(dl)  ;
    }
#endif

//syntax dl.trump dl.first dl.remainCards[Compass][Suit] dl.currentTrickSuit[3] dl.currentTrickRank[3]=0,
 dl.first = lho[h];  /* solveOneboard results depend on who is on lead. CalcDDtable results dont */
 dl.trump = dds_strain[s] ;
 #ifdef JGMDBG
    if (jgmDebug >= 6) {
        fprintf(stderr, "true_SolveBoard.312 Calling Solveboard ,Dealer[trump=%d, Decl=%d] DDS[ trump=%d, First=%d] ,dbg_dds_lib_calls=%d\n",
              s,h, dl.trump, dl.first, dbg_dds_lib_calls );
    }
#endif
 rc = SolveBoard(dl, -1, 1, 0, &fut, 0 ) ;
 if (rc != RETURN_NO_FAULT) {
    ErrorMessage(rc, line);  // convert the numeric err msg to text in the char array line[]
    fprintf(stderr, "DDS error [%d] in true_SolveBoard\n",rc );
    assert(0);
 }
 dds_dealnum = ngen;
 dds_res_bin.CacheStatus = CACHE_OK ;
 trix = 13 - fut.score[0];  /* since we have calc the trix for our LHO our result is 13 - his */
 #ifdef JGMDBG
    if (jgmDebug >= 7) {
        fprintf(stderr, "true_SolveBoard.326 tricks=[%d , %d], ngen=[%d], dds_dealnum=[%d], dds_lib_calls=[%d], dds_res_calls=[%d]\n",
                                       fut.score[0], trix, ngen,dds_dealnum,dbg_dds_lib_calls, dbg_dds_res_calls ) ;
    }
 #endif
 return trix ;
} /* End true_SolveBoard */
/* End true_SolveBoard */

int dds_parscore(int compass ) {
    int CacheState;
    dbg_parscore_calls++;
#ifdef JGMDBG
    if (jgmDebug >= 6 ) {fprintf(stderr, "IN DDS_PARSCORE.333 par_calls=%d ngen=%d dds_dealnum=%d, dds_mode=%d \n",
                           dbg_parscore_calls, ngen, dds_dealnum, dds_mode ); }
#endif

    if (dds_mode != DDS_TABLE_MODE) {
        fprintf(stderr, "In ParScore:: Using Parscore requires TableMode. Setting Mode and continuing..\n");
        SetDDSmode(DDS_TABLE_MODE) ;
        dds_dealnum = -1 ; //force a call to fill the Cache
    }
    CacheState = CheckCache(&dds_res_bin, ngen, dds_dealnum , 0, 0 ) ;
    if    (CacheState == CACHE_INV || CacheState == CACHE_UPD ) {
        dds_res_bin.CacheStatus = CACHE_INV ;
        dbg_dds_lib_calls++;
        dds_res_bin = true_CalcTable (curdeal, par_vuln, DDS_TABLE_MODE) ;
    }
#ifdef JGMDBG
    int t1 = dds_res_bin.parScore_NS ;
    if (jgmDebug >= 6) { fprintf(stderr, "Leaving DDS_PARSCORE.350 dbg_dds_lib_calls=%d, par=%d\n",
                           dbg_dds_lib_calls, t1 ); }
#endif
    /* at this point the global cache of 20 results + a par score is valid */
     if(compass == COMPASS_NORTH || compass == COMPASS_SOUTH || compass == SIDE_NS ) {
         return dds_res_bin.parScore_NS ;
     }
     else return -(dds_res_bin.parScore_NS) ;
  } /* end dds_parscore */

/* csv_trix will return tricks for all 5 strains,  for any number of hands 1-4 */
int csv_trix( char *buff, int h_mask ) {  //ngen, dds_dealnum, dds_res_bin, dds_mode are globals
    int t0, t1;
    int CacheState ;
    int i, h, s, nch ;

    dbg_dds_res_calls++;
 #ifdef JGMDBG
    if (jgmDebug >= 5 ) {
       fprintf(stderr, "IN csv_trix res_calls=%d ngen=%d dds_dealnum=%d,h_mask=%d, dds_mode=%d \n",
                           dbg_dds_res_calls, ngen, dds_dealnum, h_mask, dds_mode ); }
 #endif
     if (dds_mode != DDS_TABLE_MODE) {  /* this will only happen once */
        fprintf(stderr, "In csv_trix:: Using csv_trix requires TableMode. Setting Mode and continuing..\n");
        SetDDSmode(DDS_TABLE_MODE) ;
        dds_dealnum = -1 ; //force a call to fill the Cache
    }
    CacheState = CheckCache(&dds_res_bin, ngen, dds_dealnum, COMPASS_SOUTH, SUIT_NT ) ; /* in mode 2 any combo will do */
 #ifdef JGMDBG
    if (jgmDebug >= 5 ) {
       fprintf(stderr, "csv_trix: CheckCache returns[%d], ngen=%d,dds_dealnum=%d, trix=%d \n",
                        CacheState, ngen, dds_dealnum, dds_res_bin.tricks[0][0] );
    }
 #endif

    if (CacheState != CACHE_OK ) { /* ngen is on a new deal */
        ZeroCache( &dds_res_bin);
        dds_res_bin.CacheStatus = CACHE_INV ;
        dbg_dds_lib_calls++;

#ifdef JGMDBG
         if (jgmDebug >= 5 ) {fprintf(stderr, "IN csv_trix:: CacheInvalid lib_calls=%d, mode=%d, dds_dealnum=[%d]  \n",
                           dbg_dds_lib_calls, dds_mode, dds_dealnum ); }
 #endif
        dds_res_bin = true_CalcTable (curdeal, par_vuln, DDS_TABLE_MODE) ;
     }  /* end if CacheState */
 #ifdef JGMDBG
    if (jgmDebug >= 6 ) {fprintf(stderr, "IN csv_trix:: true_CalcTable Done. res_calls=%d ngen=%d dds_dealnum=%d \n",
                           dbg_dds_res_calls, ngen, dds_dealnum ); }
 #endif

    // assert The cache entry for dds_res_bin.tricks[compass][strain] is valid


#ifdef JGMDBG
    if (jgmDebug >= 6 ) {
       fprintf(stderr, "Leaving csv_trix.174 ngen=%d  lib_calls=%d, res_calls=%d tricks=%d, par=%d\n",
                           ngen, dbg_dds_lib_calls, dbg_dds_res_calls, t0, t1 );
    }
#endif
      // now format the buffer giving trix in all 5 strains for each hand asked for.
      nch = 0 ;
      for(i = 0 ; i < 4 ; i++ ) {
         h = (i + csv_firsthand ) & 3 ;
         if ( !(h_mask & 1 << h) ) { continue ; }
      #ifdef JGMDBG
         if (jgmDebug >= 6 ) { fprintf(stderr, "hand=%d, ",h); }
      #endif
         for (s=0; s < 5 ; s++ ) { // for each strain clubs to NT
            nch += sprintf(buff+nch, "%d,", dds_res_bin.tricks[h][s] ) ;
      #ifdef JGMDBG
         if (jgmDebug >= 6 ) { fprintf(stderr, "s=%d, tricks=%d, nch=%d, ",s,dds_res_bin.tricks[h][s], nch ); }
      #endif
           } /* end for s */
       #ifdef JGMDBG
         if (jgmDebug >= 6 ) { fprintf(stderr,"\n"); }  // begin next hand debug on new line
      #endif
      } /* end for i  aka hand */
         buff[nch-1] = '\0' ; // replace last comma with null terminator
         return nch-1 ;       // return length of the trix buffer contents

} /* end csv_trix */


