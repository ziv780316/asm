
int g_dyn_pic_static = 1;
int g_dyn_pic_global = 2;

int f_dyn_pic_local ( int x );

int f_dyn_pic_export ( int x )
{
	return g_dyn_pic_static;
}

int f_dyn_pic_local ( int x )
{
	return x;
}
