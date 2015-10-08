#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
POST_STD_LIB

#include "system/LuaX.h"
#include "system/Config.h"

namespace System {
	namespace Luaclid {
		
		void SetUp(lua_State *s, System::Config const& cfg);

	}
}
