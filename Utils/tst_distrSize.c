/* tst_distrSize.c */
#include <stdio.h>
#include <errno.h>
#include "../include/dbgprt_macros.h"
#define DISTR_ITEM long int
#define MAXDISTR 8*sizeof(long)
// cant use sizeof(DISTR_ITEM) in the above because ? macros dont recurse? does not seem right.
enum { FIRST , SECOND } ;
int main(int argc, char *argv[] ) {
DISTR_ITEM distr[10] ;
int maxdistr = MAXDISTR ;
printf(" Max distr=%d, Size of distr[]=%zd \n",maxdistr, sizeof(distr) ) ;
printf(" Enum First=%d, Second=%d \n", FIRST, SECOND );
return (0 ) ;

} /* end main */
