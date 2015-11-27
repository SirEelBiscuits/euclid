#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
POST_STD_LIB

#include "rendering/RenderingPrimitives.h"
#include "MapRenderingShared.h"

namespace World {
	class Map;
}

namespace Rendering {
	struct Context;
	namespace World {
		class TopDownMapRenderer {
		public:
			static void Render(Rendering::Context &ctx, ::World::Map &map, View const &view, Rendering::Color c);
		};
	}
}