#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <elf.h>
#include <sys/procfs.h>
#include <sys/user.h>

#include "opts.h"
#include "macro_string_map.h"

#define LINE_WIDTH 30

typedef union {
	unsigned char hex[8];
	char ch;
	short int h_inum; // half-word num
	short unsigned int h_unum; // half-word num
	int w_inum; // word num
	unsigned int w_unum; 
	long int g_inum; // giant-word num
	long unsigned int g_unum; 
	float w_float;
	double g_float;
} data_buf;

void print_info ( char *fmt, ... );
void print_warning ( char *fmt, ... );
void print_error ( char *fmt, ... );
bool is_str_character ( int ch );
void read_n_byte ( FILE *fin, size_t n, void *buf );
void write_n_byte ( FILE *fin, size_t n, void *buf );
void read_string ( FILE *fin, void *buf );
void seek_file ( FILE *fin, Elf64_Off offset );
void read_section_header ( FILE *fin, Elf64_Ehdr *elf_header, Elf64_Half idx, void *buf );
void read_section_data ( FILE *fin, Elf64_Shdr *sh_header, void *buf );
void read_elf_header ( FILE *fin, Elf64_Ehdr *elf_header );
Elf64_Half search_section_idx ( FILE *fin, Elf64_Ehdr *elf_header, char *name );
void dump_elf_string ( FILE *fin, Elf64_Ehdr *elf_header, char *section_name );
void find_elf_string ( FILE *fin, Elf64_Ehdr *elf_header, char *section_name, char *search_pattern, bool exact_match );
void dump_elf_symbol ( FILE *fin, Elf64_Ehdr *elf_header, char *section_name, char *search_pattern, bool exact_match );
void dump_elf_rela ( FILE *fin, Elf64_Ehdr *elf_header, char *section_name );
void dump_core_file ( FILE *fin, Elf64_Ehdr *elf_header );
void search_hex_in_core ( FILE *fin, Elf64_Ehdr *elf_header, char *pattern );
void read_symbol_data ( FILE *fin, Elf64_Shdr *sh_header, Elf64_Sym *sym, data_buf *buf );
void modify_elf_code( FILE *fin, Elf64_Off offset, size_t n_hex, char *lsb_hex_bytes );
char *get_section_name ( Elf64_Ehdr *elf_header, Elf64_Half st_shndx );
#define IDX_NOT_FOUND 0

char **hash_section_idx_to_name = NULL;
Elf64_Shdr **hash_section_idx_to_sh_header = NULL;

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

void print_warning ( char *fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	vfprintf( stderr, fmt, args );
	va_end( args );
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

void seek_file ( FILE *fin, Elf64_Off offset )
{
	if ( -1 == fseek( fin, offset, SEEK_SET ) )
	{
		print_error( "[Error] fseek fail --> %s\n", strerror(errno) );
	}
}

void read_section_header ( FILE *fin, Elf64_Ehdr *elf_header, Elf64_Half idx, void *buf )
{
	seek_file( fin, elf_header->e_shoff + (idx * elf_header->e_shentsize) );
	read_n_byte( fin, elf_header->e_shentsize, buf );
}

void read_section_data ( FILE *fin, Elf64_Shdr *sh_header, void *buf )
{
	seek_file( fin, sh_header->sh_offset );
	read_n_byte( fin, sh_header->sh_size, buf );
}

char *get_section_name ( Elf64_Ehdr *elf_header, Elf64_Half st_shndx )
{
	char *belong_section_name;
	if ( st_shndx >= elf_header->e_shnum )
	{
		switch( st_shndx )
		{
			case SHN_ABS:
				belong_section_name = "ABS";
				break;

			case SHN_COMMON:
				belong_section_name = "COMMON";
				break;

			default:
				belong_section_name = "?";
				break;
		}
	}
	else if ( 0 == st_shndx )
	{
		belong_section_name = "UND";
	}
	else
	{
		belong_section_name = hash_section_idx_to_name[st_shndx];
	}
}

//typedef struct
//{
//  unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
//  Elf64_Half	e_type;			/* Object file type */
//  Elf64_Half	e_machine;		/* Architecture */
//  Elf64_Word	e_version;		/* Object file version */
//  Elf64_Addr	e_entry;		/* Entry point virtual address */
//  Elf64_Off	e_phoff;		/* Program header table file offset */
//  Elf64_Off	e_shoff;		/* Section header table file offset */
//  Elf64_Word	e_flags;		/* Processor-specific flags */
//  Elf64_Half	e_ehsize;		/* ELF header size in bytes */
//  Elf64_Half	e_phentsize;		/* Program header table entry size */
//  Elf64_Half	e_phnum;		/* Program header table entry count */
//  Elf64_Half	e_shentsize;		/* Section header table entry size */
//  Elf64_Half	e_shnum;		/* Section header table entry count */
//  Elf64_Half	e_shstrndx;		/* Section header string table index */
//} Elf64_Ehdr;
void read_elf_header ( FILE *fin, Elf64_Ehdr *elf_header )
{
	read_n_byte ( fin, sizeof(Elf64_Ehdr), (void *) elf_header );

	if ( (0x7f != elf_header->e_ident[EI_MAG0]) ||
	     ('E'  != elf_header->e_ident[EI_MAG1]) ||
	     ('L'  != elf_header->e_ident[EI_MAG2]) ||
	     ('F'  != elf_header->e_ident[EI_MAG3]) )
	{
		print_error( "[Error] file '%s' is not ELF\n", g_opts.elf_file );
	}

	
	print_info( "cpu = %s\n", cpu_type_string_map( elf_header->e_machine ) );

	if ( ELFCLASS64 == elf_header->e_ident[EI_CLASS] )
	{
		print_info( "machine = 64-bit\n" );
		if ( MACHINE_X86_64 != g_opts.machine  )
		{
			print_error( "[Error] target hack ELF not 64-bit\n" );
		}
	}
	else
	{
		print_info( "machine = 32-bit\n" );
		if ( MACHINE_X86_32 != g_opts.machine  )
		{
			print_error( "[Error] target hack ELF not 32-bit\n" );
		}
	}

	if ( ELFDATA2LSB == elf_header->e_ident[EI_DATA] )
	{
		print_info( "endian = 2's complement LSB\n" );
	}
	else
	{
		print_error( "[Error] ELF file '%s' is not LSB\n", g_opts.elf_file );
	}

	print_info( "OS/ABI = %s\n", osabi_type_string_map( elf_header->e_ident[EI_OSABI] ) );

	if ( ET_EXEC == elf_header->e_type )
	{
		print_info( "type = executable\n" );
	}
	else if ( ET_REL == elf_header->e_type )
	{
		print_info( "type = .obj\n" );
	}
	else if ( ET_DYN == elf_header->e_type )
	{
		print_info( "type = .so\n" );
	}
	else if ( ET_CORE == elf_header->e_type )
	{
		print_info( "type = core file\n" );
	}

	print_info( "section header entry size = %hu\n", elf_header->e_shentsize );
	print_info( "program header entry size = %hu\n", elf_header->e_phentsize );
	print_info( "section header table entry num = %hu\n", elf_header->e_shnum );
	print_info( "program header table entry num = %hu\n", elf_header->e_phnum );
	print_info( "section header table file offset = %0#10lx\n", elf_header->e_shoff );
	print_info( "program header table file offset = %0#10lx\n", elf_header->e_phoff );

	// cache all section
	hash_section_idx_to_sh_header = (Elf64_Shdr **) malloc ( sizeof(Elf64_Shdr *) * elf_header->e_shnum );
	Elf64_Shdr *sh_header;
	for ( int i = 0; i < elf_header->e_shnum; ++i )
	{
		// read section and keep in hash table 
		sh_header = (Elf64_Shdr *) malloc ( sizeof(Elf64_Shdr) );
		read_section_header( fin, elf_header, i, sh_header );
		hash_section_idx_to_sh_header[i] = sh_header;
	}

	// read dynamic linker path
	if ( ET_EXEC == elf_header->e_type )
	{
		Elf64_Half interp_section_idx = search_section_idx ( fin, elf_header, ".interp" );
		Elf64_Shdr *interp_header;
		char dynamic_linker_path[BUFSIZ];
		interp_header = hash_section_idx_to_sh_header[interp_section_idx];
		seek_file( fin, interp_header->sh_offset );
		read_string( fin, dynamic_linker_path );
		print_info( "dynamic linker = %s\n", dynamic_linker_path );
	}

	// read necessarily dynamic library
	if ( (ET_EXEC == elf_header->e_type) || (ET_DYN == elf_header->e_type) )
	{
		Elf64_Half dynamic_section_idx = search_section_idx ( fin, elf_header, ".dynamic" );
		Elf64_Half dynstr_section_idx = search_section_idx ( fin, elf_header, ".dynstr" );
		Elf64_Shdr *dynamic_header;
		Elf64_Shdr *dynstr_header;
		dynamic_header = hash_section_idx_to_sh_header[dynamic_section_idx];
		dynstr_header = hash_section_idx_to_sh_header[dynstr_section_idx];
		if ( dynamic_header->sh_entsize != sizeof( Elf64_Dyn ) )
		{
			print_error( "[Error] %s entsize=%lu != sizeof(Elf64_Dyn)=%lu\n", ".dynamic", dynamic_header->sh_entsize, sizeof(Elf64_Dyn) );
		}
		if ( 0 != (dynamic_header->sh_size % sizeof( Elf64_Dyn )) )
		{
			print_error( "[Error] %s total_size=%lu %% sizeof(Elf64_Dyn)=%lu != 0\n", "dynamic", dynamic_header->sh_entsize, sizeof(Elf64_Dyn) );
		}

		size_t n_dyn_ent = dynamic_header->sh_size / dynamic_header->sh_entsize;
		Elf64_Off origin_file_pos;
		Elf64_Off dyn_str_offset;
		Elf64_Off plt_got_addr = 0;
		Elf64_Dyn dynamic_ent;
		char necessarily_so[BUFSIZ];
		seek_file( fin, dynamic_header->sh_offset );
		print_info( "necessarily .so =\n" );
		for ( size_t i = 0; i < n_dyn_ent; ++i )
		{
			read_n_byte( fin, dynamic_header->sh_entsize, &dynamic_ent );
			origin_file_pos = ftell( fin );
			switch ( dynamic_ent.d_tag )
			{
				case DT_NEEDED: 
					dyn_str_offset = dynamic_ent.d_un.d_val;
					seek_file( fin, dynstr_header->sh_offset + dyn_str_offset );
					read_string( fin, necessarily_so );
					seek_file( fin, origin_file_pos );
					print_info( " %s\n", necessarily_so );
					break;

				case DT_PLTGOT: 
					plt_got_addr = dynamic_ent.d_un.d_val;
					break;

				case DT_SONAME: 
					// nothing now ...
					break;

				default:
					break;
			}
		}

		// print GOT (global offset table) information of PTL functions (lazy binding)
		print_info( "PLT GOT virtual addr = %0#10lx\n", plt_got_addr );
	}

	// print all section name, check .shstrtab exist
	if ( SHN_UNDEF == elf_header->e_shstrndx )
	{
		print_error( "[Error] cannot find section .shstrtab\n" );
	}
	Elf64_Half shstrtab_idx = search_section_idx ( fin, elf_header, ".shstrtab" );
	Elf64_Shdr *shstrtab = hash_section_idx_to_sh_header[shstrtab_idx];
	print_info( "section header string table index = %hu\n", elf_header->e_shstrndx );

	// read all section name
	char *name_buf = malloc ( shstrtab->sh_size );
	hash_section_idx_to_name = (char **) malloc ( sizeof(char *) * elf_header->e_shnum );
	read_section_data( fin, shstrtab, (void *)name_buf );
	print_info( "sections (name size virtual_addr):\n" );
	for ( int i = 0; i < elf_header->e_shnum; ++i )
	{
		// read section and keep in hash table 
		sh_header = hash_section_idx_to_sh_header[i];
		print_info( " %-30s %-10lu %0#10lx\n", name_buf + sh_header->sh_name, sh_header->sh_size, sh_header->sh_addr );

		// create map section index to section name 
		hash_section_idx_to_name[i] = (char *) malloc ( strlen(name_buf + sh_header->sh_name) + 1 );
		strcpy( hash_section_idx_to_name[i], name_buf + sh_header->sh_name );
	}

	free( name_buf );
}

Elf64_Half search_section_idx ( FILE *fin, Elf64_Ehdr *elf_header, char *name )
{
	Elf64_Shdr shstrtab;
	read_section_header( fin, elf_header, elf_header->e_shstrndx, &shstrtab );

	char *name_buf = malloc ( shstrtab.sh_size );
	read_section_data( fin, &shstrtab, (void *)name_buf );

	bool is_find = false;
	Elf64_Half idx;
	Elf64_Shdr sh_header;
	for ( int i = 0; i < elf_header->e_shnum; ++i )
	{
		read_section_header( fin, elf_header, i, &sh_header );
		if ( 0 == strcmp( name, name_buf + sh_header.sh_name) )
		{
			is_find = true;
			idx = i;
			break;
		}
	}

	if ( !is_find )
	{
		print_warning( "[Warning] cannot find section '%s'\n", name );
		idx = 0;
	}

	free( name_buf );
	return idx;
}

//typedef struct
//{
//  Elf64_Word	sh_name;		/* Section name (string tbl index) */
//  Elf64_Word	sh_type;		/* Section type */
//  Elf64_Xword	sh_flags;		/* Section flags */
//  Elf64_Addr	sh_addr;		/* Section virtual addr at execution */
//  Elf64_Off	sh_offset;		/* Section file offset */
//  Elf64_Xword	sh_size;		/* Section size in bytes */
//  Elf64_Word	sh_link;		/* Link to another section */
//  Elf64_Word	sh_info;		/* Additional section information */
//  Elf64_Xword	sh_addralign;		/* Section alignment */
//  Elf64_Xword	sh_entsize;		/* Entry size if section holds table */
//} Elf64_Shdr;
//Elf64_Shdr
void dump_elf_string ( FILE *fin, Elf64_Ehdr *elf_header, char *section_name )
{
	Elf64_Half idx = search_section_idx( fin, elf_header, section_name );
	if ( IDX_NOT_FOUND == idx )
	{
		return;
	}
	print_info( "dump section %s idx=%d ...\n", section_name, idx );

	Elf64_Shdr sh_header;
	read_section_header( fin, elf_header, idx, &sh_header );

	bool start_str = false;
	bool start_nonstr = false;
	int ch;
	seek_file( fin, sh_header.sh_offset );
	for ( Elf64_Xword i = 0; i < sh_header.sh_size; ++i )
	{
		ch = fgetc( fin );
		if ( EOF == ch )
		{
			print_error( "[Error] fgetc fail --> %s\n", strerror(errno) );
		}

		// unprintable character show '*'
		if ( isprint( ch ) )
		{
			if ( start_nonstr )
			{
				start_nonstr = false;
				print_info( "\n" );
			}
			if ( !start_str )
			{
				start_str = true;
				// print virtual addr and file read offset
				printf( "%0#10lx %0#10lx ", sh_header.sh_addr + i, sh_header.sh_offset + i );
			}
			printf( "%c", ch );
		}
		else
		{
			if ( start_str )
			{
				if ( '\t' == ch )
				{
					printf( "\\t" );
				}
				else if ( '\n' == ch )
				{
					printf( "\\n" );
				}
				else if ( '\0' == ch )
				{
					// null-terminated string
					if ( start_str )
					{
						start_str = false;
						printf ( "\n" );
					}
				}
				else
				{
					// non-null-terminated string
					start_str = false;
					printf ( "\n" );
				}
			}
			else
			{
				// ignore other character
				if ( !start_nonstr )
				{
					start_nonstr = true;
					print_info( "(non-string) %0#10lx %0#10lx ", sh_header.sh_addr + i, sh_header.sh_offset + i );
				}
				print_info( " %#2hhx", ch );
			}
		}
	}

	if ( start_nonstr )
	{
		print_info( "\n" );
	}
}

//typedef struct
//{
//  Elf64_Word	st_name;		/* Symbol name (string tbl index) */
//  unsigned char	st_info;		/* Symbol type and binding */
//  unsigned char st_other;		/* Symbol visibility */
//  Elf64_Section	st_shndx;		/* Section index */
//  Elf64_Addr	st_value;		/* Symbol value */
//  Elf64_Xword	st_size;		/* Symbol size */
//} Elf64_Sym;
void dump_elf_symbol ( FILE *fin, Elf64_Ehdr *elf_header, char *section_name, char *search_pattern, bool exact_match )
{
	// read symtab section
	Elf64_Half idx = search_section_idx( fin, elf_header, section_name );
	if ( IDX_NOT_FOUND == idx )
	{
		return;
	}
	print_info( "dump section %s idx=%d ...\n", section_name, idx );

	Elf64_Shdr *symtab_header;
	symtab_header = hash_section_idx_to_sh_header[idx];

	if ( (SHT_SYMTAB != symtab_header->sh_type) && (SHT_DYNSYM != symtab_header->sh_type) )
	{
		print_error( "[Error] '%s' is not symtab section (%d)\n", section_name, idx );
	}

	// read strtab section (store symbol name)
	Elf64_Half strtab_idx; 
	char *symtab_loc;
	if ( SHT_SYMTAB == symtab_header->sh_type )
	{
		strtab_idx = search_section_idx( fin, elf_header, ".strtab" );
	}
	else if ( SHT_DYNSYM == symtab_header->sh_type )
	{
		strtab_idx = search_section_idx( fin, elf_header, ".dynstr" );
	}
	Elf64_Shdr *strtab_header;
	strtab_header = hash_section_idx_to_sh_header[strtab_idx];

	if ( symtab_header->sh_entsize != sizeof( Elf64_Sym ) )
	{
		print_error( "[Error] %s entsize=%lu != sizeof(Elf64_Sym)=%lu\n", section_name, symtab_header->sh_entsize, sizeof(Elf64_Sym) );
	}
	if ( 0 != (symtab_header->sh_size % sizeof( Elf64_Sym )) )
	{
		print_error( "[Error] %s total_size=%lu %% sizeof(Elf64_Sym)=%lu != 0\n", section_name, symtab_header->sh_entsize, sizeof(Elf64_Sym) );
	}

	Elf64_Off origin_file_pos;
	Elf64_Off sym_data_pos;
	size_t n_symbol = symtab_header->sh_size / symtab_header->sh_entsize;
	Elf64_Sym symbol;
	Elf64_Shdr *belong_sh_header;
	Elf64_Half bss_section_idx = search_section_idx ( fin, elf_header, ".bss" );
	char symbol_name[BUFSIZ];
	char *bind_type;
	char *data_type;
	char *belong_section_name;
	bool is_match_pattern;
	int ch;
	data_buf num_buf;
	seek_file( fin, symtab_header->sh_offset );
	for ( size_t i = 0; i < n_symbol; ++i )
	{
		// read symbol entry from symtab
		read_n_byte( fin, symtab_header->sh_entsize, &symbol );
		origin_file_pos = ftell( fin );

		// read symbol name from strtab
		seek_file( fin, strtab_header->sh_offset + symbol.st_name );
		read_string( fin, symbol_name );

		// back to symtab file offset
		seek_file( fin, origin_file_pos );

		// check name match search pattern if necessary
		if ( NULL != search_pattern )
		{
			is_match_pattern = false;
			if ( exact_match )
			{
				if ( 0 == strcmp( symbol_name, search_pattern ) )
				{
					is_match_pattern = true;
				}
			}
			else
			{
				if ( NULL != strstr( symbol_name, search_pattern ) )
				{
					is_match_pattern = true;
				}
			}

			if ( !is_match_pattern )
			{
				continue;
			}
		}
		
		switch( ELF64_ST_TYPE( symbol.st_info ) )
		{
			case STT_NOTYPE:
				data_type = "none";
				break;
				
			case STT_FUNC:
				if ( strstr(symbol_name, ".part.") )
				{
					data_type = "partial_inline_function";
				}
				else
				{
					data_type = "function";
				}
				break;

			case STT_OBJECT:
				data_type = "data";
				break;

			case STT_FILE:
				data_type = "obj_name";
				break;

			case STT_SECTION:
				data_type = "section";
				break;

			default:
				data_type = "?";
				break;

		}

		switch( ELF64_ST_BIND( symbol.st_info ) )
		{
			case STB_LOCAL:
				if ( (STT_OBJECT == ELF64_ST_TYPE(symbol.st_info)) && strstr(symbol_name, ".") )
				{
					bind_type = "static_var_in_function";
				}
				else
				{
					bind_type = "local";
				}
				break;

			case STB_GLOBAL:
				bind_type = "global";
				break;

			case STB_WEAK:
				bind_type = "weak";
				break;

			default:
				bind_type = "?";
				break;

		}

		belong_section_name = get_section_name( elf_header, symbol.st_shndx );

		if ( (STT_OBJECT == ELF64_ST_TYPE( symbol.st_info )) && (symbol.st_shndx != bss_section_idx) && (symbol.st_size > 0) && !(symbol.st_shndx & 0xf000) )
		{
			// hash section
			belong_sh_header = hash_section_idx_to_sh_header[symbol.st_shndx];
			sym_data_pos = belong_sh_header->sh_offset + (symbol.st_value - belong_sh_header->sh_addr);

			printf( "%0#10lx %0#10lx size=%-6lu section=%-10s type=%-10s bind=%-10s %-30s", symbol.st_value, sym_data_pos, symbol.st_size, belong_section_name, data_type, bind_type, symbol_name );

			// dereference data
			printf( "\n%-10s", "+ hex=" );
			seek_file( fin, sym_data_pos );
			for ( int j = 1; j <= symbol.st_size; ++j )
			{
				ch = fgetc( fin );
				if ( EOF == ch )
				{
					print_error( "[Error] fgetc fail --> %s\n", strerror(errno) );
				}
				printf( " %#2hhx", ch );
				if ( 0 == (j % LINE_WIDTH) )
				{
					printf( "\n%-10s", "" );
				}
			}

			printf( "\n%-10s", "+ string=" );
			seek_file( fin, sym_data_pos );
			for ( int j = 1; j <= symbol.st_size; ++j )
			{
				ch = fgetc( fin );
				if ( EOF == ch )
				{
					print_error( "[Error] fgetc fail --> %s\n", strerror(errno) );
				}
				if ( isprint( ch ) )
				{
					printf( "%c", ch );
				}
				else if ( '\t' == ch )
				{
					printf( "\\t" );
				}
				else if ( '\n' == ch )
				{
					printf( "\\n" );
				}
				else
				{
					printf( "*" );
				}
				if ( 0 == (j % LINE_WIDTH) )
				{
					printf( "\n%-10s", "" );
				}
			}

			if ( (0 == symbol.st_size % 8) && (symbol.st_size > 8) )
			{
				printf( "\n%-10s", "+ doubles=" );
				seek_file( fin, sym_data_pos );
				size_t n_double = (symbol.st_size / 8);
				for ( int j = 1; j <= n_double; ++j )
				{
					read_n_byte ( fin, 8, &num_buf );
					if ( EOF == ch )
					{
						print_error( "[Error] fgetc fail --> %s\n", strerror(errno) );
					}
					printf( " %.15le", num_buf.g_float );
					if ( 0 == (j % 4) )
					{
						printf( "\n%-10s", "" );
					}
				}
			}

			// read symbol from belong section
			if ( symbol.st_size <= 8 )
			{
				read_symbol_data ( fin, belong_sh_header, &symbol, &num_buf );
				if ( 1 == symbol.st_size )
				{
					printf( "\n%-10s", "+ char hex= " );
					printf( "%c %hhx", num_buf.ch, num_buf.hex[0] );
				}
				else if ( 2 == symbol.st_size )
				{
					printf( "\n%-10s", "+ short int= " );
					printf( "%hd %hu", num_buf.h_inum, num_buf.h_unum );
				}
				else if ( 4 == symbol.st_size )
				{

					printf( "\n%-10s", "+ int= " );
					printf( "%d %u", num_buf.w_inum, num_buf.w_unum );
					printf( "\n%-10s", "+ float= " );
					printf( "%.7e", num_buf.w_float );
				}
				else if ( 8 == symbol.st_size )
				{
					printf( "\n%-10s", "+ long int= " );
					printf( "%ld %lu", num_buf.g_inum, num_buf.g_unum );
					printf( "\n%-10s", "+ double= " );
					printf( "%.15le", num_buf.g_float );
				}
			}

			printf( "\n" );
		}
		else if ( STT_FUNC == ELF64_ST_TYPE( symbol.st_info ) )
		{
			// hash section
			belong_sh_header = hash_section_idx_to_sh_header[symbol.st_shndx];
			sym_data_pos = belong_sh_header->sh_offset + (symbol.st_value - belong_sh_header->sh_addr);
			printf( "%0#10lx %0#10lx size=%-6lu section=%-10s type=%-10s bind=%-10s %-30s", symbol.st_value, sym_data_pos, symbol.st_size, belong_section_name, data_type, bind_type, symbol_name );
			printf( "\n" );
		}
		else
		{
			printf( "%0#10lx size=%-6lu section=%-10s type=%-10s bind=%-10s %-30s", symbol.st_value, symbol.st_size, belong_section_name, data_type, bind_type, symbol_name );
			printf( "\n" );
		}

		seek_file( fin, origin_file_pos );
	}
}

void read_symbol_data ( FILE *fin, Elf64_Shdr *sh_header, Elf64_Sym *symbol, data_buf *buf )
{
	seek_file( fin, sh_header->sh_offset + (symbol->st_value - sh_header->sh_addr) );
	read_n_byte( fin, symbol->st_size, (void *) buf );
}

void find_elf_string ( FILE *fin, Elf64_Ehdr *elf_header, char *section_name, char *search_pattern, bool exact_match )
{
	Elf64_Half idx = search_section_idx( fin, elf_header, section_name );
	if ( IDX_NOT_FOUND == idx )
	{
		return;
	}
	print_info( "dump section %s idx=%d ...\n", section_name, idx );

	Elf64_Shdr sh_header;
	read_section_header( fin, elf_header, idx, &sh_header );

	bool start_str = false;
	bool start_nonstr = false;
	bool is_find_exact = false;
	int ch;
	Elf64_Off match_pos = 0;
	Elf64_Off str_pos = 0;
	Elf64_Off str_file_pos = 0;
	Elf64_Off origin_file_pos;
	size_t match_cnt = 0;
	size_t pattern_len = strlen( search_pattern );
	char full_str[BUFSIZ];
	seek_file( fin, sh_header.sh_offset );
	for ( Elf64_Xword i = 0; i < sh_header.sh_size; ++i )
	{
		ch = fgetc( fin );
		if ( EOF == ch )
		{
			print_error( "[Error] fgetc fail --> %s\n", strerror(errno) );
		}

		if ( 0 == match_cnt )
		{
			if ( is_str_character( ch ) )
			{
				if ( !start_str )
				{
					start_str = true;
					str_pos = sh_header.sh_addr + i;
					str_file_pos = ftell( fin ) - 1;
				}
			}
			else
			{
				start_str = false;
			}

			match_pos = sh_header.sh_addr + i;
		}

		if ( search_pattern[match_cnt] == ch )
		{
			++match_cnt;
		}
		else if ( ('\n' == ch) && (match_cnt == pattern_len - 1) )
		{
			// user does not type last character '\n'
			++match_cnt;
		}
		else
		{
			match_cnt = 0;
		}

		if ( match_cnt == pattern_len )
		{
			// check next character is '\0' 
			ch = fgetc( fin );
			if ( EOF == ch )
			{
				print_error( "[Error] fgetc fail --> %s\n", strerror(errno) );
			}

			if ( ('\0' == ch) && (str_pos == match_pos) )
			{
				printf( "exact match '%s' at %0#10lx %0#10lx in section %s\n", search_pattern, str_pos, str_file_pos, section_name );
				is_find_exact = true;
			}
			else
			{
				// partial match, show full string (read character until '\0')
				fseek( fin, -1, SEEK_CUR );
				origin_file_pos = ftell( fin );
				fseek( fin, str_file_pos, SEEK_SET ); 
				int j = 0;
				while ( true )
				{
					ch = fgetc( fin );
					full_str[j] = ch;
					++j;
					if ( EOF == ch )
					{
						print_error( "[Error] fgetc fail --> %s\n", strerror(errno) );
					}
					if ( '\0' == ch )
					{
						full_str[j] = '\0';
						break;
					}
				}
				fseek( fin, origin_file_pos, SEEK_SET ); 
				
				if ( exact_match )
				{
					print_info( "partial match '%s' at %0#10lx %0#10lx in section %s (full-str = '%s')\n", search_pattern, str_pos, str_file_pos, section_name, full_str );
				}
				else
				{
					printf( "partial match '%s' at %0#10lx %0#10lx in section %s (full-str = '%s')\n", search_pattern, str_pos, str_file_pos, section_name, full_str );
				}

			}

			match_cnt = 0;
		}
	}
}

void dump_elf_rela ( FILE *fin, Elf64_Ehdr *elf_header, char *section_name )
{
	Elf64_Half idx = search_section_idx( fin, elf_header, section_name );
	if ( IDX_NOT_FOUND == idx )
	{
		return;
	}

	Elf64_Shdr *rela_header;
	rela_header = hash_section_idx_to_sh_header[idx];
	
	if ( rela_header->sh_entsize != sizeof( Elf64_Rela ) )
	{
		print_error( "[Error] %s entsize=%lu != sizeof(Elf64_Rela)=%lu\n", section_name, rela_header->sh_entsize, sizeof(Elf64_Rela) );
	}
	if ( 0 != (rela_header->sh_size % sizeof( Elf64_Rela )) )
	{
		print_error( "[Error] %s total_size=%lu %% sizeof(Elf64_Rela)=%lu != 0\n", section_name, rela_header->sh_entsize, sizeof(Elf64_Rela) );
	}

	Elf64_Half dynsym_idx = search_section_idx( fin, elf_header, ".dynsym" );
	Elf64_Half dynstrtab_idx = search_section_idx( fin, elf_header, ".dynstr" );
	if ( 0 == dynsym_idx )
	{
		print_error( "[Error] cannot find section .dynsym\n" );
	}
	if ( 0 == dynstrtab_idx )
	{
		print_error( "[Error] cannot find section .dynstr\n" );
	}

	Elf64_Shdr *symtab = hash_section_idx_to_sh_header[dynsym_idx];
	Elf64_Shdr *strtab = hash_section_idx_to_sh_header[dynstrtab_idx];

	size_t n_ent = rela_header->sh_size / rela_header->sh_entsize;
	Elf64_Off origin_file_pos;
	Elf64_Rela ent;
	Elf64_Sym symbol;
	char *belong_section_name;
	char symbol_name[BUFSIZ];
	seek_file( fin, rela_header->sh_offset );
	printf( "Relocation section '%s' contain %lu entries\n", section_name, n_ent );
	printf( "rela_virtual_addr(GOT)\ttype\tname\tsymbol_virtual_addr\tsection\tsymbol_size\n" );

	for ( size_t i = 0; i < n_ent; ++i )
	{
		// read relocation entry from .rela.*
		read_n_byte( fin, rela_header->sh_entsize, &ent );
		origin_file_pos = ftell( fin );

		// need to reallocate virtual addr (GOT ent addr)
		printf( "%0#10lx\t", ent.r_offset );

		// read symbol infomation
		seek_file( fin, symtab->sh_offset + (sizeof(Elf64_Sym)*ELF64_R_SYM(ent.r_info)) );
		read_n_byte( fin, symtab->sh_entsize, &symbol );
		seek_file( fin, strtab->sh_offset + symbol.st_name );
		read_string( fin, symbol_name );
		belong_section_name = get_section_name( elf_header, symbol.st_shndx );
		printf( "%s\t%s\t%0#10lx\t%s\t%lu\n", rela_type_string_map( ELF64_R_TYPE(ent.r_info) ), symbol_name, symbol.st_value, belong_section_name, symbol.st_size );

		seek_file( fin, origin_file_pos );
	}
}

//typedef struct
//{
//  Elf64_Word	p_type;			/* Segment type */
//  Elf64_Word	p_flags;		/* Segment flags */
//  Elf64_Off	p_offset;		/* Segment file offset */
//  Elf64_Addr	p_vaddr;		/* Segment virtual address */
//  Elf64_Addr	p_paddr;		/* Segment physical address */
//  Elf64_Xword	p_filesz;		/* Segment size in file */
//  Elf64_Xword	p_memsz;		/* Segment size in memory */
//  Elf64_Xword	p_align;		/* Segment alignment */
//} Elf64_Phdr;
//typedef struct
//{
//	Elf64_Word n_namesz;			/* Length of the note's name.  */
//	Elf64_Word n_descsz;			/* Length of the note's descriptor.  */
//	Elf64_Word n_type;			/* Type of the note.  */
//} Elf64_Nhdr;
void dump_core_file ( FILE *fin, Elf64_Ehdr *elf_header )
{
	// dump process infomation in PT_NOTE header
	Elf64_Phdr ph_header;
	Elf64_Phdr note_phdr;
	seek_file( fin, elf_header->e_phoff );
	for ( int i = 0; i < elf_header->e_phnum; ++i )
	{
		// read section and keep in hash table 
		read_n_byte( fin, elf_header->e_phentsize, &ph_header );
		if ( PT_NOTE == ph_header.p_type )
		{
			note_phdr = ph_header;
		}
	}

	// read program header of process note 
	Elf64_Nhdr nhdr;
	prstatus_t process_status;
	prfpregset_t fp_reg;
	size_t byte_cnt = 0;
	size_t name_pad;
	size_t data_pad;
	char name[BUFSIZ];
	char *type;
	printf( "\nprogram note (name type data_size):\n" );
	seek_file( fin, note_phdr.p_offset );
	while ( byte_cnt < note_phdr.p_filesz )
	{
		read_n_byte( fin, sizeof(Elf64_Nhdr), &nhdr );
		if ( (nhdr.n_namesz % sizeof(Elf64_Word)) > 0 )
		{
			name_pad = sizeof(Elf64_Word) - (nhdr.n_namesz % sizeof(Elf64_Word));
		}
		else
		{
			name_pad = 0;
		}
		if ( (nhdr.n_descsz % sizeof(Elf64_Word)) > 0 )
		{
			data_pad = sizeof(Elf64_Word) - (nhdr.n_descsz % sizeof(Elf64_Word));
		}
		else
		{
			data_pad = 0;
		}

		type = note_type_string_map( nhdr.n_type );
		byte_cnt += sizeof(Elf64_Nhdr) + nhdr.n_namesz + name_pad + + nhdr.n_descsz + data_pad;
		read_string( fin, name );
		print_info( "name_pad=%d data_pad=%d\n", name_pad, data_pad );
		printf( " %s %s(%0#x) %u\n", name, type, nhdr.n_type, nhdr.n_descsz );

		// skip name pad
		seek_file( fin, ftell(fin) + name_pad );

		if ( NT_PRSTATUS == nhdr.n_type )
		{
			// read general registers
			if ( sizeof(prstatus_t) != nhdr.n_descsz )
			{
				print_error( "[Error] read prstatus_t fail\n" );
			}
			read_n_byte( fin, sizeof(prstatus_t), &process_status );

			// skip data pad
			seek_file( fin, ftell(fin) + data_pad );
		}
		else if ( NT_FPREGSET == nhdr.n_type )
		{
			// read float number registers
			if ( sizeof(prfpregset_t) != nhdr.n_descsz )
			{
				print_error( "[Error] read fpregset_t fail\n" );
			}
			read_n_byte( fin, sizeof(prfpregset_t), &fp_reg );

			// skip data pad
			seek_file( fin, ftell(fin) + data_pad );
		}
		else
		{
			// skip data
			seek_file( fin, ftell(fin) + nhdr.n_descsz + data_pad );
		}
	}

	if ( byte_cnt != note_phdr.p_filesz )
	{
		print_error( "[Error] note read fail\n" );
	}
	
	// read process infomation
	//struct elf_prstatus
	//{
	//	struct elf_siginfo pr_info;		/* Info associated with signal.  */
	//	short int pr_cursig;		/* Current signal.  */
	//	unsigned long int pr_sigpend;	/* Set of pending signals.  */
	//	unsigned long int pr_sighold;	/* Set of held signals.  */
	//	__pid_t pr_pid;
	//	__pid_t pr_ppid;
	//	__pid_t pr_pgrp;
	//	__pid_t pr_sid;
	//	struct timeval pr_utime;		/* User time.  */
	//	struct timeval pr_stime;		/* System time.  */
	//	struct timeval pr_cutime;		/* Cumulative user time.  */
	//	struct timeval pr_cstime;		/* Cumulative system time.  */
	//	elf_gregset_t pr_reg;		/* GP registers.  */
	//	int pr_fpvalid;			/* True if math copro being used.  */
	//};
	printf( "\nprocess information:\n" );
	printf( " PID  = %hu\n", process_status.pr_pid );
	printf( " PPID = %hu\n", process_status.pr_ppid );
	printf( "\nregisters:\n" );
	struct user_regs_struct *gp_reg = (struct user_regs_struct *) process_status.pr_reg;
	//struct user_regs_struct
	//{
	//  __extension__ unsigned long long int r15;
	//  __extension__ unsigned long long int r14;
	//  __extension__ unsigned long long int r13;
	//  __extension__ unsigned long long int r12;
	//  __extension__ unsigned long long int rbp;
	//  __extension__ unsigned long long int rbx;
	//  __extension__ unsigned long long int r11;
	//  __extension__ unsigned long long int r10;
	//  __extension__ unsigned long long int r9;
	//  __extension__ unsigned long long int r8;
	//  __extension__ unsigned long long int rax;
	//  __extension__ unsigned long long int rcx;
	//  __extension__ unsigned long long int rdx;
	//  __extension__ unsigned long long int rsi;
	//  __extension__ unsigned long long int rdi;
	//  __extension__ unsigned long long int orig_rax;
	//  __extension__ unsigned long long int rip;
	//  __extension__ unsigned long long int cs;
	//  __extension__ unsigned long long int eflags;
	//  __extension__ unsigned long long int rsp;
	//  __extension__ unsigned long long int ss;
	//  __extension__ unsigned long long int fs_base;
	//  __extension__ unsigned long long int gs_base;
	//  __extension__ unsigned long long int ds;
	//  __extension__ unsigned long long int es;
	//  __extension__ unsigned long long int fs;
	//  __extension__ unsigned long long int gs;
	//};
	printf( " r15 = %0#10llx\n", gp_reg->r15 );
	printf( " r14 = %0#10llx\n", gp_reg->r14 );
	printf( " r13 = %0#10llx\n", gp_reg->r13 );
	printf( " r12 = %0#10llx\n", gp_reg->r12 );
	printf( " rbp = %0#10llx\n", gp_reg->rbp );
	printf( " rbx = %0#10llx\n", gp_reg->rbx );
	printf( " r11 = %0#10llx\n", gp_reg->r11 );
	printf( " r10 = %0#10llx\n", gp_reg->r10 );
	printf( " r9 = %0#10llx\n", gp_reg->r9 );
	printf( " r8 = %0#10llx\n", gp_reg->r8 );
	printf( " rax = %0#10llx\n", gp_reg->rax );
	printf( " rcx = %0#10llx\n", gp_reg->rcx );
	printf( " rdx = %0#10llx\n", gp_reg->rdx );
	printf( " rsi = %0#10llx\n", gp_reg->rsi );
	printf( " rdi = %0#10llx\n", gp_reg->rdi );
	printf( " orig_rax = %0#10llx\n", gp_reg->orig_rax );
	printf( " rip = %0#10llx\n", gp_reg->rip );
	printf( " cs = %0#10llx\n", gp_reg->cs );
	printf( " eflags = %0#10llx\n", gp_reg->eflags );
	printf( " rsp = %0#10llx\n", gp_reg->rsp );
	printf( " ss = %0#10llx\n", gp_reg->ss );
	printf( " fs_base = %0#10llx\n", gp_reg->fs_base );
	printf( " gs_base = %0#10llx\n", gp_reg->gs_base );
	printf( " ds = %0#10llx\n", gp_reg->ds );
	printf( " es = %0#10llx\n", gp_reg->es );
	printf( " fs = %0#10llx\n", gp_reg->fs );
	printf( " gs = %0#10llx\n", gp_reg->gs );

	//struct user_fpregs_struct
	//{
	//  unsigned short int	cwd;
	//  unsigned short int	swd;
	//  unsigned short int	ftw;
	//  unsigned short int	fop;
	//  __extension__ unsigned long long int rip;
	//  __extension__ unsigned long long int rdp;
	//  unsigned int		mxcsr;
	//  unsigned int		mxcr_mask;
	//  unsigned int		st_space[32];   /* 8*16 bytes for each FP-reg = 128 bytes */
	//  unsigned int		xmm_space[64];  /* 16*16 bytes for each XMM-reg = 256 bytes */
	//  unsigned int		padding[24];
	//};
	union 
	{
		unsigned char hex[16];
		unsigned int inum[4];
		double fnum[2];
	} num;
	for ( int i = 0; i < 16; ++i )
	{
		num.inum[0] = fp_reg.xmm_space[i*4];
		num.inum[1] = fp_reg.xmm_space[i*4 + 1];
		num.inum[2] = fp_reg.xmm_space[i*4 + 3];
		num.inum[3] = fp_reg.xmm_space[i*4 + 4];
		printf( " xmm%-2u = %.15le = ", i, num.fnum[0] );
		for ( int j = 0; j < 8; ++j )
		{
			printf( "%02hhx", num.hex[j] );
		}
		printf( "\n" );
	}
	for ( int i = 0; i < 8; ++i )
	{
		num.inum[0] = fp_reg.st_space[i*4];
		num.inum[1] = fp_reg.st_space[i*4 + 1];
		num.inum[2] = fp_reg.st_space[i*4 + 2];
		num.inum[3] = fp_reg.st_space[i*4 + 3];
		printf( " fpu_st[%u] = %.15le = ", i, num.fnum[0] );
		for ( int j = 0; j < 8; ++j )
		{
			printf( "%02hhx", num.hex[j] );
		}
		printf( "\n" );
	}

}

void search_hex_in_core ( FILE *fin, Elf64_Ehdr *elf_header, char *pattern_str )
{
	// remove space 
	char *pattern_str_reduce = (char *) calloc ( strlen(pattern_str), sizeof(char) );
	size_t pattern_size = 0;
	for ( int i = 0; '\0' != pattern_str[i]; ++i )
	{
		if ( !isspace(pattern_str[i]) )
		{
			pattern_str_reduce[pattern_size] = pattern_str[i];
			++pattern_size;
		}
	}
	if ( (pattern_size > 2) && ('0' == pattern_str_reduce[0]) && ('x' == pattern_str_reduce[1]) )
	{
		pattern_str_reduce = &(pattern_str_reduce[2]); // ignore '0x'
		pattern_size -= 2;
	}

	// check pattern correct
	size_t n_char = strlen( pattern_str_reduce );
	size_t n_bytes;
	if ( 0 != (n_char % 2) )
	{
		print_error( "[Error] hex num need 2 multiplier (2 hex make 1 byte)\n" );
	}
	n_bytes = n_char / 2;

	// transfer char to raw hex (LSB)
	int lsb_idx;
	char *pattern_raw = (char *) calloc ( n_bytes, sizeof(char) );
	char *endptr;
	char tmp_buf[3];
	for ( int i = 0; i < n_bytes; ++i )
	{
		lsb_idx = n_bytes - i - 1;
		tmp_buf[0] = pattern_str_reduce[i*2];
		tmp_buf[1] = pattern_str_reduce[i*2 + 1];
		tmp_buf[2] = '\0';
		pattern_raw[lsb_idx] = strtoul( tmp_buf, &endptr, 16 );
		print_info( "transefer %c%c -> %02hhx\n", pattern_str_reduce[i*2], pattern_str_reduce[i*2+1], pattern_raw[lsb_idx] );
	}
	print_info( "hex pattern = ");
	for ( int i = 0; i < n_bytes; ++i )
	{
		print_info( "%02hhx", pattern_raw[i] );
	}
	print_info( "\n" );

	// dump process infomation in PT_NOTE header
	Elf64_Phdr ph_header;
	Elf64_Phdr note_phdr;
	seek_file( fin, elf_header->e_phoff );
	for ( Elf64_Half k = 0; k < elf_header->e_phnum; ++k )
	{
		// read section and keep in hash table 
		read_n_byte( fin, elf_header->e_phentsize, &ph_header );
		if ( PT_LOAD == ph_header.p_type )
		{
			bool is_find_exact = false;
			Elf64_Addr maximum_match_pos = 0;
			Elf64_Addr hex_pos = 0; // virtual address
			Elf64_Off origin_file_pos = ftell( fin );
			int ch;
			size_t match_cnt = 0;
			size_t maximum_match_cnt = 0;
			size_t pattern_len = pattern_size;

			seek_file( fin, ph_header.p_offset );
			for ( Elf64_Xword i = 0; i < ph_header.p_filesz; ++i )
			{
				ch = fgetc( fin );
				if ( EOF == ch )
				{
					print_error( "[Error] fgetc fail --> %s\n", strerror(errno) );
				}

				if ( 0 == match_cnt )
				{
					hex_pos = ph_header.p_vaddr + (Elf64_Addr)i;
				}
				
				if ( pattern_raw[match_cnt] == ch )
				{
					++match_cnt;
				}
				else
				{
					if ( match_cnt > maximum_match_pos )
					{
						maximum_match_pos = hex_pos;
						maximum_match_cnt = match_cnt;
					}
					match_cnt = 0;
				}

				if ( match_cnt == pattern_len )
				{
					is_find_exact = true;
					printf( "exact match '%s' at virtual address %0#10lx (%lu bytes) in load header %hu\n", pattern_str_reduce, hex_pos, match_cnt, k );
					match_cnt = 0;
				}
			}

			if ( !is_find_exact && maximum_match_cnt > 0 )
			{
				printf( "patial match '%*s' at virtual address %0#10lx (%lu bytes) in load header %hd\n", (int) maximum_match_cnt * 2, pattern_str_reduce, maximum_match_pos, maximum_match_cnt, k );
			}

			seek_file( fin, origin_file_pos );
		}
	}
}

int main ( int argc, char **argv )
{
	// parse command line arguments
	parse_cmd_options( argc, argv );

	// read ELF header
	FILE *fin = fopen( g_opts.elf_file, "r" );
	if ( !fin )
	{
		print_error( "[Error] open ELF file '%s' fail --> %s\n", g_opts.elf_file, strerror(errno) );
	}

	Elf64_Ehdr elf_header;
	if ( DUMP_ELF_INFO == g_opts.utility )
	{
		g_opts.debug = true; // enable print_info
	}
	read_elf_header( fin, &elf_header );

	// start to hack
	if ( DUMP_STRING == g_opts.utility )
	{
		if ( g_opts.section_name )
		{
			dump_elf_string( fin, &elf_header, g_opts.section_name );
		}
		else
		{
			dump_elf_string( fin, &elf_header, ".rodata" );
			dump_elf_string( fin, &elf_header, ".data" );
		}

	}
	else if ( FIND_STRING == g_opts.utility )
	{
		if ( g_opts.section_name )
		{
			find_elf_string( fin, &elf_header, g_opts.section_name, g_opts.search_pattern, g_opts.exact_match );
		}
		else
		{
			find_elf_string( fin, &elf_header, ".rodata", g_opts.search_pattern, g_opts.exact_match );
			find_elf_string( fin, &elf_header, ".data", g_opts.search_pattern, g_opts.exact_match );
		}
	}
	else if ( DUMP_SYMBOL == g_opts.utility )
	{
		if ( g_opts.section_name )
		{
			dump_elf_symbol( fin, &elf_header, g_opts.section_name, NULL, false );
		}
		else
		{
			dump_elf_symbol( fin, &elf_header, ".symtab", NULL, false );
			dump_elf_symbol( fin, &elf_header, ".dynsym", NULL, false );
		}
	}
	else if ( FIND_SYMBOL == g_opts.utility )
	{
		if ( g_opts.section_name )
		{
			dump_elf_symbol( fin, &elf_header, g_opts.section_name, g_opts.search_pattern, g_opts.exact_match );
		}
		else
		{
			dump_elf_symbol( fin, &elf_header, ".symtab", g_opts.search_pattern, g_opts.exact_match );
			dump_elf_symbol( fin, &elf_header, ".dynsym", g_opts.search_pattern, g_opts.exact_match );
		}
	}
	else if ( (DUMP_RELA == g_opts.utility) && (ET_REL != elf_header.e_type) )
	{
		if ( g_opts.section_name )
		{
			dump_elf_rela( fin, &elf_header, g_opts.section_name );
		}
		else
		{
			dump_elf_rela( fin, &elf_header, ".rela.dyn" ); // dynamic library global variable
			dump_elf_rela( fin, &elf_header, ".rela.plt" ); // dynamic library global functions bind by PLT (lazy binding)
		}
	}
	else if ( DUMP_CORE == g_opts.utility ) 
	{
		if ( ET_CORE != elf_header.e_type )
		{
			print_error( "%s is not core file\n", g_opts.elf_file );
		}
		else
		{
			dump_core_file( fin, &elf_header );
		}
	}
	else if ( FIND_HEX == g_opts.utility ) 
	{
		if ( ET_CORE == elf_header.e_type )
		{
			search_hex_in_core( fin, &elf_header, g_opts.search_pattern );
		}
		else
		{
		}
	}

	return EXIT_SUCCESS;
}

