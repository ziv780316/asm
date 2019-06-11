#ifndef OPTS_H
#define OPTS_H

#include <stdbool.h>

typedef enum 
{
	UTILITY_NONE,

	DUMP_STRING,
	DUMP_SYMBOL,
	DUMP_FUNCTION,

	FIND_STRING,
	FIND_SYMBOL,
	FIND_FUNCTION,

	MODIFY_CODE

} hack_utility;

typedef enum 
{
	NONE,

	MACHINE_X86_32,
	MACHINE_X86_64

} machine_type;

typedef struct
{
	hack_utility utility;

	machine_type machine;

	// use to search in ELF 
	char *elf_file;
	char *section_name;
	char *search_pattern;

	// use to dump .text to readible asm
	char *addr_name_map;
	char *reg_name_map;

	int dump_string_width;

	bool exact_match;
	bool debug;

	// use to modify code
	size_t n_hex;
	unsigned long offset;
	char *lsb_hex_bytes; 
} opt_t;

extern opt_t g_opts;

extern void parse_cmd_options ( int argc, char **argv );

#endif
