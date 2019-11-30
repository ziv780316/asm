.DEFAULT_GOAL=default 

CC=gcc
LIB=-static -lm
CFLAGS= -O0 -std=c99 -fPIC
TARGET=fetest
PERF=perf stat --repeat 5 -e cycles,instructions,cache-misses,cache-references

%: %.c 
	$(CC) $(CFLAGS) -E $< -o $@.i 
	$(CC) $(CFLAGS) $< -o $@ $(LIB) 
	objdump -d $@ > $@.asm

default: $(TARGET)

cscope:
	find . -name "*.c" -o -name "*.h" > cscope.files 
	cscope -Rbq -i cscope.files

