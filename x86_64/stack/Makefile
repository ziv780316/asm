#=================================================
# Makefile settings
#=================================================
.PHONY: create_dir # specify phony target, do not generate file as target name

.DEFAULT_GOAL=default # specify default target if there is no target specified

VPATH=asm # specify directory where to check target file update to date or not

SHELL=/bin/bash

#=================================================
# Compiler settings
#=================================================
CC         = gcc
LINKER     = $(CC)
DEFINES    = 
BUILD_TYPE = -O0
CFLAGS     = $(BUILD_TYPE) -Wno-unused-parameter -Wfatal-errors -std=gnu99 -fPIC 
LIBS       = -lm 
INCLUDE    = 
MOVE       = mv -f

#=================================================
# Build target
#=================================================
ASM_DIR       	  := asm
BIN_DIR       	  := bin
TARGET            := func_args

#=================================================
# Compile implicit rules
#=================================================
%.s:%.c
	@echo "build $(basename $@) ..."
	$(CC) -S $< -o $@ $(CFLAGS) $(INCLUDE)
	@$(MOVE) $@ $(ASM_DIR) # @ means do not echo command

#=================================================
# Target rules
#=================================================
default: create_dir $(TARGET)

func_args: func_args.s
	$(LINKER) -o $@ $(ASM_DIR)/$(notdir $<) $(LIBS);\
	$(MOVE) $@ $(BIN_DIR)

create_dir:
	@mkdir -p $(ASM_DIR)
	@mkdir -p $(BIN_DIR)

clean:
	rm -rf $(ASM_DIR)/* $(BIN_DIR)/*


