/* File DealGenerator.c 2022-10-24 */
#ifndef _GNU_SOURCE
   #define _GNU_SOURCE
#endif

#define CODEDCARD(s,r)   ( (card)( ((s)<<4)&0xF0 ) | ( (r)&0xF ) )
#define C_Suit(c)    (   ((c)>>4)&0xF     )
#define C_Rank(c)    (   ((c)&0x0F)       )
#define NO_CARD 0x7F

#include <assert.h>
#include <getopt.h>
#include <math.h>           /* for atof */
#include <stdio.h>
#include <stdlib.h>         /* for srand48, mrand48 and friends */
#include <string.h>
#include <sys/random.h>   /* for getrandom() */
#include <unistd.h>
#include "../include/dealtypes.h"
#include "../include/dds_dll.h"         /* minimal api for DDS library */
      // Masks to set rank bits in DDS deal: deuce   trey    4       5       6       7
unsigned int    DDS_BITMASK[13] = {         0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
      // Masks to set rank bits contd          8       9       T       J       Q       K       A      --

                                            0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000
  } ;
typedef char CARD52_k ;
enum Suit_et {Nosuit = -1, Clubs=0, Diamonds, Hearts, Spades } ;
enum Rank_et {Nocard = -1, Two=0, Deuce=0, Trey,Four,Five,Six,Seven,Eight,Nine,Ten,Jack,Queen,King,Ace=12 } ;
enum Seat_et {north=0,east,south,west};
int jgmDebug = 8;

/*
 * Subs for showing deal, sorting deal, etc.
 */
#include "../Utils/DealingSubs.c"

char *ExGIB="n AKT5..KQJT75432. e Q62..A986.AQT843 s .AQ9652.KJ75.962 w J98743.KJT8743..:";
int main ( int argc, char *argv[] ) {
    int rc = 1;
    int dlcount;
    int curr_count = 0;
    int pnum, ds , dr ;
    long int seed = 901 ;
    typedef CARD52_k deal[52] ;
    deal curdeal , newpack;
    struct ddTableDealPBN dds_PBN_deal;
    struct ddTableDeal    dds_BIN_deal;
    init_rand48(seed) ;
    switch (argc) {
        case 1 :
            fprintf(stdout, "Usage: %s #ofDeal [DebugLevel]\n", argv[0]);
            dlcount = 1 ;
            break ;
        case 2 :
            dlcount = atoi(argv[1]);
            break;
        case 3 :
            dlcount = atoi(argv[1]);
            jgmDebug = atoi(argv[2]);
        default :
             dlcount = atoi(argv[1]);
             break ;
    } /* end switch (argc) */
    printf("Shuffling Newpack with seed=%ld \n", seed ) ;
    freshpack(newpack);  /* newpack[0] = c2 newpack[51]= sA; North has Clubs, West has Spades*/
    Shuffle(newpack);
    sortDeal(newpack);  /*  sort the hands in descending order */
    printf("------- sr_show of first sorted shuffled newpack ------ \n" );
    sr_deal_show(newpack);
   printf("---- Starting real loop ------\n");
    while (dlcount--) {
        curr_count++;
        freshpack(curdeal);
        Shuffle(curdeal);
        // hexdeal_show(curdeal);
        sortDeal(curdeal);
        printf("For Deal #%d ..Sorted  \n",curr_count );
        // sr_deal_show(curdeal);
        dump_Deal52(curdeal) ;
    } /* end while */


    return 0 ;
} /* end main */

