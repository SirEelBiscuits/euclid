
all: lua 

lua:
	make -C lua

SDL2:
	cd SDL2; mkdir bin 2> /dev/null; cd bin; cmake ..
	#Apparently the below might be needed?
	#./configure --host=armv71-raspberry-linux-gnueabihf --disable-video-opengl
	make -C SDL2/bin

SDL2_image:
	cd SDL2_image; mkdir bin 2> /dev/null; cd bin; cmake ..
	make -C SDL2_image/bin

clean:
	make -C lua clean
	rm -rf SDL2/bin
	rm -rf SDL2_image/bin

.PHONY: lua SDL2 all clean SDL2_image
