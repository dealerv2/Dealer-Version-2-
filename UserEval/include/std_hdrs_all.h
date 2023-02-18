/* File std_hdrs.h-- by ;;JGM COllect all the standard headers used in one place.  Use -Iinclude as a compile flag ... */
/* This being a 'new' program we define GNU_SOURCE to profit from all the features possible in the language */
#ifndef STD_HDRS_H
#define STD_HDRS_H 1
#ifndef _GNU_SOURCE
   #define _GNU_SOURCE
#endif
#include <assert.h>         /* dbg  suppress with -DNDEBUG on the gcc line*/
#include <ctype.h>          /* toupper, tolower etc. */
#include <errno.h>          /* errno, ECHILD            */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <getopt.h>
#include <limits.h>
#include <malloc.h>           /* for malloc_usable_size for debugging.  */
#include <math.h>             /* for float_t, double_t, atof , sqrt roundf, these funcs need -lm as a linker flag*/
#include <pthread.h>          /* required for posix semaphore use? */
#include <semaphore.h>        /* sem_open(), sem_destroy(), sem_wait().. Posix version */
#include <signal.h>           /* to Kill process and also report on child exit status ? */
#include <stddef.h>           /* offsetof                 */
#include <stdio.h>            /* printf()                 */
#include <stdlib.h>           /* exit(), malloc(), free(), rand48,  */
#include <string.h>
#include <time.h>             /* localtime(), asctime, ctime, gmtime,  */
#include <unistd.h>           /* fsync, sysconf -- to get Pagesize */
#include <sys/mman.h>         /* mmap                     */
#include <sys/random.h>       /* for getrandom() */
#include <sys/stat.h>         /* To define stat struct. used to check existence of files */
#include <sys/time.h>         /* why do we need both time.h and this one? */
#include <sys/types.h>        /* /usr/include/x86_64-linux-gnu/sys   for pid_t,  sem_t, key_t, ...*/
#include <sys/wait.h>         /* Wait status etc. for server */

#endif /* ifndef std_hdrs_h */



