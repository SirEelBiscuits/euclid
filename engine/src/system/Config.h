#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
POST_STD_LIB

#include "LuaX.h"

namespace System {

class Config {
public:
	Config();
	Config(char const* filename);
	~Config();

	template<typename T>
	T GetValue(char const* name) const {
		return luaX_getglobal<T>(s, name);
	};
	
	template<typename T>
	void SetValue(char const* name, T value) {
		luaX_setglobal(s, name, value);
	};

	bool IsValueSet(char const *name) const;

private:
	luaX_State s;
};

}