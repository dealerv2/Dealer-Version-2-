/* File std_hdrs.h-- by ;;JGM COllect all the standard headers used in one place.  Use -Iinclude as a compile flag ... */
/* The header files used by the main dealerv2 code and associated functions */
/* This being a 'new' program we define GNU_SOURCE to profit from all the features possible in the language */
#ifndef STD_HDRS_H
#define STD_HDRS_H 1
#ifndef _GNU_SOURCE
   #define _GNU_SOURCE
#endif
#include <assert.h>
#include <ctype.h>            /* toupper, tolower etc. */
#include <errno.h>            /* errno, ECHILD        */
#include <getopt.h>
#include <limits.h>           /* bits/char; max and min for ints and unsigned ints etc. */
#include <malloc.h>           /* for malloc_usable_size for debugging.  */
#include <math.h>             /* for atof and sqrt -- sqrt needs -lm as a linker flag*/
#include <stddef.h>           /* offsetof                            */
#include <stdio.h>            /* printf(), scanf,                    */
#include <stdlib.h>           /* exit(), malloc(), free(), rand48,   */
#include <string.h>           /* memcpy, memset, strcpy, strlen etc. */
#include <time.h>             /* localtime(), asctime, ctime, gmtime,*/
#include <unistd.h>           /* sysconf, usleep, */
#include <sys/random.h>       /* for getrandom() */
#include <sys/time.h>         /* why do we need both time.h and this one? */
#include <sys/types.h>        /* /usr/include/x86_64-linux-gnu/sys   for pid_t  */

#endif /* ifndef std_hdrs_h */

