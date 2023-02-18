/* File std_hdrs.h-- by ;;JGM COllect all the standard headers used in one place.  Use -Iinclude as a compile flag ... */
/* This being a 'new' program we define GNU_SOURCE to profit from all the features possible in the language */
#ifndef STD_HDRS_H
#define STD_HDRS_H 1
#ifndef _GNU_SOURCE
   #define _GNU_SOURCE
#endif
#include <assert.h>
#include <ctype.h>      /* toupper, tolower etc. */
#include <getopt.h>
#include <limits.h>
#include <malloc.h>     /* for malloc_usable_size for debugging.  */
#include <math.h>       /* for float_t, atof , sqrt, roundf, these funcs need -lm as a linker flag*/
#include <stdio.h>
#include <stdlib.h>     /* for rand48 among other things */
#include <string.h>
#include <time.h>       /* localtime(), asctime, ctime, gmtime,  */
#include <unistd.h>     /* sysconf */
#include <sys/random.h> /* for getrandom() */
#include <sys/time.h>   /* why do we need both time.h and this one? */
#include <sys/types.h>  /* /usr/include/x86_64-linux-gnu/sys   for pid_t, sem_t,   */

#endif /* ifndef std_hdrs_h */

