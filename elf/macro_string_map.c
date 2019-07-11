// vim substitue command 
// '<,'>s;#define\s\+\(\S\+\)\s\+.*\/\*\s*\(\S\+.*\S\+\)\s*\*\/.*;case \1: return "\2"\;;

#include "macro_string_map.h"

char *note_type_string_map ( Elf64_Word type )
{
	switch ( type )
	{
		case NT_PRSTATUS: return "NT_PRSTATUS";
		case NT_FPREGSET: return "NT_FPREGSET";
		case NT_PRPSINFO: return "NT_PRPSINFO";
		case NT_PRXREG: return "NT_PRXREG";
		case NT_PLATFORM: return "NT_PLATFORM";
		case NT_AUXV: return "NT_AUXV";
		case NT_GWINDOWS: return "NT_GWINDOWS";
		case NT_ASRS: return "NT_ASRS";
		case NT_PSTATUS: return "NT_PSTATUS";
		case NT_PSINFO: return "NT_PSINFO";
		case NT_PRCRED: return "NT_PRCRED";
		case NT_UTSNAME: return "NT_UTSNAME";
		case NT_LWPSTATUS: return "NT_LWPSTATUS";
		case NT_LWPSINFO: return "NT_LWPSINFO";
		case NT_PRFPXREG: return "NT_PRFPXREG";
		case NT_SIGINFO: return "NT_SIGINFO";
		case NT_FILE: return "NT_FILE";
		case NT_PRXFPREG: return "NT_PRXFPREG";
		case NT_PPC_VMX: return "NT_PPC_VMX";
		case NT_PPC_SPE: return "NT_PPC_SPE";
		case NT_PPC_VSX: return "NT_PPC_VSX";
		case NT_386_TLS: return "NT_386_TLS";
		case NT_386_IOPERM: return "NT_386_IOPERM";
		case NT_X86_XSTATE: return "NT_X86_XSTATE";
		case NT_S390_HIGH_GPRS: return "NT_S390_HIGH_GPRS";
		case NT_S390_TIMER: return "NT_S390_TIMER";
		case NT_S390_TODCMP: return "NT_S390_TODCMP";
		case NT_S390_TODPREG: return "NT_S390_TODPREG";
		case NT_S390_CTRS: return "NT_S390_CTRS";
		case NT_S390_PREFIX: return "NT_S390_PREFIX";
		case NT_S390_LAST_BREAK: return "NT_S390_LAST_BREAK";
		case NT_S390_SYSTEM_CALL: return "NT_S390_SYSTEM_CALL";
		case NT_S390_TDB: return "NT_S390_TDB";
		case NT_ARM_VFP: return "NT_ARM_VFP";
		case NT_ARM_TLS: return "NT_ARM_TLS";
		case NT_ARM_HW_BREAK: return "NT_ARM_HW_BREAK";
		case NT_ARM_HW_WATCH: return "NT_ARM_HW_WATCH";
		default : return "?";
	}
}

char *cpu_type_string_map ( Elf64_Half type )
{
	switch ( type )
	{
		case EM_NONE: return "No machine";
		case EM_M32: return "AT&T WE 32100";
		case EM_SPARC: return "SUN SPARC";
		case EM_386: return "Intel 80386";
		case EM_68K: return "Motorola m68k family";
		case EM_88K: return "Motorola m88k family";
		case EM_860: return "Intel 80860";
		case EM_MIPS: return "MIPS R3000 big-endian";
		case EM_S370: return "IBM System/370";
		case EM_MIPS_RS3_LE: return "MIPS R3000 little-endian";
		case EM_PARISC: return "HPPA";
		case EM_VPP500: return "Fujitsu VPP500";
		case EM_SPARC32PLUS: return "Sun's \"v8plus\"";
		case EM_960: return "Intel 80960";
		case EM_PPC: return "PowerPC";
		case EM_PPC64: return "PowerPC 64-bit";
		case EM_S390: return "IBM S390";
		case EM_V800: return "NEC V800 series";
		case EM_FR20: return "Fujitsu FR20";
		case EM_RH32: return "TRW RH-32";
		case EM_RCE: return "Motorola RCE";
		case EM_ARM: return "ARM";
		case EM_FAKE_ALPHA: return "Digital Alpha";
		case EM_SH: return "Hitachi SH";
		case EM_SPARCV9: return "SPARC v9 64-bit";
		case EM_TRICORE: return "Siemens Tricore";
		case EM_ARC: return "Argonaut RISC Core";
		case EM_H8_300: return "Hitachi H8/300";
		case EM_H8_300H: return "Hitachi H8/300H";
		case EM_H8S: return "Hitachi H8S";
		case EM_H8_500: return "Hitachi H8/500";
		case EM_IA_64: return "Intel Merced";
		case EM_MIPS_X: return "Stanford MIPS-X";
		case EM_COLDFIRE: return "Motorola Coldfire";
		case EM_68HC12: return "Motorola M68HC12";
		case EM_MMA: return "Fujitsu MMA Multimedia Accelerator";
		case EM_PCP: return "Siemens PCP";
		case EM_NCPU: return "Sony nCPU embeeded RISC";
		case EM_NDR1: return "Denso NDR1 microprocessor";
		case EM_STARCORE: return "Motorola Start*Core processor";
		case EM_ME16: return "Toyota ME16 processor";
		case EM_ST100: return "STMicroelectronic ST100 processor";
		case EM_TINYJ: return "Advanced Logic Corp. Tinyj emb.fam";
		case EM_X86_64: return "AMD x86-64 architecture";
		case EM_PDSP: return "Sony DSP Processor";
		case EM_FX66: return "Siemens FX66 microcontroller";
		case EM_ST9PLUS: return "STMicroelectronics ST9+ 8/16 mc";
		case EM_ST7: return "STmicroelectronics ST7 8 bit mc";
		case EM_68HC16: return "Motorola MC68HC16 microcontroller";
		case EM_68HC11: return "Motorola MC68HC11 microcontroller";
		case EM_68HC08: return "Motorola MC68HC08 microcontroller";
		case EM_68HC05: return "Motorola MC68HC05 microcontroller";
		case EM_SVX: return "Silicon Graphics SVx";
		case EM_ST19: return "STMicroelectronics ST19 8 bit mc";
		case EM_VAX: return "Digital VAX";
		case EM_CRIS: return "Axis Communications 32-bit embedded processor";
		case EM_JAVELIN: return "Infineon Technologies 32-bit embedded processor";
		case EM_FIREPATH: return "Element 14 64-bit DSP Processor";
		case EM_ZSP: return "LSI Logic 16-bit DSP Processor";
		case EM_MMIX: return "Donald Knuth's educational 64-bit processor";
		case EM_HUANY: return "Harvard University machine-independent object files";
		case EM_PRISM: return "SiTera Prism";
		case EM_AVR: return "Atmel AVR 8-bit microcontroller";
		case EM_FR30: return "Fujitsu FR30";
		case EM_D10V: return "Mitsubishi D10V";
		case EM_D30V: return "Mitsubishi D30V";
		case EM_V850: return "NEC v850";
		case EM_M32R: return "Mitsubishi M32R";
		case EM_MN10300: return "Matsushita MN10300";
		case EM_MN10200: return "Matsushita MN10200";
		case EM_PJ: return "picoJava";
		case EM_OPENRISC: return "OpenRISC 32-bit embedded processor";
		case EM_ARC_A5: return "ARC Cores Tangent-A5";
		case EM_XTENSA: return "Tensilica Xtensa Architecture";
		case EM_ALTERA_NIOS2: return "Altera Nios II";
		case EM_AARCH64: return "ARM AARCH64";
		case EM_TILEPRO: return "Tilera TILEPro";
		case EM_MICROBLAZE: return "Xilinx MicroBlaze";
		case EM_TILEGX: return "Tilera TILE-Gx";
		default : return "?";
	}
}

char *osabi_type_string_map ( unsigned char type )
{
	switch ( type )
	{
		case ELFOSABI_SYSV: return "UNIX System V ABI";
		case ELFOSABI_HPUX: return "HP-UX";
		case ELFOSABI_NETBSD: return "NetBSD.";
		case ELFOSABI_GNU: return "Object uses GNU ELF extensions.";
		case ELFOSABI_SOLARIS: return "Sun Solaris.";
		case ELFOSABI_AIX: return "IBM AIX.";
		case ELFOSABI_IRIX: return "SGI Irix.";
		case ELFOSABI_FREEBSD: return "FreeBSD.";
		case ELFOSABI_TRU64: return "Compaq TRU64 UNIX.";
		case ELFOSABI_MODESTO: return "Novell Modesto.";
		case ELFOSABI_OPENBSD: return "OpenBSD.";
		case ELFOSABI_ARM_AEABI: return "ARM EABI";
		case ELFOSABI_ARM: return "ARM";
		case ELFOSABI_STANDALONE: return "Standalone (embedded) application";
		default : return "?";
	}
}

char *rela_type_string_map ( Elf64_Half type )
{
	switch ( type )
	{
		case R_X86_64_NONE: return "No reloc";
		case R_X86_64_64: return "Direct 64 bit";
		case R_X86_64_PC32: return "PC relative 32 bit signed";
		case R_X86_64_GOT32: return "32 bit GOT entry";
		case R_X86_64_PLT32: return "32 bit PLT address";
		case R_X86_64_COPY: return "Copy symbol at runtime";
		case R_X86_64_GLOB_DAT: return "Create GOT entry";
		case R_X86_64_JUMP_SLOT: return "Create PLT entry";
		case R_X86_64_RELATIVE: return "Adjust by program base";
		case R_X86_64_GOTPCREL: return "32 bit signed PC relative offset to GOT";
		case R_X86_64_32: return "Direct 32 bit zero extended";
		case R_X86_64_32S: return "Direct 32 bit sign extended";
		case R_X86_64_16: return "Direct 16 bit zero extended";
		case R_X86_64_PC16: return "16 bit sign extended pc relative";
		case R_X86_64_8: return "Direct 8 bit sign extended";
		case R_X86_64_PC8: return "8 bit sign extended pc relative";
		case R_X86_64_DTPMOD64: return "ID of module containing symbol";
		case R_X86_64_DTPOFF64: return "Offset in module's TLS block";
		case R_X86_64_TPOFF64: return "Offset in initial TLS block";
		case R_X86_64_TLSGD: return "32 bit signed PC relative offset to two GOT entries for GD symbol";
		case R_X86_64_TLSLD: return "32 bit signed PC relative offset to two GOT entries for LD symbol";
		case R_X86_64_DTPOFF32: return "Offset in TLS block";
		case R_X86_64_GOTTPOFF: return "32 bit signed PC relative offset to GOT entry for IE symbol";
		case R_X86_64_TPOFF32: return "Offset in initial TLS block";
		case R_X86_64_PC64: return "PC relative 64 bit";
		case R_X86_64_GOTOFF64: return "64 bit offset to GOT";
		case R_X86_64_GOTPC32: return "32 bit signed pc relative offset to GOT";
		case R_X86_64_GOT64: return "64-bit GOT entry offset";
		case R_X86_64_GOTPCREL64: return "64-bit PC relative offset to GOT entry";
		case R_X86_64_GOTPC64: return "64-bit PC relative offset to GOT";
		case R_X86_64_GOTPLT64: return "like GOT64, says PLT entry needed";
		case R_X86_64_PLTOFF64: return "64-bit GOT relative offset to PLT entry";
		case R_X86_64_SIZE32: return "Size of symbol plus 32-bit addend";
		case R_X86_64_SIZE64: return "Size of symbol plus 64-bit addend";
		case R_X86_64_GOTPC32_TLSDESC: return "GOT offset for TLS descriptor.";
		case R_X86_64_TLSDESC_CALL: return "Marker for call through TLS descriptor.";
		case R_X86_64_TLSDESC: return "TLS descriptor.";
		case R_X86_64_IRELATIVE: return "Adjust indirectly by program base";
		case R_X86_64_RELATIVE64: return "64-bit adjust by program base";
		default : return "?";
	}
}
