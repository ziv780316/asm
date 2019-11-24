#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h> // definition ORIG_RAX, RAX
#include <sys/syscall.h> // definition SYS_write
#include <sys/user.h> // definition of user_regs_struct
#include <sys/personality.h> // set process execution doman (change program ABI), disable ASLR 

#define TRAP_MASK 0xffffffffffffff00
#define TRAP_INST 0xcc // trap instruction int3

void str_to_upper ( char *str )
{
	for ( int i = 0; str[i] != '\0' ; ++i )
	{
		str[i] = toupper( str[i] );
	}
}

void get_bytes ( pid_t child_pid, unsigned long addr, unsigned char *buf, unsigned long len )
{
	union 
	{
		long val;
		char qbytes[sizeof(long)];
	} data;

	int offset;
	int cnt = 0;
	int nq = len / sizeof(long);

	while ( cnt < nq )
	{
		offset = cnt * 8;
		data.val = ptrace( PTRACE_PEEKDATA, child_pid, addr + offset, NULL );
		memcpy( buf + offset, data.qbytes, sizeof(long) );
		++cnt;
	}

	if ( (len % sizeof(long)) != 0 )
	{
		offset = cnt * 8;
		data.val = ptrace( PTRACE_PEEKDATA, child_pid, addr + offset, NULL );
		memcpy( buf + offset, data.qbytes, (len % sizeof(long)) );
	}

}

void put_bytes ( pid_t child_pid, unsigned long addr, unsigned char *buf, unsigned long len )
{
	union 
	{
		long val;
		char qbytes[sizeof(long)];
	} data;

	int offset;
	int cnt = 0;
	int nq = len / sizeof(long);

	while ( cnt < nq )
	{
		offset = cnt * 8;
		memcpy( data.qbytes, buf + offset, sizeof(long) );
		ptrace( PTRACE_POKEDATA, child_pid, addr + offset, data.val );
		++cnt;
	}

	if ( (len % sizeof(long)) != 0 )
	{
		offset = cnt * 8;
		memcpy( data.qbytes, buf + offset, (len % sizeof(long)) );
		ptrace( PTRACE_POKEDATA, child_pid, addr + offset, data.val );
	}

}

int main ( int argc, char **argv )
{
	bool b_main = false;
	bool start_ni = false;
	bool start_main = false;
	int status;
	int insyscall = 0;
	long rax;
	long orig_rax;
	unsigned long instruction;
	unsigned long backup_inst;
	unsigned long main_addr;
	unsigned long long params[10];
	struct user_regs_struct regs;
	pid_t child_pid;

	fflush( stdout );
	child_pid = fork();
	if ( 0 == child_pid )
	{
		// child
		// disable ASLR (address space layout randomization)
		if ( -1 == personality( ADDR_NO_RANDOMIZE ) )
		{
			fprintf( stderr, "[Error] disable ASLR fail --> %s\n", strerror(errno) );
			abort();
		}

		// enable ptrace tracing
		ptrace( PTRACE_TRACEME, 0, NULL, NULL );

		// exec
		execl( "./bin/test", "ls", NULL );

	}
	else
	{
		// parent

		while( 1 ) 
		{
			waitpid( child_pid, &status, 0 );
			if ( WIFSTOPPED(status) ) // only true when child is traced and stop by get signal 
			{
				printf( "[debugger] child stop by signal %d\n", WSTOPSIG(status) );
				if ( SIGSEGV == WSTOPSIG(status) )
				{
					fprintf( stderr, "[Error] SIGSEGV\n" );
					abort();
				}

				if ( start_ni )
				{
					// step after main
					ptrace( PTRACE_GETREGS, child_pid, NULL, &regs ); 
					instruction = ptrace( PTRACE_PEEKTEXT, child_pid, regs.rip,  NULL ); 
					printf( "pc=%0#10lx inst=%0#10lx\n", (unsigned long) regs.rip, instruction );
					ptrace( PTRACE_CONT, child_pid, NULL, NULL ); 
				}
				else if ( !b_main )
				{
					// first trap, set break point on main
					b_main = true;
					main_addr = 0x400526;
					instruction = ptrace( PTRACE_PEEKTEXT, child_pid, main_addr );
					ptrace( PTRACE_POKETEXT, child_pid, main_addr, (instruction & TRAP_MASK) | TRAP_INST ); 
					backup_inst = instruction;
					ptrace( PTRACE_CONT, child_pid, NULL, NULL ); 

					printf( "original inst=%0#10lx\n", instruction );
					printf( "trap     inst=%0#10lx\n", (instruction & TRAP_MASK) | TRAP_INST );
				}
				else if ( !start_main )
				{
					// start at main due to break point set
					start_main = true;
					ptrace( PTRACE_GETREGS, child_pid, NULL, &regs ); 
					instruction = ptrace( PTRACE_PEEKTEXT, child_pid, main_addr );
					printf( "stop at main=%0#10lx rip=%0#10lx inst=%0#10lx\n", main_addr, (unsigned long) regs.rip, instruction );

					// roll-back instruction
					// since int3 replace orignal 1 instruction, so we need -1 rip and let CPU evaluate orignal instruction
					ptrace( PTRACE_POKETEXT, child_pid, main_addr, backup_inst );
					regs.rip -= 1;
					ptrace( PTRACE_SETREGS, child_pid, NULL, &regs ); 

					// check roll-back results
					ptrace( PTRACE_GETREGS, child_pid, NULL, &regs ); 
					instruction = ptrace( PTRACE_PEEKTEXT, child_pid, main_addr );
					printf( "roll-back   =%0#10lx rip=%0#10lx inst=%0#10lx\n", main_addr, (unsigned long) regs.rip, instruction );

					// ni
					ptrace( PTRACE_SINGLESTEP, child_pid, NULL, NULL ); 
					start_ni = true;
				}
				else
				{
					fprintf( stderr, "[Error] logic wrong\n" );
					abort();
				}

			}
			else if ( WIFEXITED( status) )
			{
				printf( "[debugger] child exit %d\n", WEXITSTATUS( status ) );
				break;
			}
			else
			{
				abort();
			}
		}
	}

	printf( "[debugger] done\n" );

	return EXIT_SUCCESS;
}


