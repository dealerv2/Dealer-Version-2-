/* File skel2.c Dec 28 2021 */
#ifndef _GNU_SOURCE
   #define _GNU_SOURCE
#endif

#define CODEDCARD(s,r)   ( (card)( ((s)<<4)&0xF0 ) | ( (r)&0xF ) )
#define C_Suit(c)    (   ((c)>>4)&0xF     )
#define C_Rank(c)    (   ((c)&0x0F)       )

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
                                            0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000} ;
int jgmDebug = 8;

#include "../Utils/DealingSubs.c"
char *ExGIB="n AKT5..KQJT75432. e Q62..A986.AQT843 s .AQ9652.KJ75.962 w J98743.KJT8743..:";
int main ( int argc, char *argv[] ) {
    int rc = 1;
    int dlcount;
    int curr_count = 0;
    int pnum, ds , dr ;
    long int seed = 13 ;
    typedef unsigned char card ;
    typedef card deal[52] ;
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
    freshpack(newpack);  /* newpack[0] = c2 newpack[51]= sA; North has Clubs, West has Spades*/
    //Shuffle(newpack);
    sortDeal(newpack);  /*  sort the hands in descending order */
    sr_deal_show(newpack);
    dds_BIN_deal =  Deal52_to_DDSBIN (newpack);
    dump_DDSdeal(dds_BIN_deal);
    //dds_PBN_deal =  Deal52_to_DDSPBN (newpack);
    //fprintf(stdout, "FreshPak Sorted returns:\n\t[%s]\n",dds_PBN_deal.cards) ;
    printf("GIB Deal=[%s] \n",ExGIB );
    rc = GIB_to_Deal52(curdeal, ExGIB);
    if ( rc != 1 ) { fprintf(stderr, "GIBtoDeal52 Failed!\n"); return 0 ; }
    // printf("GIB Converted to Deal52:\n"); sr_deal_show(curdeal);
    //dds_PBN_deal =  Deal52_to_DDSPBN (curdeal ) ;
    //fprintf(stdout, "Deal52_to_DDSPBN returns:\n\t[%s]\n",dds_PBN_deal.cards) ;
    dds_BIN_deal =  Deal52_to_DDSBIN (curdeal);
    dump_DDSdeal(dds_BIN_deal);
    printf("---- Starting real loop ------\n");
    while (dlcount--) {
        curr_count++;
        freshpack(curdeal);
        Shuffle(curdeal);
        // hexdeal_show(curdeal);
        sortDeal(curdeal);
        printf("For Deal #%d ..Sorted  \n",curr_count );
        sr_deal_show(curdeal);
        //dds_PBN_deal =  Deal52_to_DDSPBN (curdeal ) ;
        //fprintf(stdout, "Deal52_to_DDSPBN returns:\n\t[%s]\n",dds_PBN_deal.cards) ;
        //dump_curdeal(curdeal);
        dds_BIN_deal =  Deal52_to_DDSBIN (curdeal);
        if (jgmDebug > 6) {
            printf("The ints for the DDSBIN deal are: Hand - suit )\n");
            for (pnum = 0 ; pnum < 4 ; pnum++ ) {
                for (ds = 0 ; ds < 4 ; ds++ ) {  /* Spades are suit 0 */
                    printf("Hand=%d CodedSuit[%d]= <%02x> \n", pnum, ds, dds_BIN_deal.cards[ds][pnum] );
                }
            } /* end for pnum */
        } /* endif jgmDebug */
        dump_DDSdeal(dds_BIN_deal);
    } /* end while */


    return 0 ;
} /* end main */

