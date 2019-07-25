#include <stdio.h>
#include <stdlib.h>
#include <float.h>

typedef union
{
	double f;
	unsigned long long hex;
} num;

int main ( int argc, char **argv )
{
	num x;
	num y;
	num z;

	x.f = 1.0;
	y.f = 1e-17; 

	// SSE 64 bit DBL_EPSILON 2^-53 = 2.22044604925031308085e-16L
	z.f = (x.f + y.f) - x.f;
	printf( "SSE 64 bit round off results = %.16le\n", z.f ); 

	// FPU 80 bit LDBL_EPSILON 2^-63 = 1.08420217248550443401e-19L;
	__asm__( "fldl %0" : "+m"(x) ); // st0 = x
	__asm__( "fldl %0" : "+m"(y) ); // st0 = y, st1 = x
	__asm__( "fadd %st(1), %st(0)" ); // st0 = y + x
	__asm__( "fsubr %st(1), %st(0)" ); // st0 = (y + x) - x
	__asm__( "fstpl %0" : "=m"(z) ); // pop z = st0 = (y + x) - x 
	printf( "FPU 80 bit round off results = %.16le\n", z.f );

	return EXIT_SUCCESS;
}
