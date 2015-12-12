#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
POST_STD_LIB

#include "rendering/RenderingPrimitives.h"
#include "lib/BasicTypes.h"
#include "MapRenderingShared.h"

namespace Rendering {
	
	struct Context;
	class Texture;

	namespace World {

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
			void RenderRoom(View const &view, int minX, int maxX, int MaxPortalDepth = -1, int portalDepth = 0);

			void DrawHorizontalPlanes(
				View view,
				int minX, int maxX,                                ///< x slot to render to, inclusive
				Mesi::Meters ceilHeight, Mesi::Meters floorHeight, ///< height of floor and ceiling to draw
				Rendering::Texture *floorTex,
				Rendering::Texture *ceilTex,
				btStorageType lightlevel,
				Rendering::Color tmpceil,
				Rendering::Color tmpfloor,
				uint8_t stencil
			);

			void DrawSprites(
				View view,
				int minX, int maxX,
				btStorageType lightLevel,
				Mesi::Meters ceilHeight, Mesi::Meters floorHeight,
				int portalDepth
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
