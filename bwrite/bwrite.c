#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "opts.h"

typedef union
{
	char hexs[16];
	int64_t i64;
	uint64_t ui64;
	float f32;
	double f64;
	long double f80;
} num_buf;

void print_info ( char *fmt, ... );
void print_error ( char *fmt, ... );
bool is_str_character ( int ch );
void read_n_byte ( FILE *fin, size_t n, void *buf );
void write_n_byte ( FILE *fin, size_t n, void *buf );
void reverse_n_byte ( size_t n, void *buf );
void read_string ( FILE *fin, void *buf );
void seek_file ( FILE *fin, long offset );
void bwrire( FILE *fin, long offset, source_type type, size_t n_bytes, char *source, bool is_lsb );

void print_info ( char *fmt, ... )
{
	if ( g_opts.debug )
	{
		va_list args;
		va_start( args, fmt );
		vprintf( fmt, args );
		va_end( args );
	}
}

void print_error ( char *fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	vfprintf( stderr, fmt, args );
	va_end( args );
	abort();
}

bool is_str_character ( int ch )
{
	return isprint(ch) || ('\n' == ch) || ('\t' == ch);
}

void read_n_byte( FILE *fin, size_t n, void *buf )
{
	size_t n_read;
	size_t total_read = 0;
	size_t n_need = n;
	while ( n_need > 0 )
	{
		n_read = fread( buf + total_read, 1, n_need, fin );
		if ( 0 == n_read )
		{
			print_error( "[Error] fread fail (n=%lu total_read=%lu) --> %s\n", n, total_read, strerror(errno) );
		}

		total_read += n_read;
		n_need = n - n_read;
	}
}

// read null-terminated string
void read_string( FILE *fin, void *buf )
{
	size_t total_read = 0;
	size_t n_read;
	while ( true )
	{
		n_read = fread( buf + total_read, 1, 1, fin );
		if ( 0 == n_read )
		{
			print_error( "[Error] fread fail (n_read=%lu total_read=%lu) --> %s\n", n_read, total_read, strerror(errno) );
		}

		if ( '\0' == *((char *)buf + total_read) )
		{
			break;
		}

		total_read += n_read;
	}
}

void write_n_byte( FILE *fin, size_t n, void *buf )
{
	size_t n_write;
	size_t total_write = 0;
	size_t n_need = n;
	while ( n_need > 0 )
	{
		n_write = fwrite( buf + total_write, 1, n_need, fin );
		if ( 0 == n_write )
		{
			print_error( "[Error] fwrite fail (n=%lu total_write=%lu) --> %s\n", n, total_write, strerror(errno) );
		}

		total_write += n_write;
		n_need = n - n_write;
	}
}

void reverse_n_byte ( size_t n, void *buf )
{
	size_t n_stop = n / 2;
	char *ch_buf = (char *) buf;
	char tmp;
	for ( size_t i = 0; i < n_stop; ++i )
	{
		tmp = ch_buf[i];
		ch_buf[i] = ch_buf[n - i - 1];
		ch_buf[n - i - 1] = tmp;
	}
}

void seek_file ( FILE *fin, long offset )
{
	if ( -1 == fseek( fin, offset, SEEK_SET ) )
	{
		print_error( "[Error] fseek fail --> %s\n", strerror(errno) );
	}
}

void bwrire( FILE *fin, long offset, source_type type, size_t n_bytes, char *source, bool is_lsb )
{
	struct stat st;
	if ( -1 == fstat( fileno(fin), &st ) )
	{
		print_error( "[Error] fstat fail --> %s\n", strerror(errno) );
	}

	// count string or hex bytes
	char n_char;
	if ( SOURCE_STRING == type )
	{
		n_char = strlen( source );
		n_bytes = n_char;
	}
	else if ( SOURCE_HEX == type )
	{
		n_char = strlen( source );
		if ( 0 != (n_char % 2) )
		{
			print_error( "[Error] hex num need 2 multiplier (2 hex make 1 byte)\n" );
		}
		n_bytes = n_char / 2;
	}

	// check write out-of-bound or not
	if ( offset + n_bytes > st.st_size )
	{
		print_error( "[Error] final write offset=%ld beyond file size=%lu\n", offset + n_bytes, st.st_size );
	}

	// move to position, ready to write 
	seek_file( fin, offset );

	if ( (SOURCE_INT == type) || (SOURCE_UNSIGNED_INT == type) || (SOURCE_FLOAT == type) )
	{
		// write num
		if ( (1 != n_bytes) && (2 != n_bytes) && (4 != n_bytes) && (8 != n_bytes) && (10 != n_bytes) )
		{
			print_error( "[Error] cannot write %d bytes num\n", n_bytes );
		}

		char *endptr;
		num_buf buf;
		if ( SOURCE_INT == type )
		{
			buf.i64 = strtoll( source, &endptr, 10 );
			if ( (ERANGE == errno) || (EINVAL == errno) || (source == endptr) )
			{
				fprintf( stderr, "[Error] convert int %s fail --> %s\n", source, strerror(errno) );
			}

		}
		else if ( SOURCE_UNSIGNED_INT == type )
		{
			buf.ui64 = strtoull( source, &endptr, 10 );
			if ( (ERANGE == errno) || (EINVAL == errno) || (source == endptr) )
			{
				fprintf( stderr, "[Error] convert unsigned_int %s fail --> %s\n", source, strerror(errno) );
			}

		}
		else if ( SOURCE_FLOAT == type )
		{
			if ( 4 == n_bytes )
			{
				buf.f32 = strtof( source, &endptr );
				if ( (ERANGE == errno) || (EINVAL == errno) || (source == endptr) )
				{
					fprintf( stderr, "[Error] convert float %s fail --> %s\n", source, strerror(errno) );
				}
			}
			else if ( 8 == n_bytes )
			{
				buf.f64 = strtod( source, &endptr );
				if ( (ERANGE == errno) || (EINVAL == errno) || (source == endptr) )
				{
					fprintf( stderr, "[Error] convert double %s fail --> %s\n", source, strerror(errno) );
				}
			}
			else if ( 10 == n_bytes )
			{
				buf.f80 = strtold( source, &endptr );
				if ( (ERANGE == errno) || (EINVAL == errno) || (source == endptr) )
				{
					fprintf( stderr, "[Error] convert long double %s fail --> %s\n", source, strerror(errno) );
				}
			}

		}

		if ( !is_lsb )
		{
			// reverse bytes order to MSB
			reverse_n_byte( n_bytes, &buf );
		}

		write_n_byte( fin, n_bytes, &buf );
	}
	else
	{
		// write bytes data
		if ( SOURCE_STRING == type )
		{
			if ( is_lsb )
			{
				for ( size_t i = 0; i < n_bytes; ++i )		
				{
					write_n_byte( fin, 1, source + i );
				}
			}
			else
			{
				for ( size_t i = 0; i < n_bytes; ++i )		
				{
					write_n_byte( fin, 1, source + n_bytes - i - 1 );
				}
			}
		}
		else if ( SOURCE_HEX == type )
		{
			char *startptr;
			char *endptr;
			char *hex_arr = (char *) malloc ( n_bytes );
			unsigned char hex;
			unsigned char tmp_hex;
			for ( size_t i = 0; i < n_bytes; ++i )		
			{
				startptr = source + (i*2);
				endptr = source + (i*2) + 2;
				tmp_hex = *endptr;
				*endptr = '\0';
				hex = (unsigned char) strtoul( startptr, &endptr, 16 );
				if ( (ERANGE == errno) || (EINVAL == errno) || (source == endptr) )
				{
					fprintf( stderr, "[Error] convert hex %s fail --> %s\n", source, strerror(errno) );
				}
				*endptr = tmp_hex;
				hex_arr[i] = hex;
			}

			if ( is_lsb )
			{
				for ( size_t i = 0; i < n_bytes; ++i )		
				{
					write_n_byte( fin, 1, hex_arr + i );
				}
			}
			else
			{
				for ( size_t i = 0; i < n_bytes; ++i )		
				{
					write_n_byte( fin, 1, hex_arr + n_bytes - i - 1 );
				}
			}

			free( hex_arr );
		}
	}

	// sync to disk
	fsync( fileno(fin) );
}

int main ( int argc, char **argv )
{
	// parse command line arguments
	parse_cmd_options( argc, argv );

	// read ELF header
	FILE *fin = fopen( g_opts.dest_file, "r+" );
	if ( !fin )
	{
		print_error( "[Error] open file '%s' fail --> %s\n", g_opts.dest_file, strerror(errno) );
	}

	bwrire( fin, g_opts.offset, g_opts.input_type, g_opts.n_bytes, g_opts.source, g_opts.is_lsb );

	return EXIT_SUCCESS;
}

