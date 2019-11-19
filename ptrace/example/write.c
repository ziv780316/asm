#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

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
		str[i] = towupper( str[i] );
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
						printf( "[debugger] call write( %llx, %#10llx, %llx)\n", params[0], params[1], params[2] );

						// extract all regs
						ptrace( PTRACE_GETREGS, child_pid, NULL, &regs ); 
						printf( "[debugger] call write( %llx, %#10llx, %llx)\n", regs.rdi, regs.rsi, regs.rdx );
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

