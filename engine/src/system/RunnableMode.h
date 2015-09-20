#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
POST_STD_LIB

namespace Rendering {
	struct Context;
}

namespace System {
	class Config;

	class RunnableMode {
	public:
		RunnableMode(Rendering::Context &ctx, Config &config)
			: ctx(ctx)
			, cfg(config)
		{}
		virtual ~RunnableMode() {}

		virtual void Run() = 0;
		virtual bool Update() = 0;

	protected:
		Rendering::Context &ctx;
		Config& cfg;
	};
}