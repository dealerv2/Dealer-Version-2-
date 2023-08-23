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
#undef DEBUG
 int main (int argc, char **argv)  {

 int arr[] = {37, 29, 4, 53, 56, 46, 5, 60, 41, 44, 13, 27, 33, 30, 34};
 int big_arr[] = {4,37,53,29,56,46,5,60,41,44,58,10,34,42,21,59,51,10,24,56,56,0,16,60,
                  31,46,53,41,22,41,58,15,38,27,39,44,13,44,33,30,34,31,24,40,46,58,36,29,56,33,22,9,51,17,24,34,59,15,48 };
        int h[13] ;
        int l, maxloops ;
        int d[4],e[4];
        double time_ascend13, time_descend13, time_qsort, elapsed_time ;
/* prologue */
#ifdef DEBUG
        int a[4], b[4],c[4], x[8], z[13];
        memcpy(a, &arr[0], sizeof(a) ) ;
        sort4(a , 4) ;
        printf("a[]=%d,%d,%d,%d\n",a[0],a[1],a[2],a[3] ) ;
        memcpy(b, &arr[4], sizeof(b) ) ;
        sort4(b,4) ;
        printf("b[]=%d,%d,%d,%d\n",b[0],b[1],b[2],b[3] ) ;
        merge(a, b, x, 4, 4) ;
        printf("A&B Merged x[]= ");
        for (int k = 0 ; k < 8 ; k++ ) {printf("%d,", x[k] ) ; }
         printf("\n");
        memcpy(c, &arr[8], sizeof(c) ) ;
        sort4(c,4) ;
        printf("c[]=%d,%d,%d,%d\n",c[0],c[1],c[2],c[3] ) ;
        merge(x, c, z, 8, 4) ;
        printf("A&B Merged x[]= ");
        for (int k = 0 ; k < 12 ; k++ ) {printf("%d,", z[k] ) ; }
        printf("\n");
        printf(" Testing Ascending sort13  \n");
          memcpy(h, arr, sizeof(h) ) ;
          show_arr(h, 13, "Initial memcpy H");
          sort13(h) ;
          show_arr(h,13,"Ascending Sorted Hand H");

          printf(" **** Testing Descending dSort13****  \n");
          memcpy(h, &big_arr[20], sizeof(h) ) ;
          show_arr(h, 13, "Initial memcpy big_arr[20] to H");
          dsort13(h) ;
          show_arr(h,13,"Descending Sorted Hand[20] H");


          memcpy(h, &big_arr[30], sizeof(h) ) ;
          show_arr(h, 13, "Initial memcpy big_arr[30] to H");
          dsort13(h) ;
          show_arr(h,13,"Descending Sorted Hand[30] H");

          printf(" Testing dsort4  \n");
          int d[4] = { 29,4,37,53 };
          dsort4(d,4) ; show_arr(d,4,"4 D items Descending");
          int e[4] = { 4,37,29,60 };
          dsort4(e,3); show_arr(e,3,"3 E items Descending");
          dsort4(e,4); show_arr(e,4,"4 E items Descending");
          maxloops = 10000 ;


#else
         maxloops = 100000000;
#endif

        /* Sorting a hand */
         if (argc > 1 ) {maxloops = atoi(argv[1]) ;}

        struct timeval tvstart, tvstop;

      printf("Test Desc Sort4 with Maxloops=%d\n", maxloops);
      gettimeofday (&tvstart, (void *) 0);
      for (l=0; l< maxloops ; l++ ) {
          memcpy(d, arr, sizeof(d) ) ;
          dsort4(d,4) ;
      }
      gettimeofday (&tvstop, (void *) 0);
      elapsed_time = (double) (tvstop.tv_sec + tvstop.tv_usec / 1000000.0 -
                              (tvstart.tv_sec + tvstart.tv_usec / 1000000.0) );
      printf("Desc 4 Sorted     ** ");
      for (int k = 0 ; k < 4 ; k++ ) {
         printf("%d, ",d[k]);
      }
      printf("\n") ;

      printf ("Time needed for %d x     dSort4: %10.6f sec \n", maxloops, elapsed_time);

      printf("Simulate Ascending Hand Sort with Maxloops=%d\n", maxloops);
      gettimeofday (&tvstart, (void *) 0);
      for (l=0; l< maxloops ; l++ ) {
          memcpy(h, &big_arr[20], sizeof(h) ) ;
          sort13(h) ;
      }
      gettimeofday (&tvstop, (void *) 0);
      time_ascend13 = (double) (tvstop.tv_sec + tvstop.tv_usec / 1000000.0 -
                              (tvstart.tv_sec + tvstart.tv_usec / 1000000.0) );
      printf("Ascending Sort13     ** ");
      for (int k = 0 ; k < 13 ; k++ ) {
         printf("%d, ",h[k]);
      }
      printf("\n") ;

      printf("Simulate Descending Hand Sort with Maxloops=%d\n", maxloops);
      gettimeofday (&tvstart, (void *) 0);
      for (l=0; l< maxloops ; l++ ) {
          memcpy(h, &big_arr[20], sizeof(h) ) ;
          dsort13(h) ;
      }
      gettimeofday (&tvstop, (void *) 0);
      time_descend13 = (double) (tvstop.tv_sec + tvstop.tv_usec / 1000000.0 -
                              (tvstart.tv_sec + tvstart.tv_usec / 1000000.0) );
      printf("Descending Sort13     ** ");
      for (int k = 0 ; k < 13 ; k++ ) {
         printf("%d, ",h[k]);
      }
      printf("\n") ;

      printf("Simulate Descending Hand xSort with Maxloops=%d\n", maxloops);
      gettimeofday (&tvstart, (void *) 0);
      for (l=0; l< maxloops ; l++ ) {
          memcpy(h, &big_arr[20], sizeof(h) ) ;
          xsort13(h) ;
      }
      gettimeofday (&tvstop, (void *) 0);
      elapsed_time = (double) (tvstop.tv_sec + tvstop.tv_usec / 1000000.0 -
                              (tvstart.tv_sec + tvstart.tv_usec / 1000000.0) );
      printf("Descending xSort13     ** ");
      for (int k = 0 ; k < 13 ; k++ ) {
         printf("%d, ",h[k]);
      }
      printf("\n") ;

      printf("Simulate Hand Q-Sort with Maxloops=%d\n", maxloops);
      gettimeofday (&tvstart, (void *) 0);
      for (l=0; l< maxloops ; l++ ) {
          memcpy(h, arr, sizeof(h) ) ;
          qsort(h, 13, sizeof(int), cmpxy ) ;
      }
      gettimeofday (&tvstop, (void *) 0);
      time_qsort = (double) (tvstop.tv_sec + tvstop.tv_usec / 1000000.0 -
                              (tvstart.tv_sec + tvstart.tv_usec / 1000000.0) );

      printf("13 Q -_Sorted ** ");
      for (int k = 0 ; k < 13 ; k++ ) {
         printf("%d, ",h[k]);
      }
      printf("\n") ;
      printf ("Time needed for %d x   Q-Sort13: %10.6f sec \n", maxloops, time_qsort);
      printf ("Time needed for %d x  AscSort13: %10.6f sec \n", maxloops, time_ascend13);
      printf ("Time needed for %d x DescSort13: %10.6f sec \n", maxloops, time_descend13);
      printf ("Time needed for %d x Desc_xSort: %10.6f sec \n", maxloops, elapsed_time);
      return 0;
}
