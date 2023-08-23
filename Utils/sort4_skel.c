#define _GNU_SOURCE 1
#include "../include/jgmdebug.h"
#include "../include/dbgprt_macros.h"
#include "../include/libbridge.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>    /* for qsort  */
#include <string.h>    /* for memcpy */
#include <sys/time.h>  /* for gettimeofday, struct timeval */
/* testing some timing of variou sort strategies.
 * the following macros work; the SWAP macro takes 3 times as long as the Swap macro
 */
 #define MIN( A , B ) ( (B) < (A) ) ? (B) : (A)
 #define MAX( A , B ) ( (B) < (A) ) ? (A) : (B)
 #define SWAP(A , B ) { int _tmpZQX = (A) ; (A) = (B) ; (B) = _tmpZQX ; }
 void show_arr( int a[] , int asz , char *hdg ) {
    printf("----- %s -----\n",hdg) ;
    for (int n=0; n < asz ; n++ ) { printf("%d,", a[n]); }
    printf("----------------\n");
    return  ;
}

int dsort4(  int a[] , int len ) { /* sort an integer array of 0, 1, 2, 3, or 4 items in Descending order */
   int t ;
   switch (len) {
      case 0:
      case 1:
         return 0 ;
      case 2:
         if (a[1] > a[0] ) { t = a[1] ; a[1] = a[0] ; a[0] = t ; } // a[0] > a[1]
         return 0;
      case 3:
         if (a[1] > a[0] ) { t = a[1] ; a[1] = a[0] ; a[0] = t ; }    // a[0] > a[1]
         if (a[2] > a[0] ) { t=a[2]; a[2]=a[1];a[1]=a[0];a[0]=t;}     // 3rd elem to a[0]; shuffle other two up one
         else if (a[2] > a[1] ) {t=a[2]; a[2]=a[1]; a[1] = t ; }      // swap 2nd and 3rd elems
         return 0 ;
      case 4:  /* first do sort3 then place 4th element */
         if (a[1] > a[0] ) { t = a[1] ; a[1] = a[0] ; a[0] = t ; }    // a[0] > a[1]
         if (a[2] > a[0] ) { t=a[2]; a[2]=a[1];a[1]=a[0];a[0]=t;}     // 3rd elem to a[0]; shuffle other two up one
         else if (a[2] > a[1] ) {t=a[2]; a[2]=a[1]; a[1] = t ; }      // swap 2nd and 3rd elems

         if      (a[3] >= a[0] ) { t=a[3];a[3]=a[2];a[2]=a[1];a[1]=a[0];a[0]=t; } // shuffle all up one place
         else if (a[3] >= a[1] ) { t=a[3];a[3]=a[2];a[2]=a[1];a[1]=t; }           // shuffle 1..3 up one place
         else if (a[3] >  a[2] ) { t=a[3];a[3]=a[2];a[2]=t;}                      // swap a[2] with a[3]
         return 0 ;
      default :
         return -1 ;
   } /* end switch (len) */
} /* end dsort4 */


 int main (int argc, char **argv)  {

 int arr[] = {37, 29, 4, 53, 56, 46, 5, 60, 41, 44, 13, 27, 33, 30, 34};
        int l, maxloops ;
        double  elapsed_time ;
/* prologue */
#ifdef DEBUG
          printf(" Testing dsort4  \n");
          int d[4] = { 29,4,37,53 };
          dsort4(d,4) ; show_arr(d,4,"4 D items Descending");
          int e[4] = { 4,37,29,60 };
          dsort4(e,3) ; show_arr(e,3,"3 E items Descending");
          dsort4(e,4) ; show_arr(e,4,"4 E items Descending");
          return 0 ;

          maxloops = 10000 ;
#else
         maxloops = 100000000;
#endif

        /* Sorting a hand */
         if (argc > 1 ) {maxloops = atoi(argv[1]) ;}

        struct timeval tvstart, tvstop;

      printf("Simulate Hand Sort with Maxloops=%d\n", maxloops);
      gettimeofday (&tvstart, (void *) 0);
      for (l=0; l< maxloops ; l++ ) {
          memcpy(d, arr, sizeof(d) ) ;
          dsort4(d,4) ;
      }
      gettimeofday (&tvstop, (void *) 0);
      elapsed_time = (double) (tvstop.tv_sec + tvstop.tv_usec / 1000000.0 -
                              (tvstart.tv_sec + tvstart.tv_usec / 1000000.0) );
      printf("4 Sorted     ** ");
      for (int k = 0 ; k < 4 ; k++ ) {
         printf("%d, ",d[k]);
      }
      printf("\n") ;

      printf ("Time needed for %d x     dSort4: %10.6f sec \n", maxloops, elapsed_time);
      return 0;
}
