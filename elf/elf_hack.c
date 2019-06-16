#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <search.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <elf.h>

#include "opts.h"

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
void read_symbol_data ( FILE *fin, Elf64_Shdr *sh_header, Elf64_Sym *sym, data_buf *buf );
void modify_elf_code( FILE *fin, Elf64_Off offset, size_t n_hex, char *lsb_hex_bytes );

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

	if ( ELFOSABI_SYSV == elf_header->e_ident[EI_OSABI] )
	{
		print_info( "OS/ABI = UNIX system V ABI\n" );
	}
	else
	{
		print_error( "[Warning] ELF file '%s' may not compatible in UNIX system V ABI\n", g_opts.elf_file );
	}

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

	if ( SHN_UNDEF == elf_header->e_shstrndx )
	{
		print_error( "[Error] cannot find section .shstrtab\n" );
	}
	print_info( "section header string table index = %hu\n", elf_header->e_shstrndx );

	// print all section name
	Elf64_Shdr shstrtab;
	read_section_header( fin, elf_header, elf_header->e_shstrndx, &shstrtab );

	// read section names
	char *name_buf = malloc ( shstrtab.sh_size );
	read_section_data( fin, &shstrtab, (void *)name_buf );

	// create hash to map name to section header
	hash_section_idx_to_name = (char **) malloc ( sizeof(char *) * elf_header->e_shnum );
	hash_section_idx_to_sh_header = (Elf64_Shdr **) malloc ( sizeof(Elf64_Shdr *) * elf_header->e_shnum );
	ENTRY e, *ep;
	hcreate( elf_header->e_shnum );

	Elf64_Shdr *sh_header;
	print_info( "sections (name size virtual_addr):\n" );
	for ( int i = 0; i < elf_header->e_shnum; ++i )
	{
		// read section and keep in hash table 
		sh_header = (Elf64_Shdr *) malloc ( sizeof(Elf64_Shdr) );
		read_section_header( fin, elf_header, i, sh_header );
		hash_section_idx_to_sh_header[i] = sh_header;
		print_info( " %-30s %-10lu %0#10lx\n", name_buf + sh_header->sh_name, sh_header->sh_size, sh_header->sh_addr );

		// create map section index to section name 
		hash_section_idx_to_name[i] = (char *) malloc ( strlen(name_buf + sh_header->sh_name) + 1 );
		strcpy( hash_section_idx_to_name[i], name_buf + sh_header->sh_name );

		// create map section name to file offset
		e.key = hash_section_idx_to_name[i];
		e.data = (void *) sh_header;
		ep = hsearch( e, ENTER );
		if ( !ep )
		{
			print_error( "hsearch record key %s fail\n", e.key );
		}
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
		print_error( "[Error] cannot find section '%s'\n", name );
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
	print_info( "dump section %s idx=%d ...\n", section_name, idx );

	Elf64_Shdr symtab_header;
	read_section_header( fin, elf_header, idx, &symtab_header );

	if ( SHT_SYMTAB != symtab_header.sh_type )
	{
		print_error( "[Error] '%s' is not symtab section\n", section_name );
	}

	// read strtab section (store symbol name)
	Elf64_Half strtab_idx = search_section_idx( fin, elf_header, ".strtab" );
	Elf64_Shdr strtab_header;
	read_section_header( fin, elf_header, strtab_idx, &strtab_header );

	if ( symtab_header.sh_entsize != sizeof( Elf64_Sym ) )
	{
		print_error( "[Error] %s entsize=%lu != sizeof(Elf64_Sym)=%lu\n", section_name, symtab_header.sh_entsize, sizeof(Elf64_Sym) );
	}
	if ( 0 != (symtab_header.sh_size % sizeof( Elf64_Sym )) )
	{
		print_error( "[Error] %s total_size=%lu %% sizeof(Elf64_Sym)=%lu != 0\n", section_name, symtab_header.sh_entsize, sizeof(Elf64_Sym) );
	}

	Elf64_Off origin_file_pos;
	Elf64_Off sym_data_pos;
	size_t n_symbol = symtab_header.sh_size / symtab_header.sh_entsize;
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
	seek_file( fin, symtab_header.sh_offset );
	for ( size_t i = 0; i < n_symbol; ++i )
	{
		// read symbol entry from symtab
		read_n_byte( fin, symtab_header.sh_entsize, &symbol );
		origin_file_pos = ftell( fin );

		// read symbol name from strtab
		seek_file( fin, strtab_header.sh_offset + symbol.st_name );
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

		if ( symbol.st_shndx >= elf_header->e_shnum )
		{
			switch( symbol.st_shndx )
			{
				case SHN_ABS:
					belong_section_name = "ABS";
					break;

				case SHN_UNDEF:
					belong_section_name = "UND";
					break;

				case SHN_COMMON:
					belong_section_name = "COMMON";
					break;

				default:
					belong_section_name = "?";
					break;
			}
		}
		else
		{
			belong_section_name = hash_section_idx_to_name[symbol.st_shndx];
		}

		if ( (STT_OBJECT == ELF64_ST_TYPE( symbol.st_info )) && (symbol.st_shndx != bss_section_idx) && (symbol.st_size > 0) )
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
		}
	}

	return EXIT_SUCCESS;
}

