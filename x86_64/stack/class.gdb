# load binary
file bin/class

# basic gdb setting
set pagination off 

# * means stop before push %rbp
b test::test

run

set variable $this=$rdi

printf "this = %#x\n", $this


