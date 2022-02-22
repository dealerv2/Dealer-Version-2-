/* File std_hdrs.h-- by ;;JGM COllect all the standard headers used in one place.  Use -Iinclude as a compile flag ... */
/* This being a 'new' program we define GNU_SOURCE to profit from all the features possible in the language */
#ifndef STD_HDRS_H
#define STD_HDRS_H 1
#ifndef _GNU_SOURCE
   #define _GNU_SOURCE
#endif
#include <ansidecl.h>
#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>       /* for atof and sqrt -- sqrt needs -lm as a linker flag*/
#include <stddef.h>     /* size_t ...  */
#include <stdio.h>
#include <stdlib.h>    /* for rand48 among other things */
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/random.h>    /* for getrandom() */
#include <sys/time.h>       /* for gettime and timeval structs etc. */
#include <sys/types.h>      /* This is not working. for u_int16_t and u_int_64_t as defined by ISO. */
// #include "/usr/include/x86_64-linux-gnu/sys/types.h"
#endif /* ifndef std_hdrs_h */

