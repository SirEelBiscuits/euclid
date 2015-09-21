#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <string>
#include <memory>
#include <type_traits>
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

struct luaX_emptytable {
	int narr;
	int nrec;
};

luaX_State luaX_newstate();

template<
	typename T,
	typename = std::enable_if_t<!std::is_pointer<T>::value, T>
>
T luaX_returnglobal(lua_State *s, char const *name);

template<
	typename T,
	typename = std::enable_if_t<std::is_pointer<T>::value, T>
>
T* luaX_returnglobal(lua_State *s, char const *name) {
	lua_getglobal(s, name);
	auto ud = lua_touserdata(s, -1);
	lua_pop(lua, 1);
	return ud;
}

template<typename T>
void luaX_push(lua_State *s, T value);

template<typename T>
void luaX_push(lua_State *s, T *value) {
	lua_pushlightuserdata(s, value);
}

void luaX_showErrors(lua_State* s, char const *name, int errCode);

bool luaX_dofile(lua_State *s, char const *filename);

template<typename... Types>
int luaX_getglobal(lua_State *lua, char const *first, Types... rest) {
	luaX_getglobal(lua, first);
	if(lua_istable(lua, -1))
		return 1 + luaX_getlocal(lua, rest...);
	return 1;
}

template<typename... Types>
int luaX_getlocal(lua_State *lua, char const *first, Types... rest) {
	lua_getfield(lua, -1, first);
	if(lua_istable(lua, -1))
		return 1 + luaX_getlocal(lua, rest...);
	return 1;
}

int luaX_getglobal(lua_State *lua, char const *first);
int luaX_getlocal(lua_State *lua, char const *first);

template<typename... Args>
void luaX_createtable(lua_State *lua, int narr, int nrec, Args... args) {
	lua_createtable(lua, narr, nrec);
	luaX_settable(lua, args...);
}

template<typename... Args>
void luaX_createtable(lua_State *lua, Args... args) {
	lua_createtable(lua, 0, 0);
	luaX_settable(lua, args...);
}

template<typename T, typename... Args>
void luaX_settable(lua_State *lua, char const *firstKey, T val, Args... args) {
	luaX_settable(lua, firstKey, val);
	luaX_settable(lua, args...);
}

template<typename T>
void luaX_settable(lua_State *lua, char const *key, T val) {
	luaX_push(lua, val);
	lua_setfield(lua, -2, key);
}

template<typename T>
void luaX_setglobal(lua_State *s, char const *name, T value) { 
	luaX_push(s, value);
	lua_setglobal(s, name);
}

template<typename... Args>
void luaX_setglobal(lua_State* s, char const *name, Args... args) {
	lua_getglobal(s, name);
	if(lua_istable(s, -1))
		luaX_setlocal(s, vargs...);
}

template<typename... Args>
void luaX_setlocal(lua_State *lua, char const *firstKey, Args... args) {
	lua_getfield(lua, -1, firstKey);
	if(lua_istable(lua, -1))
		luaX_setlocalobj(lua, args);
}

//the fact this is identical to luaX_settable(lua_State, char const*, T) is a coincidence
// and they should remain separate(!)
template<typename T>
void luaX_setlocal(lua_State *lua, char const *key, T val) {
	luaX_push(val);
	lua_setfield(lua, -2, key);
}
