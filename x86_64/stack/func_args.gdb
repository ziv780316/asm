# load binary
file bin/func_args

# basic gdb setting
set pagination off 

# * means stop before push %rbp
b *func

run

# fetch all arguments, (long*) means dereference 8 byte, (int *) means deference 4 byte store in $rsp address
set variable $ret_addr=(*(long*)($rsp))
set variable $i1=$rdi
set variable $i2=$rsi
set variable $i3=$rdx
set variable $i4=$rcx
set variable $i5=$r8
set variable $i6=$r9
set variable $i7=(*(int*)($rsp + 8*1))
set variable $i8=(*(int*)($rsp + 8*2))
set variable $i9=(*(long*)($rsp + 8*3))
set variable $i10=(*(long*)($rsp + 8*4))
set variable $f1=$xmm0.v4_float[0]
set variable $f2=$xmm1.v4_float[0]
set variable $d3=$xmm2.v2_double[0]
set variable $d4=$xmm3.v2_double[0]
set variable $d5=$xmm4.v2_double[0]
set variable $d6=$xmm5.v2_double[0]
set variable $d7=$xmm6.v2_double[0]
set variable $d8=$xmm7.v2_double[0]
set variable $d9=(*(long*)($rsp + 8*5))
set variable $d10=(*(long*)($rsp + 8*6))
set variable $d11=(*(long*)($rsp + 8*7))
set variable $d12=(*(long*)($rsp + 8*8))
set variable $f13=(*(int*)($rsp + 8*9))
set variable $s1=(*(long*)($rsp + 8*10))
set variable $s2=((*(long*)*(long *)($rsp + 8*11)))

printf "ret_addr  = %x\n", $ret_addr
printf "i1        = %d\n", $i1
printf "i2        = %d\n", $i2
printf "i3        = %d\n", $i3
printf "i4        = %d\n", $i4
printf "i5        = %d\n", $i5
printf "i6        = %d\n", $i6
printf "i7        = %d\n", $i7
printf "i8        = %d\n", $i8
printf "i9        = %lx\n", $i9
printf "i10       = %lx\n", $i10
printf "f1        = %f\n", $f1
printf "f2        = %f\n", $f2
printf "d3        = %f\n", $d3
printf "d4        = %f\n", $d4
printf "d5        = %f\n", $d5
printf "d6        = %f\n", $d6
printf "d7        = %f\n", $d7
printf "d8        = %f\n", $d8
printf "d9        = %f\n", $d9
printf "d10       = %f\n", $d10
printf "d11       = %f\n", $d11
printf "d12       = %f\n", $d12
printf "f13       = %f\n", $f13
printf "s1        = %s\n", $s1
printf "s2        = %s\n", $s2

#void func (
#	int i1,
#	int i2,
#	int i3,
#	int i4,
#	int i5,
#	int i6,
#	int i7,
#	int i8,
#	long long int i9,
#	long long int i10,
#	float f1,
#	float f2,
#	double d3,
#	double d4,
#	double d5,
#	double d6,
#	char *s1,
#	char **s2
#)

