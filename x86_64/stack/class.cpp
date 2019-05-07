#include <stdio.h>
#include <stdlib.h>

class test
{
	public: 

		static int test_cnt;

		test ()
		{
			a = 1;
			b = 2;
			test_cnt += 1;
		}

		void show_member ( void )
		{
			printf( "a=%d b=%d\n", a, b );
		}

		void show_static ( void )
		{
			printf( "test_cnt=%d\n", test_cnt );
		}

		void set ( int x, int y )
		{
			a = x;	
			b = y;
		}

	private:

		int a;
		int b;
};

int test::test_cnt = 0;

int main ( int argc, char **argv )
{
	test t1;
	test t2;

	t1.set( 1, 2 );
	t1.show_member();
	t1.show_static();

	printf( "sizeof(test)=%lu\n", sizeof(test) );

	return EXIT_SUCCESS;
}

