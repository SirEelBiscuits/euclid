#include "TopDownMapRenderer.h"

#include "world/Map.h"
#include "rendering/RenderingSystem.h"

namespace Rendering {
	namespace World {
		void TopDownMapRenderer::Render(Rendering::Context & ctx, ::World::Map &map, View const & view, Rendering::Color c) {
			auto const scale = 5.f;

			auto Width = ctx.GetWidth();
			auto Height = ctx.GetHeight();
			PositionVec2 halfScreen(Mesi::Meters(Width/2.f), Mesi::Meters(Height/2.f));

			for(auto si = 0u; si < map.GetNumSectors(); ++si) {
				auto &sec = *map.GetSector(si);
				for(auto wi = 0u; wi < sec.GetNumWalls(); ++wi) {
					auto &wall = *sec.GetWall(wi);
					auto start = *wall.start;
					auto end = *(*sec.GetWall((wi+1) % sec.GetNumWalls())).start;

					start = scale * view.ToViewSpace(start);
					end   = scale * view.ToViewSpace(end);

					start.y = -start.y;
					end.y = -end.y;

					start += halfScreen;
					end   += halfScreen;

					ctx.DrawLine({(int)start.x.val, (int)start.y.val}, {(int)end.x.val, (int)end.y.val}, c);
				}
			}
		}
	}
}