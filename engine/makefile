# Generic makefile for C libraries
# TODO add include directories, library directories, and libraries

TARGET=euclid
CXX=g++
SRC_DIR=src
OBJ_DIR=int
OUT_DIR=bin
INC_DIRS+=src ../extern/SDL2/include ../extern/lua/src
LIB_DIRS+=../extern/SDL2/bin ../extern/lua/

C_FLAGS+= -std=c++14 -fpermissive -w -pthread
C_FLAGS+= $(patsubst %, -I%, $(INC_DIRS))
LD_FLAGS+= -llua -lSDL2 -lSDL2main -ldl
LD_FLAGS+= $(patsubst %, -L%, $(LIB_DIRS))

SRC_FILES = $(shell find $(SRC_DIR) -name '*.cpp')
OBJ_FILES := $(patsubst %.cpp, %.o, $(patsubst $(SRC_DIR)%, $(OBJ_DIR)%, $(SRC_FILES)))

all: $(TARGET)

$(TARGET): .depend $(OBJ_FILES)
	@echo "Building $(TARGET)"
	@$(CXX) $(C_FLAGS) $(LD_FLAGS) $(OBJ_FILES) $(LD_FLAGS) -o $(OUT_DIR)/$(TARGET)
	@echo "Done"

$(OBJ_DIR)/%.depend : %.cpp
	@rm -f .depend
	echo "Building dependencies"
	@$(foreach var, $(SRC_FILES), $(CXX) $(C_FLAGS) -MM $(var) >> .depend;)
	@echo "Dependencies done"

-include .depend

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo "Building $<"
	@$(CXX) $(C_FLAGS) -c $< -o $@

clean:
	@echo "Cleaning intermediate files"
	@find $(OBJ_DIR) -name "*.o" -print0 | xargs -0 rm
	@echo "Cleaning output files"
	@rm $(OUT_DIR)/$(TARGET).a
	@echo "Done"

.PHONY: clean .depend
