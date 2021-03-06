#include <stdio.h>
#include <stdlib.h>
#include <float.h>

int main ( int argc, char **argv )
{
	double x = 1.0;
	double y = 1e-10;
	double z = 1e-11;
	long int iter = 500000000;

	__asm__( "movsd %0, %%xmm0" : "+m"(x) ); 
	__asm__( "movsd %0, %%xmm1" : "+m"(y) ); 
	__asm__( "movsd %0, %%xmm2" : "+m"(z) ); 

	__asm__( "xorq %rcx, %rcx" ); 
	__asm__( "movq %0, %%r8" : "+m"(iter) ); 
	__asm__( "jmp loop_cmp" );

// --------------------------------------------------------------------------

	// use SISD to accumulate x += y - z
	__asm__( "loop_start:" ); 
	__asm__( "addsd %xmm1, %xmm0" ); 
	__asm__( "subsd %xmm2, %xmm0" ); 

// --------------------------------------------------------------------------

	__asm__( "incq %rcx" ); // ++i
	__asm__( "loop_cmp:" );
	__asm__( "cmpq %r8, %rcx" ); // i <= iter
	__asm__( "jle loop_start" );

	__asm__( "movsd %%xmm0, %0" : "+m"(x) ); 

	printf( "SISD accumulate results:\n");
	printf( "x[0] = %.16le\n", x );

	return EXIT_SUCCESS;
}

