#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <string>
POST_STD_LIB

#include <lua.hpp>

template<typename T>
T luaX_getglobal(lua_State *s, char const *name);
template<typename T>
void luaX_setglobal(lua_State *s, char const *name, T value) { luaX_push(s, value); lua_setglobal(s, name); }
template<typename T>
void luaX_push(lua_State *s, T value);

bool luaX_dofile(lua_State *s, char const *filename);