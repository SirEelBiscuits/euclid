PWD = $(shell pwd)

all: lua

lua:
	make -C lua

#TODO
# Make creation of the makefile a separate line, so we don't have to run ./configure every time?
SDL2: SDL2/bin/libSDL2-2.0.so

SDL2/Makefile:
	cd SDL2; ./configure --host=armv71-raspberry-linux-gnueabihf --disable-video-opengl --disable-pulseaudio --disable-esd --disable-video-mir --disable-video-wayland --disable-video-x11;

SDL2/bin/libSDL2-2.0.so: SDL2/Makefile
	cd SDL2; mkdir bin 2> /dev/null; cd bin; cmake ..
	make -C SDL2/bin

SDL2_image: SDL2_image/bin/libSDL2_image-2.0.so

SDL2_image/Makefile:
	$(eval SDL2_CONFIG=$(PWD)/SDL2/bin/sdl2-config)
	@echo "$(SDL2_CONFIG)"
	cd SDL2_image; ./configure --host=armv71-raspberry-linux-gnueabihf  CPPFLAGS="-I../SDL2/include" LDFLAGS="-L../SDL2/bin"; 
	
SDL2_image/bin/libSDL2_image-2.0.so: SDL2_image/Makefile
	mkdir bin 2> /dev/null; cd bin; 
	make -C SDL2_image

clean:
	make -C lua clean
	rm -rf SDL2/bin
	rm -rf SDL2_image/bin

.PHONY: lua SDL2 all clean SDL2_image
