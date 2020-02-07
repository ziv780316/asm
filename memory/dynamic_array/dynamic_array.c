#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void *dynamic_array_realloc( void *mem, size_t origin_n_memb, size_t new_n_memb, size_t memb_size )
{
	const bool debug = true;

	const size_t growth_rate = 2;

	// find power of growth_rate
	size_t val;
	size_t origin_bound = 1;
	val = origin_n_memb;
	do
	{
		val /= growth_rate;
		origin_bound *= growth_rate;
	} while ( val > 0 );

	size_t new_bound = 1;
	val = new_n_memb;
	do
	{
		val /= growth_rate;
		new_bound *= growth_rate;
	} while ( val > 0 );

	// allocate
	// amortize algorithm, each call of dynamic_array_realloc is comsume O(1)
	// pf:
	// N dynamic_array_realloc call and new_n_memb from 1 ~ n
	// 1 + 2 + 4 + 8 + ... + 2^(log2(new_n_memb) + 1) < 2 * new_n_memb
	// amortized each call consume only (2 * new_n_memb)/N is O(1)
	if ( 0 == origin_n_memb )
	{
		mem = (void *) malloc( new_bound * sizeof(memb_size) );
		if ( debug )
		{
			fprintf( stderr, "[Dynamic Array] growth space 0 -> %lu\n", new_bound );
		}
	}
	else if ( origin_bound < new_bound )
	{
		if ( debug )
		{
			fprintf( stderr, "[Dynamic Array] growth space %lu -> %lu\n", origin_bound, new_bound );
		}
		mem = (void *) realloc( mem, (new_bound * sizeof(memb_size)) );
	}
	else
	{
		// there are remain spice, nothing to do
	}

	return mem;
}
