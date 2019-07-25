.DEFAULT_GOAL=default 

CC=gcc
CFLAGS= -O0 -std=c99 -fPIC
TARGET=round

%: %.c 
	$(CC) -S $(CFLAGS) $< -o $@.s
	$(CC) -E $(CFLAGS) $< -o $@.i
	$(CC) $(CFLAGS) $< -o $@

default: $(TARGET)

cscope:
	find . -name "*.c" -o -name "*.h" > cscope.files 
	cscope -Rbq -i cscope.files
