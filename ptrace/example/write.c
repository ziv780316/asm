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
	int status;
	int insyscall = 0;
	long rax;
	long orig_rax;
	unsigned long long params[10];
	struct user_regs_struct regs;
	pid_t child_pid;

	fflush( stdout );
	child_pid = fork();
	if ( 0 == child_pid )
	{
		// child
		ptrace( PTRACE_TRACEME, 0, NULL, NULL );
		execl( "/bin/ls", "ls", NULL );

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

				orig_rax = ptrace( PTRACE_PEEKUSER, child_pid, 8 * ORIG_RAX, NULL ); // orig_rax store system call num
				if ( SYS_write == orig_rax )
				{
					if ( 0 == insyscall )
					{
						// enter syscall
						insyscall = 1;
						
						// extract function parameters by signle register
						params[0] = ptrace( PTRACE_PEEKUSER, child_pid, 8 * RDI, NULL ); 
						params[1] = ptrace( PTRACE_PEEKUSER, child_pid, 8 * RSI, NULL ); 
						params[2] = ptrace( PTRACE_PEEKUSER, child_pid, 8 * RDX, NULL ); 
						printf( "[debugger] call write( %ld, %#10llx, %ld)\n", (long) params[0], params[1], (long) params[2] );

						// extract all regs
						ptrace( PTRACE_GETREGS, child_pid, NULL, &regs ); 
						printf( "[debugger] call write( %ld, %#10llx, %ld)\n", (long) regs.rdi, regs.rsi, (long) regs.rdx );

						// get data
						char buf[BUFSIZ] = {0};
						get_bytes( child_pid, regs.rsi, buf, regs.rdx );
						printf( "[debugger] write str='%s'\n", buf );
						printf( "[debugger] change str to uppoer\n" );
						str_to_upper( buf );
						put_bytes( child_pid, regs.rsi, buf, regs.rdx );
					}
					else
					{
						// exit syscall
						insyscall = 0;

						// etract return value
						rax = ptrace( PTRACE_PEEKUSER, child_pid, 8 * RAX, NULL ); // extract general register and process info
						printf( "[debugger] write return %ld\n", rax );
					}
					printf( "[debugger] handle system call write\n" );
				}
				printf( "[debugger] child made a system call %ld\n", orig_rax );

				//ptrace( PTRACE_CONT, child_pid, NULL, NULL ); // continue until child exit
				ptrace( PTRACE_SYSCALL, child_pid, NULL, NULL ); // continue but stop at next system call enter or exit
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

