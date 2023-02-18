/* File bktfreq_subs.c */
#ifndef GNU_SOURCE
  #define GNU_SOURCE
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "../include/dbgprt_macros.h"
#ifndef DEAL_BKTFREQ_SUBS_H
  #include "../include/deal_bktfreq_subs.h"
#endif

extern int jgmDebug ;
int *alloc_bkts1D(struct bucket_st *bkt) {
   int *alloc_ptr ;
   alloc_ptr = calloc( (size_t)bkt->Num , sizeof(int) ) ;
   if (NULL == alloc_ptr ) {
      perror(" Cant calloc space for 1D BktNames ");
      exit (-7);
   }
   bkt->Names = alloc_ptr ;
   bkt->Names[0]              = bkt->Lo - bkt->Sz ; // Dummy init for uflow bkt name
   bkt->Names[bkt->Num - 1]   = bkt->Hi + bkt->Sz ; // Dummy init for uflow bkt name
   alloc_ptr  = calloc( (size_t) (bkt->Num) , sizeof(int) ) ; // room to store counters.
   if (NULL == alloc_ptr ) {
      perror(" Cant calloc space for 1D Counters ");
      exit (-7);
   }
   JGMDPRT(4,"ALLOC1D:: Freq1D ptr = %p , Num of Buckets=%d \n",
                       (void *)alloc_ptr, bkt->Num );
   return ( alloc_ptr ) ;  // freq1D set to this value on return
} /* end allocate 1D Ram */

int upd_bkt1D( int val, int *bkt_arr, struct bucket_st *bkt ) {
   /* returns the bkt_id, 0..numbkts,  used to store the value */
   /* bkt_arr[0] is LO i.e. values less than lowbound
    * bkt_arr[1] is for lowbound <= val < (lowbound + bkt_sz)
    * bkt_arr[num_bkts-2] is for HI i.e. hibound <= val < hibound+bktsz
    * bkt_arr[num_bkts - 1] is for val >= (hibound + bktsz) (Note the = sign)
    */
    int bkt_id = 0 ;
    if(jgmDebug >= 7 ) {
      DBGLOC(" :: upd_bkt1D with val=%d \n", val) ;
    }
    if (val < bkt->Lo )                     { bkt_arr[0]++ ; return (0 ) ; }
    if (val >= bkt->Lo + (bkt->Num-2) * bkt->Sz ) { bkt_arr[bkt->Num-1]++ ; return (bkt->Num - 1 ) ; } // OK
    bkt_id = ( 1 + (val - bkt->Lo)/bkt->Sz ) ;
    bkt_arr[bkt_id]++ ;  // OK
    if(jgmDebug >= 7 ) {
      DBGLOC( "::upd_bkt1D:: VAl=%d  incr bkt[%d] = upto %d returning %d \n", val, bkt_id, bkt_arr[bkt_id], bkt_id );
    }
    return (bkt_id) ;
} /* end upd_bkt */

int show_freq1D (int *freq, char *descr, struct bucket_st *bkt, char direction ) {
   /* freq contains the counts, numbkts is ((hibound - lobound)/bkt_sz + 1) + 2(for uflow and oflow)
    * freq[0] is uflow, freq[bkt->Num - 1] is for oflow
    * uflow and oflow are not printed if they are zero
    * direction is whether to print the whole freq array on one line ('a' = across ) limited in practice to 21 data buckets,
    * or print one line per bucket down the page ('d' = down )
    */
        int i ;
        switch (tolower(direction) ) {
        case 'd':
        default :
            printf ("\nFrequency %s:\n", descr );
            if (freq[0] > 0 )
               printf ("Low\t%8d\n", freq[0]);
            for (i = 1; i < (bkt->Num - 1); i++)
                  printf ("%5d\t%8d\n", bkt->Names[i], freq[i] );
            if (freq[bkt->Num - 1] > 0 )
               printf ( "High\t%8d\n", freq[bkt->Num - 1] );
            break ;
       case 'a' :
          printf ("Frequency %s:\n", descr );
          if (freq[0] > 0 ) printf ("|  Low|");
          for (i = 1; i < (bkt->Num - 1); i++) { printf ("%6d ", bkt->Names[i] ); }
          if (freq[(bkt->Num - 1)] > 0 )  printf ("|  High|");
          printf("\n");
          if (freq[0] > 0 )  printf ("%6d ", freq[0]);
          for(i=1; i<(bkt->Num - 1) ; i++ ) { printf("%6d ",freq[i] )  ; }
          if (freq[(bkt->Num - 1)] > 0 )  printf ("|%6d|", freq[(bkt->Num - 1)]);
          break ;
      } /* end switch direction */
      return(1);
} /* end show freq1D */

int fill_bkt_names(struct bucket_st *bkt ) {  // Used by both 1D and 2D. calloc allowed for two extra Names: High and Low
   int idx ;
   int uflow_idx, oflow_idx ;
   // bkt_id 0 is for underflow
   // bkt_id bkt.Num-1  is for overflow. Struct.Num is Data Buckets + 2 . Data goes from 1 to Num-2; Num-1 is Oflow.
   // 10 data buckets means bkts 1 .. 10 get real names.
   uflow_idx = 0 ;
   oflow_idx = bkt->Num - 1 ;
   int *pnames = bkt->Names ;
   for (idx = 1 ; idx <= (bkt->Num - 2)  ; idx++ ) {
      *(pnames + idx) =  bkt->Lo + (idx-1) * bkt->Sz;
   }

   *(pnames + uflow_idx) =  bkt->Lo - bkt->Sz; // Dummy name for the uflow area. Could be Misunderstood. Maybe fix.
   *(pnames + oflow_idx) =  bkt->Hi + bkt->Sz ; // Dummy name for the oflow area
   // with Lo,Hi,Sz = 2,8,2 we should get bkt_id= 0  1  2  3  4  5
   //                                     Name =  0  2  4  6  8  10
     #ifdef JGMDBG
        if(jgmDebug >= 4) {
         printf("FILL BKT NAMES Result for Bucket %c \n",bkt->Dir );
         for(idx=0; idx < bkt->Num ; idx++ ) {printf(" %4d",idx); }
         printf("\n");
         for(idx=0; idx < bkt->Num ; idx++ ) {printf(" %4d",bkt->Names[idx]); }
         printf("\n");
         //show_array(bkt->Num, bkt->Names) ;
      } /* end jgmDebug >= 4 */
    #endif
   return ( 1 ) ;
} /* end fill bkt names */

int *alloc_bkts2D ( struct bucket_st *d_bkt, struct bucket_st *a_bkt ) {
// alloc two vectors for the BktNames, and one 2d area for the Freq counters. bkt.Num is 2 more than data buckets.
   int *alloc_ptr ;
   alloc_ptr = calloc( (size_t)d_bkt->Num , sizeof(int) ) ;
   if (NULL == alloc_ptr ) {
      perror(" Cant calloc space for 2D BktNamesDown ");
      exit (-7);
   }
   d_bkt->Names = alloc_ptr ;
   d_bkt->Names[0]              = d_bkt->Lo - d_bkt->Sz ; // Dummy init for uflow bkt name
   d_bkt->Names[d_bkt->Num - 1] = d_bkt->Hi + d_bkt->Sz ; // Dummy init for uflow bkt name

   alloc_ptr = calloc((size_t)a_bkt->Num , sizeof(int) ) ;
   if (NULL == alloc_ptr ) {
      perror(" Cant calloc space for 2D BktNamesAcross ");
      exit (-7);
   }
   a_bkt->Names = alloc_ptr ;
   a_bkt->Names[0]              = a_bkt->Lo - a_bkt->Sz ; // Dummy init for uflow bkt name
   a_bkt->Names[a_bkt->Num - 1] = a_bkt->Hi + a_bkt->Sz ; // Dummy init for uflow bkt name

   alloc_ptr  = calloc( (size_t) (a_bkt->Num * d_bkt->Num ) , sizeof(int) ) ; // Both Nums already two extra.
   if (NULL ==  alloc_ptr ) {
      perror(" Cant calloc space for 2D Bkts Freq Counters ");
      exit (-7);
   }
   JGMDPRT(4,"ALLOC2D:: Freq2D ptr = %p , Num of Buckets=%d \n",
                       (void *)alloc_ptr, (a_bkt->Num * d_bkt->Num )  );
   return ( alloc_ptr ) ;  // freq2D set to this value on return
} /* end allocate 2D Ram */

int find_bkt_id(int val, struct bucket_st *bkt ) { /* used only in the 2D case */
    int bkt_id ;
    int uflow = 0 ;
    int oflow = bkt->Num - 1 ;
    if (val < bkt->Lo ) {  return (uflow ) ; }  // uflow
    if (val >= bkt->Lo + ( oflow ) * bkt->Sz ) { return ( oflow ) ; } // oflow
    bkt_id = ( 1 + (val - bkt->Lo )/bkt->Sz );
    /* ex Lo=100, Hi=4100, Sz=400 ; bkt->Num =  1 + (4100 - 100)/400 = 11 */
    JGMDPRT(6,"FIND_BKT_ID:: val=%d, bkt->Lo=%d, bkt->Sz= %d, bkt_id=%d \n", val, bkt->Lo, bkt->Sz, bkt_id ) ;
    return (bkt_id) ;
} /* end find_bkt_id */

int upd_bkt2D(int val_dwn, int val_acr, int *freq2D, struct bucket_st *d, struct bucket_st *a) {
int offset2D ;
int d_bkt_id, a_bkt_id ;
   d_bkt_id = find_bkt_id(val_dwn, d) ;
   a_bkt_id = find_bkt_id(val_acr, a) ;
   offset2D =  (d_bkt_id * a->Num ) + a_bkt_id ; // freq2D[dwn][acr]
   (*(freq2D + offset2D))++ ;                  // update the correct cell in the tbl

   JGMDPRT(6,"UPD2D:: val_dwn=%d, d_bkt_id=%d, val_acr=%d, a_bkt_id=%d, offset2D=%d, Counter[d][a]=%d \n",
                     val_dwn, d_bkt_id, val_acr, a_bkt_id, offset2D, *(freq2D + offset2D) ) ;
   return offset2D ;  // return the offset to the updated counter.
} // end upd_bkt2D

int show_freq2D(int *freq2D, char *description, struct bucket_st *d_bkt, struct bucket_st *a_bkt ) {
   int r, c, oflow_row, oflow_col, row_g_tot, col_g_tot, ctr_val;
   int row_tot, *col_tot ;
   int ctr_offset ;
   printf("\nShowing 2D freq with Description[ %s ]\n", description ) ;

   oflow_col = a_bkt->Num - 1 ;
   oflow_row = d_bkt->Num - 1 ;
   row_g_tot = 0 ;  // keep both of these for cross check
   col_g_tot = 0 ;

// allocate ram for (a_bkt->Num) col totals save ptr in int *col_tot. Access via col_tot[c]
   col_tot = (int *)calloc( (size_t)a_bkt->Num , sizeof(int) ) ;   //calloc will set them to zero to start.
   if (NULL == col_tot ) { perror("show_freq2D cant allocate RAM for Column Totals"); return(-7) ; }

   // across is the number of columns, down is the number of rows.
   // Print the Heading line         Low      1      2      3     4   High    Sum
   printf("         Low");  // c == 0
   for (c = 1 ; c < oflow_col; c++ ) { printf(" %6d", a_bkt->Names[c] ) ; }
   printf("   High    Sum\n"); // c == oflow_col
   // print the rows of counters starting with the underflow row (0)
   for (r = 0 ; r <= oflow_row ;  r++ ) {
      if (0 == r ) { printf(" Low "); }                     // do the row name.
      else if ( oflow_row == r ) { printf(" High"); }
      else { printf("%4d ",d_bkt->Names[r] ) ; }
      row_tot = 0;
      ctr_offset = (r * a_bkt->Num);
      col_tot[c] = 0;  // not necessary since calloc has zeroed them. ?
      for (c = 0 ; c <= oflow_col; c++ ) { // include uflow and oflow cols
         ctr_val = *(freq2D + ctr_offset) ;
         ctr_offset++ ;
         row_tot    += ctr_val ;
         col_tot[c] += ctr_val ;
         printf(" %6d",ctr_val ) ;
      } // end for c
      printf(" %6d\n", row_tot ) ;
      row_g_tot +=  row_tot ;
   } // end for r Done all rows except the col total row.
  printf(" Sum ");
  for (c = 0 ; c <= oflow_col ; c++ ) {
      printf(" %6d", col_tot[c] ) ;
      col_g_tot += col_tot[c] ;
   }
   printf(" %6d\n", col_g_tot );
   if (row_g_tot != col_g_tot ) {
      printf("\n** CANT HAPPEN: Row Grand Total[%d] is NOT EQUAL to Col Grand Total[%d] \n", row_g_tot, col_g_tot );
   }
   return(1) ;
} /* end show_freq2D */



