/* File std_hdrs.h-- by ;;JGM COllect all the standard headers used in one place.  Use -Iinclude as a compile flag ... */
/* The headers used by the main dealer code, augmented by the stuff needed by the UserEval code */
#ifndef STD_HDRS_H
#define STD_HDRS_H 1
#ifndef _GNU_SOURCE
   #define _GNU_SOURCE
#endif
#include <assert.h>           /* dbg  suppress with -DNDEBUG on the gcc line*/
#include <ctype.h>            /* toupper, tolower etc. */
#include <errno.h>            /* errno, ECHILD            */
#include <fcntl.h>            /* O_CREAT, O_EXEC          */
#include <getopt.h>
#include <limits.h>           /* bits/char; max and min for ints and unsigned ints etc. */
#include <malloc.h>           /* for malloc_usable_size for debugging.  */
#include <math.h>             /* for atof and sqrt -- sqrt needs -lm as a linker flag*/
#include <pthread.h>          /* required for posix semaphore use? */
#include <semaphore.h>        /* sem_open(), sem_destroy(), sem_wait().. Posix version */
#include <signal.h>           /* to Kill process and also report on child exit status ? */
#include <stddef.h>           /* offsetof , size_t                  */
#include <stdio.h>            /* printf(), scanf,                   */
#include <stdlib.h>           /* exit(), malloc(), free(), rand48,  */
#include <string.h>           /* memcpy, memset, strcpy, strlen etc. */
#include <time.h>             /* localtime(), asctime, ctime, gmtime,  */
#include <unistd.h>           /* sysconf -- to get Pagesize */
#include <sys/mman.h>         /* mmap                     */
#include <sys/random.h>       /* for getrandom() */
#include <sys/stat.h>         /* To define stat struct. used to check existence of files */
#include <sys/time.h>         /* why do we need both time.h and this one? */
#include <sys/types.h>        /* /usr/include/x86_64-linux-gnu/sys   for pid_t  */
#include <sys/wait.h>         /* Wait status etc. for server */

#endif /* ifndef std_hdrs_h */



