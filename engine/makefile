# Generic makefile for C libraries

TARGET=euclid
CXX=g++
SRC_DIR=src
OBJ_DIR=int
OUT_DIR=bin/rpi
INC_DIRS+=src ../extern/lua/src
#INC_DIRS+=../extern/SDL2/include ../extern/SDL2_image 
#LIB_DIRS+=../extern/SDL2/bin
LIB_DIRS+=../extern/lua/bin

C_FLAGS+= -std=c++14 -fpermissive -w -pthread -static-libstdc++ -O3
C_FLAGS+= $(patsubst %, -I%, $(INC_DIRS))
C_FLAGS+= $(shell sdl2-config --cflags)
LD_FLAGS+= -llua -lSDL2 -lSDL2main -lSDL2_image -ldl
LD_FLAGS+= $(patsubst %, -L%, $(LIB_DIRS))
LD_FLAGS+= $(shell sdl2-config --libs)

SRC_FILES = $(shell find $(SRC_DIR) -name '*.cpp' | grep -v SDLAndWindows)
OBJ_FILES := $(patsubst %.cpp, %.o, $(patsubst $(SRC_DIR)%, $(OBJ_DIR)%, $(SRC_FILES)))

all: $(TARGET)

check:
	@echo "$(C_FLAGS)"

make_dirs:
	@echo "creating directories"
	@find $(SRC_DIR) -type d | sed "s/$(SRC_DIR)/$(OBJ_DIR)/" | xargs mkdir -p
	@mkdir -p $(OUT_DIR)

$(TARGET): make_dirs $(OBJ_FILES)
	@echo "Building $(TARGET)"
	@$(CXX) $(C_FLAGS) $(LD_FLAGS) $(OBJ_FILES) $(LD_FLAGS) -o $(OUT_DIR)/$(TARGET)
	@echo "Done"

$(OBJ_DIR)/%.depend : %.cpp
	@rm -f .depend
	@echo "Building dependencies"
	@$(foreach var, $(SRC_FILES), $(CXX) $(C_FLAGS) -MM $(var) >> .depend;)
	@echo "Dependencies done"

-include .depend

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo "Building $<"
	@$(CXX) $(C_FLAGS) -c $< -o $@

clean:
	@echo "Cleaning intermediate files"
	@rm -rf $(OBJ_DIR)
	@echo "Cleaning output files"
	@rm -rf $(OUT_DIR)
	@echo "Done"

.PHONY: clean .depend check
