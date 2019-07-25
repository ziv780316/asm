#include <stdio.h>
#include <stdlib.h>
#include <float.h>

int main ( int argc, char **argv )
{
	double x[] = {1.0, 2.0};
	double y[] = {1e-10, 2e-10};
	double z[] = {1e-11, 2e-11};
	register long int iter = 500000000;

	// FPU 80 bit LDBL_EPSILON 2^-63 = 1.08420217248550443401e-19L;
	__asm__( "movlps %0, %%xmm0" : "+m"(x[0]) ); // xmm0.v2_double[0] = x[0]
	__asm__( "movhps %0, %%xmm0" : "+m"(x[1]) ); // xmm0.v2_double[1] = x[1]
	__asm__( "movlps %0, %%xmm1" : "+m"(y[0]) ); // xmm1.v2_double[0] = y[0]
	__asm__( "movhps %0, %%xmm1" : "+m"(y[1]) ); // xmm1.v2_double[1] = y[1]
	__asm__( "movlps %0, %%xmm2" : "+m"(z[0]) ); // xmm2.v2_double[0] = z[0]
	__asm__( "movhps %0, %%xmm2" : "+m"(z[1]) ); // xmm2.v2_double[1] = z[1]

	// use SIMD to accumulate
	for ( register long int i = 0; i < iter; ++i )
	{
		__asm__( "addpd %xmm1, %xmm0" ); 
		__asm__( "subpd %xmm2, %xmm0" );
	}

	__asm__( "movlps %%xmm0, %0" : "+m"(x[0]) ); // x[0] = xmm0.v2_double[0] 
	__asm__( "movhps %%xmm0, %0" : "+m"(x[1]) ); // x[1] = xmm0.v2_double[1] 

	printf( "SIMD accumulate results:\n");
	printf( "x[0] = %.16le\n", x[0] );
	printf( "x[1] = %.16le\n", x[1] );

	return EXIT_SUCCESS;
}
