#ifndef MACRO_STRING_MAP_H
#define MACRO_STRING_MAP_H

#include <elf.h>

char *note_type_string_map ( Elf64_Word type );
char *cpu_type_string_map ( Elf64_Half type );
char *osabi_type_string_map ( unsigned char type );
char *rela_type_string_map ( Elf64_Half type );

#endif

