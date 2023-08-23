#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include <math.h>
#include <float.h>


// ctype.h is definitions for character handling routines
#include <ctype.h>
#include <sys/types.h>
#include <gnu/stubs.h>
#include <features.h>
// Mydefs including VERSION
#include "jgm1.h"
void show_float_rounding( float_t ) ;
void show_float_details ( void ) ;
void show_double_details ( void ) ;
void show_long_double_details ( void ) ;
int show_type_sizes( void ) ;
struct decnum_st {
   short int f ;
   short int i ;
   int       v_100;
   float_t   vf ;

};
union decnum_ut {
   int dnv ;
   struct decnum_st dns ;
} ;
char *arglist[4] = { "Cat", "Dog", "Horse", (char *)NULL }  ;
int main() {
   int i;
   char cars[][10] = {"Volvo", "BMW", "Ford", "Mazda","DONE" }; // Note first subscript not needed
   char fullName[] = "James Tiberius Kirk";
   char result[20] ;
   union decnum_ut hcpx ;
   struct decnum_st cpu_pts;
   show_float_rounding(1234.25) ;
   show_float_rounding(1234.50) ;
   show_float_rounding(1234.75) ;
   return 0 ;
   printf( "\n Hello %s you are testing Version %s \n", fullName, VERSION ) ;
   printf( " Enter YOUR name: " );
   scanf("%s", fullName ) ;
   printf( "\n Welcome %s The honor is all yours.\n", fullName);
   hcpx.dns.i=30 ; hcpx.dns.f=75 ;
   cpu_pts.i = 40 ; cpu_pts.f = 25 ;  cpu_pts.v_100 = cpu_pts.i*100 + cpu_pts.f ;
   printf("Decnum Ipart=%d, Fpart=%d, fullVal=%d, floatVal=%10.4f \n",
               hcpx.dns.i, hcpx.dns.f, hcpx.dnv, (float_t)hcpx.dnv ) ;
   cpu_pts.i = 40 ; cpu_pts.f = 25 ;
   cpu_pts.v_100 = cpu_pts.i*100 + cpu_pts.f ;
   cpu_pts.vf = cpu_pts.i + cpu_pts.f/100.0 ;
   printf("CPUpts Ipart=%d, Fpart=%d, fullVal=%d, floatVal=%10.4f \n",
               cpu_pts.i, cpu_pts.f, cpu_pts.v_100, cpu_pts.vf ) ;

   int tod_hh = 20;
   strcpy(result,  (tod_hh < 18) ? "Good day." : "Good evening." ) ;
   printf ("%s\n",result);
   for ( i= 0 ; i<5 ; i++ ) {
      printf("Car[%d]=%s\n", i, cars[i] );
   }
   for (i = 0 ; arglist[i] ; i++ ) { printf("arglist[%d]=%p -> %s \n", i, arglist[i], arglist[i] ); }
   i = 0;
   printf( "While Looping .... \n" );
   while (i < 5) {
      printf("%d , ", i );
      i++;
   } // end while
   printf("  i after while loop exit=%d\n", i);

   printf( "For Looping .... \n" );
   for ( i = 6; i > 0; i--) {
      printf("%d,  ", i );
   } // end for
   printf("i after for downto 0  loop exit=%d\n", i);
   for (i = 1 ; i < 4 ; i++ ) { ; }
   printf("i after for upto 4 loop exit=%d\n", i);
     show_type_sizes() ;
     show_float_details() ;
     show_double_details() ;
     show_long_double_details() ;
   return 0;
} // end main

int show_type_sizes() {
   long int short_size, int_size, long_size, quad_size, ptr_size, char_size, float_size, double_size, long_dsize;
   short int i;
   int j;
   long int k ;
   long long int l ;
   char ch;
   float fnum;
   double dnum ;
   long double ldnum ;
   char *ptr;
short_size = sizeof( i );
int_size = sizeof( j );
long_size = sizeof( k );
quad_size = sizeof( l );
ptr_size = sizeof ( ptr ) ;
char_size = sizeof (  ch ) ;
float_size = sizeof (  fnum ) ;
double_size = sizeof (  dnum ) ;
long_dsize = sizeof( ldnum ) ;

printf( "char_size   = %ld\n",   char_size    ) ;
printf( "short_size  = %ld\n",    short_size  ) ;
printf( "int_size    = %ld\n",   int_size     ) ;
printf( "long_size   = %ld\n",   long_size    ) ;
printf( "quad_size   = %ld\n",   quad_size    ) ;
printf( "ptr_size    = %ld\n",   ptr_size     ) ;
printf( "float_size  = %ld\n",   float_size   ) ;
printf( "double_size = %ld\n",   double_size  ) ;
printf( "long_d_size = %ld\n",   long_dsize   ) ;

return 0 ;
} // end show_type_sizes

void show_float_details ( void ) {
  puts("\nCharacteristics of type float");
  printf("Storage size: %zd bytes \n"
         "Smallest positive value: %E\n"
         "Greatest positive value: %E\n"
         "Precision: %d decimal digits\n",
         sizeof(float), FLT_MIN, FLT_MAX, FLT_DIG ) ;
  double dvar = 12345.6;
  float  fvar = (float)dvar ;
  printf("The floating point number          : %18.10f\n", dvar  );
  printf("Has been stored in a float variable: %18.10f\n", fvar  );
  printf("The rounding error is              : %18.10f\n", (dvar - fvar) ) ;
  return ;
}
void show_double_details ( void ) {
  puts("\nCharacteristics of type double");
  printf("Storage size: %zd bytes \n"
         "Smallest positive value: %E\n"
         "Greatest positive value: %E\n"
         "Precision: %d decimal digits\n",
         sizeof(double), DBL_MIN, DBL_MAX, DBL_DIG ) ;
  long double ldvar = 12345.6;
  double  dvar = (double)ldvar ;
  printf("The long double floating  point number: %18.10Lf\n", ldvar  );
  printf("Has been stored in a double variable  : %18.10f\n", dvar  );
  printf("The rounding error is                 : %18.10Lf\n", (ldvar - dvar) ) ;
  return ;
}
void show_long_double_details ( void ) {
  puts("\nCharacteristics of type long double");
  printf("Storage size: %zd bytes \n"
         "Smallest positive value: %LE\n"
         "Greatest positive value: %LE\n"
         "Precision: %d decimal digits\n",
         sizeof(long double), LDBL_MIN, LDBL_MAX, LDBL_DIG ) ;
  long double ldvar = 12345.6;
  long double  dvar = (long double)ldvar ;
  printf("The long double floating  point number: %18.10Lf\n", ldvar  );
  printf("Has been stored in a long double      : %18.10Lf\n", dvar  );
  printf("The rounding error is                 : %18.10Lf\n", (ldvar - dvar) ) ;
  return ;
}
void show_float_rounding( float_t z )  {
   float_t x, ox, cx, fx, nx, rx , tx ; // round(x), ceil(x), floor(x), nearbyint(x), rint(x), trunc(x)
   x = z;
   ox = roundf(x);            // round away from zero
   cx = ceilf(x);             // smallest int, >= x
   fx = floorf(x);            // largest  int  <= x
   nx = nearbyintf(x);        // quiet version of rint. IEEE rounding ; 1/2 goes to nearest even
   rx = rintf(x);             //  IEEE rounding ; halfway cases go to nearest even int.
   tx = truncf(x);            // round to nearest int not larger in magnitude than x.
   printf("[ %12.7E : %10.7G] = %10.7G, %10.7G, %10.7G, %10.7G, %10.7G, %10.7G \n", x,x, ox, cx, fx, nx, rx, tx );

   x = 100.0*z ;
   ox = roundf(x);            // round away from zero
   cx = ceilf(x);             // smallest int, >= x
   fx = floorf(x);            // largest  int  <= x
   nx = nearbyintf(x);        // quiet version of rint. IEEE rounding ; 1/2 goes to nearest even
   rx = rintf(x);             //  IEEE rounding ; halfway cases go to nearest even int.
   tx = truncf(x);            // round to nearest int not larger in magnitude than x.
   printf("[ %12.7E : %10.7G] = %10.7G, %10.7G, %10.7G, %10.7G, %10.7G, %10.7G \n", x,x, ox, cx, fx, nx, rx, tx );
   x = -z ;
   ox = roundf(x);            // round away from zero
   cx = ceilf(x);             // smallest int, >= x
   fx = floorf(x);            // largest  int  <= x
   nx = nearbyintf(x);        // quiet version of rint. IEEE rounding ; 1/2 goes to nearest even
   rx = rintf(x);             //  IEEE rounding ; halfway cases go to nearest even int.
   tx = truncf(x);            // round to nearest int not larger in magnitude than x.
   printf("[ %12.7E : %10.7G] = %10.7G, %10.7G, %10.7G, %10.7G, %10.7G, %10.7G \n", x,x, ox, cx, fx, nx, rx, tx );


   /* if x = -1234.56 the above asks for 7 significant digits. The results are:
[ -1.2345601E+03 :   -1234.56] =      -1235,      -1234,      -1235,      -1235,      -1235,      -1234
[  1.2345601E+03 :    1234.56] =       1235,       1235,       1234,       1235,       1235,       1234
  */
  printf("---------------------------------------\n");
   return ;
}
#if 0
/*
 * the output is
Hello James Tiberius Kirk you are testing Version 0.05 std
Good evening.
While Looping ....
0
1
2
3
4
For Looping ....
6
5
4
3
2
1
*   these next ones are the same in C++
char_size   = 1
short_size  = 2
int_size    = 4
long_size   = 8
quad_size   = 8
ptr_size    = 8
float_size  = 4
double_size = 8
*/
/* the showtypes.c program tests the sizes of things like uint32_t etc.
 * Results are similar to above, so we can substitute if needed.
Hello Types
uint16_size  = 2
uint32_size  = 4
uint64_size  = 8
quad_size    = 8
ll_size      = 8
*/
#endif



