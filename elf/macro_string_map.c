// vim substitue command 
// '<,'>s;#define\s\+\(\S\+\)\s\+.*\/\*\s*\(\S\+.*\S\+\)\s*\*\/.*;case \1: return "\2"\;;
// '<,'>s;.*case\s*\(\S\+\)\s*:.*$;#ifdef \1\r&\r#endif

#include "macro_string_map.h"

char *note_type_string_map ( Elf64_Word type )
{
	switch ( type )
	{
#ifdef NT_PRSTATUS
		case NT_PRSTATUS: return "NT_PRSTATUS";
#endif
#ifdef NT_FPREGSET
		case NT_FPREGSET: return "NT_FPREGSET";
#endif
#ifdef NT_PRPSINFO
		case NT_PRPSINFO: return "NT_PRPSINFO";
#endif
#ifdef NT_PRXREG
		case NT_PRXREG: return "NT_PRXREG";
#endif
#ifdef NT_PLATFORM
		case NT_PLATFORM: return "NT_PLATFORM";
#endif
#ifdef NT_AUXV
		case NT_AUXV: return "NT_AUXV";
#endif
#ifdef NT_GWINDOWS
		case NT_GWINDOWS: return "NT_GWINDOWS";
#endif
#ifdef NT_ASRS
		case NT_ASRS: return "NT_ASRS";
#endif
#ifdef NT_PSTATUS
		case NT_PSTATUS: return "NT_PSTATUS";
#endif
#ifdef NT_PSINFO
		case NT_PSINFO: return "NT_PSINFO";
#endif
#ifdef NT_PRCRED
		case NT_PRCRED: return "NT_PRCRED";
#endif
#ifdef NT_UTSNAME
		case NT_UTSNAME: return "NT_UTSNAME";
#endif
#ifdef NT_LWPSTATUS
		case NT_LWPSTATUS: return "NT_LWPSTATUS";
#endif
#ifdef NT_LWPSINFO
		case NT_LWPSINFO: return "NT_LWPSINFO";
#endif
#ifdef NT_PRFPXREG
		case NT_PRFPXREG: return "NT_PRFPXREG";
#endif
#ifdef NT_SIGINFO
		case NT_SIGINFO: return "NT_SIGINFO";
#endif
#ifdef NT_FILE
		case NT_FILE: return "NT_FILE";
#endif
#ifdef NT_PRXFPREG
		case NT_PRXFPREG: return "NT_PRXFPREG";
#endif
#ifdef NT_PPC_VMX
		case NT_PPC_VMX: return "NT_PPC_VMX";
#endif
#ifdef NT_PPC_SPE
		case NT_PPC_SPE: return "NT_PPC_SPE";
#endif
#ifdef NT_PPC_VSX
		case NT_PPC_VSX: return "NT_PPC_VSX";
#endif
#ifdef NT_386_TLS
		case NT_386_TLS: return "NT_386_TLS";
#endif
#ifdef NT_386_IOPERM
		case NT_386_IOPERM: return "NT_386_IOPERM";
#endif
#ifdef NT_X86_XSTATE
		case NT_X86_XSTATE: return "NT_X86_XSTATE";
#endif
#ifdef NT_S390_HIGH_GPRS
		case NT_S390_HIGH_GPRS: return "NT_S390_HIGH_GPRS";
#endif
#ifdef NT_S390_TIMER
		case NT_S390_TIMER: return "NT_S390_TIMER";
#endif
#ifdef NT_S390_TODCMP
		case NT_S390_TODCMP: return "NT_S390_TODCMP";
#endif
#ifdef NT_S390_TODPREG
		case NT_S390_TODPREG: return "NT_S390_TODPREG";
#endif
#ifdef NT_S390_CTRS
		case NT_S390_CTRS: return "NT_S390_CTRS";
#endif
#ifdef NT_S390_PREFIX
		case NT_S390_PREFIX: return "NT_S390_PREFIX";
#endif
#ifdef NT_S390_LAST_BREAK
		case NT_S390_LAST_BREAK: return "NT_S390_LAST_BREAK";
#endif
#ifdef NT_S390_SYSTEM_CALL
		case NT_S390_SYSTEM_CALL: return "NT_S390_SYSTEM_CALL";
#endif
#ifdef NT_S390_TDB
		case NT_S390_TDB: return "NT_S390_TDB";
#endif
#ifdef NT_ARM_VFP
		case NT_ARM_VFP: return "NT_ARM_VFP";
#endif
#ifdef NT_ARM_TLS
		case NT_ARM_TLS: return "NT_ARM_TLS";
#endif
#ifdef NT_ARM_HW_BREAK
		case NT_ARM_HW_BREAK: return "NT_ARM_HW_BREAK";
#endif
#ifdef NT_ARM_HW_WATCH
		case NT_ARM_HW_WATCH: return "NT_ARM_HW_WATCH";
#endif
		default : return "?";
	}
}

char *cpu_type_string_map ( Elf64_Half type )
{
	switch ( type )
	{
#ifdef EM_NONE
		case EM_NONE: return "No machine";
#endif
#ifdef EM_M32
		case EM_M32: return "AT&T WE 32100";
#endif
#ifdef EM_SPARC
		case EM_SPARC: return "SUN SPARC";
#endif
#ifdef EM_386
		case EM_386: return "Intel 80386";
#endif
#ifdef EM_68K
		case EM_68K: return "Motorola m68k family";
#endif
#ifdef EM_88K
		case EM_88K: return "Motorola m88k family";
#endif
#ifdef EM_860
		case EM_860: return "Intel 80860";
#endif
#ifdef EM_MIPS
		case EM_MIPS: return "MIPS R3000 big-endian";
#endif
#ifdef EM_S370
		case EM_S370: return "IBM System/370";
#endif
#ifdef EM_MIPS_RS3_LE
		case EM_MIPS_RS3_LE: return "MIPS R3000 little-endian";
#endif
#ifdef EM_PARISC
		case EM_PARISC: return "HPPA";
#endif
#ifdef EM_VPP500
		case EM_VPP500: return "Fujitsu VPP500";
#endif
#ifdef EM_SPARC32PLUS
		case EM_SPARC32PLUS: return "Sun's \"v8plus\"";
#endif
#ifdef EM_960
		case EM_960: return "Intel 80960";
#endif
#ifdef EM_PPC
		case EM_PPC: return "PowerPC";
#endif
#ifdef EM_PPC64
		case EM_PPC64: return "PowerPC 64-bit";
#endif
#ifdef EM_S390
		case EM_S390: return "IBM S390";
#endif
#ifdef EM_V800
		case EM_V800: return "NEC V800 series";
#endif
#ifdef EM_FR20
		case EM_FR20: return "Fujitsu FR20";
#endif
#ifdef EM_RH32
		case EM_RH32: return "TRW RH-32";
#endif
#ifdef EM_RCE
		case EM_RCE: return "Motorola RCE";
#endif
#ifdef EM_ARM
		case EM_ARM: return "ARM";
#endif
#ifdef EM_FAKE_ALPHA
		case EM_FAKE_ALPHA: return "Digital Alpha";
#endif
#ifdef EM_SH
		case EM_SH: return "Hitachi SH";
#endif
#ifdef EM_SPARCV9
		case EM_SPARCV9: return "SPARC v9 64-bit";
#endif
#ifdef EM_TRICORE
		case EM_TRICORE: return "Siemens Tricore";
#endif
#ifdef EM_ARC
		case EM_ARC: return "Argonaut RISC Core";
#endif
#ifdef EM_H8_300
		case EM_H8_300: return "Hitachi H8/300";
#endif
#ifdef EM_H8_300H
		case EM_H8_300H: return "Hitachi H8/300H";
#endif
#ifdef EM_H8S
		case EM_H8S: return "Hitachi H8S";
#endif
#ifdef EM_H8_500
		case EM_H8_500: return "Hitachi H8/500";
#endif
#ifdef EM_IA_64
		case EM_IA_64: return "Intel Merced";
#endif
#ifdef EM_MIPS_X
		case EM_MIPS_X: return "Stanford MIPS-X";
#endif
#ifdef EM_COLDFIRE
		case EM_COLDFIRE: return "Motorola Coldfire";
#endif
#ifdef EM_68HC12
		case EM_68HC12: return "Motorola M68HC12";
#endif
#ifdef EM_MMA
		case EM_MMA: return "Fujitsu MMA Multimedia Accelerator";
#endif
#ifdef EM_PCP
		case EM_PCP: return "Siemens PCP";
#endif
#ifdef EM_NCPU
		case EM_NCPU: return "Sony nCPU embeeded RISC";
#endif
#ifdef EM_NDR1
		case EM_NDR1: return "Denso NDR1 microprocessor";
#endif
#ifdef EM_STARCORE
		case EM_STARCORE: return "Motorola Start*Core processor";
#endif
#ifdef EM_ME16
		case EM_ME16: return "Toyota ME16 processor";
#endif
#ifdef EM_ST100
		case EM_ST100: return "STMicroelectronic ST100 processor";
#endif
#ifdef EM_TINYJ
		case EM_TINYJ: return "Advanced Logic Corp. Tinyj emb.fam";
#endif
#ifdef EM_X86_64
		case EM_X86_64: return "AMD x86-64 architecture";
#endif
#ifdef EM_PDSP
		case EM_PDSP: return "Sony DSP Processor";
#endif
#ifdef EM_FX66
		case EM_FX66: return "Siemens FX66 microcontroller";
#endif
#ifdef EM_ST9PLUS
		case EM_ST9PLUS: return "STMicroelectronics ST9+ 8/16 mc";
#endif
#ifdef EM_ST7
		case EM_ST7: return "STmicroelectronics ST7 8 bit mc";
#endif
#ifdef EM_68HC16
		case EM_68HC16: return "Motorola MC68HC16 microcontroller";
#endif
#ifdef EM_68HC11
		case EM_68HC11: return "Motorola MC68HC11 microcontroller";
#endif
#ifdef EM_68HC08
		case EM_68HC08: return "Motorola MC68HC08 microcontroller";
#endif
#ifdef EM_68HC05
		case EM_68HC05: return "Motorola MC68HC05 microcontroller";
#endif
#ifdef EM_SVX
		case EM_SVX: return "Silicon Graphics SVx";
#endif
#ifdef EM_ST19
		case EM_ST19: return "STMicroelectronics ST19 8 bit mc";
#endif
#ifdef EM_VAX
		case EM_VAX: return "Digital VAX";
#endif
#ifdef EM_CRIS
		case EM_CRIS: return "Axis Communications 32-bit embedded processor";
#endif
#ifdef EM_JAVELIN
		case EM_JAVELIN: return "Infineon Technologies 32-bit embedded processor";
#endif
#ifdef EM_FIREPATH
		case EM_FIREPATH: return "Element 14 64-bit DSP Processor";
#endif
#ifdef EM_ZSP
		case EM_ZSP: return "LSI Logic 16-bit DSP Processor";
#endif
#ifdef EM_MMIX
		case EM_MMIX: return "Donald Knuth's educational 64-bit processor";
#endif
#ifdef EM_HUANY
		case EM_HUANY: return "Harvard University machine-independent object files";
#endif
#ifdef EM_PRISM
		case EM_PRISM: return "SiTera Prism";
#endif
#ifdef EM_AVR
		case EM_AVR: return "Atmel AVR 8-bit microcontroller";
#endif
#ifdef EM_FR30
		case EM_FR30: return "Fujitsu FR30";
#endif
#ifdef EM_D10V
		case EM_D10V: return "Mitsubishi D10V";
#endif
#ifdef EM_D30V
		case EM_D30V: return "Mitsubishi D30V";
#endif
#ifdef EM_V850
		case EM_V850: return "NEC v850";
#endif
#ifdef EM_M32R
		case EM_M32R: return "Mitsubishi M32R";
#endif
#ifdef EM_MN10300
		case EM_MN10300: return "Matsushita MN10300";
#endif
#ifdef EM_MN10200
		case EM_MN10200: return "Matsushita MN10200";
#endif
#ifdef EM_PJ
		case EM_PJ: return "picoJava";
#endif
#ifdef EM_OPENRISC
		case EM_OPENRISC: return "OpenRISC 32-bit embedded processor";
#endif
#ifdef EM_ARC_A5
		case EM_ARC_A5: return "ARC Cores Tangent-A5";
#endif
#ifdef EM_XTENSA
		case EM_XTENSA: return "Tensilica Xtensa Architecture";
#endif
#ifdef EM_ALTERA_NIOS2
		case EM_ALTERA_NIOS2: return "Altera Nios II";
#endif
#ifdef EM_AARCH64
		case EM_AARCH64: return "ARM AARCH64";
#endif
#ifdef EM_TILEPRO
		case EM_TILEPRO: return "Tilera TILEPro";
#endif
#ifdef EM_MICROBLAZE
		case EM_MICROBLAZE: return "Xilinx MicroBlaze";
#endif
#ifdef EM_TILEGX
		case EM_TILEGX: return "Tilera TILE-Gx";
#endif
		default : return "?";
	}
}

char *osabi_type_string_map ( unsigned char type )
{
	switch ( type )
	{
#ifdef ELFOSABI_SYSV
		case ELFOSABI_SYSV: return "UNIX System V ABI";
#endif
#ifdef ELFOSABI_HPUX
		case ELFOSABI_HPUX: return "HP-UX";
#endif
#ifdef ELFOSABI_NETBSD
		case ELFOSABI_NETBSD: return "NetBSD.";
#endif
#ifdef ELFOSABI_GNU
		case ELFOSABI_GNU: return "Object uses GNU ELF extensions.";
#endif
#ifdef ELFOSABI_SOLARIS
		case ELFOSABI_SOLARIS: return "Sun Solaris.";
#endif
#ifdef ELFOSABI_AIX
		case ELFOSABI_AIX: return "IBM AIX.";
#endif
#ifdef ELFOSABI_IRIX
		case ELFOSABI_IRIX: return "SGI Irix.";
#endif
#ifdef ELFOSABI_FREEBSD
		case ELFOSABI_FREEBSD: return "FreeBSD.";
#endif
#ifdef ELFOSABI_TRU64
		case ELFOSABI_TRU64: return "Compaq TRU64 UNIX.";
#endif
#ifdef ELFOSABI_MODESTO
		case ELFOSABI_MODESTO: return "Novell Modesto.";
#endif
#ifdef ELFOSABI_OPENBSD
		case ELFOSABI_OPENBSD: return "OpenBSD.";
#endif
#ifdef ELFOSABI_ARM_AEABI
		case ELFOSABI_ARM_AEABI: return "ARM EABI";
#endif
#ifdef ELFOSABI_ARM
		case ELFOSABI_ARM: return "ARM";
#endif
#ifdef ELFOSABI_STANDALONE
		case ELFOSABI_STANDALONE: return "Standalone (embedded) application";
#endif
		default : return "?";
	}
}

char *rela_type_string_map ( Elf64_Half type )
{
	switch ( type )
	{
#ifdef R_X86_64_NONE
		case R_X86_64_NONE: return "No reloc";
#endif
#ifdef R_X86_64_64
		case R_X86_64_64: return "Direct 64 bit";
#endif
#ifdef R_X86_64_PC32
		case R_X86_64_PC32: return "PC relative 32 bit signed";
#endif
#ifdef R_X86_64_GOT32
		case R_X86_64_GOT32: return "32 bit GOT entry";
#endif
#ifdef R_X86_64_PLT32
		case R_X86_64_PLT32: return "32 bit PLT address";
#endif
#ifdef R_X86_64_COPY
		case R_X86_64_COPY: return "Copy symbol at runtime";
#endif
#ifdef R_X86_64_GLOB_DAT
		case R_X86_64_GLOB_DAT: return "Create GOT entry";
#endif
#ifdef R_X86_64_JUMP_SLOT
		case R_X86_64_JUMP_SLOT: return "Create PLT entry";
#endif
#ifdef R_X86_64_RELATIVE
		case R_X86_64_RELATIVE: return "Adjust by program base";
#endif
#ifdef R_X86_64_GOTPCREL
		case R_X86_64_GOTPCREL: return "32 bit signed PC relative offset to GOT";
#endif
#ifdef R_X86_64_32
		case R_X86_64_32: return "Direct 32 bit zero extended";
#endif
#ifdef R_X86_64_32S
		case R_X86_64_32S: return "Direct 32 bit sign extended";
#endif
#ifdef R_X86_64_16
		case R_X86_64_16: return "Direct 16 bit zero extended";
#endif
#ifdef R_X86_64_PC16
		case R_X86_64_PC16: return "16 bit sign extended pc relative";
#endif
#ifdef R_X86_64_8
		case R_X86_64_8: return "Direct 8 bit sign extended";
#endif
#ifdef R_X86_64_PC8
		case R_X86_64_PC8: return "8 bit sign extended pc relative";
#endif
#ifdef R_X86_64_DTPMOD64
		case R_X86_64_DTPMOD64: return "ID of module containing symbol";
#endif
#ifdef R_X86_64_DTPOFF64
		case R_X86_64_DTPOFF64: return "Offset in module's TLS block";
#endif
#ifdef R_X86_64_TPOFF64
		case R_X86_64_TPOFF64: return "Offset in initial TLS block";
#endif
#ifdef R_X86_64_TLSGD
		case R_X86_64_TLSGD: return "32 bit signed PC relative offset to two GOT entries for GD symbol";
#endif
#ifdef R_X86_64_TLSLD
		case R_X86_64_TLSLD: return "32 bit signed PC relative offset to two GOT entries for LD symbol";
#endif
#ifdef R_X86_64_DTPOFF32
		case R_X86_64_DTPOFF32: return "Offset in TLS block";
#endif
#ifdef R_X86_64_GOTTPOFF
		case R_X86_64_GOTTPOFF: return "32 bit signed PC relative offset to GOT entry for IE symbol";
#endif
#ifdef R_X86_64_TPOFF32
		case R_X86_64_TPOFF32: return "Offset in initial TLS block";
#endif
#ifdef R_X86_64_PC64
		case R_X86_64_PC64: return "PC relative 64 bit";
#endif
#ifdef R_X86_64_GOTOFF64
		case R_X86_64_GOTOFF64: return "64 bit offset to GOT";
#endif
#ifdef R_X86_64_GOTPC32
		case R_X86_64_GOTPC32: return "32 bit signed pc relative offset to GOT";
#endif
#ifdef R_X86_64_GOT64
		case R_X86_64_GOT64: return "64-bit GOT entry offset";
#endif
#ifdef R_X86_64_GOTPCREL64
		case R_X86_64_GOTPCREL64: return "64-bit PC relative offset to GOT entry";
#endif
#ifdef R_X86_64_GOTPC64
		case R_X86_64_GOTPC64: return "64-bit PC relative offset to GOT";
#endif
#ifdef R_X86_64_GOTPLT64
		case R_X86_64_GOTPLT64: return "like GOT64, says PLT entry needed";
#endif
#ifdef R_X86_64_PLTOFF64
		case R_X86_64_PLTOFF64: return "64-bit GOT relative offset to PLT entry";
#endif
#ifdef R_X86_64_SIZE32
		case R_X86_64_SIZE32: return "Size of symbol plus 32-bit addend";
#endif
#ifdef R_X86_64_SIZE64
		case R_X86_64_SIZE64: return "Size of symbol plus 64-bit addend";
#endif
#ifdef R_X86_64_GOTPC32_TLSDESC
		case R_X86_64_GOTPC32_TLSDESC: return "GOT offset for TLS descriptor.";
#endif
#ifdef R_X86_64_TLSDESC_CALL
		case R_X86_64_TLSDESC_CALL: return "Marker for call through TLS descriptor.";
#endif
#ifdef R_X86_64_TLSDESC
		case R_X86_64_TLSDESC: return "TLS descriptor.";
#endif
#ifdef R_X86_64_IRELATIVE
		case R_X86_64_IRELATIVE: return "Adjust indirectly by program base";
#endif
#ifdef R_X86_64_RELATIVE64
		case R_X86_64_RELATIVE64: return "64-bit adjust by program base";
#endif
		default : return "?";
	}
}
