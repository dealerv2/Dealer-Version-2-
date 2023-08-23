/* union_skel.c */
#define _GNU_SOURCE
#define JGMDBG 1
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "../include/dbgprt_macros.h"
int main(int argc, char *argv[] ) {
 struct results_st {
    char descr [16] ;
    short int  utag  ;
    union {
      struct {
         short  int  Hs[4][4]   ;   // North[Cl, Di, He, Sp] : East[Cl, Di, He, Sp] : S[...] : W[...]
         short  int  Hand_tot[4];
         short  int  Ss[2][5]   ;  // NS[Cl, Di, He, Sp, NT] : EW[Cl, Di, He, Sp, NT]
         short  int  Side_tot[2];  // NS, EW ; not really a total of above just the final evaluation for the side.
             } dl ;                // Only 32 ints even though the union is 64 ints.
      struct {             /* first the ones that need only one hand hence on a per hand basis*/
               short int hcp_nd[4], hcp_n, hcp_sd[4], hcp_s, len_nd[4], len_n, len_sd[4], len_s,
                         syq_nd[4], syq_n, syq_sd[4], syq_s, rich_n, rich_s, sq_n, sq_s, tot_n, tot_s ;  //36
                         /* 2nd the ones that need both hands, hence on a per side basis */
               short int fn_d[4], fn, HfDm_d[4], HfDm, waste_d[4], waste, Dfit[4] , Dfit_l, NT_ded, mirror, tot_NS ; //59
               short int spare[5] ;
             } f;                /* the 'Factors' struct. */
      short int v[64] ;       /* generic access to any int in the above */
      char buf[128]   ;       /* generic access to any byte in the above */
   } u ;
} evl ;   /* syntax evl.u.dl.Hs[compass][suit] or evl.u.f.hcp_nd[suit] ...  */
strcpy(evl.descr, "64 Ints" );
evl.utag = 1 ;

printf("Sizes of the External Structure members using 16 bit ints \n");
printf("Results Struct [%zd] \n", sizeof(evl) );
printf("Union evl Union [%zd] \n", sizeof(evl.u) );
printf("Factor evl.u.f[%zd] \n", sizeof(evl.u.f) );
printf("Value Array evl.u.v[%zd] \n", sizeof(evl.u.v) );

for (int i=0 ; i<64 ; i++ ) { evl.u.v[i] = i*i ; }
for (int i=0 ; i<8 ; i++ ) { printf("%d |", evl.u.v[i] ); } printf("\n") ;

for (int i=8 ; i<16 ; i++ ) { printf("%d |" , evl.u.v[i]); } printf("\n") ;
for (int i=16; i<24 ; i++ ) { printf("%d |" , evl.u.v[i]); } printf("\n") ;
for (int i=24; i<32 ; i++ ) { printf("%d |" , evl.u.v[i]); } printf("\n") ;
for (int i=32; i<40 ; i++ ) { printf("%d |" , evl.u.v[i]); } printf("\n") ;
for (int i=40; i<48 ; i++ ) { printf("%d |" , evl.u.v[i]); } printf("\n") ;
for (int i=48; i<56 ; i++ ) { printf("%d |" , evl.u.v[i]); } printf("\n") ;
for (int i=56; i<64 ; i++ ) { printf("%d |" , evl.u.v[i]); } printf("\n") ;


for (int c=0; c<4 ; c++ ) {
   printf("%c= ","NESW"[c] ) ;
   for (int s=0; s<4 ; s++ ) {
      printf("%d ", evl.u.dl.Hs[c][s] );
   }
   printf("Hand tot=%d \n",evl.u.dl.Hand_tot[c]) ;
}
for (int S=0; S<2 ; S++ ) {
   printf("%c%c= ","NE"[S],"SW"[S] ) ;
   for (int s=0; s<5 ; s++ ) {
      printf("%d ", evl.u.dl.Ss[S][s] );
   }
   printf("Side tot=%d \n",evl.u.dl.Side_tot[S]) ;
}


} /* end main */
#if 0
// Results of the above are:
Sizes of the External Structure members using 16 bit ints
Results Struct [146]
Union evl Union [128]
Factor evl.u.f[128]
Value Array evl.u.v[128]
0 |1 |4 |9 |16 |25 |36 |49 |
64 |81 |100 |121 |144 |169 |196 |225 |
256 |289 |324 |361 |400 |441 |484 |529 |
576 |625 |676 |729 |784 |841 |900 |961 |
1024 |1089 |1156 |1225 |1296 |1369 |1444 |1521 |
1600 |1681 |1764 |1849 |1936 |2025 |2116 |2209 |
2304 |2401 |2500 |2601 |2704 |2809 |2916 |3025 |
3136 |3249 |3364 |3481 |3600 |3721 |3844 |3969 |
N= 0 1 4 9 Hand tot=256
E= 16 25 36 49 Hand tot=289
S= 64 81 100 121 Hand tot=324
W= 144 169 196 225 Hand tot=361
NS= 400 441 484 529 576 Side tot=900
EW= 625 676 729 784 841 Side tot=961
#endif
