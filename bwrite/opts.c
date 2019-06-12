#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include "opts.h"


opt_t g_opts = {
	.dest_file = NULL,
	.source = NULL,

	.input_type = SOURCE_NONE,

	.n_bytes = 0,
	.offset = -1,
	.is_lsb = true, 

	.debug = false
};

void show_help ()
{
	printf( "*------------------------------------*\n" 
		"*              Bin Write             *\n"
		"*------------------------------------*\n" 
		"[Usage] modify binary file\n"
		"\n"
		"[Write Signed Integer]\n"
		"  bwrite -e <exec_file> -o <0xoffset> -t int -n <n_bytes> -i <num>\n"
		"\n"
		"[Write Unsigned Signed Integer]\n"
		"  bwrite -e <exec_file> -o <0xoffset> -t unsigned_int -n <n_bytes> -i <num>\n"
		"\n"
		"[Write Float]\n"
		"  bwrite -e <exec_file> -o <0xoffset> -t float -n <n_bytes> -i <float>\n"
		"\n"
		"[Write String]\n"
		"  bwrite -e <exec_file> -o <0xoffset> -t string -i <string>\n"
		"\n"
		"[Write Hex]\n"
		"  bwrite -e <exec_file> -o <0xoffset> -t hex -i <hex>\n"
		"\n"
		"[Other]\n"
		"  -h  =>  show help\n"
		"  -d  =>  show debug information\n"
		"  -m  =>  wirte in MSB instead of LSB\n"
		);
}

void parse_cmd_options ( int argc, char **argv )
{
	char *endptr;
	int c;

	while ( true )
	{
		static struct option long_options[] =
		{
			// flag options
			{"help", no_argument, 0, 'h'},
			{"debug", no_argument, 0, 'd'},
			{"msb", no_argument, 0, 'm'},

			// setting options
			{"type", required_argument, 0, 't'},
			{"exec_file", required_argument, 0, 'e'},
			{"offset", required_argument, 0, 'o'},
			{"nbytes", required_argument, 0, 'n'},
			{"input_num", required_argument, 0, 'i'},

			{0, 0, 0, 0}
		};

		// getopt_long stores the option index here
		int option_index = 0;

		c = getopt_long( argc, argv, "hdmt:e:o:n:i:", long_options, &option_index );

		// detect the end of the options
		if ( -1 == c )
		{
			break;
		}

		switch ( c )
		{
			case 'h':
				show_help();
				exit( EXIT_SUCCESS );
				break;

			case 'd':
				g_opts.debug = true;
				break;

			case 'm':
				g_opts.is_lsb = false;
				break;


			case 't':
				if ( 0 == strcmp(optarg, "int") )
				{
					g_opts.input_type = SOURCE_INT;
				}
				else if ( 0 == strcmp(optarg, "unsigned_int") )
				{
					g_opts.input_type = SOURCE_UNSIGNED_INT;
				}
				else if ( 0 == strcmp(optarg, "float") )
				{
					g_opts.input_type = SOURCE_FLOAT;
				}
				else if ( 0 == strcmp(optarg, "string") )
				{
					g_opts.input_type = SOURCE_STRING;
				}
				else if ( 0 == strcmp(optarg, "hex") )
				{
					g_opts.input_type = SOURCE_HEX;
				}
				else
				{
					fprintf( stderr, "[Error] unknown input type '%s'\n", optarg );
					abort();
				}
				break;

			case 'e':
				g_opts.dest_file = optarg;
				break;

			case 'o':
				{
					char *endptr;
					g_opts.offset = strtol( optarg, &endptr, 16 );
					if ( (ERANGE == errno) || (EINVAL == errno) || (optarg == endptr) )
					{
						fprintf( stderr, "[Error] convert hex offset %s fail --> %s\n", optarg, strerror(errno) );
					}
				}
				break;

			case 'n':
				{
					char *endptr;
					g_opts.n_bytes = strtol( optarg, &endptr, 10 );
					if ( (ERANGE == errno) || (EINVAL == errno) || (optarg == endptr) )
					{
						fprintf( stderr, "[Error] convert n_bytes %s fail --> %s\n", optarg, strerror(errno) );
					}
				}

			case 'i':
				{
					g_opts.source = optarg;
				}
				break;

			case '?':
				/* getopt_long already printed an error message. */
				break;

			default:
				abort ();
				break;
		}
	}

	// print any remaining command line arguments (not options)
	if (optind < argc)
	{
		fprintf( stderr, "[Warning] non-option ARGV-elements: " );
		while ( optind < argc )
		{
			fprintf( stderr, "%s ", argv[optind++] );
		}
		fprintf( stderr, "\n" );
	}

	// check mode
	if ( NULL == g_opts.dest_file )
	{
		fprintf( stderr, "please input -e <exec_file>\n" );
		exit( EXIT_FAILURE );
	}
	if ( SOURCE_NONE == g_opts.input_type )
	{
		fprintf( stderr, "please input -t <type>\n" );
		exit( EXIT_FAILURE );
	}
	if ( -1 == g_opts.offset )
	{
		fprintf( stderr, "please input -o <0xoffset>\n" );
		exit( EXIT_FAILURE );
	}
	if ( NULL == g_opts.source )
	{
		fprintf( stderr, "please input -i <num|float|string|hex>\n" );
		exit( EXIT_FAILURE );
	}
}

