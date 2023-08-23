/* This file does NOT compile on its own.
 * It should be #included with another file that provides all the defs and types.
 * Example of such a file is tst_Dealing.c
 */

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
    fprintf(stderr, "DONE Initializing RNG from jgm init_program-> init_rand48, %d, %d, %d, %ld\n",
                su.sss[0], su.sss[1], su.sss[2], u_seed48);
#endif
    return u_seed48;
} /* end init_rand48 */

int gen_rand_slot ( int topval ) { /* return a random number in the range [0 .. (topval - 1)] */
        /* mrand48 returns a number between in range -2^31 .. +2^31 i.e. a 32 bit range. */
        return ( abs( (int) (mrand48() % (long int) topval) ) ) ;
}

int HasCard (deal d, int player, card thiscard){
  int i;
  for (i = player * 13; i < (player + 1) * 13; i++)
    if (d[i] == thiscard) return 1;
  return 0;
}

void freshpack(deal d ) {

  /* create cards in order Club Deuce up to Spade Ace */
  int place;
  place = 0;
  int suit, rank ;
  for (suit = Clubs; suit <= Spades; suit++) {
      for (rank = Two; rank <= Ace; rank++)  {           /* JGM mod so rank is enum; a DEUCE is 0, an ACE is 12 */
        d[place++] = CODEDCARD(suit, rank);
      }
   }
}  /* end newpack */

int Shuffle (deal d) {   /* Algorithm per Knuth */
  int i, j;
  card t;
    /* Algorithm according to Knuth. */
    for (i = 0; i < 52; i++) {           /* for each card in the deck  ... */
        j = gen_rand_slot(52) ;             /* from dealjgm_subs.c */
        t = d[j];
        d[j] = d[i];
        d[i] = t;
    }    /* end for each card in the deck */
    return 1;
} /* end shuffle */

/* simple insertion sort:  Descending Order Ace to Deuce */
void sortHand(card *hnd_ptr, int size) {
    int key, j, step;
    /* we can use this to sort the 'hands' in deal. pass it deal[0], deal[13], deal[26] or deal[39] with size of 13 */
  for (step = 1; step < size; step++) {
    key = *(hnd_ptr + step);
    // fprintf(stderr, "\t\tstep=%d, keyCard=[%02x], \n",step, key );
    j = step - 1;

    // Compare key with each element on the left of it until an element greater than it is found.
    // For ascending order, change key>array[j] to key<array[j].
    while (key > *(hnd_ptr + j) && j >= 0) {
      *(hnd_ptr + j+1) = *(hnd_ptr + j);
      // fprintf(stderr, "\t\t\t Comparing: key[%02x] > Card[%02x] \n",key,*(hnd_ptr + j) );
      --j;
    }
    *(hnd_ptr + j+1) = key;
  }
} /* end sortHand */

void sortDeal(deal dl ) {
    enum Seat_et p ;
    CARD52_k *h_ptr ;
    for (p = north ; p <= west ; p++ ) { /* p is the player We sort each quarter of the deal separately */
        h_ptr = &dl[p*13];
        if(jgmDebug > 6 ) {
            fprintf(stderr, "Sorting hand[%d] starting with card [%02x] -> [%02x]\n", p, dl[p*13], *h_ptr );
        }
        sortHand( h_ptr, 13 ) ;
    }
} /* end sortDeal */

void hexdeal_show(deal dl ) {
    int i ;
    char sp ;
    i = 0;
    fprintf (stdout, "HX=[");
    for (i=0; i<52 ; i++ ) {  /* print hex deal */
        sp = ( (i+1)%13 == 0 ) ? ':' : ' ';
        fprintf(stdout, "%02x%c", dl[i],sp );
    } /* end hex deal */
    fprintf(stdout, "]\n");
} /* end hexdeal_show */

void sr_deal_show(deal dl ) {
    char rns[] = "23456789TJQKA";
    char sns[] ="CDHS";
    char rn, sn, sp ;
    int s, r , i ;
    i = 0;
    // fprintf (stderr," Showing Deal using CARD_SUIT and CARD_RANK in sr_deal_show\n");
    fprintf (stdout,"SR=[");
   for (i=0; i<52 ; i++ ) {
       s=CARD_SUIT(dl[i]); sn=sns[s];
       r=CARD_RANK(dl[i]); rn=rns[r];
       sp = ((i+1)%13 == 0) ? ':' : ' ';
       fprintf (stdout,"%c%c%c", sn,rn,sp );
    }
    fprintf (stdout,"]\n");
} /* end sr_deal_show */

struct ddTableDeal Deal52_to_DDSBIN(deal d) {
  struct ddTableDeal dl;
  int s, r, h, dds_s;
  int di ;
  for (h = 0 ; h < DDS_HANDS; h++ ) {       /* Zero all the ints so only the only one bits are what we set */
      for (s = 0 ; s < DDS_SUITS; s++ ) {
          dl.cards[h][s] = 0;
      }
  } /* end for h < DDS_HANDS zero out*/
  h = 0 ;
  di = 0;
  while (di < 52 ) {        /* we are doing North for the first 13 cards then east etc. */
      s = CARD_SUIT(d[di]);    /* Dealer coding: s=0 -> clubs */
      dds_s = 3 - s ;        /* Clubs: s=0, dds_s=3, Diam: s=1, dds_s=2; Hearts s=2, dds_s=1; Spades: s= 3 dds_s=0 */
      r = CARD_RANK(d[di]);  /* deuce = 0, .... Ace=12 */
      dl.cards[dds_s][h]  |= DDS_BITMASK[r]; //DDS_BITMASK[0]=0x0004 which is the Deuce; [13]=0x800 which is Ace */
      if (jgmDebug > 8 ) {
        fprintf( stderr, "Dealer: hand[%d], suit=%d, rank=%d, :: Set DDS[%d][%d]=%02x\n",
                                        h, s, r, dds_s, h, dl.cards[dds_s][h] );
      }
      di++ ;
      if  (di % 13 == 0 )  h++ ; /* next hand coming up */
  } /* end while di < 52 */
  return dl ;
} /* end Deal52_to_DDSBIN */

struct ddTableDealPBN Deal52_to_DDSPBN (deal d ) {
    /* d will be modified by sort. should not matter as Dealer does not care*/

// Some Test Deals for ref
    // N:832.T763.J42.K72 AKJ6.A984.Q6.Q43 T4.Q52.T8753.T65 Q975.KJ.AK9.AJ98
    // N:...AKQJT98765432 ..AKQJT98765432. .AKQJT98765432.. AKQJT98765432...  is the pathological case
  char pt[] = "nesw";
  char UCrep[13]="23456789TJQKA";
  int s, p, r, di, kard,suit;
  struct ddTableDealPBN pbn_deal ;
  char *pbn_ptr ;
  pbn_ptr = &pbn_deal.cards[0] ;

    *pbn_ptr++  =  pt[0];  // DDS takes the first char in the 'deal' as the leader; returns tricks for him
    *pbn_ptr++  = ':';
    sortDeal(d) ;         // sort each hand in descending order i.e the pbn order DDS wants
    suit = SPADES ; p = 0 ; di = p*13 ;
     kard = d[di++]; s = C_SUIT(kard) ; r = C_RANK(kard) ;
     while (di <= 52 ) {
         while ( s != suit ) {
             if (suit != CLUBS ) {
                *pbn_ptr++ = '.' ;
                --suit ;
            }
            else {
                *pbn_ptr++ = ' ' ;
                suit = SPADES ; p++;
            }
         }
         /* s == suit */
         *pbn_ptr++ = UCrep[r] ;
         kard = d[di++]; s = C_SUIT(kard) ; r = C_RANK(kard) ;
     } /* end while di <= 52. All cards done */
     if ( jgmDebug  > 4 ) { printf("End While di[%d] suit=%d, s:r=[%d:%d]\n", di, suit, s, r ); }
     while( suit != CLUBS  ) {  *pbn_ptr++ = '.' ; --suit ; }
     *pbn_ptr++ = ':';
     *pbn_ptr = '\0'; /* Null terminate the pbn string */
     if ( jgmDebug  > 2 ) fprintf(stderr,"PBN[%s]\n", pbn_deal.cards);
     return pbn_deal ;
} /* end Deal52_to_DDSPBN */

int GIB_to_Deal52( deal dl , char *t ) { /* convert a GIB style text string to Deal52 */
    size_t slen ;
    int dlcnt ;
    int tc , seat ;
    enum rank_ek r;
    enum suit_ek s;
    int endofdeal = 0;
    char *exdl = "n T952.842.A98.432 e KQJ.A3.KQJT2.AKT s 64.KJT7.75.QJ875 w A873.Q965.643.96:";
    /* get a text string such as:
     * n T952.842.A98.432 e KQJ.A3.KQJT2.AKT s 64.KJT7.75.QJ875 w A873.Q965.643.96:
     * and convert it into a deal.
     * The input string shoud have exactly (2+13+4)*4 = 76 chars.
     */
     slen = strlen(t) ;
     if (slen != DEALSTRSIZE ) {
         fprintf(stderr, "The source deal is in the wrong format.\n Submit deal as a COLON terminated %d char string \nExample[%s]\n",
                          DEALSTRSIZE, exdl);
         return -1 ;
     }
     s=0; r=0; dlcnt = 0;
     for ( tc=0; tc<slen; tc++) {
         if(endofdeal == 1 ) { break ; } /* ignore extra chars at end of deal. should never be any */
         switch (t[tc]) {
             case 'n' : s=3 ; seat = 0 ; dlcnt = seat*13; break ;/* player 0 is north ; reset suit to spades */
             case 'e' : s=3 ; seat = 1 ; dlcnt = seat*13; break ;
             case 's' : s=3 ; seat = 2 ; dlcnt = seat*13; break ;
             case 'w' : s=3 ; seat = 3 ; dlcnt = seat*13; break ;
             case ' ' : break ;
             case '.' : --s   ; break ; /* marks the end of a suit. decr to next lower suit */
             /* I could group next cases with statements like:
              *  r =  t[tc] - '0' and r = t[tc] -'A' + 10
              * but I like the way this looks :)!
              */
             case 'A' : r = ACE;   dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case 'K' : r = KING;  dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case 'Q' : r = QUEEN; dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case 'J' : r = JACK;  dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case 'T' : r = TEN;   dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case '9' : r = NINE;  dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case '8' : r = EIGHT; dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case '7' : r = SEVEN; dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case '6' : r = SIX;   dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case '5' : r = FIVE;  dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case '4' : r = FOUR;  dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case '3' : r = THREE; dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case '2' : r = TWO;   dl[dlcnt++] = CODEDCARD(s,r) ; break ;
             case ':' : endofdeal = 1 ;  break ;
             default:
                fprintf(stderr, "Invalid Char in deal source string\n") ;
                return -1 ;
         } /* end switch */
   } /* end for tc */
    DBGPRT( "Created Deal OK", 1, t);
    #ifdef JGMDBG
      fprintf(stdout, "Deal in Hex=[");
      for (dlcnt=0 ; dlcnt < 26 ; dlcnt++ ) {
          fprintf(stdout, "%02X ", dl[dlcnt] );
      }
      fprintf(stdout,"\n             ");
      for (dlcnt=26; dlcnt<52 ; dlcnt++) {
          fprintf(stdout, "%02X ", dl[dlcnt] );
      }
      fprintf(stdout,"]\n");
    #endif

    return 1 ;
} /* end GIB_to_Deal52 */

void dump_DDSdeal(struct ddTableDeal dl) {
    int  pnum , cardcount;
    char *pname="NESW";
    char *sname="SHDC";     /* In DDS Spades is suit 0, Clubs is suit 4 */
    char *rname="23456789TJQKA"; /* 2 is subscript 0 in the following bit mask; Ace is 12 */
        // Masks to set rank bits in DDS deal:    deuce   trey    4       5       6       7
    unsigned int    DDS_BITMASK[13] = {         0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
        // Masks to set rank bits contd          8       9       T       J       Q       K       A      --
                                                0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000} ;
    int r, s;  /* no point in making them enums; compiler could just screw us up with wrong choice of type */
    char sid ;
    pnum = 0 ;
    cardcount = 0;
    char pn, rn ;
    pn='*'; rn='-';
    /* this is already a debugging routine; no need to use DBGPRT */
    if (jgmDebug > 4) fprintf(stderr, "Debugging Deal in dump_DDSdeal\n");
    for (pnum = COMPASS_NORTH; pnum <= COMPASS_WEST; pnum++) {
        cardcount = 0;
        pn = pname[pnum];
        fprintf(stdout, "\ndump_DDS deal Looping for Player number %d : %c ",pnum, pname[pnum]);
        fprintf (stdout,  " Player[%c ] \n", pname[pnum]);
        for (s = 0; s <= 3; s++) {  /* Spades are suit 0 in DDS binary*/
          assert(0 <= s && s <= 3); /* */
          sid = sname[s];
          fprintf(stdout, "Suit %2d=[%c] { ", s, sid);
          for (r = 12; r >= 0; r--) {     /* r goes from Ace to Deuce. r must be a signed int, not an unsigned */
              assert(0 <= r && r<= 12 );  /* if r is not signed this will not be true */
              rn = rname[r];
              if ( dl.cards[s][pnum] & DDS_BITMASK[r] ) {        /* player p has this card .. */
                     fprintf(stdout, "%c%c ",sid, rn );
                     cardcount++ ;
              } /* end if DDS_BITMASK[r] */
            } /* end for r = 12 downto 2 */
            fprintf (stdout, "}\n");
        } /* end for s suit */
        fprintf(stdout, "Done for player%d=%c, found=%d\n", pnum, pn, cardcount);
        assert(cardcount == 13 );  /* must find 13 cards for each player */
  } /* end for player pnum */
  fprintf(stdout, "----------------dump DDS deal done ----------------\n");
} /* end dump_DDSdeal */

void dump_Deal52(deal d) {
    int  pnum , cardcount;
    char *pname="NESW";
    char *sname="CDHS";
    char *rname="23456789TJQKA";
    int r, s;  /* no point in making them enums; compiler could just screw us up with wrong choice of type */
    char sid ;
    pnum = 0 ;

    cardcount = 0;
    unsigned int crd ;
    char pn, rn ;
    pn='*'; rn='-';
    /* this is already a debugging routine; no need to use DBGPRT */
    if(jgmDebug > 4) fprintf(stderr, "Debugging Deal in dump_Deal52\n");
    for (pnum = COMPASS_NORTH; pnum <= COMPASS_WEST; pnum++) {
        cardcount = 0;
        pn = pname[pnum];
        fprintf(stdout, "\ndump_Deal52 Looping for Player number %d : %c ",pnum, pname[pnum]);
        fprintf (stdout,  " Player[%c ] \n", pname[pnum]);
        for (s = SPADES; s >= CLUBS; s--) {  /* s must be signed for this to work */
          assert(CLUBS <= s && s <= SPADES); /* if s is not signed this will not be true */
          sid = sname[s];
          fprintf(stdout, "Suit %2d=[%c] { ", s, sid);
          for (r = ACE; r >= TWO; r--) {     /* r goes from Ace to Deuce. r must be a signed int, not an unsigned */
              assert(TWO <= r && r<= ACE );  /* if r is not signed this will not be true */
              rn = rname[r];
              crd = (unsigned int)CODEDCARD (s, r);
              // fprintf(stdout, "%02X => %d:%c ", crd, r, rn);
              if (HasCard(d, pnum, crd)) {
                     fprintf(stdout, "%c%c ",sid, rn );
                  cardcount++ ;
              } /* end if hascard */
            } /* end for r = ACE */
            fprintf (stdout, "}\n");
        } /* end for s suit */
        fprintf(stdout, "Done for player%d=%c, found=%d\n", pnum, pn, cardcount);
        assert(cardcount == 13 );  /* must find 13 cards for each player */
  } /* end for player */
  fprintf(stdout, "----------------dump curr deal done ----------------\n");
} /* end dump curr deal */



