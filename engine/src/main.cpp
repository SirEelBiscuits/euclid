#include <SDL.h>

#include <iostream>
#include <cstdlib>

#include <lua.hpp>

int main(int argc, char* argv[]) {
	SDL_Event e;
	SDL_PollEvent(&e);


	auto test = luaL_newstate();

	std::cout << "hello world" << test << std::endl;
	return EXIT_SUCCESS;
}