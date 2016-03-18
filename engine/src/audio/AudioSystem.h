#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <memory>
POST_STD_LIB

namespace Audio {
	struct Context;

	std::unique_ptr<Context> GetContext();
	
	struct Context {
		Context() {}
		virtual ~Context() {}
	};
}