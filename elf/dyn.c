
int g_dyn_static = 1;
int g_dyn_global = 2;

int f_dyn_local ( int x );

int f_dyn_export ( int x )
{
	return g_dyn_static;
}

int f_dyn_local ( int x )
{
	return x;
}
