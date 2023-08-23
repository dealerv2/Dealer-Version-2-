/* mmap_parent_skel.c */
#define _GNU_SOURCE
#define JGMDBG 1
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/dbgprt_macros.h"
/* I do not think that main can modifiy the contents of argv or of argv[i]; These were setup by the kernel */
int main(int argc, char *argv[] ) {
   char childname[] = "Mmapchild";
   char child_fd[32] = "9876543210";
   char *args[3] ;   /* array of 3 pointers to char */
   char *defaults[3] = {"/usr/bin/default", "-t text parm", NULL} ; // init two of the ptrs to point to anon arrays; 3rd ptr=NULL */
   args[0] = childname ;      // copy the address of childname[0] to args[0]; args[0] now pts to the 1st byte of childname.
   args[1] = child_fd  ;      // copy the address of child_fd[0] to args[1]; args[1] now pts to the 1st byte of child_fd
   args[2] = (char *)NULL ;   // put a value of zero into the args[2] slot.
   char Str[3][32] ;          // allocate a block of storage for Str. Str is an arry of 3 ptrs to 3 arrays of 32 chars.
   char *ptr0, *ptr1, *ptr2 ;
   int xx_fd = 6 ;
   printf("&args[0]=%p, *args[0]=%c, args[0]=%s \n", args[0], *args[0], args[0] ) ;
   printf("&defaults[0]=%p, *defaults[0]=%c, defaults[0]=%s \n", defaults[0], *defaults[0], defaults[0] ) ;
   ptr2 = Str[2] ;
   snprintf(Str[2], 31, "%s" , "String Num 2" ) ;
   printf("                &Str2=%p, Str2=[%s], Str2len=%ld \n", Str[2], Str[2], strlen(Str[2]) ) ;
   printf("With Pointers:: &ptr2=%p, ptr2=[%s], ptr2len=%ld \n", ptr2, ptr2, strlen(ptr2) ) ;
      printf("MAIN :: Args[0]=%s, Args[1]=%s \n",args[0],args[1] ) ;
      printf("MAIN :: Args[0]_ptr=%p, Args[1_ptr]=%p Args[2]_ptr=%p\n",args[0],args[1],args[2] ) ;
      ptr0 = args[0] ;
      ptr1 = args[1] ;
      ptr2 = args[2] ;
      printf("MAIN :: ptr0=%p, ptr1=%p ptr2=%p\n",args[0],args[1],args[2] ) ;
      printf("MAIN :: ptr0=%s, ptr1=%s ptr2=%s\n",ptr0, ptr1, ptr2) ;
      strcpy( ptr1, "7654321" ) ;
      printf("MAIN :: ptr0=%s, ptr1=%s ptr2=%s\n",ptr0, ptr1, ptr2) ;
      snprintf( ptr1, 5, "%4d",xx_fd);
      printf("MAIN :: ptr0=%s, ptr1=%s ptr2=%s\n",ptr0, ptr1, ptr2) ;
      return (0) ;
}
#if 0
The above compiles with no warnings.
The output is:
&args[0]=0x7fff0af8f3d6, *args[0]=M, args[0]=Mmapchild
&defaults[0]=0x5630bbaca182, *defaults[0]=/, defaults[0]=/usr/bin/default
                &Str2=0x7fff0af8f440, Str2=[String Num 2], Str2len=12
With Pointers:: &ptr2=0x7fff0af8f440, ptr2=[String Num 2], ptr2len=12
MAIN :: Args[0]=Mmapchild, Args[1]=9876543210
MAIN :: Args[0]_ptr=0x7fff0af8f3d6, Args[1_ptr]=0x7fff0af8f3e0 Args[2]_ptr=(nil)
MAIN :: ptr0=0x7fff0af8f3d6, ptr1=0x7fff0af8f3e0 ptr2=(nil)
MAIN :: ptr0=Mmapchild, ptr1=9876543210 ptr2=(null)
MAIN :: ptr0=Mmapchild, ptr1=7654321 ptr2=(null)
MAIN :: ptr0=Mmapchild, ptr1=   6 ptr2=(null)
#endif

