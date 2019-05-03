
void func (
	int i1,
	int i2,
	int i3,
	int i4,
	int i5,
	int i6,
	int i7,
	int i8,
	long int i9,
	long int i10,
	float f1,
	float f2,
	double d3,
	double d4,
	double d5,
	double d6,
	double d7,
	double d8,
	double d9,
	double d10,
	double d11,
	double d12,
	float f13,
	char *s1,
	char **s2
)
{
	int local_i1 = 1;
	char local_c1 = '1';
	char local_c2 = '2';

	return;
}

int main ( int argc, char **argv )
{
	char *local_s2 = "s2";

	int i1 = 1;
	int i2 = 2;
	int i3 = 3;
	int i4 = 4;
	int i5 = 5;
	int i6 = 6;
	int i7 = 7;
	int i8 = 8;
	long int i9 = 0x009fffffffffffffL;
	long int i10 = 0x010fffffffffffffL;
	float f1 = 1.0f;
	float f2 = 2.0f;
	double d3 = 3.1;
	double d4 = 4.1;
	double d5 = 5.1;
	double d6 = 6.1;
	double d7 = 7.1;
	double d8 = 8.1;
	double d9 = 9.1;
	double d10 = 10.1;
	double d11 = 11.1;
	double d12 = 12.1;
	float f13 = 13.1;
	char *s1 = "s1";
	char **s2 = &local_s2;

	func (
	i1,
	i2,
	i3,
	i4,
	i5,
	i6,
	i7,
	i8,
	i9,
	i10,
	f1,
	f2,
	d3,
	d4,
	d5,
	d6,
	d7,
	d8,
	d9,
	d10,
	d11,
	d12,
	f13,
	s1,
	s2
	);

	return 0;
}

