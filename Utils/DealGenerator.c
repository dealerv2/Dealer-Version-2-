// DealerV2 -- Scaffolding  jgmdeal version
// Version 1.0  remove MSC stuff ... & Francois Stuff ... & GIB Library file
#define _GNU_SOURCE 1
#include <unistd.h>
#include <sys/time.h>

#include "../include/std_hdrs.h"  /* all the stdio.h stdlib.h string.h assert.h etc. */

#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/dealexterns.h"
#include "../include/dealprotos.h"
#include "../include/dealglobals.c"
#include "../include/dealjgm_subs.h"
#ifndef FALSE
 #define FALSE 0
#endif
#ifndef TRUE
 #define TRUE 1
#endif
#ifndef PBNLINE
 #define PBNLINE 76
#endif
    unsigned char pbn_buff[256] = { ' ' };
    unsigned char *pbn_buff_ptr = pbn_buff;
    unsigned char *Deal52_to_PBN (deal d, unsigned char *pbn_buff);
    int validPBNline(unsigned char *pbn_ptr );
    int make_pbn_deal( void );
int main (int argc, char **argv) {
    int max_deals = 13 ;
    int rc ;
    int deal_count = 0 ;
    int outcount = 0;

   newpack (fullpack);  /* fullpack[0] = club 2, fullpack[51] = spade Ace */
   seed = SRANDOM(seed) ; /* use init_rand48() to init the RNG with the users seed */
   for (deal_count = 0 ; deal_count < max_deals ; deal_count++) {
       rc = make_pbn_deal() ;
       if (rc >= 0 ) outcount++;
    }
    fprintf(stderr, "%d Deals asked for, %d written to stdout\n",max_deals, outcount);
    return 0 ;
} /* end main */

int make_pbn_deal ( void ) {
   int i, rc ;
   for (i = 0 ; i < 52; i++ ) {curdeal[i] = fullpack[i] ; }
   shuffle (curdeal);
   pbn_buff_ptr = Deal52_to_PBN (curdeal, pbn_buff);
   printf("[%s]\n",pbn_buff);
   rc = validPBNline(pbn_buff);
   if (rc != 1) { fprintf(stderr, "ERROR PBN deal is invalid \n"); return -1 ; }
   /* now we can call the DDS solution routines */
   /* for now we write the deals to a file and we then read that file from another pgm */
   /* Easiest is just to redirect stdout ... */
   fputs(pbn_buff , stdout);


} /* end make_pbn_deal */

int validPBNline(unsigned char *pbn_ptr ) {
    size_t bufflen;
  // FALSE defined as 0, TRUE defined as 1
    bufflen = strlen(pbn_ptr) ;
    if (*pbn_ptr != 'N' && *pbn_ptr != 'n' && *pbn_ptr != 'S' && *pbn_ptr != 's' &&
        *pbn_ptr != 'E' && *pbn_ptr != 'e' && *pbn_ptr != 'W' && *pbn_ptr != 'w' ) return FALSE ;
    if (*(pbn_ptr+1) != ' ' && *(pbn_ptr+1) != ':' ) return FALSE ; // not strictly necessary? but safer.

    if (bufflen < PBNLINE || bufflen > PBNLINE+1) return FALSE ;

    return TRUE ;
} /* end validPBNline */

unsigned char *Deal52_to_PBN (deal d, unsigned char *pbn_buff) {
  char pt[] = "nesw";
  int s, p, r;
  unsigned char *pbn_ptr;
  pbn_ptr = pbn_buff ;
  pbn_ptr=pbn_buff;
 /* Dealer will process an input that looks like this:
 N:832.T763.J42.K72 AKJ6.A984.Q6.Q43 T4.Q52.T8753.T65 Q975.KJ.AK9.AJ98
 * It is quite fussy. It will not handle blank lines, or lines with extra compass directions in them and so on.
 */
  for (p = COMPASS_NORTH; p <= COMPASS_WEST; p++) {
    *pbn_ptr++ = pt[p];
    for (s = SUIT_SPADE; s >= SUIT_CLUB; s--) {
      for (r = 12; r >= 0; r--)  {                 /* r goes from Ace to deuce */
        if (HAS_CARD (d, p, MAKECARD (s, r))) *pbn_buff++ =  ucrep[r];
      } /* end for r */
      if (s > 0) *pbn_ptr++ = '.';  /* put a dot between suits, but not after clubs  */
    } /* end for suit */
    if (p < 3 ) *pbn_ptr++ = ' '; /* put a space between hands */
  } /* end for p = compass */
  *pbn_ptr='\0';
  return pbn_ptr;
}   /* end convert to PBN */



