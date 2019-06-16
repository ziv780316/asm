#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include "opts.h"


opt_t g_opts = {
	.utility = DUMP_ELF_INFO,

	.machine = MACHINE_X86_64,

	.elf_file = NULL,
	.section_name = NULL, 
	.search_pattern = NULL,

	.addr_name_map = NULL,
	.reg_name_map = NULL,

	.dump_string_width = 30,

	.exact_match = true,
	.debug = false
};

void show_help ()
{
	printf( "*------------------------------------*\n" 
		"*              ELF HACK              *\n"
		"*------------------------------------*\n" 
		"[Usage] hack ELF\n"
		"\n"
		"[Dump ELF Info]\n"
		"  elf_hack -e <elf_file>\n"
		"\n"
		"[Dump String]\n"
		"  elf_hack -e <elf_file> -u dump_string -s <.section> -w <width>\n"
		"\n"
		"[Dump Symbol]\n"
		"  elf_hack -e <elf_file> -u dump_symbol -p <pattern>\n"
		"\n"
		"[Dump Function]\n"
		"  elf_hack -e <elf_file> -u dump_function -f <function>\n"
		"\n"
		"[String Search]\n"
		"  elf_hack -e <elf_file> -u find_string -p <pattern>\n"
		"\n"
		"[Symbol Search]\n"
		"  elf_hack -e <elf_file> -u find_symbol -p <pattern>\n"
		"\n"
		"[Function Search]\n"
		"  elf_hack -e <elf_file> -u find_funtion -p <pattern>\n"
		"\n"
		"[Other]\n"
		"  -h  =>  show help\n"
		"  -d  =>  show debug information\n"
		"  -t  =>  partial match pattern\n"
		"  -b  =>  specify map table\n"
		"  -m  <machine> \n"
		"    + x86_32\n"
		"    + x86_64 (default)\n"
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
			{"partial_match", no_argument, 0, 't'},
			{"debug", no_argument, 0, 'd'},

			// setting options
			{"utility", required_argument, 0, 'u'},
			{"machine", required_argument, 0, 'm'},
			{"elf", required_argument, 0, 'e'},
			{"section", required_argument, 0, 's'},
			{"pattern", required_argument, 0, 'p'},
			{"width", required_argument, 0, 'w'},
			{"table", required_argument, 0, 'b'},

			{0, 0, 0, 0}
		};

		// getopt_long stores the option index here
		int option_index = 0;

		c = getopt_long( argc, argv, "htdu:m:e:s:p:w:b:", long_options, &option_index );

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

			case 't':
				g_opts.exact_match = false;
				break;

			case 'd':
				g_opts.debug = true;
				break;

			case 'u':
				if ( 0 == strcmp(optarg, "dump_string") )
				{
					g_opts.utility = DUMP_STRING;
				}
				else if ( 0 == strcmp(optarg, "dump_symbol") )
				{
					g_opts.utility = DUMP_SYMBOL;
				}
				else if ( 0 == strcmp(optarg, "dump_function") )
				{
					g_opts.utility = DUMP_FUNCTION;
				}
				else if ( 0 == strcmp(optarg, "find_string") )
				{
					g_opts.utility = FIND_STRING;
				}
				else if ( 0 == strcmp(optarg, "find_symbol") )
				{
					g_opts.utility = FIND_SYMBOL;
				}
				else if ( 0 == strcmp(optarg, "find_funtion") )
				{
					g_opts.utility = FIND_FUNCTION;
				}
				else
				{
					fprintf( stderr, "[Error] unknown utility '%s'\n", optarg );
					abort();
				}
				break;

			case 'e':
				g_opts.elf_file = optarg;
				break;

			case 's':
				g_opts.section_name = optarg;
				break;

			case 'p':
				g_opts.search_pattern = optarg;
				break;

			case 'w':
				g_opts.dump_string_width = atoi( optarg );
				break;

			case 'm':
				if ( 0 == strcmp(optarg, "x86_32") )
				{
					g_opts.utility = MACHINE_X86_32;
				}
				else if ( 0 == strcmp(optarg, "x86_64") )
				{
					g_opts.utility = MACHINE_X86_64;
				}
				else
				{
					fprintf( stderr, "[Error] unknown machine '%s'\n", optarg );
					abort();
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
	if ( NULL == g_opts.elf_file )
	{
		fprintf( stderr, "please input -e <ELF file>\n" );
		exit( EXIT_FAILURE );
	}
}

