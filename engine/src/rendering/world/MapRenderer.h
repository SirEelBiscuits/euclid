#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <vector>
POST_STD_LIB

#include "rendering/RenderingPrimitives.h"
#include "lib/BasicTypes.h"
#include "MapRenderingShared.h"

namespace World {
	class Sprite;
}

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
				Rendering::Color tmpfloor
			);

			void AddDeferedSpriteDraw(
				View view,
				btStorageType lightLevel,
				::World::Sprite *sprite
			);

			void CollectSprites(
				View view,
				btStorageType lightLevel
			);

			void DrawSprites();

			//////////
			// Data //
			//////////
		private:
			Rendering::Context &ctx;

			std::vector<int> wallRenderableTop{};
			std::vector<int> wallRenderableBottom{};
			std::vector<int> floorRenderableTop{};
			std::vector<int> floorRenderableBottom{};
			std::vector<int> ceilRenderableTop{};
			std::vector<int> ceilRenderableBottom{};
			std::vector<Mesi::Meters> distances{};


			struct SpriteDefer {
				SpriteDefer(View v, btStorageType ll, ::World::Sprite *s) : view(v), lightLevel(ll), sprite(s) {}
				View view;
				btStorageType lightLevel;
				::World::Sprite *sprite;
			};

			class SpriteDeferCompare {
			public:
				bool operator()(SpriteDefer const a, SpriteDefer const b) const {
					return a.sprite < b.sprite || (a.sprite == b.sprite &&
						a.view.sector < b.view.sector || (a.view.sector == b.view.sector &&
						a.lightLevel < b.lightLevel || ( a.lightLevel == b.lightLevel &&
						a.view.eye.x < b.view.eye.x || ( a.view.eye.x == b.view.eye.x &&
						a.view.eye.y < b.view.eye.y || ( a.view.eye.y == b.view.eye.y &&
						a.view.eye.z < b.view.eye.z
					)))))
					;
				}
			};

			std::vector<SpriteDefer> spriteDefers;
		};
	}
}
