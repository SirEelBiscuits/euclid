# Generic makefile for C libraries

TARGET=lua
CC=gcc
SRC_DIR=src
OBJ_DIR=bin
OUT_DIR=.
C_FLAGS+=

SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst %.c, %.o, $(patsubst $(SRC_DIR)%, $(OBJ_DIR)%, $(SRC_FILES)))

all: $(TARGET)

$(TARGET): .depend $(OBJ_FILES)
	@echo "Building $(TARGET)"
	@ar rvs $(TARGET).a `find $(OBJ_DIR) -name '*.o'` > /dev/null
	@echo "Done"

$(OBJ_DIR)/%.depend : %.c
	@rm -f .depend
	echo "Building dependencies"
	@$(foreach var, $(SRC_FILES), $(CC) $(C_FLAGS) -MM $(var) >> .depend;)
	@echo "Dependencies done"

-include .depend

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@echo "Building $<"
	@$(CC) $(C_FLAGS) -c $< -o $@

clean:
	@echo "Cleaning intermediate files"
	@find $(OBJ_DIR) -name "*.o" -print0 | xargs -0 rm
	@echo "Cleaning output files"
	@rm $(OUT_DIR)/$(TARGET).a
	@echo "Done"

.PHONY: clean .depend
