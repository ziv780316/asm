#include <stdio.h>
#include <stdlib.h>
#include <float.h>

int main ( int argc, char **argv )
{
	double x[] = {1.0, 2.0};
	double y[] = {1e-10, 2e-10};
	double z[] = {1e-11, 2e-11};
	register long int iter = 500000000;

	// use SIMD to accumulate
	for ( register long int i = 0; i < iter; ++i )
	{
		x[0] += y[0] - z[0];
		x[1] += y[1] - z[1];
	}

	printf( "SISD accumulate results:\n");
	printf( "x[0] = %.16le\n", x[0] );
	printf( "x[1] = %.16le\n", x[1] );

	return EXIT_SUCCESS;
}

