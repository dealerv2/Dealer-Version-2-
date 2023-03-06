/* File dealeval_subs.c  JGM 2022-Feb-15
 *  2021-11-25 -- Removing refs to library file and Francois, and Microsoft */
/*  2022-01-10  JGM  Adding code for OPC trees */
/*  2022-03-07  JGM  Fine tune debug levels
 *  2023/01/07 -- Merged in changes from V4 to fix predeal; dealcards_subs.c and globals, etc.
 */
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
#define SAFETY 0
// This next def not used but put here for documentation. bridge is a bad name for linux; it refers to LAN card setup
#ifndef DDPGM
  #define DDPGM /usr/games/gibcli
#endif
#ifndef DDS_TABLE_MODE
   #define DDS_TABLE_MODE 2      // import from the DDS namespace
#endif
#include "../include/std_hdrs.h"
#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/dealexterns.h"
#include "../include/dealprotos.h"
#include "../include/dealdebug_subs.h"
#include "../include/dbgprt_macros.h"
#ifndef intresting
#define interesting() ((int)evaltree(decisiontree))
#endif
void show_opcRes(struct opc_Vals_st *resp ) {
   int i ;
   fprintf(stderr, "OPC_RESULTS in FLOATS\n");
   fprintf(stderr, "DOP_long=[%8.2f], ", resp->DOP_long ) ;
   for (i=0;i<5;i++) { fprintf(stderr, "DOP_strain[%d]=[%8.2f], ",i, resp->DOP[i] ) ; }
   fprintf(stderr, "QL_SUIT=[%8.2f], QL_NT=[%8.2f] \n", resp->QL_suit, resp->QL_nt );
   fprintf(stderr, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   return ;
}

void show_opcVals(struct sidestat_st *resp ) {
   int i ;
   fprintf(stderr, "OPC_RESULTS in SIDESTAT\n");
   fprintf(stderr, "DOP_long=[%d], ", resp->ss_opc_hldf_l ) ;
   fprintf(stderr, "DOP_NT=[%d], ",   resp->ss_opc_hlf_nt ) ;
   for (i=0;i<4;i++) { fprintf(stderr, "DOP_strain[%d]=[%d], ",i, resp->ss_opc_hldf[i] ) ; }
   fprintf(stderr, "QL_SUIT=[%d], QL_NT=[%d] \n", resp->ss_quick_losers_suit, resp->ss_quick_losers_nt );
   fprintf(stderr, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   return ;
}


int opc_value(int side, int strain, struct tree *t ) ;
int isCard (char crd) {
   char Ranks[14]="23456789TJQKA" ;
   unsigned char uc ;
   int rc = 0 ;
   uc = toupper((unsigned char ) crd ) ;
   for (rc = 0 ; rc<14 ; rc++ ) {
      if ( uc == Ranks[rc] )  return rc ;
   }
   return -1;
}


/* search 13 slots in deal looking  for one specific card -- original version of hascard
 *  we need this slower version for actions at end of run, when the handstat struct is no longer current
 * even tho it can take over 600 accesses to the deal to print one hand
 */
int hasKard (deal d, int player, card thiscard){
  int i;
  for (i = player * 13; i < (player + 1) * 13; i++)
    if (d[i] == thiscard) return 1;
  return 0;
} /* end hasKard */


/* JGM redo to use the Has_card[s][r] array in handstat keep same prototype even tho deal not needed anymore */
int hascard (deal d, int player, card thiscard) {
   #ifdef JGMDBG
    if (jgmDebug >= 7 ) {
       int s, r;
       s= CARD_SUIT(thiscard) ; r=CARD_RANK(thiscard) ;
        fprintf(stderr, "EvalSubs.Has_Card %2x,hs[%d].Has_Card[%d][%d]=%d \n", thiscard, player, s,r,hs[player].Has_card[s][r]);
    }
#endif
  return (hs[player].Has_card[CARD_SUIT(thiscard)][CARD_RANK(thiscard)] );
}

void predeal_cmdparms(int compass, char *holding) { // preDeal[n]
        char suit, ch;
        size_t holding_len ;
        int  l  ;
        int cnt = 0 ;
        holding_len = strlen(holding) ;

#ifdef JGMDBG
          if(jgmDebug >= 4 ) {
             fprintf(stderr, "Predeal_cmdparms:: Compass=%d, holding=[%s]\n", compass, holding) ;
          }
#endif
        suit = ' ';
        for (l = 0 ; l < holding_len; l++ ) {
            ch = *(holding + l) ;

            if (ch == 'C' || ch == 'D' || ch == 'H' || ch == 'S' ) { suit = ch ; }
            else if (isCard(ch) >= 0 ) { predeal(compass, make_card(ch, suit)  );  cnt++ ;}
            /* if not a suit or  card skip it; probably a comma or other suit separator */
        }
   #ifdef JGMDBG
          if(jgmDebug >= 4 ) {
             fprintf(stderr, "Predeal_cmdparms:: Holding len=%ld, cards predealt = %d\n", holding_len, cnt ) ;
          }
   #endif

} /* end predeal_cmdparms() */

/* Over-ride what was in input file with what was entered on the cmd line via switches */
void initprogram ( struct options_st *opt_ptr) {
 /* this function primarily allows user to override what is in the input file.
  * also does some rudimentary consistency checking
  */
  int i;
  void ZeroCache( DDSRES_k *Results) ;

    /* if the use_side flag is set, we need to set the use_compass flags for both seats regardless of what the parser did*/
    if (use_side[0] == 1 ) {use_compass[0] = 1; use_compass[2]= 1;  } /* NS */
    if (use_side[1] == 1 ) {use_compass[1] = 1; use_compass[3]= 1;  } /* EW */
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
       title_len = opt_ptr->title_len  ;
       strncpy( title, opt_ptr->title, MAXTITLESIZE - 1 );
    }
    JGMDPRT(4,"NewTitle=[%s], NewTitleLen=[%ld]\nCmdlineTitle=[%s],CmdLineTitleLen=[%ld]\n",
                  title, title_len, opt_ptr->title, opt_ptr->title_len ) ;

     /* need to check the seed stuff here, since yyparse() may have set it also */
    if( opt_ptr->seed_provided > 0 ) {  /* getopts will set this to 1 if there is a -s switch on cmd line */
       seed_provided = opt_ptr->seed_provided  ;
       seed = opt_ptr->seed;
    }
    JGMDPRT(4, "New Seed[%ld],Cmdline seed[%ld]\n", seed, opt_ptr->seed ) ;

    /* Next we allow user to over-ride any predeal statements in the input file via cmd line switches */
    /* Undo any predealing yyparse has done */
     if (opt_ptr->preDeal_len[0] > 0 || opt_ptr->preDeal_len[1] > 0 ||
         opt_ptr->preDeal_len[2] > 0 || opt_ptr->preDeal_len[3] > 0 )  {
        JGMDPRT(4, "Setting up predeal from the cmdline \n");
        /* Reset everything relating to deal initialization to reverse whatever yyparse might have done */
        newpack(fullpack) ;
        memcpy(small_pack, fullpack, 52) ;
        memset(stacked_pack, NO_CARD, 52 ) ;
        stacked_size = 0 ;
        small_size = 52 ;
        memset(curdeal, NO_CARD, 52 ) ;
        for (i =0 ; i < 4 ; i++ ) {
           if (opt_ptr->preDeal_len[i] > 0 ) {
              JGMDPRT(4, "Calling Predeal hand = %c [%s] \n", "NESW"[i], opt_ptr->preDeal[i] );
              predeal_cmdparms(i, opt_ptr->preDeal[i] ) ;
           }
        }
      } /* end if checking the lengths for predeal */
      #ifdef JGMDBG
         if (jgmDebug >=4 ) {
            fprintf(stderr, "init_program.244:: Predeal check, stacked_size=%d FullPack, StackedPack, curdeal\n",stacked_size);
            sr_deal_show(fullpack);
            sr_deal_show(stacked_pack);
            sr_deal_show(curdeal) ;
         }
      #endif

    /* These next ones are never set in the Input file */
    /* These next two are set by main before calling getopts, but may have been overridden */
    if ( par_vuln != -1 ) { /* user has said he will be doing Par calcs */
        if (dds_mode != DDS_TABLE_MODE ) {
           dds_mode = DDS_TABLE_MODE ;
        } /* Must do full TableCalc to get par scores */
    }

    ZeroCache(&dds_res_bin) ; /* dds_res_bin is a global struct holding tricks for 20 combos of leader and strain */
    JGMDPRT(4, "Done initprogram.260 dds_mode=%d, Dbg_Verbosity=%d, vers=%s\n", dds_mode, jgmDebug, VERSION ) ;
} /* end init program */

/* Various handshapes can be asked for. For every shape the user is
   interested in a number is generated. In every distribution that fits that
   shape the corresponding bit is set in the distrbitmaps 4-dimensional array.
   This makes looking up a shape a small constant cost.
*/
void initdistr () {
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

// The 'tops' functions are related to the new modern ltc that calculates in half-losers by JGM
void dbgtops(int topcards[NSUITS][3]) {
  int s;
  for (s=0; s<4; s++) {
    fprintf(stderr, "topcards-suit#%i: %i  %i  %i\n",s, topcards[s][0], topcards[s][1], topcards[s][2]   );
  }
} /* end dbgtops */
void init_tops(struct handstat *hs ) {
    int s ;
    for (s = SUIT_CLUB; s <= SUIT_SPADE; s++) {
        hs->topcards[s][0] = LTC_VOID_WEIGHT;
        hs->topcards[s][1] = LTC_VOID_WEIGHT;
        hs->topcards[s][2] = LTC_VOID_WEIGHT;
//        JGMDPRT(8,"Init hs->topcards[%d][0,1,2]=t%d,%d,%d\n ",s,(hs->topcards[s][0],hs->topcards[s][1],hs->topcards[s][2] );
    } /* end for SUIT */
    return ;
} /* end init_tops */

void upd_topcards(struct handstat *hs, int suit,int  rank) {
  int   weight;
  assert(suit<=3);
  assert(rank<13);
  weight = CardAttr_RO[idxLTCwts][rank];


 JGMDPRT(9,  "debug: upd_topcards:: suit=%i  rank=%i  weight=%i  \n", suit, rank, weight);

  if      (hs->topcards[suit][0] == LTC_VOID_WEIGHT) {hs->topcards[suit][0] = weight;}
  else if (hs->topcards[suit][0] < weight) {  /* latest card has higher rank than previous highest move them all down */
           hs->topcards[suit][2]= hs->topcards[suit][1];
           hs->topcards[suit][1]= hs->topcards[suit][0];
           hs->topcards[suit][0]= weight;}
  else if (hs->topcards[suit][1] == LTC_VOID_WEIGHT) {hs->topcards[suit][1] = weight;}
  else if (hs->topcards[suit][1] < weight) {
           hs->topcards[suit][2]= hs->topcards[suit][1];
           hs->topcards[suit][1]= weight; }
  else if (hs->topcards[suit][2] == LTC_VOID_WEIGHT) {hs->topcards[suit][2] = weight;}
  else if (hs->topcards[suit][2] < weight) {hs->topcards[suit][2]= weight; }

  JGMDPRT(9, "Top Cards Weights=%d, %d, %d\n",hs->topcards[suit][0],hs->topcards[suit][1],hs->topcards[suit][2]);

  return ;
} /* end upd top cards */

int countltc( struct handstat *hs) {
        /* update both the modern LTC (which allows half losers) and old style losers also */
  int s, ltc_weight;
  int ltc_suit, losers_suit;
  hs->hs_totalltc = 0;
  for (s=SUIT_CLUB; s<=SUIT_SPADE; s++) {
    ltc_weight = hs->topcards[s][0] + hs->topcards[s][1] + hs->topcards[s][2];

   JGMDPRT(9,  "debug: countltc:: SUIT=%i, hs->topcards=%i  %i  %i Tot Weight=%d\n",
               s, hs->topcards[s][0], hs->topcards[s][1], hs->topcards[s][2], ltc_weight);
    switch (ltc_weight) {  /* we count losers x 100 to avoid decimal places */
    case 56:   /* AKQ */
    case 112:  /* AK- */
    case 160:  /* A-- */
    case 192:  /* --- */
        {ltc_suit= 0; losers_suit= 0 ; break;}  /* 0 losers for AKQ(56), AK-, A--, --- */
    case 44:  /* AQJ */
    case 52:  /* AKJ */
    case 104: /* AQ- */
    case 144: /* K-- */
        {ltc_suit= 50;  losers_suit = 1 ; break;} /* 0.5 half a loser for combos like AQJ, AQ-*/
    case 25:  /* KQx */
    case 26:  /* KQT */
    case 28:  /* KQJ */
    case 41:  /* AQx */
    case 42:  /* AQT */
    case 49:  /* AKx */
    case 50:  /* AKT */
    case 81:  /* Kx- */
    case 82:  /* KT- */
    case 84:  /* KJ- */
    case 88:  /* KQ- */
    case 97:  /* Ax- */
    case 98:  /* AT- */
    case 100: /* AJ- */
    case 129: /* x-- */
    case 130: /* T-- */
    case 132: /* J-- */
    case 136: /* Q-- */
        {ltc_suit = 100; losers_suit = 1; break;} /* one loser */
    case 22:  /* KJT */
    case 38:  /* AJT */
        {ltc_suit = 150; losers_suit = 2; break;} /* 1.5 losers for KJT and AJT */
    case 13:  /* QJx */
    case 14:  /* QJT */
    case 18:  /* Kxx */
    case 19:  /* KTx */
    case 21:  /* KJx */
    case 34:  /* Axx */
    case 35:  /* ATx */
    case 37:  /* AJx */
    case 66:  /* xx- */
    case 67:  /* Tx- */
    case 69:  /* Jx- */
    case 70:  /* JT- */
    case 73:  /* Qx- */
    case 74:  /* QT- */
    case 76:  /* QJ- */
        {ltc_suit = 200; losers_suit = 2; break;}  /* Two losers */
    case 10:  /* Qxx */
    case 11:  /* QTx */
        {ltc_suit = 250; losers_suit = 2; break;}  /* 2.5 losers for Qxx or QTx */
    case 3:  /* xxx */
    case 4:  /* Txx */
    case 6:  /* Jxx */
    case 7:  /* JTx */
        {ltc_suit = 300; losers_suit = 3; break;} /* 3 losers */
      default:  { fprintf(stderr, "ERROR! suit:[%d] invalid weight[%d]\n", s, ltc_weight);
                  dbgtops(hs->topcards);
                   assert(0);
              break;  }
    }  /* end switch ltc_weight */
    hs->hs_ltc[s] = ltc_suit;
    hs->hs_totalltc += ltc_suit;
    hs->hs_loser[s] = losers_suit ;
    hs->hs_totalloser += losers_suit ;

    JGMDPRT(9, "countltc::  For Suit#[%d]:: suit ltc= %d  totalltc=%i\n",s, ltc_suit, hs->hs_totalltc);
    JGMDPRT(9,  "countltc::                 Old Losers=%d  TotalLosers=%d\n", losers_suit,  hs->hs_totalloser);

  }  /* end for s */
  return hs->hs_totalltc;
} /* end countltc */
void fill_Has_card(deal d, struct handstat *hsbase ) {
   char k ;
   int p,r,s,c,t;
   t = 0 ;
   JGMDPRT(7,  "fill_Has_card called for ngen=%d\n", ngen ) ;

   for ( p = 0; p <= 3 ; p++ ) {
      memset(hsbase[p].Has_card, 0, sizeof(hsbase[p].Has_card) ) ; /* zero out the current hand 52 item Has_card */
      JGMDPRT(9, "p=%d,@p=%p, card0=%d,card52=%d\n",
            p,(void *)&hsbase[p], hsbase[p].Has_card[0][0],  hsbase[p].Has_card[3][12] );
      for ( c = 0 ; c < 13 ; c++ ) {  /* 13 cards in this player's hand. set them to 1 */
            k = d[t++] ;
            r = CARD_RANK( k );
            s = CARD_SUIT( k );
            hsbase[p].Has_card[s][r] = 1 ;

//          JGMDPRT(10,"p=%d,k=%02x, s=%d,r=%d,hascard=%d\n",p, k, s, r ,hsbase[p].Has_card[s][r] );
      }
#ifdef JGMDBG
   if (jgmDebug >=9 ) {
      fprintf(stderr, "Hascard for Player=%d Spade Ace downto Club deuce\n",p);
      for (s=3 ; s>=0 ; s-- ) {
         for (r=12 ; r>=0 ; r-- ) {
            fprintf(stderr, "%2d",hsbase[p].Has_card[s][r] ) ;
         }
         fprintf(stderr, ": " );
      }
      fprintf(stderr, "\n");
   }
#endif
   } /* end for p = 0 line 360 */
} /* end fill_Has_card line 354 */

      /*
       * populate the handstat structures for later use by eval routines -- ends about line 600
       */
void analyze (deal d, struct handstat *hsbase) {
   /* Analyze a hand. Since this function is called on EVERY deal only those aspects of a hand that are:
    * 1) Relatively quick to generate and 2) very likely to be used should be done here.
    * Metrics that are slow (opc, tricks, dds, par, evalcontract ) or infrequently used (maybe ltc?) should not be done here.
    */
  /*
   * Modified by HU to count controls and losers.
   * Further mod by AM to count several alternate pointcounts too
   * JGM Fixed bug in altpoint count reset routine -- altcount n did not reset table n but table n-2
   * Further mod by JGM to for Modern LTC with half losers defined, also create Has_card array to speed up hascard function
   */
  int player, next, c, r, s, t;
  card curcard;
  struct handstat *hs;
  JGMDPRT(7, " Analyzing Current Deal for ngen=%d, jgmDebug=%d\n", ngen, jgmDebug);
  #ifdef JGMDBG
    if(jgmDebug >= 9) {    sr_deal_show(d); }  // was dump_curdeal
  #endif
                       /* for each player loop runs from here to line 598*/
  for (player = COMPASS_NORTH; player <= COMPASS_WEST; player++ ) {
    /* If the expressions in the input never mention a player we do not calculate his hand statistics. */
   JGMDPRT(7,"Doing Handstat for player=%d usePlayer=%d in analyze before memset stuff\n", player,use_compass[player]);
   if (use_compass[player] == 0) { /* skip players that did not appear in input file */
                                    /* JGM: some side functions set both players for the side */
        #ifdef JGMDBG
             /* In debug mode, blast the UNUSED handstat, so that we can recognize it */
             /*   as garbage should if we accidently read from it
              *   When sidestat implemented will have to do the same for it
              */
            hs = hsbase + player;  /* player is unused here. blasting memory we should not be accessing */
            memset (hs, 0x7E, sizeof (struct handstat));
        #endif /* JGMDBG */
        JGMDPRT(8,"Skipping Unused player=%d in analyze after memset stuff\n", player);
        continue;  /* skip the rest of the "for player" loop lines 470 - 550 */
    } /* end if use_compass */

    hs = hsbase + player;  /* player is of interest here. */
    JGMDPRT(7,"hs base=%p, player=%d, hs=%p sizeof_hs=%lx\n",(void *)hsbase, player, (void *) hs, sizeof(struct handstat) ) ;
    memset (hs, 0x00, sizeof (struct handstat)); // safe since only ints in this struct
    hs->hs_seat = player; /* make it easy to deduce player given the ptr */
    init_tops(hs);  /* set the top cards to voids to begin */

     /*
      * start from the first card for this player, and walk through them all -
       use the player offset to jump to the first card.  Can't just increment through the deck, because
       we skip those players who are not part of the analysis.
       JGM has added code so that if SIDE  NS is asked for, both compasses set; ditto for EW
    */

    next = 13 * player;
    JGMDPRT(7,"Analyze::Debugging player[%d] at deal_offset=%d in Analyze after memset and init_tops\n",player,next  );
    for (c = 0; c < 13; c++) {  /* 13 cards in player's hand runs to line 545 or so*/
      curcard = d[next++];
      s = C_SUIT (curcard);
      r = C_RANK (curcard);

     JGMDPRT(9,"Player=%c, curr Card=%c %c \n", seat_id[player],strain_id[s],card_id[r] );
      hs->hs_length[s]++;
      hs->hs_points[s]    += points[r];  /* JGM hcp in this suit */
      hs->hs_totalpoints  += points[r];  /* JGM total hcp in the hand */
      hs->hs_control[s]   += CardAttr_RO[idxControls][r]; /* JGM Add */
      hs->hs_totalcontrol += CardAttr_RO[idxControls][r];
      upd_topcards(hs, s, r) ; /* update the top cards array  used for modern ltc*/
#ifdef JGMDBG
      if (jgmDebug >= 9 ) {
             fprintf(stderr, "JGM-HCP[s=%d,r=%d] incr=%d, SuitTot=%d, HandTot=%d\n",
                          s,r,points[r], hs->hs_points[s], hs->hs_totalpoints );
             fprintf(stderr, "JGM-CTL[s=%d,r=%d] incr=%d, Suit_CTLS=%d, HandCTLS=%d\n",
                          s,r, CardAttr_RO[idxControls][r],hs->hs_control[s],hs->hs_totalcontrol);
      }
      if (jgmDebug >= 9 ) {
             fprintf(stderr, "PntCntType   Incr    Tot\n");
      }
#endif
      for (t = 0; t < idxEnd; ++t) { /* begin filling the alternate (point)count arrays 0 .. 9 & idxHCP*/
        hs->hs_counts[t][s]   += tblPointcount[t][r]; /* tblPointcount init at compile time. see dealglobals.c */
        hs->hs_totalcounts[t] += tblPointcount[t][r]; /* add to total and to suit at same time. Keep in sync */
#ifdef JGMDBG
        if (jgmDebug >= 9 ) {
             fprintf(stderr,"%d, %d %d \n", t, tblPointcount[t][r], hs->hs_counts[t][s]);
        }
#endif
      } /* end for t line 468 */
#ifdef JGMDBG
     if (jgmDebug >= 9 ) {fprintf(stderr,  "\n");} // finish off the debug printout for the array
#endif
    }  /* end for curr card line 445*/
    /* finished the card by card totals All cards in hand accounted for handstat almost complete */

    countltc(hs); /* calculate the ltc and the losers for each suit and total for hand */
    /* Check for 4333 square hand; KnR, DOP, many others will use this. */
    hs->square_hand = 0 ;
    if  (( hs->hs_length[0] == 3 ||  hs->hs_length[0] == 4 ) &&
         ( hs->hs_length[1] == 3 ||  hs->hs_length[1] == 4 ) &&
         ( hs->hs_length[2] == 3 ||  hs->hs_length[2] == 4 ) &&
         ( hs->hs_length[3] == 3 ||  hs->hs_length[3] == 4 ) )
         { hs->square_hand = 1 ; }

      JGMDPRT(7,"Hand tot HCP=%d PlayerName=%s\n",hs->hs_totalpoints, player_name[player] );
      JGMDPRT(7,"totCounts[%d]=%d PlayerName=%s\n", idxHcp, hs->hs_totalcounts[idxHcp], player_name[player]  );

    // set the shape bit for this hand pattern.
    hs->hs_bits = distrbitmaps[hs->hs_length[SUIT_CLUB]]
                                 [hs->hs_length[SUIT_DIAMOND]]
                                 [hs->hs_length[SUIT_HEART]]
                                 [hs->hs_length[SUIT_SPADE]];
      JGMDPRT(8, "HS Distr Bits for %c= %04X : [%d] \n", "nesw"[player], hs->hs_bits, hs->hs_bits );
#ifdef JGMDBG
      if(jgmDebug >= 7 ) {
         fprintf(stderr, "Losers[C,D,H,S,Tot]:%d, %d, %d, %d, = %d\n",
            hs->hs_loser[0],hs->hs_loser[1],hs->hs_loser[2],hs->hs_loser[3],hs->hs_totalloser);
         fprintf(stderr, "Controls[C,D,H,S, Tot]:%d, %d, %d, %d, = %d\n",
            hs->hs_control[0],hs->hs_control[1],hs->hs_control[2],hs->hs_control[3],hs->hs_totalcontrol);
         fprintf(stderr, "LTC[C,D,H,S, Tot]:%d, %d, %d, %d, = %d\n",
            hs->hs_ltc[0],hs->hs_ltc[1],hs->hs_ltc[2],hs->hs_ltc[3], hs->hs_totalltc);
      } /* end jgmDebug >= 9 */
#endif
JGMDPRT(7,"Finished analyzing Player %d in_use=%d \n",player, use_compass[player]);
   }  /* end for each player loop; starts around line 409 ... */
   /* set the Has_card array for all 4 players, as we may need to print them out even if no analyze done. */
   fill_Has_card(d, hsbase) ;
   return ;
} /* end analyze deal starts at line 390 or so*/

// end analyze deal (starts around line 390 )

int evaltree (struct tree *t) {                 /* walk thru the user's request and compare to handstat */
   JGMDPRT(8, "evaltree treetype[%d] tr_int1=%d, tr_int2=%d \n",
                        t->tr_type, t->tr_int1, t->tr_int2 ) ;
   switch (t->tr_type) {
    default:
      assert ( 0 );
    case TRT_NUMBER:
        return t->tr_int1;
    case TRT_AND2:
      return evaltree (t->tr_leaf1) && evaltree (t->tr_leaf2);
    case TRT_OR2:
      return evaltree (t->tr_leaf1) || evaltree (t->tr_leaf2);
    case TRT_ARPLUS:
      return evaltree (t->tr_leaf1) + evaltree (t->tr_leaf2);
    case TRT_ARMINUS:
      return evaltree (t->tr_leaf1) - evaltree (t->tr_leaf2);
    case TRT_ARTIMES:
      return evaltree (t->tr_leaf1) * evaltree (t->tr_leaf2);
    case TRT_ARDIVIDE:
      return evaltree (t->tr_leaf1) / evaltree (t->tr_leaf2);
    case TRT_ARMOD:
      return evaltree (t->tr_leaf1) % evaltree (t->tr_leaf2);
    case TRT_CMPEQ:
      return evaltree (t->tr_leaf1) == evaltree (t->tr_leaf2);
    case TRT_CMPNE:
      return evaltree (t->tr_leaf1) != evaltree (t->tr_leaf2);
    case TRT_CMPLT:
      return evaltree (t->tr_leaf1) < evaltree (t->tr_leaf2);
    case TRT_CMPLE:
      return evaltree (t->tr_leaf1) <= evaltree (t->tr_leaf2);
    case TRT_CMPGT:
      return evaltree (t->tr_leaf1) > evaltree (t->tr_leaf2);
    case TRT_CMPGE:
      return evaltree (t->tr_leaf1) >= evaltree (t->tr_leaf2);
    case TRT_NOT:
      return !evaltree (t->tr_leaf1);
    case TRT_LENGTH:      /* suit, compass */
      assert (t->tr_int1 >= SUIT_CLUB && t->tr_int1 <= SUIT_SPADE);
      assert (t->tr_int2 >= COMPASS_NORTH && t->tr_int2 <= COMPASS_WEST);
      return hs[t->tr_int2].hs_length[t->tr_int1];
    case TRT_HCPTOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalpoints;
    case TRT_PT0TOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalcounts[idxTens];
    case TRT_PT1TOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalcounts[idxJacks];
    case TRT_PT2TOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalcounts[idxQueens];
    case TRT_PT3TOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalcounts[idxKings];
    case TRT_PT4TOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalcounts[idxAces];
    case TRT_PT5TOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalcounts[idxTop2];
    case TRT_PT6TOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalcounts[idxTop3];
    case TRT_PT7TOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalcounts[idxTop4];
    case TRT_PT8TOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalcounts[idxTop5];
    case TRT_PT9TOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalcounts[idxC13];
    case TRT_HCP:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_points[t->tr_int2];
    case TRT_PT0:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_counts[idxTens][t->tr_int2];
    case TRT_PT1:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_counts[idxJacks][t->tr_int2];
    case TRT_PT2:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_counts[idxQueens][t->tr_int2];
    case TRT_PT3:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_counts[idxKings][t->tr_int2];
    case TRT_PT4:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_counts[idxAces][t->tr_int2];
    case TRT_PT5:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_counts[idxTop2][t->tr_int2];
    case TRT_PT6:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_counts[idxTop3][t->tr_int2];
    case TRT_PT7:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_counts[idxTop4][t->tr_int2];
    case TRT_PT8:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_counts[idxTop5][t->tr_int2];
    case TRT_PT9:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_counts[idxC13][t->tr_int2];
    case TRT_SHAPE:      /* compass, shapemask */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      /* assert (t->tr_int2 >= 0 && t->tr_int2 < MAXDISTR); */
      return (hs[t->tr_int1].hs_bits & t->tr_int2) != 0;
    case TRT_HASCARD:      /* compass, card */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hascard (curdeal, t->tr_int1, (card)t->tr_int2);
    case TRT_LOSERTOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalloser;
    case TRT_LOSER:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_loser[t->tr_int2];
    case TRT_CONTROLTOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalcontrol;
    case TRT_CONTROL:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_control[t->tr_int2];
    case TRT_CCCC:
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return cccc (t->tr_int1);   /* calling cccc(compass) will return KnR cccc x100 */
    case TRT_QUALITY:
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return quality (t->tr_int1, t->tr_int2);  /* returns suit Qual x100 */
    case TRT_IF:
      assert (t->tr_leaf2->tr_type == TRT_THENELSE);
      return (evaltree (t->tr_leaf1) ? evaltree (t->tr_leaf2->tr_leaf1) :
                                       evaltree (t->tr_leaf2->tr_leaf2));
    case TRT_TRICKS:      /* compass, suit. The dd routine compensates for the fact that GIB always returns tricks for South*/
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= 1 + SUIT_SPADE);  // could make this SUIT_NT here.
      return dd (curdeal, t->tr_int1, t->tr_int2);
    case TRT_SCORE:      /* vul/non_vul, contract, tricks in leaf1 */
      assert (t->tr_int1 >= NON_VUL && t->tr_int1 <= VUL);
      return score (t->tr_int1, t->tr_int2, evaltree (t->tr_leaf1));
    case TRT_IMPS:
      return imps (evaltree (t->tr_leaf1));
    case TRT_RND:
      return (int) (  RANDOM( (double)evaltree (t->tr_leaf1) ) ); /* ret rand int between 0 .. (expr in leaf1) */
                                                    /* JGM redefined RANDOM to use std lib call drand48 */
    case TRT_DECNUM:
      return t->tr_int1 ;  /* Lexer will already Mult x100; cant do here bec tree's only hold ints */
    case TRT_LTCTOTAL:      /* compass */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      return hs[t->tr_int1].hs_totalltc;
    case TRT_LTC:      /* compass, suit */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= SUIT_SPADE);
      return hs[t->tr_int1].hs_ltc[t->tr_int2];
    case TRT_DDS:      /* compass, strain */
      assert (t->tr_int1 >= COMPASS_NORTH && t->tr_int1 <= COMPASS_WEST);
      assert (t->tr_int2 >= SUIT_CLUB && t->tr_int2 <= 1 + SUIT_SPADE);  // could make this SUIT_NT here.

      JGMDPRT(7,"evaltree.761 calling DDS dds_tricks; dds_mode=%d \n", dds_mode);

      return dds_tricks (t->tr_int1, t->tr_int2);
    case TRT_PAR:     /* side  */
      return dds_parscore ( t->tr_int1 ) ;
    case TRT_OPCSIDE :
        return opc_value( t->tr_int1, t->tr_int2 , t ) ; // side NS or EW, strain (0..5) and tree_t *t for fut */
    case TRT_USEREVAL :
           JGMDPRT(6, "evaltree.USEREVAL call ask_query for TRT=%d, tr_int1=%d, tr_int2=%d, tr_int3=x%08x \n",
               t->tr_type, t->tr_int1, t->tr_int2, t->tr_int3 ) ;
         return ask_query(t->tr_int1, t->tr_int2 , t->tr_int3 ); //qtag, side, qcoded
  } /* end switch t->tr_type starts at line 619 */

} /* end evaltree() Starts around line 526 */

/* GIB returns only one value per call. Just in case the user uses the same value in more than one place
 * we cache it here, to save having to call GIB again to get the same info.
 * The dealer evaltree function and the various action functions do not save intermediate results.
 * Even Vars are re-calculated each time the var is referred to.
 */
/* cached DD analysis -- It calculates in ints and stores result in an array of char */
int dd (deal d, int l, int c) {        /* l is the compass direction we want tricks for (see evaltree TRT_TRICKS*/
  static int cached_ngen = -1;
  static char cached_tricks[4][5];
    dbg_dd_calls++;
  if (ngen != cached_ngen) {
      JGMDPRT(8,  "dd calling True DD ngen=%d,, cached_ngen=%d\n",ngen, cached_ngen ) ;
      memset (cached_tricks, -1, sizeof (cached_tricks));
      cached_ngen = ngen;
  }
  if (cached_tricks[l][c] == -1) {                  // call the GIB dd engine.
      cached_tricks[l][c] = true_dd (d, l, c);     // true dd returns an int. cached_tricks is defined as a char.

  }
  return cached_tricks[l][c];
}  /* end dd -- cached tricks version */

/* true_dd -- run the external GIB DD Engine -- if present on the system */
int true_dd (deal d, int l, int c) {      /* l is the compass direction we want tricks for but GIB always returns South*/
    /* GIB wants to know who is on lead, and what the contract strain is in order to calc # of tricks*/
    /* No matter who is on lead, the -d mode of GIB reports how many tricks SOUTH can take.
     * You can check this by issuing: /usr/games/gibcli -d -v -q inputfile
     * the -v switch gives the result in a sentence and it always says (quote) " South can take n tricks."
     * In this verbose mode, 'n' is a true number so 12 tricks is shown as 12 not as C which you get without the -v switch
     */
/* 2023/01/14 -- JGM replaced tmpnam with mkstemp to avoid constant nagging re insecure tmpnam */
    FILE *f;
    char cmd[1024];
    char gib_res;
    char tn1[256] = "/tmp/dealer_GIB_XXXXXX"; /* the XXXXXX will be replaced with random characters making a unique filename*/
    char tn2[256] = "/tmp/dealer_GIB_XXXXXX";
    int rc , t ;
    int tmp_fd1,tmp_fd2 ;
    dbg_tdd_calls++;

    JGMDPRT(7,  "IN True DD ngen=%d,tot_TRUE_dd=%d\n",ngen, dbg_tdd_calls ) ;

    tmp_fd1 = mkstemp( tn1 );
    // tmpnam (tn1);   /* the file we write the deal to, in compact notation 4 lines per deal +1 for leader and strain */
    f = fdopen (tmp_fd1, "w+");  /* bind stream f to the fd for tn1 */
    if (f == 0 ) error ("Can't open temporary output file for printcompact ");
    fprintcompact (f, d, 0);   // the zero means do NOT do oneline output GIB wants the deal in four lines */
    fprintf (f, "%c %c\n", "eswn"[l], "cdhsn"[c]); /* Tell GIB the opening leader. if l is 0 (Decl=North) then east is leader */
    fclose (f); /*this will also close tmp_fd1 */
    tmp_fd2 = mkstemp( tn2 ) ; close(tmp_fd2) ; /* in case there is a problem with shared access */
    // tmpnam (tn2);
    /* sprintf (cmd, "bridge -d -q %s >%s", tn1, tn2); */
    sprintf (cmd, "%s -d -q %s >%s", DD_PGM, tn1, tn2);  // See dealdefs.h for defn of DD_PGM
    rc = system (cmd);
    if ( rc != 0 ) { fprintf(stderr, "rc = %d from cmd[%s]\n", rc , cmd ); }
    f = fopen (tn2, "r");
    if (f == 0) error ("Can't read output of analysis");
    rc = fscanf (f, "%*[^\n]\n%c", &gib_res);
    #ifdef JGMDBG
    if (jgmDebug >= 7 )
            if ( rc != 0 ) { fprintf(stderr, "rc = %d from fscanf for GIB results\n", rc ) ; }
    #endif
    fclose (f);
    remove (tn1);
    remove (tn2);
    /* l = 1 means the TRT_TRICKS code wants tricks for east, l = 3 means west. GIB always gives SOUTH. */
    /* Since GIB always gives SOUTH if we want east or west we subtract GIB's result from 13 */
    /* Verified extensively by JGM on 2021/12/23 */
    /* Note that GIB evaluates the tricks based on who is on lead. */

    t = ((l == 1) || (l == 3)) ? 13 - gib_tricks (gib_res) : gib_tricks (gib_res);

    JGMDPRT(7, "GIB results for hand[%d] strain[%c] Tricks=%d\n", l,"cdhsn"[c],t ) ;
    return t ;
} /* end true_dd() */


int gib_tricks (char c) {  /* converts the char [0-9A-D] returned by GIB to a number */
  if (c >= '0' && c <= '9')
    return c - '0';
  return c - 'A' + 10;
} /* end trix() */

int imps (int scorediff) {
  int i, j;
  j = abs (scorediff);
  for (i = 0; i < 24; i++)
    if (imp_tbl[i] >= j) return scorediff < 0 ? -i : i;
  return scorediff < 0 ? -i : i;
} /* end imps() */

int undertricks( int utricks, int vul, int dbl_flag) {  // dbl_flag is either 1 (dbled) or 2 (re-dbled)
    int score_res ;

    JGMDPRT(7, "SCORE_DBL_Undertricks: utricks=%d, vul=%d, dbl_flag=%d\n",utricks, vul, dbl_flag);

    if (vul == 1 ) {
        score_res = (300*utricks - 100) * dbl_flag ; // (200, 500, 800, etc. ) x2 for redoubled)
        return -score_res;
    }
    if (utricks <= 3) {  // Non Vul Dbled first 3 tricks are 100, 300, 500 penalty
        score_res = (200*utricks - 100) * dbl_flag ; // (100, 300, 500) x2 for redoubled
        return -score_res ;
    }
    else {              // non Vul Dbled first 3 tricks are 500, next tricks are 300 each.
        score_res = (300*utricks - 400) * dbl_flag ; // (800, 1100, 1400, etc. ) x2 for redoubled
        return -score_res ;
    }
} /* end undertricks */

int trickscore(int strain, int tricks ) {  // tricks is total taken; only get pts for tricks > 6
    int score_res ;
    score_res = ((strain >= SUIT_HEART) ? 30 : 20) * (tricks - 6);

  /* NT bonus */
  if (strain == SUIT_NT) score_res += 10;
  JGMDPRT(7, "SCORE_DBL_trickscore: strain=%d, tricks=%d, score_res=%d\n",strain, tricks, score_res);
  return score_res ;
}

int undbled_score (int vuln, int suit, int level, int tricks) { // handles undertricks, overtricks and just making
  JGMDPRT(7, "undbled_SCORE: vul=%d,suit=%d,level=%d, tricks=%d\n",vuln, suit, level, tricks);
  int total = 0;
  /* going down. undoubled */
  if (tricks < 6 + level) return -50 * (1 + vuln) * (6 + level - tricks);

  /* Contract made, calculate positive score */
  /* Tricks score */
  total = total + ((suit >= SUIT_HEART) ? 30 : 20) * (tricks - 6);

  /* NT bonus */
  if (suit == SUIT_NT) total += 10;

  /* part score bonus */
  total += 50;

  /* game bonus for NT  minus partscore bonus */
  if ((suit == SUIT_NT) && level >= 3) total += 250 + 200 * vuln;

  /* game bonus for major minus partscore bonus */
  if ((suit == SUIT_HEART || suit == SUIT_SPADE) && level >= 4) total += 250 + 200 * vuln;

  /* game bonus for minor minus partscore bonus */
  if ((suit == SUIT_CLUB || suit == SUIT_DIAMOND) && level >= 5) total += 250 + 200 * vuln;

  /* small slam bonus */
  if (level == 6) total += 500 + 250 * vuln;

  /* grand slam bonus */
  if (level == 7) total += 1000 + 500 * vuln;

  return total;
} /* end undbled score() */

int dbled_making ( int dbl_flag, int vul, int strain, int ctricks) { // score for making  dbled or redbled contract exactly
    #ifdef JGMDBG
    if(jgmDebug >= 7 )
        fprintf(stderr, "SCORE_DBL_making: ctricks=%d, strain=%d, vul=%d, dbl_flag=%d\n",ctricks, strain, vul, dbl_flag);
    #endif
    int t0;
    int score_res = 0;

    t0 = trickscore(strain, ctricks );
    if (dbl_flag == 1 ) t0 += t0 ;
    else                t0 = t0 * 4 ; // Redoubled
    score_res = t0 ;
     if(jgmDebug >= 7) { JGMDPRT(7, "trick score = %d\n ",score_res ); }
    // Game and Slam Bonuses do not get x2; and you must bid 6 to get slam

    if ( t0 >= 100 ) {  // game bonus
        score_res += 300 + vul*200 ;
     if(jgmDebug >= 7) {    JGMDPRT(7,"+game Bonus=%d\n ",score_res  ); }
    }
    else {  // part_score bonus
        score_res += 50 ;
    }
    score_res += 50*dbl_flag;  //  insult bonus 50 for dbled, 100 for redbled

    if ( ctricks == 12 ) { // small slam bonus
        score_res += 500 + 250*vul ;
    }
    if ( ctricks == 13 ) { // Grand Slam bonus
        score_res += 1000 + 500*vul ;
     }
     if(jgmDebug >= 7) {JGMDPRT(7,"returning  DBL/RDBL making score=%d\n ", score_res ); }
    return score_res ;
} /* end dbled_making */

// (new) score calls undbled_score, undertricks, dbled_making as req'd. Calcs OverTrick bonus for DBL or RDBL contracts itself.
int score(int vul, int coded_contract, int tricks ) {
    int dbl_flag, strain, level ;
    int t0, t1, ctricks, score_res;

    dbl_flag = coded_contract / 40 ;  // 0, 1, or 2
    t0 = coded_contract % 40 ;        // 5..39
    strain = t0 % 5 ;                 // 0 = Clubs, 4 = NT
    level  = t0 / 5 ;                // 1 - 7
    ctricks = 6 + level ;           /* number of tricks contracted for */
    #ifdef JGMDBG
      if(jgmDebug >= 7 )
        fprintf(stderr, "NEW_SCORE:: CodedContract=%d, DBLFLG=%d, Strain=%d, Level=%d\n",coded_contract,dbl_flag,strain,level);
    #endif
    if (dbl_flag == 0 ) {   /* undoubled use simple score function */
        if(jgmDebug >= 7) { JGMDPRT(7,"SCORE_DBL:: Calling undbled_score-- ctricks =%d\n",ctricks); }
       return undbled_score( vul, strain,  level,  tricks) ;
    }
    assert (dbl_flag ==1 || dbl_flag == 2 ) ;

    if ( ctricks > tricks ) {  /* contract went down */
       score_res = undertricks( (ctricks - tricks), vul, dbl_flag) ;
       if(jgmDebug >= 7) { JGMDPRT(7,"NEW_SCORE_DBL::Returning UnderScore=%d\n",score_res); }
       return score_res ;
    }
    score_res = dbled_making(dbl_flag, vul, strain, ctricks);  // includes, partscore/game/slam & insult bonus for DBL or RDBL
    if(jgmDebug >= 7) { JGMDPRT(7,"NEW_SCORE_DBL:: Making=%d\n",score_res); }
    if ( ctricks == tricks ) { /* contract made; no overtricks */
       return score_res ;
    }
        /* Contract made with overtricks. Add the Overtricks bonus to the making score */
    t1 = tricks - ctricks ;
    if(jgmDebug >= 7) { JGMDPRT(7,"SCORE_DBL:: Overtricks=%d \n",t1); }
    t0 = t1*100*(1+vul); // NV dbled is 100/trick, Vul Dbled is 200/trick
    t0 = t0*dbl_flag ; // RDBL OT are 2x DBL OT.
    score_res += t0 ;
     if(jgmDebug >= 7) { JGMDPRT(7,"SCORE_DBL:: Final result for +ve DBLed contract = %d\n",score_res ); }
    return score_res ;
} /* end score */

/* +++++++++++++++++++++ Begin the series of functions that implement the OPC call ++++++++++ */
/* write 13 cards from curdeal to an offset in the opc_buff;
 * The hand has been sorted high to low, spades first.
 */
char *Hand52_to_buff ( char *buff,  int p, deal dl ) {
/* hand is sorted. dl[p*13+0] = Highest Spade; dl[p*13+12] = lowest club. */
   char suit_sep = '/';
   char r_ids[] = "23456789TJQKA";
   int curr_suit, card_rank, card_suit;
   int di, count;
   unsigned char kard ;

   char *bp ;
   // buff_len = strlen(buff);
   // bp = buff + buff_len ;
   bp = buff ;
#ifdef JGMDBG
   if (jgmDebug >= 9 ) fprintf(stderr, "Hand52:: bp=%p \n",bp ) ;
#endif
   di = p*13 ;
   count = 0 ;
   curr_suit = 3 ; // spades
   JGMDPRT(7, "Hand52_to_buff:: p=%d, di=%d, dl[di]=%02x\n",p,di,dl[di] ) ;
   JGMDPRT(7,  "buff=%p, bp=%p \n", buff, bp );

   while (count < 13 ) {
       kard = dl[di] ; card_suit = C_SUIT(kard); card_rank = C_RANK(kard) ;
    #ifdef JGMDBG
        if (jgmDebug >= 8 ) { fprintf(stderr,"Top Big While::Kard=%02x, card_suit=%d, card_rank=%d, count=%d, curr_suit=%d\n",
                                                kard, card_suit, card_rank, count, curr_suit ) ; }
    #endif
       while( curr_suit != card_suit ) { /* write void mark for missing suits */
            *bp++ = '-';
            *bp++ = suit_sep;
             curr_suit-- ;
        } /* end while curr_suit != card_suit */
        assert(card_suit == curr_suit) ;
        while ( (curr_suit == card_suit) && (count < 13) ) { /* write the cards in this suit */
            kard = dl[di]; card_suit = C_SUIT(kard); card_rank = C_RANK(kard) ;
        #ifdef JGMDBG
            if (jgmDebug >= 9 ) {
                fprintf(stderr,"Top Small While::Kard=%02x, card_suit=%d, card_rank=%d, count=%d, curr_suit=%d\n",
                                                kard, card_suit, card_rank, count, curr_suit ) ;
            }
        #endif
            if (curr_suit != card_suit ) break;
           *bp++ = r_ids[card_rank];
           count++; di++;
           #ifdef JGMDBG
              if (jgmDebug >= 9 ) { fprintf(stderr," Num[%d]=%c%c ", count, "CDHS"[curr_suit], *(bp-1) ) ; }
           #endif
        }
        if (jgmDebug >= 9 ) { fprintf(stderr,"\n"); }
       *bp++ = suit_sep;
        curr_suit-- ; /* Move to next suit */
    } /* end while count < 13 -- line 1038*/
    assert(count == 13 ) ;

    // Normal case curr_suit is -1; void clubs curr_suit = 0, void clubs, diamonds, and hearts curr_suit = 2
    // In case there were voids at the end of 13 cards
        while ( curr_suit >= 0 ) {
            *bp++ = '-' ;
            *bp++ = suit_sep ;
            curr_suit-- ;
        }
        /* the last char is the suit separator which we don't need after the club suit, so replace it with a space */
        if ( *(bp-1) == suit_sep ) { *(bp-1) = ' ' ; }
        else { fprintf(stderr, "CANT HAPPEN in Hand52_to_Buff, last char is not a suit_separator %c \n", *(bp-1) ); }
        *bp = '\0' ; // terminate the buffer as a string
        return bp  ; /* return pointer to null byte in case we want to append another hand to the buffer */
} /* end Hand52_to_buff Line 1014*/

char *fmt_side(char *buff , int side, deal dl ) {
  char side_hands[]="NSEW";
  int h ;
  char *bp ;
  bp = buff ;
  // If we pass in the beginning of the opc_cmd_buff we need these next ones.
  // size_t buff_len ;
  // buff_len = strlen(buff);
  // bp = buff + buff_len ;
  h = side ;
  *bp++ = '-'; *bp++ = side_hands[side*2]; *bp++ = ' '; *bp='\0';
#ifdef JGMDBG
  if ( jgmDebug >= 7 ) fprintf(stderr,"Format Side:: bp=%p \n", bp ) ;
  if ( jgmDebug >= 8 ) { fprintf( stderr, "fmt_side Hand=%c,%d %p\n", side_hands[side*2], h, bp ) ; }
  if ( jgmDebug >= 9 ) sr_hand_show(h, dl) ;
#endif
  sortHand( (dl + h*13) , 13 ) ;
  bp = Hand52_to_buff(bp, h, dl) ; // [-E xxx/xxxx/xxx/xxx]
  *bp++ = ' ';
   h = 2 + side ; // have done first hand of side, either north(0), or east(1) so now do south(2) or west(3)
  *bp++ = '-'; *bp++ = side_hands[1+side*2]; *bp++ = ' '; *bp='\0';
   sortHand( (dl + h*13) , 13 ) ;
   bp = Hand52_to_buff(bp, h, dl) ; //[-E xxx/xxxx/xxx/xxx -W xx/xxxxx/xx/xxxx ]
  *bp++ = ' ';
  *bp='\0';
#ifdef JGMDBG
  if ( jgmDebug >= 8 ) {
      fprintf( stderr, "fmt_side Hand=%c,%d bp=%p \n", side_hands[1+side*2], h, bp ) ;
      fprintf( stderr, "fmt_side -- Hand52 to buff done for h=%d, bp=%p \n",h, bp) ;
      fprintf( stderr, "fmt_side returns %p\n", bp ) ;
  }
#endif
  return bp ;
} /* end fmt_side */

/*                      opc_cmd_buff  side=0|1   curdeal */
char *fmt_opc_cmd_buff( char *buff , int side , deal dl ) {
  size_t offset = strlen(buff) ;  /* buff typically initialized with the DOP cmd path at compile time */
  char *bp = buff + offset ;
    // first 23 chars of buff should already be set but maybe for generality we should do it here with a strcpy? */
    // buff = "/usr/local/bin/DOP/dop"
  *bp++ = ' '; *bp++ = ' '; *bp++ = '-'; *bp++ = 'O'; *bp++ = opc_opener ; *bp++ = ' '; *bp = '\0';
#ifdef JGMDBG
  if ( jgmDebug >= 8 ) { fprintf( stderr, "fmt_opc_cmd_buff:[%s]\n", buff ) ; }
   // at this point: buff = "/usr/local/bin/DOP/dop  -OW "
   if ( jgmDebug >= 9 ) sr_hand_show(2, dl);
#endif
   bp = fmt_side(bp, side, dl ) ;
  // by now buff = "/usr/local/bin/DOP/dop  -OW -E xxxx/xxx/xxx/xxx  -W xx/xxxxx/xxx/xxx"
  /* put in a string to tell DOP we want the Terse report */
  *bp++ = '-' ; *bp++ = 'q' ; *bp++ = ' '; *bp = '\0' ;
  // buff = "/usr/local/bin/DOP/dop  -OW -W xxxx/xxx/xxx/xxx  -E xx/xxxxx/xxx/xxx -q "

  JGMDPRT(8, "FMT_OPC_CMD::[%s]\n", buff   ) ;
  JGMDPRT(8, "fmt_cmd_buff bp = %p \n", bp ) ;


  return bp ;  /* bp points to end of buffer, so we can append rest of cmd to it */
} /* end fmt_opc_cmd_buff */

/* issue the popen cmd using the previously formatted cmd_buff; Parse the results with fscanf and put into opcRes struct */
int get_opc_vals (struct opc_Vals_st *opcRes, char *cmd_buff ) {
    FILE *fp;
    int status;
    fp = popen(cmd_buff , "r" ) ;
    if (fp == NULL) {
        perror( "get_opc_vals:: popen returned NULL" ) ;
        fprintf(stderr, "get_opc_vals:: popen DOP cmd failed ! \n");
        return -1 ;
    }
    opcRes->fields = fscanf(fp, "%5f %5f %5f %5f %5f %5f :",
                        &opcRes->DOP[0], &opcRes->DOP[1], &opcRes->DOP[2], &opcRes->DOP[3], &opcRes->DOP[4], &opcRes->DOP_long );
    opcRes->fields += fscanf(fp, " %5f %5f", &opcRes->QL_suit, &opcRes->QL_nt );

        DBGLOC("DOP fscanf Calls return fields=%d\n",opcRes->fields);
 #ifdef JGMDBG
    if (jgmDebug >= 8 ) {
        int i ;
        fprintf(stderr, "GET_opc_vals:: \n    ");
        for (i=0; i<5 ; i++ ) { fprintf(stderr, "DOP[%d] = %5.2f ", i, opcRes->DOP[i] ) ; }
        fprintf(stderr, " DOP_long=%5.2f, QL_suit=%5.2f, QL_nt=%5.2f\n", opcRes->DOP_long, opcRes->QL_suit, opcRes->QL_nt );
        fprintf(stderr, "===============\n" );
    }
#endif
    status = pclose(fp);
    if (status == -1) {
        perror("get_opc_vals:: pclose reported error ") ;
    }
  return 1 ;
} /* end get_opc_vals */

/* populate the sidestat fields with OPC values converted to int x100 */
/* return the OPC value for the asked for strain; C=0, D=1, H=2, S=3, N=4, LongestFit=5 */
int opc_value(int side, int strain, struct tree *t ) {
   int s, opcval ;
   dbg_opc_calls++;

     JGMDPRT(7,"OPC_VALUES side=%d,strain=%d,opcCalls=%d,ngen=%d,opc_dealnum=%d\n",
                        side, strain, dbg_opc_calls, ngen, opc_dealnum ) ;

   if (opc_dealnum != ngen ) { // the cache for both sides is out of date
        JGMDPRT(8, "OPC cache is out of date. ngen=%d, opcDealnum=%d\n", ngen, opc_dealnum);
       ss[0].ss_cached = 0 ;  // should not be necessary to zap all the past values
       ss[1].ss_cached = 0 ;  // but it might be safer
   }
   else if (ss[side].ss_cached == 1 ) {  // the cache for the current side is OK.
   #ifdef JGMDBG
      if (jgmDebug >= 8 ) {
         fprintf(stderr, "OPC_VALUE.1135::cache is OK. side=%d,strain=%d, Long=[%d], NT=[%d]\n",
                           side, strain, ss[side].ss_opc_hldf_l,ss[side].ss_opc_hlf_nt ) ;
         for (s=0;s<4;s++){fprintf(stderr, "[%d]=[%d], ",s,ss[side].ss_opc_hldf[s] ) ; } fprintf(stderr, "\n");
      }
   #endif

        if      ( strain == 5 ) {   opcval= ss[side].ss_opc_hldf_l       ; return opcval ; }
        else if ( strain == 4 ) {   opcval= ss[side].ss_opc_hlf_nt       ; return opcval ; }
        else                    {   opcval= ss[side].ss_opc_hldf[strain] ; return opcval ; }
     fprintf(stderr, "CANT HAPPEN in opc_Val.1144 -- strain fall thru [%d]\n",strain);
     assert(0 <= strain && strain <=5) ;
   }  /* end cached == 1 */
   /* The cache for the current side (and maybe also the other side) needs refreshing */
   dbg_opc_cmd_calls++ ;

   opc_cmd_buff[opc_pgmlen] = '\0' ; // Reset buff to just the pgm name string
   fmt_opc_cmd_buff(opc_cmd_buff, side, curdeal ) ;
   get_opc_vals(&opcRes, opc_cmd_buff );

   DBGDO(7, show_opcRes(&opcRes) );

   ss[side].ss_quick_losers_suit = (int) (opcRes.QL_suit  * 100 ) ;
   ss[side].ss_quick_losers_nt =   (int) (opcRes.QL_nt    * 100 ) ;
   ss[side].ss_opc_hlf_nt =        (int) (opcRes.DOP[4]   * 100 ) ;
   ss[side].ss_opc_hldf_l =        (int) (opcRes.DOP_long * 100 ) ;
   for (s=0 ; s < 4; s++ ) {
      ss[side].ss_opc_hldf[s] =    (int) (opcRes.DOP[s]   * 100 ) ;
   }
   ss[side].ss_cached = 1 ;
   opc_dealnum = ngen ;

   if ( strain == 5 )      {   opcval= ss[side].ss_opc_hldf_l       ; }
   else if ( strain == 4 ) {   opcval= ss[side].ss_opc_hlf_nt       ; }
   else                    {   opcval= ss[side].ss_opc_hldf[strain] ; }
      JGMDPRT(7, "Returning opc_val for side=%d, strain=%d, val=%d ", side, strain, opcval );
   return opcval ;
} /* end opc_value */

