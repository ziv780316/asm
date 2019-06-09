#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <elf.h>

#include "opts.h"

void print_info ( char *fmt, ... );
void print_error ( char *fmt, ... );
void read_n_byte ( FILE *fin, size_t n, void *buf );
void seek_file ( FILE *fin, Elf64_Off offset );
void read_section_header ( FILE *fin, Elf64_Ehdr *elf_header, Elf64_Half idx, void *buf );
void read_section_data ( FILE *fin, Elf64_Shdr *sh_header, void *buf );
void read_elf_header ( FILE *fin, Elf64_Ehdr *elf_header );
Elf64_Half search_section_idx ( FILE *fin, Elf64_Ehdr *elf_header, char *name );
void dump_elf_string ( FILE *fin, Elf64_Ehdr *elf_header );

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
	print_info( "section header table file offset = %#lx\n", elf_header->e_shoff );
	print_info( "program header table file offset = %#lx\n", elf_header->e_phoff );

	if ( SHN_UNDEF == elf_header->e_shstrndx )
	{
		print_error( "[Error] cannot find section .shstrtab\n" );
	}
	print_info( "section header string table index = %hu\n", elf_header->e_shstrndx );

	// print all section name
	Elf64_Shdr shstrtab;
	read_section_header( fin, elf_header, elf_header->e_shstrndx, &shstrtab );

	char *name_buf = malloc ( shstrtab.sh_size );
	read_section_data( fin, &shstrtab, (void *)name_buf );

	Elf64_Shdr sh_header;
	print_info( "sections (name size virtual_addr):\n" );
	for ( int i = 0; i < elf_header->e_shnum; ++i )
	{
		read_section_header( fin, elf_header, i, &sh_header );
		print_info( " %-30s %-10lu %-#10lx\n", name_buf + sh_header.sh_name, sh_header.sh_size, sh_header.sh_addr );
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
void dump_elf_string ( FILE *fin, Elf64_Ehdr *elf_header )
{
	char *section_name;
	if ( g_opts.section_name )
	{
		section_name = g_opts.section_name;	
	}
	else
	{
		section_name = ".rodata";
	}

	Elf64_Half idx = search_section_idx( fin, elf_header, section_name );
	print_info( "dump section %s idx=%d ...\n", section_name, idx );

	Elf64_Shdr sh_header;
	read_section_header( fin, elf_header, idx, &sh_header );

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
			printf( "%c", ch );
		}
		else
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
				printf( "\\0" );
			}
			else
			{
				printf( "*" );
			}
		}
	}
	printf( "\n" );
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
		dump_elf_string( fin, &elf_header );
	}
	else if ( FIND_STRING == g_opts.utility )
	{
		//find_elf_string( fin, &elf_header );
	}

	return EXIT_SUCCESS;
}

