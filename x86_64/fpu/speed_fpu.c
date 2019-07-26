#include <stdio.h>
#include <stdlib.h>
#include <float.h>

int main ( int argc, char **argv )
{
	double x = 1.0;
	double y = 1e-10;
	double z = 1e-11;
	long int iter = 500000000;

	__asm__( "fldl %0" : "+m"(z) ); // st0 = z, 
	__asm__( "fldl %0" : "+m"(y) ); // st0 = y, st1 = z
	__asm__( "fldl %0" : "+m"(x) ); // st0 = x, st1 = y, st2 = z

	__asm__( "xorq %rcx, %rcx" ); 
	__asm__( "movq %0, %%r8" : "+m"(iter) ); 
	__asm__( "jmp loop_cmp" );

// --------------------------------------------------------------------------

	// use FPU to accumulate x += y - z
	__asm__( "loop_start:" ); 
	__asm__( "fadd %st(1), %st(0)" ); // st0 = x + y
	__asm__( "fsubr %st(2), %st(0)" ); // st0 = (x + y) - z

// --------------------------------------------------------------------------

	__asm__( "incq %rcx" ); // ++i
	__asm__( "loop_cmp:" );
	__asm__( "cmpq %r8, %rcx" ); // i <= iter
	__asm__( "jle loop_start" );

	__asm__( "fstpl %0" : "=m"(x) ); // pop x = st0

	printf( "FPU accumulate results:\n");
	printf( "x0 = %.16le\n", x );

	return EXIT_SUCCESS;
}

