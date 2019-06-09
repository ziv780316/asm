#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void sum ( int nnum, ... )
{
	int sum = 0;
	va_list args;
	va_start( args, nnum );
	for ( int i = 0; i < nnum; ++i )
	{
		sum += va_arg( args, int );
	}
	printf( "sum=%d\n", sum );
}

int main ( int argc, char **argv )
{
	sum( 3, 1, 2, 3 );

	return EXIT_SUCCESS;
}

