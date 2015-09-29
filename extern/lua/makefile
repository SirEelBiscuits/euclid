# Generic makefile for C libraries

TARGET=lua
CC=gcc
SRC_DIR=src
OBJ_DIR=int
OUT_DIR=bin
C_FLAGS+=

SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst %.c, %.o, $(patsubst $(SRC_DIR)%, $(OBJ_DIR)%, $(SRC_FILES)))

all: $(TARGET)

$(TARGET): make_dirs $(OBJ_FILES)
	@echo "Building $(TARGET)"
	@ar rvs $(OUT_DIR)/lib$(TARGET).a `find $(OBJ_DIR) -name '*.o'` > /dev/null
	@echo "Done"

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@echo "Building $<"
	@$(CC) $(C_FLAGS) -c $< -o $@

make_dirs:
	@echo "creating directories"
	@find $(SRC_DIR) -type d | sed "s/$(SRC_DIR)/$(OBJ_DIR)/" | xargs mkdir -p
	@mkdir -p $(OUT_DIR)

clean:
	@echo "Cleaning intermediate files"
	@rm -rf $(OBJ_DIR)
	@echo "Cleaning output files"
	@rm -rf $(OUT_DIR)
	@echo "Done"

.PHONY: clean .depend make_dirs
