#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h> // definition ORIG_RAX, RAX
#include <sys/syscall.h> // definition SYS_write
#include <sys/user.h> // definition of user_regs_struct

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
	bool start_ni = false;
	int status;
	int insyscall = 0;
	long rax;
	long orig_rax;
	long instruction;
	unsigned long long params[10];
	struct user_regs_struct regs;
	pid_t child_pid;

	fflush( stdout );
	child_pid = fork();
	if ( 0 == child_pid )
	{
		// child
		ptrace( PTRACE_TRACEME, 0, NULL, NULL );
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
				if ( start_ni )
				{
					ptrace( PTRACE_GETREGS, child_pid, NULL, &regs ); 
					instruction = ptrace( PTRACE_PEEKTEXT, child_pid, regs.rip,  NULL ); 
					printf( "pc=%0#10lx inst=%0#10lx\n", (long) regs.rip, instruction );
					ptrace( PTRACE_SYSCALL, child_pid, NULL, NULL ); // continue but stop at next system call enter or exit
				}
				else
				{
					ptrace( PTRACE_SINGLESTEP, child_pid, NULL, NULL ); 
					start_ni = true;
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


