
int g_dyn_static = 1;
int g_dyn_global = 2;

extern double g_f;
int f_dyn_local ( int x );

int f_dyn_export ( int x )
{
	return g_dyn_global + g_f;
}

int f_dyn_local ( int x )
{
	return x;
}
