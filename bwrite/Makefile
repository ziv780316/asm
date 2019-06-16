#=================================================
# Makefile settings
#=================================================
.PHONY: default create_dir clean # specify phony target, do not generate file as target name

.DEFAULT_GOAL=default # specify default target if there is no target specified

VPATH=library # specify directory where to check target file update to date or not

SHELL=/bin/bash

#=================================================
# Compiler settings
#=================================================
CC         = gcc
CPP        = g++
LINKER     = $(CC)
CPPLINKER  = $(CPP)
DEFINES    = 
BUILD_TYPE = -O2
CFLAGS     = $(BUILD_TYPE) -Wno-unused-parameter -Wfatal-errors -std=gnu99 -fPIC 
CPPFLAGS   = $(BUILD_TYPE) -Wno-unused-parameter -Wfatal-errors -std=c++98 -fPIC 
LIBS       = -lm 
INCLUDE    = -I.
MOVE       = mv -f

#=================================================
# Build target
#=================================================
BIN_DIR       	  := bin
LIB_DIR       	  := library
TARGET            := bwrite test 

#=================================================
# Compile implicit rules
#=================================================
%.o:%.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)
	@$(MOVE) $@ $(LIB_DIR) # @ means do not echo command

%.s:%.c
	$(CC) -S $< -o $@ $(CFLAGS) $(INCLUDE)
	@$(MOVE) $@ $(LIB_DIR) # @ means do not echo command

#=================================================
# Target rules
#=================================================
bwrite: bwrite.o opts.o 
	cd $(LIB_DIR);\
	$(LINKER) -o $@ $(notdir $^) $(LIBS);\
	$(MOVE) $@ ../$(BIN_DIR)

test: test.o test.s
	$(LINKER) -o $@ $(LIB_DIR)/$(notdir $<) $(LIBS)
	$(MOVE) $@ $(BIN_DIR)

#=================================================
# Phony rules
#=================================================
default: create_dir $(TARGET)

create_dir:
	@echo "create dir ..."
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(BIN_DIR)

clean:
	rm -rf $(LIB_DIR)/* $(BIN_DIR)/*

cscope:
	find . -name "*.c" -o -name "*.h" > cscope.files 
	cscope -Rbq -i cscope.files



