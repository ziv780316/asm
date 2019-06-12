#ifndef OPTS_H
#define OPTS_H

#include <stdbool.h>

typedef enum 
{
	SOURCE_NONE,
	SOURCE_HEX,
	SOURCE_UNSIGNED_INT,
	SOURCE_INT,
	SOURCE_FLOAT,
	SOURCE_STRING
} source_type;

typedef struct
{
	char *dest_file;
	char *source;

	source_type input_type;

	size_t n_bytes;
	long offset;
	bool is_lsb;

	// print debug information
	bool debug;

} opt_t;

extern opt_t g_opts;

extern void parse_cmd_options ( int argc, char **argv );

#endif
