
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/mapmutex_hdrs.h"


#ifndef C_SUIT
  #define C_SUIT(c)    ( ((c)>>4)&0xF )
  #define C_RANK(c)    (  (c)&0xF     )
#endif
struct data_st {
   int flag ;
   int kards[4][13] ;
} ;
struct data_st ds[4];
struct data_st *pds ;
int            *pkard, **ppkard ;


int main(int argc, char *argv[] ) {
   int  crd , seat ;
   pds = ds ;
   pds = &ds[0] ;
   struct mmap_hdr_st mmap_hdr;           /*   48 bytes */
   struct query_data_st query_data ;      /* 1560 bytes */
   struct reply_data_st reply_data ;      /*  628 bytes */
   struct handstat_st hs ;                   /*  368 bytes */
   struct results_st results ;            /*  148 bytes */
   typedef struct mmap_hdr_st MMAP_K ;
   printf("Size Of mmap_hdr_st= %zd \n", sizeof(mmap_hdr) );
   printf("Size Of query_data_st= %zd \n", sizeof(query_data) );
   printf("Size Of reply_data_st= %zd \n", sizeof(reply_data) );
   printf("Size Of handstat_st= %zd \n", sizeof(hs) );
   printf("Size Of results_st= %zd \n", sizeof(results) );
   printf("----- Testing sizeof on the struct definitions not on the variables \n");
   printf("Size Of mmap_hdr_st= %zd \n", sizeof(struct mmap_hdr_st) );
   printf("Size Of MMAP_K= %zd \n", sizeof(MMAP_K) );

   /* syntax check  all these next ones work. */
   crd = ds[2].flag ;
   pds = &ds[2] ;
   crd = pds->flag ;
   seat = 2 ;
   pds = ds ;
   pds += seat ;
   crd = pds->flag ;
   crd = (pds + seat)->flag;
   crd = pds->kards[1][11];
 /* next ones do not */
   // crd = (*pds + seat).flag ;

   /* Checking pointer to 1D kards array */
   crd = ds[3].kards[2][0];
   pkard = &ds[3].kards[2][0];
   crd = *(pkard+4);
   crd = pkard[4];
/* Checking pointer to 2D kards array */
   ppkard = &ds[1].kards[1];








   return(0) ;
}





