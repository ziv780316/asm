#=================================================
# Makefile settings
#=================================================
.PHONY: default create_dir clean # specify phony target, do not generate file as target name

.DEFAULT_GOAL=default # specify default target if there is no target specified

VPATH=asm # specify directory where to check target file update to date or not

SHELL=/bin/bash

#=================================================
# Compiler settings
#=================================================
CC         = gcc
CPP        = g++
LINKER     = $(CC)
CPPLINKER  = $(CPP)
DEFINES    = 
BUILD_TYPE = -O0
CFLAGS     = $(BUILD_TYPE) -Wno-unused-parameter -Wfatal-errors -std=gnu99 -fPIC 
CPPFLAGS   = $(BUILD_TYPE) -Wno-unused-parameter -Wfatal-errors -std=c++98 -fPIC 
LIBS       = -lm 
INCLUDE    = 
MOVE       = mv -f

#=================================================
# Build target
#=================================================
ASM_DIR       	  := asm
BIN_DIR       	  := bin
TARGET            := func_args class vprintf

#=================================================
# Compile implicit rules
#=================================================
%.s:%.c
	@echo "build $(basename $@) ..."
	$(CC) -S $< -o $@ $(CFLAGS) $(INCLUDE)
	@$(MOVE) $@ $(ASM_DIR) # @ means do not echo command

%.s:%.cpp
	@echo "build $(basename $@) ..."
	$(CPP) -S $< -o $@ $(CPPFLAGS) $(INCLUDE)
	@$(MOVE) $@ $(ASM_DIR) # @ means do not echo command

#=================================================
# Target rules
#=================================================
func_args: func_args.s
	$(LINKER) -o $@ $(ASM_DIR)/$(notdir $<) $(LIBS);\
	$(MOVE) $@ $(BIN_DIR)

class: class.s
	$(CPPLINKER) -o $@ $(ASM_DIR)/$(notdir $<) $(LIBS);\
	$(MOVE) $@ $(BIN_DIR)

vprintf: vprintf.s
	$(LINKER) -o $@ $(ASM_DIR)/$(notdir $<) $(LIBS);\
	$(MOVE) $@ $(BIN_DIR)

#=================================================
# Phony rules
#=================================================
default: create_dir $(TARGET)

create_dir:
	@echo "create dir ..."
	@mkdir -p $(ASM_DIR)
	@mkdir -p $(BIN_DIR)

clean:
	rm -rf $(ASM_DIR)/* $(BIN_DIR)/*


