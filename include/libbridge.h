#ifndef LIB_BRIDGE_H
  #define LIB_BRIDGE_H
#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif

void show_arr( int a[] , int asz , char *hdg ) ;
int cmpxy( const void *x, const void *y) ;
int merge( int a[] , int b[] , int c[] , int aN, int bN ) ;
int sort4(  int a[] , int len ) ; /* sort an integer array of 0, 1, 2, 3, or 4 items in ascending order */
int dsort4( int a[] , int len ) ; /* sort an integer array of 0, 1, 2, 3, or 4 items in Descending order */
int sort13( int a[] ) ;           /* sort 13 integers at base address a into Ascending order */
int dsort13(int a[] ) ;           /* sort 13 integers at base address a into Descending order Inline. No func calls*/
int xsort13(int a[] ) ;           /* sort 13 integers at base address a into Descending order using dsort4 func calls*/

int sort_hand( char a[] ) ;     /* sort 13 hex characters at base address a into Descending order */


#endif /* LIB_BRIDGE_H */
