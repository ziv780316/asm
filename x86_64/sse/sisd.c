#include <stdio.h>
#include <stdlib.h>
#include <float.h>

int main ( int argc, char **argv )
{
	double x[] = {1.0, 2.0};
	double y[] = {1e-10, 2e-10};
	double z[] = {1e-11, 2e-11};
	long int iter = 500000000;

	__asm__( "movsd %0, %%xmm0" : "+m"(x[0]) ); 
	__asm__( "movsd %0, %%xmm1" : "+m"(x[1]) ); 
	__asm__( "movsd %0, %%xmm2" : "+m"(y[0]) ); 
	__asm__( "movsd %0, %%xmm3" : "+m"(y[1]) ); 
	__asm__( "movsd %0, %%xmm4" : "+m"(z[0]) ); 
	__asm__( "movsd %0, %%xmm5" : "+m"(z[1]) ); 

	__asm__( "xorq %rcx, %rcx" ); 
	__asm__( "movq %0, %%r8" : "+m"(iter) ); 
	__asm__( "jmp loop_cmp" );

// --------------------------------------------------------------------------

	// use SISD to accumulate x[i] += y[i] - z[i]
	__asm__( "loop_start:" ); 
	__asm__( "addsd %xmm2, %xmm0" ); 
	__asm__( "addsd %xmm3, %xmm1" ); 
	__asm__( "subsd %xmm4, %xmm0" ); 
	__asm__( "subsd %xmm5, %xmm1" ); 

// --------------------------------------------------------------------------

	__asm__( "incq %rcx" ); // ++i
	__asm__( "loop_cmp:" );
	__asm__( "cmpq %r8, %rcx" ); // i <= iter
	__asm__( "jle loop_start" );

	__asm__( "movsd %%xmm0, %0" : "+m"(x[0]) ); 
	__asm__( "movsd %%xmm1, %0" : "+m"(x[1]) ); 

	printf( "SISD accumulate results:\n");
	printf( "x[0] = %.16le\n", x[0] );
	printf( "x[1] = %.16le\n", x[1] );

	return EXIT_SUCCESS;
}

