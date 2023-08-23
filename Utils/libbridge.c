#define _GNU_SOURCE 1
#include "../include/libbridge.h"
#include "../include/jgmdebug.h"
#include "../include/dbgprt_macros.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>    /* for qsort  */
#include <string.h>    /* for memcpy */
#include <sys/time.h>  /* for gettimeofday, struct timeval */
/*
 * the following macros work; for swap using a tmp variable is much faster than using 3 XORs (x ^= y )
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

int cmpxy( const void *x, const void *y) {                  // compare two ints ; called by qsort.
    if       ( *(int *)x < *(int *)y ) return -1 ;
    else if  ( *(int *)x > *(int *)y ) return +1 ;
    else                               return 0  ;
}

int merge( int a[] , int b[] , int c[] , int aN, int bN ) {  // merge two Asce sequences
   int i = 0 ; int j = 0 ; int k = 0 ;
   int kmax =aN + bN ;
   while (k < kmax) {
      if( i == aN)            { c[k++] = b[j++] ; /* printf("CPY B to C[%d] = b[%d] = %d\n", (k-1),j,c[k-1]);*/ }
      else if ( j == bN )     { c[k++] = a[i++] ; /* printf("CPY A to C[%d] = a[%d] = %d\n", (k-1),i,c[k-1]);*/ }
      else if (a[i] < b[j] )  { c[k++] = a[i++] ; /* printf("C[%d] = a[%d] = %d\n", (k-1),i,c[k-1]); */ }
      else                    { c[k++] = b[j++] ; /* printf("C[%d] = b[%d] = %d\n", (k-1),j,c[k-1]); */ }
   }
   assert (i == aN && j == bN ) ;
   return 0 ;
}

int dmerge( int a[] , int b[] , int c[] , int aN, int bN ) { // merge two desc sequences
   int i = 0 ; int j = 0 ; int k = 0 ;
   int kmax =aN + bN ;
   while (k < kmax) {
      if( i == aN)            { c[k++] = b[j++] ; /* printf("CPY B to C[%d] = b[%d] = %d\n", (k-1),j,c[k-1]);*/ }
      else if ( j == bN )     { c[k++] = a[i++] ; /* printf("CPY A to C[%d] = a[%d] = %d\n", (k-1),i,c[k-1]);*/ }
      else if (a[i] > b[j] )  { c[k++] = a[i++] ; /* printf("C[%d] = a[%d] = %d\n", (k-1),i,c[k-1]); */ }
      else                    { c[k++] = b[j++] ; /* printf("C[%d] = b[%d] = %d\n", (k-1),j,c[k-1]); */ }
   }
   assert (i == aN && j == bN ) ;
   return 0 ;
}


int sort4(  int a[] , int len ) { /* sort an integer array of 0, 1, 2, 3, or 4 items in Ascending order */
   int t ;
   switch (len) {
      case 0:
      case 1:
         return 0 ;
      case 2:
         if (a[1] < a[0] ) { t = a[0] ; a[0] = a[1] ; a[1] = t ; } // this is quicker than the X ^= y; y ^= x; x ^= y approach
         return 0;
      case 3:
         if (a[1] < a[0] ) { t=a[1] ; a[1] = a[0] ; a[0] = t ; }      // sort first two elements; a[0] now < a[1]
         if (a[2] < a[0] ) { t=a[2]; a[2]=a[1];a[1]=a[0];a[0]=t;}     // 3rd elem to a[0]; shuffle other two up one
         else if (a[2] < a[1] ) {t=a[2]; a[2]=a[1]; a[1] = t ; }      // swap 2nd and 3rd elems
         return 0 ;
      case 4:  /* first do sort3 then place 4th element */
         if (a[1] < a[0] ) { t=a[1] ; a[1] = a[0] ; a[0] = t ; }      // sort first two elements; a[0] now < a[1]
         if (a[2] < a[0] ) { t=a[2]; a[2]=a[1];a[1]=a[0];a[0]=t;}     // 3rd elem to a[0]; shuffle other two up one
         else if (a[2] < a[1] ) {t=a[2]; a[2]=a[1]; a[1] = t ; }      // swap 2nd and 3rd elems

         if      (a[3] <= a[0] ) { t=a[3];a[3]=a[2];a[2]=a[1];a[1]=a[0];a[0]=t; } // shuffle all up one place
         else if (a[3] <= a[1] ) { t=a[3];a[3]=a[2];a[2]=a[1];a[1]=t; }           // shuffle 1..3 up one place
         else if (a[3] <  a[2] ) { t=a[3];a[3]=a[2];a[2]=t;}                      // swap a[2] with a[3]
         return 0 ;
      default :
         return -1 ;
   } /* end switch (len) */
} /* end sort4 */

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

int sort13( int a[] ) { /* sort 13 integers at base address a into Ascending order direct inline.  No func calls */
   int u[13],v[13] ;
   int t ;
   //show_arr(a , 13, "Asc Sort13 A Start") ;
    /* sort items  0..3  in place*/
         if (a[1] < a[0] ) { t=a[1] ; a[1] = a[0] ; a[0] = t ; }      // sort first two elements; a[0] now < a[1]
         if (a[2] < a[0] ) { t=a[2]; a[2]=a[1];a[1]=a[0];a[0]=t;}     // 3rd elem to a[0]; shuffle other two up one
         else if (a[2] < a[1] ) {t=a[2]; a[2]=a[1]; a[1] = t ; }      // swap 2nd and 3rd elems

         if      (a[3] <= a[0] ) { t=a[3];a[3]=a[2];a[2]=a[1];a[1]=a[0];a[0]=t; } // shuffle all up one place
         else if (a[3] <= a[1] ) { t=a[3];a[3]=a[2];a[2]=a[1];a[1]=t; }           // shuffle 1..3 up one place
         else if (a[3] <  a[2] ) { t=a[3];a[3]=a[2];a[2]=t;} //show_arr(a , 13, "Alt Sort A[0..3]") ;

    /* sort items  4 .. 6  in place*/

         if (a[5] < a[4] ) { t=a[5] ; a[5] = a[4] ; a[4] = t ; }      // sort first two elements; a[ao+0] now < a[ao+1]
         if (a[6] < a[4] ) { t=a[6]; a[6]=a[5];a[5]=a[4];a[4]=t;}     // 3rd elem to a[ao+0]; shuffle other two up one
         else if (a[6] < a[5] ) {t=a[6]; a[6]=a[5]; a[5] = t ; }      // swap 2nd and 3rd elems
   //show_arr(a , 13, "Alt Sort A[0..6]") ;

   /* sort items  7..9  in place*/

         if (a[8] < a[7] ) { t=a[8] ; a[8] = a[7] ; a[7] = t ; }      // sort first two elements;
         if (a[9] < a[7] ) { t=a[9]; a[9]=a[8];a[8]=a[7];a[7]=t;}     // 3rd elem to a[ao+0]; shuffle other two up one
         else if (a[9] < a[8] ) {t=a[9]; a[9]=a[8]; a[8] = t ; }      // swap 2nd and 3rd elems

   //show_arr(a , 13, "Alt Sort A[0..9]") ;
   /* sort items 10..13  in place*/

         if (a[11] < a[10] ) { t=a[11] ; a[11] = a[10] ; a[10] = t ; }      // sort first two elements;
         if (a[12] < a[10] ) { t=a[12]; a[12]=a[11];a[11]=a[10];a[10]=t;}     // 3rd elem to a[ao+0]; shuffle other two up one
         else if (a[12] < a[11] ) {t=a[12]; a[12]=a[11]; a[11] = t ; }      // swap 2nd and 3rd elems
   //show_arr(a , 13, "Alt Sort A slices") ;

   merge(&a[0], &a[4], u , 4 , 3 ) ; /* items 0 .. 6 now in order in u[] */
   //show_arr(u, 7, "U sorted 0 .. 6");
   merge(&a[7], &a[10],v , 3 , 3 ) ; /* items 7 .. 12 now in order in v[] */
   //show_arr(v, 6, "V sorted 7 .. 12");
   merge(&u[0], &v[0], a , 7 , 6 ) ; /* items 0 .. 12 now in order in a[] */

   // show_arr(a, 13, "13 ALT Returning sorted A 0 .. 12");
   return 0 ;
}

int dsort13( int a[]) { /* sort 13 integers at base address a into Descending order direct inline. No func calls */
   int u[13],v[13] ;
   int t ;
   //show_arr(a , 13, "Desc Sort13 A Start") ;
    /* sort items  0..3  in place*/
         if (a[1] > a[0] ) { t = a[1] ; a[1] = a[0] ; a[0] = t ; }    // now a[0] > a[1]
         if (a[2] > a[0] ) { t=a[2]; a[2]=a[1];a[1]=a[0];a[0]=t;}     // 3rd elem to a[0]; shuffle other two up one
         else if (a[2] > a[1] ) {t=a[2]; a[2]=a[1]; a[1] = t ; }      // swap 2nd and 3rd elems

         if      (a[3] >= a[0] ) { t=a[3];a[3]=a[2];a[2]=a[1];a[1]=a[0];a[0]=t; } // shuffle all up one place
         else if (a[3] >= a[1] ) { t=a[3];a[3]=a[2];a[2]=a[1];a[1]=t; }           // shuffle 1..3 up one place
         else if (a[3] >  a[2] ) { t=a[3];a[3]=a[2];a[2]=t;}                      // swap a[2] with a[3]

    /* sort items  4 .. 6  in place*/
         if (a[5] > a[4] ) { t=a[5] ; a[5] = a[4] ; a[4] = t ; }      // sort first two elements; a[ao+0] now > a[ao+1]
         if (a[6] > a[4] ) { t=a[6]; a[6]=a[5];a[5]=a[4];a[4]=t;}     // 3rd elem to a[ao+0]; shuffle other two up one
         else if (a[6] > a[5] ) {t=a[6]; a[6]=a[5]; a[5] = t ; }      // swap 2nd and 3rd elems
   //show_arr(a , 13, "Alt Sort A[0..6]") ;

   /* sort items  7..9  in place*/

         if (a[8] > a[7] ) { t=a[8] ; a[8] = a[7] ; a[7] = t ; }      // sort first two elements;
         if (a[9] > a[7] ) { t=a[9]; a[9]=a[8];a[8]=a[7];a[7]=t;}     // 3rd elem to a[ao+0]; shuffle other two up one
         else if (a[9] > a[8] ) {t=a[9]; a[9]=a[8]; a[8] = t ; }      // swap 2nd and 3rd elems

   //show_arr(a , 13, "Alt Sort A[0..9]") ;
   /* sort items 10..13  in place*/

         if (a[11] > a[10] ) { t=a[11] ; a[11] = a[10] ; a[10] = t ; }      // sort first two elements;
         if (a[12] > a[10] ) { t=a[12]; a[12]=a[11];a[11]=a[10];a[10]=t;}     // 3rd elem to a[ao+0]; shuffle other two up one
         else if (a[12] > a[11] ) {t=a[12]; a[12]=a[11]; a[11] = t ; }      // swap 2nd and 3rd elems
   //show_arr(a , 13, "Alt Sort A slices") ;

   dmerge(&a[0], &a[4], u , 4 , 3 ) ; /* items 0 .. 6 now in order in u[] */
   //show_arr(u, 7, "U sorted 0 .. 6");
   dmerge(&a[7], &a[10],v , 3 , 3 ) ; /* items 7 .. 12 now in order in v[] */
   //show_arr(v, 6, "V sorted 7 .. 12");
   dmerge(&u[0], &v[0], a , 7 , 6 ) ; /* items 0 .. 12 now in order in a[] */

   //show_arr(a, 13, "Desc No Func Call Returning sorted A 0 .. 12");
   return 0 ;
}

int xsort13( int a[]) { /* sort 13 integers at base address a into Descending order Uses dsort4 func calls*/
   int u[13],v[13] ;
   // int t ;
   //show_arr(a , 13, "Desc Sort13 A Start") ;

   dsort4(&a[0],4) ; /* sort items  0..3  in place*/
   dsort4(&a[4],3) ; //show_arr(a , 13, "Desc Sort13 A[0..6]") ;
   dsort4(&a[7],3) ; //show_arr(a , 13, "Desc Sort13 A[0..9]") ;
   dsort4(&a[10],3); //show_arr(a , 13, "Desc Sort13 A[0..12]") ;


   dmerge(&a[0], &a[4], u , 4 , 3 ) ; /* items 0 .. 6 now in order in u[] */
   //show_arr(u, 7, "U sorted 0 .. 6");
   dmerge(&a[7], &a[10],v , 3 , 3 ) ; /* items 7 .. 12 now in order in v[] */
   //show_arr(v, 6, "V sorted 7 .. 12");
   dmerge(&u[0], &v[0], a , 7 , 6 ) ; /* items 0 .. 12 now in order in a[] */
   //show_arr(a, 13, "Desc dsort4 call Sort13 Returning sorted A 0 .. 12");
   return 0 ;
}

int sort_hand(char a[] ) { /* sort 13 characters at base address a into Descending order. direct inline. No Func Calls */
   char u[13],v[13] ;
   char t ;
   //show_arr(a , 13, "Desc Sort13 A Start") ;
    /* sort items  0..3  in place*/
         if (a[1] > a[0] ) { t = a[1] ; a[1] = a[0] ; a[0] = t ; }    // now a[0] > a[1]
         if (a[2] > a[0] ) { t=a[2]; a[2]=a[1];a[1]=a[0];a[0]=t;}     // 3rd elem to a[0]; shuffle other two up one
         else if (a[2] > a[1] ) {t=a[2]; a[2]=a[1]; a[1] = t ; }      // swap 2nd and 3rd elems

         if      (a[3] >= a[0] ) { t=a[3];a[3]=a[2];a[2]=a[1];a[1]=a[0];a[0]=t; } // shuffle all up one place
         else if (a[3] >= a[1] ) { t=a[3];a[3]=a[2];a[2]=a[1];a[1]=t; }           // shuffle 1..3 up one place
         else if (a[3] >  a[2] ) { t=a[3];a[3]=a[2];a[2]=t;}                      // swap a[2] with a[3]

    /* sort items  4 .. 6  in place*/
         if (a[5] > a[4] ) { t=a[5] ; a[5] = a[4] ; a[4] = t ; }      // sort first two elements; a[ao+0] now > a[ao+1]
         if (a[6] > a[4] ) { t=a[6]; a[6]=a[5];a[5]=a[4];a[4]=t;}     // 3rd elem to a[ao+0]; shuffle other two up one
         else if (a[6] > a[5] ) {t=a[6]; a[6]=a[5]; a[5] = t ; }      // swap 2nd and 3rd elems
   //show_arr(a , 13, "Alt Sort A[0..6]") ;

   /* sort items  7..9  in place*/

         if (a[8] > a[7] ) { t=a[8] ; a[8] = a[7] ; a[7] = t ; }      // sort first two elements;
         if (a[9] > a[7] ) { t=a[9]; a[9]=a[8];a[8]=a[7];a[7]=t;}     // 3rd elem to a[ao+0]; shuffle other two up one
         else if (a[9] > a[8] ) {t=a[9]; a[9]=a[8]; a[8] = t ; }      // swap 2nd and 3rd elems

   //show_arr(a , 13, "Alt Sort A[0..9]") ;
   /* sort items 10..13  in place*/

         if (a[11] > a[10] ) { t=a[11] ; a[11] = a[10] ; a[10] = t ; }      // sort first two elements;
         if (a[12] > a[10] ) { t=a[12]; a[12]=a[11];a[11]=a[10];a[10]=t;}     // 3rd elem to a[ao+0]; shuffle other two up one
         else if (a[12] > a[11] ) {t=a[12]; a[12]=a[11]; a[11] = t ; }      // swap 2nd and 3rd elems
   //show_arr(a , 13, "Alt Sort A slices") ;

   cmerge(&a[0], &a[4], u , 4 , 3 ) ; /* items 0 .. 6 now in order in u[] */
   //show_arr(u, 7, "U sorted 0 .. 6");
   cmerge(&a[7], &a[10],v , 3 , 3 ) ; /* items 7 .. 12 now in order in v[] */
   //show_arr(v, 6, "V sorted 7 .. 12");
   cmerge(&u[0], &v[0], a , 7 , 6 ) ; /* items 0 .. 12 now in order in a[] */

   //show_arr(a, 13, "Desc No Func Call Returning sorted A 0 .. 12");
   return 0 ;
}

} /* end sort_hand */
