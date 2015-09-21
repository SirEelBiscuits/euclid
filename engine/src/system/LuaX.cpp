#include "LuaX.h"

template<>
int luaX_returnglobal<int>(lua_State *s, char const *name) {
	lua_getglobal(s, name);
	auto ret = static_cast<int>(lua_tointeger(s, -1));
	lua_pop(s, 1);
	return ret;
}

template<>
float luaX_returnglobal<float>(lua_State *s, char const *name) {
	lua_getglobal(s, name);
	auto ret = static_cast<float>(lua_tonumber(s, -1));
	lua_pop(s, 1);
	return ret;
}

template<>
double luaX_returnglobal<double>(lua_State *s, char const *name) {
	lua_getglobal(s, name);
	auto ret = static_cast<double>(lua_tonumber(s, -1));
	lua_pop(s, 1);
	return ret;
}

template<>
bool luaX_returnglobal<bool>(lua_State *s, char const *name) {
	lua_getglobal(s, name);
	auto ret = !!lua_toboolean(s, -1);
	lua_pop(s, 1);
	return ret;
}

template<>
std::string luaX_returnglobal<std::string>(lua_State *s, char const *name) {
	if(LUA_TNIL == lua_getglobal(s, name))
		return "";
	auto ccp = lua_tostring(s, -1);
	auto ret = std::string(ccp);
	lua_pop(s, 1);
	return ret;
}

template<>
void luaX_push<int>(lua_State *s, int value) {
	lua_pushinteger(s, value);
}

template<>
void luaX_push<float>(lua_State *s, float value) {
	lua_pushnumber(s, static_cast<lua_Number>(value));
}

template<>
void luaX_push<double>(lua_State *s, double value) {
	lua_pushnumber(s, static_cast<lua_Number>(value));
}

template<>
void luaX_push<bool>(lua_State *s, bool value) {
	lua_pushboolean(s, value);
}

template<>
void luaX_push<char const*>(lua_State *s, char const *value) {
	lua_pushstring(s, value);
}

template<>
void luaX_push<std::string>(lua_State *s, std::string value) {
	lua_pushstring(s, value.c_str());
}

template<>
void luaX_push<luaX_emptytable>(lua_State *s, luaX_emptytable em) {
	lua_createtable(s, em.narr, em.nrec);
}

void luaX_showErrors(lua_State* s, char const *name, int errCode) {
	switch(errCode) {
	case LUA_OK:
		return;
	case LUA_ERRSYNTAX:
		printf("Syntax error in %s:\n", name);
		break;
	case LUA_ERRRUN:
		printf("Runtime error in %s:\n", name);
		break;
	case LUA_ERRMEM:
		printf("Memory error in %s: \n", name);
		break;
	case LUA_ERRGCMM:
		printf("Garbage collection error in %s (this is likely unrelated to the file):\n", name);
		break;
	case LUA_ERRFILE:
		printf("Error opening file %s:\n", name);
		break;
	default:
		printf("Unknown error (type: %d) in %s:\n", errCode, name);
		break;
	}
	printf("%s\n", lua_tostring(s, -1));
	lua_pop(s, 1);
}

bool luaX_dofile(lua_State *s, char const *filename) {
	auto err = luaL_loadfile(s, filename);
	if(err == LUA_OK) {
		err = lua_pcall(s, 0, 0, 0);
	}
	luaX_showErrors(s, filename, err);
	return err == LUA_OK;
}

int luaX_getglobal(lua_State *lua, char const *first) {
	lua_getglobal(lua, first);
	return 1;
}

int luaX_getlocal(lua_State *lua, char const *first) {
	lua_getfield(lua, -1, first);
	return 1;
}