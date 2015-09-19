#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <string>
#include <memory>
POST_STD_LIB

#include <lua.hpp>

#define LUAX_NEED_LIBS true

// custom smart pointer for incredibly stupid reasons
struct luaX_State {
	luaX_State() : s(luaL_newstate()) {}
	luaX_State(bool needLibs) : s(luaL_newstate()) { if(needLibs) luaL_openlibs(s); }
	~luaX_State() { lua_close(s); }

	lua_State *s;
	operator lua_State *() const {return s;}
};

luaX_State luaX_newstate();

template<typename T>
T luaX_getglobal(lua_State *s, char const *name);
template<typename T>
void luaX_setglobal(lua_State *s, char const *name, T value) { luaX_push(s, value); lua_setglobal(s, name); }
template<typename T>
void luaX_push(lua_State *s, T value);

void luaX_showErrors(lua_State* s, char const *name, int errCode);

bool luaX_dofile(lua_State *s, char const *filename);
