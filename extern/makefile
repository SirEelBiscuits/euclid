libs=lua SDL2

all: $(libs)

lua:
	make -C lua

SDL2:
	cd SDL2; mkdir bin 2> /dev/null; cd bin; cmake ..
	make -C SDL2/bin

.PHONY: lua SDL2 all
