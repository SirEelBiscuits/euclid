#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <string>
#include <memory>
#include <type_traits>
#include <functional>
#include <map>
#include <typeindex>
POST_STD_LIB

#include <lua.hpp>

#include "lib/TypeMagic.h"

#include "luaX/LuaXClasses.h"
#include "luaX/LuaXStack.h"
#include "luaX/LuaXClass.h"

/* if errCode is not LUA_ERROK, this will print the error at the top of the stack and pop it */
void luaX_showErrors(lua_State* s, char const *name, int errCode);

/* returns true on success. prints all error information available */
bool luaX_dofile(lua_State *s, char const *filename, int numReturns = 0);

/* calls a function and displays any error info */
int luaX_pcall(lua_State *s, int numArgs, int numReturns);
