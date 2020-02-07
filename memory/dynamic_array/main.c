#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

void *dynamic_array_realloc( void *mem, size_t origin_n_memb, size_t new_n_memb, size_t memb_size );

int main ()
{
	void *heap_top_old;
	void *heap_top_new;
	int *mem = 0; 
	for ( int i = 0; i <= 1000000; ++i )
	{
		heap_top_old = sbrk( 0 );
		mem = (int *) dynamic_array_realloc( mem, i, i + 1, sizeof(int) );
		heap_top_new = sbrk( 0 );

		mem[i] = i;

		if ( (unsigned long)heap_top_old != (unsigned long)heap_top_new )
		{
			fprintf( stderr, "i=%d heap_top_old=%0#10lx heap_top_new=%0#10lx diff=%lu\n", i, (unsigned long) heap_top_old, (unsigned long) heap_top_new, (unsigned long) heap_top_new - (unsigned long) heap_top_old );
		}
	}

	long int sum = 0;
	for ( int i = 0; i <= 1000000; ++i )
	{
		sum += mem[i];
	}
	fprintf( stderr, "sum=%ld ans=%ld\n", sum, 1000000*(long)1000000/2 );

	return EXIT_SUCCESS;
}

