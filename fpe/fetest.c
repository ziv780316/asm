#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fenv.h>
#include <float.h>
#include <math.h>

// use following instructions to reset fe
// fldenv for FPU
// ldmxcsr for SSE
void clear_fe ()
{
	feclearexcept ( FE_ALL_EXCEPT );
}

// use following instructions to get fe 
// fnstsw for FPU
// stmxcsr for SSE
bool check_fe ( bool print_error )
{
	bool fe_occur = false;
	if ( fetestexcept( FE_ALL_EXCEPT ) )
	{
		fe_occur = true;
	}

	if ( print_error )
	{
		if ( fetestexcept ( FE_INEXACT ) )
		{
			fprintf( stderr, "[FE] inexact result due to numerical round-off\n" );
		}
		if ( fetestexcept ( FE_DIVBYZERO ) )
		{
			fprintf( stderr, "[FE] division by zero\n" );
		}
		if ( fetestexcept ( FE_OVERFLOW ) )
		{
			fprintf( stderr, "[FE] overflow\n" );
		}
		if ( fetestexcept ( FE_UNDERFLOW ) )
		{
			fprintf( stderr, "[FE] underflow\n" );
		}
		if ( fetestexcept ( FE_INVALID ) )
		{
			fprintf( stderr, "[FE] invalid operation produce nan (e.g. sqrt(-1) or 0/0)\n" );
		}

		// following may not support
		if ( fetestexcept ( FE_UPWARD ) )
		{
			fprintf( stderr, "[FE] round to +inf\n" );
		}
		if ( fetestexcept ( FE_DOWNWARD ) )
		{
			fprintf( stderr, "[FE] round to -inf\n" );
		}
		if ( fetestexcept ( FE_TOWARDZERO ) )
		{
			fprintf( stderr, "[FE] round to 0\n" );
		}
	}

	return fe_occur;
}

int main ( int argc, char **argv )
{
	setbuf( stdout, 0 );
	union 
	{
		double dval;
		unsigned long hex;
	} num1, num2, num3, num4;
	bool fe_occur;

	// inexact operation
	num1.dval = 0.5;
	num2.hex = num1.hex;
	num2.hex += 1;
	printf( "check %.15le + %.15le :\n", num1.dval, num2.dval );

	clear_fe ();
	num3.dval = num1.dval + num2.dval; 
	fe_occur = check_fe( true );
	num4.dval = num3.dval - num1.dval; 
	
	printf( "num1=%0#10lx %.15le\n", num1.hex, num1.dval );
	printf( "num2=%0#10lx %.15le\n", num2.hex, num2.dval );
	printf( "num3=%0#10lx %.15le\n", num3.hex, num3.dval );
	printf( "num4=%0#10lx %.15le\n", num4.hex, num4.dval );

	// division by zero
	num1.dval = 1;
	num2.dval = 0;
	printf( "\ncheck %.15le / %.15le :\n", num1.dval, num2.dval );

	clear_fe ();
	num3.dval = num1.dval / num2.dval; 
	fe_occur = check_fe( true );
	printf( "num1=%0#10lx %.15le\n", num1.hex, num1.dval );
	printf( "num2=%0#10lx %.15le\n", num2.hex, num2.dval );
	printf( "num3=%0#10lx %.15le\n", num3.hex, num3.dval );

	// round to DBL_MAX 
	num1.dval = DBL_MAX;
	printf( "\ncheck %.15le * 2 :\n", num1.dval );

	clear_fe ();
	num2.dval = num1.dval * 2;
	fe_occur = check_fe( true );
	printf( "num1=%0#10lx %.15le\n", num1.hex, num1.dval );
	printf( "num2=%0#10lx %.15le\n", num2.hex, num2.dval );

	num1.dval = 1e100;
	printf( "\ncheck exp(%.15le) :\n", num1.dval );

	clear_fe ();
	num2.dval = exp( num1.dval );
	fe_occur = check_fe( true );
	printf( "num1=%0#10lx %.15le\n", num1.hex, num1.dval );
	printf( "num2=%0#10lx %.15le\n", num2.hex, num2.dval );

	// round to DBL_MIM 
	num1.dval = DBL_MIN;
	printf( "\ncheck %.15le / 2 :\n", num1.dval );

	clear_fe ();
	num2.dval = num1.dval / 3;
	fe_occur = check_fe( true );
	printf( "num1=%0#10lx %.15le\n", num1.hex, num1.dval );
	printf( "num2=%0#10lx %.15le\n", num2.hex, num2.dval );


	// invalid operation
	printf( "\ncheck sqrt(-1) :\n" );

	clear_fe ();
	num1.dval = sqrt(-1);
	fe_occur = check_fe( true );
	printf( "num1=%0#10lx %.15le\n", num1.hex, num1.dval );

	printf( "\ncheck 0/0 :\n" );

	num1.dval = 0;
	num2.dval = 0;
	clear_fe ();
	num3.dval = num1.dval / num2.dval;
	fe_occur = check_fe( true );
	printf( "num1=%0#10lx %.15le\n", num1.hex, num1.dval );
	printf( "num2=%0#10lx %.15le\n", num2.hex, num2.dval );
	printf( "num3=%0#10lx %.15le\n", num3.hex, num3.dval );

	return EXIT_SUCCESS;
}

