#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include "opts.h"


opt_t g_opts = {
	.utility = DUMP_ELF_INFO,
	.convert_type = CONVERT_NONE,

	.machine = MACHINE_X86_64,

	.elf_file = NULL,
	.section_name = NULL, 
	.search_pattern = NULL,

	.addr_name_map = NULL,
	.reg_name_map = NULL,

	.dump_string_width = 30,
	.rip = 0,

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
		"[Dump Relocation Info]\n"
		"  elf_hack -e <elf_file> -u dump_rela\n"
		"\n"
		"[Dump Core]\n"
		"  elf_hack -e <elf_file> -u dump_core\n"
		"\n"
		"[Hex Search]"
		"  elf_hack -e <elf_file> -u find_hex -p <pattern>\n"
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
		"[Evaluate Instruction]\n"
		"  elf_hack -e <elf_file> -u eval_call -r <rip> -p <function>\n"
		"  elf_hack -e <elf_file> -u eval_jmp  -r <rip> -j <jmp to vaddr>\n"
		"\n"
		"[Other]\n"
		"  -h  =>  show help\n"
		"  -d  =>  show debug information\n"
		"  -t  =>  partial match pattern\n"
		"  -b  =>  specify map table\n"
		"  -c  =>  specify hex convert type\n"
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
			{"rip", required_argument, 0, 'r'},
			{"jmp_to", required_argument, 0, 'j'},
			{"convert", required_argument, 0, 'l'},

			{0, 0, 0, 0}
		};

		// getopt_long stores the option index here
		int option_index = 0;

		c = getopt_long( argc, argv, "htdu:c:m:e:s:p:w:b:r:j:", long_options, &option_index );

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
				else if ( 0 == strcmp(optarg, "dump_rela") )
				{
					g_opts.utility = DUMP_RELA;
				}
				else if ( 0 == strcmp(optarg, "dump_core") )
				{
					g_opts.utility = DUMP_CORE;
				}
				else if ( 0 == strcmp(optarg, "find_hex") )
				{
					g_opts.utility = FIND_HEX;
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
				else if ( 0 == strcmp(optarg, "eval_call") )
				{
					g_opts.utility = EVAL_CALL;
				}
				else if ( 0 == strcmp(optarg, "eval_jmp") )
				{
					g_opts.utility = EVAL_JMP;
				}
				else
				{
					fprintf( stderr, "[Error] unknown utility '%s'\n", optarg );
					abort();
				}
				break;

			case 'c':
				if ( 0 == strcmp(optarg, "float") )
				{
					g_opts.convert_type = CONVERT_FLOAT;
				}
				else if ( 0 == strcmp(optarg, "double") )
				{
					g_opts.convert_type = CONVERT_DOUBLE;
				}
				else if ( 0 == strcmp(optarg, "int") )
				{
					g_opts.convert_type = CONVERT_INT;
				}
				else if ( 0 == strcmp(optarg, "unsigned") )
				{
					g_opts.convert_type = CONVERT_UNSIGNED;
				}
				else if ( 0 == strcmp(optarg, "long") )
				{
					g_opts.convert_type = CONVERT_LONG;
				}
				else if ( 0 == strcmp(optarg, "unsigned_long") )
				{
					g_opts.convert_type = CONVERT_UNSIGNED_LONG;
				}
				else
				{
					fprintf( stderr, "[Error] unknown convert_type '%s'\n", optarg );
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

			case 'r':
				g_opts.rip = strtoul( optarg, NULL, 16 );
				break;

			case 'j':
				g_opts.jmp_vaddr = strtoul( optarg, NULL, 16 );
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

	if ( (EVAL_CALL == g_opts.utility) && (0 == g_opts.rip) )
	{
		fprintf( stderr, "please input argument -i <rip>\n" );
		abort();
	}
}

