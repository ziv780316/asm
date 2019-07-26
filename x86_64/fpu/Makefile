.DEFAULT_GOAL=default 

CC=gcc
CFLAGS= -O0 -std=c99 -fPIC
TARGET=round speed_fpu speed_sse
PERF=perf stat --repeat 5 -e cycles,instructions,cache-misses,cache-references

%: %.c 
	$(CC) -S $(CFLAGS) $< -o $@.s
	$(CC) -E $(CFLAGS) $< -o $@.i
	$(CC) $(CFLAGS) $< -o $@

default: $(TARGET)

cscope:
	find . -name "*.c" -o -name "*.h" > cscope.files 
	cscope -Rbq -i cscope.files

bench:
	$(PERF) ./speed_fpu > /dev/null
	$(PERF) ./speed_sse > /dev/null
