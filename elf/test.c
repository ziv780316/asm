#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

__attribute__((weak)) extern int f_weak ( int x );
__attribute__((weak)) extern int g_x_weak;

// weak symbol
extern int g_x_weak;

// locate in .bss
int g_x_bss = 0;
int g_arr_bss[10] = {0};

// locate in .data
int g_x_data = 0xf5f6f7f8;
const int g_x_data_const = 0xf1f2f3f4;

// locate in .rodata
char *global_str = "string in global";
char global_str_non[] = { 'n', 'o', 'n', '-', 'n', 'u', 'l', 'l', ' ', 't', 'e', 'r', 'm', 'i', 'n', 'a', 't', 'e' };

__attribute__((noinline)) 
int f2 ( int x )
{
	int y = f_weak( x ) + g_x_weak;
	char *local_str = "string in f2";
	return y;
}

__attribute__((noinline)) 
int f1 ( int x )
{
	int y = x + 1;
	char *local_str = "string in f1\nafter\tnewline";
	return y;
}

int main ( int argc, char **argv )
{
	int x = 1;
	x = f1( x );
	printf ( "%d\n", x );

	return EXIT_SUCCESS;
}

