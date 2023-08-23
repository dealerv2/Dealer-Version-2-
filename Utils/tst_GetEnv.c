/* tst_GetEnv.c */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "../include/dbgprt_macros.h"
#define DISTR_ITEM long int
#define MAXDISTR 8*sizeof(long)
// cant use sizeof(DISTR_ITEM) in the above because ? macros dont recurse? does not seem right.
enum { FIRST , SECOND, FIFTH=5, SIXTH } ; // enums start at zero unless they are given a value.
int main(int argc, char *argv[] ) {
DISTR_ITEM distr[10] ;
int maxdistr = MAXDISTR ;
printf(" Max distr=%d, Size of distr[]=%zd \n",maxdistr, sizeof(distr) ) ;
printf(" Enum First=%d, Second=%d, Fifth=%d, Sixth=%d \n", FIRST, SECOND, FIFTH, SIXTH );

char myENVstr[256] ;
char *getenv_rc ;

if ( (getenv_rc = getenv("PAPERSIZE")) ) printf("Papersize=[ %s ] \n", getenv_rc ); else printf("PAPERSZ Not found \n");
if ( (getenv_rc = getenv("OLDPWD"))    ) printf("Oldpwd=[ %s ] \n", getenv_rc) ; else printf("OLDPWD Not found \n");
if ( (getenv_rc = getenv("JGMDBG"))    ) printf("JGMDBG=[ %s ] \n", getenv_rc) ; else printf("JGMDBG Not found \n");



return (0 ) ;

} /* end main */
