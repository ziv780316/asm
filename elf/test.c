#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

__attribute__((weak)) extern int f_weak ( int x );
__attribute__((weak)) extern int g_x_weak;

// weak symbol
extern int g_x_weak;
extern int g_x_test;

// local symbol
static int g_local; // this will not generate symbol
int g_local2;

// locate in .bss
int g_x_bss = 0;
int g_arr_bss[10] = {0};

// locate in .data
int g_x_data = 0xf5f6f7f8;
const int g_x_data_const = 0xf1fff3f4;
double g_f_arr[3] = { 0.001, 0.002, 0.003 };
double g_f = 1.234;

// locate in .rodata
char *global_str = "string in global";
char global_str_non[] = { 'n', 'o', 'n', '-', 'n', 'u', 'l', 'l', ' ', 't', 'e', 'r', 'm', 'i', 'n', 'a', 't', 'e' };
char g_ch = 'a';

// dynamic symbol
extern int g_dyn_global;
extern int g_dyn_pic_global;
extern int f_dyn_export ( int x );
extern int f_dyn_pic_export ( int x );
int f2 ( int x );

__attribute__((noinline)) 
int f1 ( int x )
{
	static int f1_sx = 1;
	int y = x + 1;
	char *local_str = "string in f1\nafter\tnewline";
	printf( "%d\n", f1_sx++ );
	return f2(y);
}

__attribute__((noinline)) 
int f2 ( int x )
{
	int y = f_weak( x ) + g_x_weak;
	char *local_str = "string in f2";
	return y;
}


// partial inline optimization
__attribute__((always_inline, optimize("partial-inlining")))
inline int f3 ( int x )
{
	if ( x > 3 )
	{
		return 0;
	}
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	x = f2(x);
	return x;
}

int main ( int argc, char **argv )
{
	double y = 1.234;
	double z;
	int x = 5;
	z = y * 2.345;
	x = f1( x );
	x = f3( x );
	x = f_dyn_export( x );
	x = f_dyn_pic_export( x );
	printf ( "%d %d %d\n", x, g_dyn_global, g_dyn_pic_global + (int)g_f );

	return EXIT_SUCCESS;
}

