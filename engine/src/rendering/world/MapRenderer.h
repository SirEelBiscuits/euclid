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

			void DrawHorizontalPlanes(
				View view,
				int minX, int maxX,                                ///< x slot to render to, inclusive
				Mesi::Meters ceilHeight, Mesi::Meters floorHeight, ///< height of floor and ceiling to draw
				Rendering::Texture *floorTex,
				Rendering::Texture *ceilTex,
				float lightlevel,
				Rendering::Color tmpceil,
				Rendering::Color tmpfloor
			);

			//////////
			// Data //
			//////////
		private:
			Rendering::Context &ctx;

			unsigned widthUsed;
			unsigned heightUsed;
			int *wallRenderableTop;
			int *wallRenderableBottom;
			int *floorRenderableTop;
			int *floorRenderableBottom;
			int *ceilRenderableTop;
			int *ceilRenderableBottom;
			Mesi::Meters *distances;
		};
	}
}
