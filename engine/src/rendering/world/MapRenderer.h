#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
POST_STD_LIB

#include "lib/BasicTypes.h"
#include "world/Map.h"

namespace Rendering {
	
	struct Context;

	namespace World {

		struct View {
			PositionVec3            eye    {};
			::World::Sector const * sector {nullptr};
			TransformMat2           forward{};
		};

		class MapRenderer {
		public:
			MapRenderer(Rendering::Context &ctx);
			~MapRenderer();

			void Render(View const &view);

		private:
			/**
				Render a single room, as described by view.

				renders on the screen pixels from minX to maxX, inclusive.

				portalDepth is the recursive depth to go to. negative values indicate infinity
			*/
			void RenderRoom(View const &view, int minX, int maxX, int portalDepth = -1);

			//////////
			// Data //
			//////////
		private:
			Rendering::Context &ctx;

			unsigned widthUsed;
			int *wallRenderableTop;
			int *wallRenderableBottom;

		};
	}
}