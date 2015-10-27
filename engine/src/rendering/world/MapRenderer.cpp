#include "MapRenderer.h"

#include "rendering/Textures.h"
#include "rendering/RenderingSystem.h"
#include "world/Map.h"

namespace Rendering {
	namespace World {
		//utility functions

		/**
			Draw a single slice of a wall
		*/
		bool DrawWallSlice(
			Rendering::Context &ctx,
			int x,                                    ///< screen x position
			int wallTop, int wallBottom,              ///< screen top and bottom positions
			btStorageType u,                          ///< texture u start
			btStorageType vStart, btStorageType vEnd, ///< texture v start and end
			int viewSlotTop, int viewSlotBottom,      ///< viewslot, will be used to clip the render
			Rendering::Texture *tex,                  ///< texture to use
			btStorageType colorScale,                 ///< used to darken the texture
			bool useAlpha = false                     ///< whether to use see-through rendering
		);

		MapRenderer::MapRenderer(Rendering::Context &ctx) 
			: ctx(ctx)
			, widthUsed(ctx.GetWidth())
			, wallRenderableTop(new int[widthUsed])
			, wallRenderableBottom(new int[widthUsed])
		{
		
		}

		MapRenderer::~MapRenderer() {
			delete[] wallRenderableTop;
			delete[] wallRenderableBottom;
		}

		void MapRenderer::Render(View const &view) {
			if(ctx.GetWidth() > widthUsed) {
				widthUsed = ctx.GetWidth();
				delete[] wallRenderableTop;
				delete[] wallRenderableBottom;
				wallRenderableTop = new int[widthUsed];
				wallRenderableBottom = new int[widthUsed];
			}

			RenderRoom(view, 0, ctx.GetWidth());
		}

		void MapRenderer::RenderRoom(View const &view, int minX, int maxX, int portalDepth) {
			if(portalDepth == 0 || minX > maxX)
				return;
			auto &sec = *view.sector;

			for(auto wi = 0u; wi < sec.GetNumWalls(); ++wi) {
				auto &wall = *sec.GetWall(wi);
				auto &nextWall = *sec.GetWall((wi+1) % sec.GetNumWalls());

				auto wallStartVS = view.forward * *wall.start;
				auto wallEndVS   = view.forward * *nextWall.start;

				//cull backfacing
				//todo should this be > or >= ?
				if(wallStartVS.x > wallEndVS.x)
					return;
				
				//project
				wallStartVS.x /= wallStartVS.y.val;
				wallEndVS.x   /= wallEndVS.y.val;
				
				auto ScreenHeight = ctx.GetHeight();
				auto ScreenWidth  = ctx.GetWidth();
				auto halfScreenWidth  = ScreenWidth/2;
				auto vFOVMult = ctx.GetVFOVMult();
				auto hFOVMult = ctx.GetHFOVMult();

				auto wallScalarStart = vFOVMult * ScreenHeight / wallStartVS.y;
				auto wallScalarEnd   = vFOVMult * ScreenHeight / wallEndVS.y;

				auto ceilHeight  = sec.ceilHeight - view.eye.z;
				auto floorHeight = sec.floorHeight - view.eye.z;

				//in pixels
				auto wallTopStart    = ScreenHeight/2 - ceilHeight * wallScalarStart;
				auto wallTopEnd      = ScreenHeight/2 - ceilHeight * wallScalarEnd;
				auto wallBottomStart = ScreenHeight/2 - floorHeight * wallScalarStart;
				auto wallBottomEnd   = ScreenHeight/2 - floorHeight * wallScalarEnd;

				auto wallStartSS = wallStartVS;
				wallStartSS.x.val = wallStartSS.x.val * halfScreenWidth + halfScreenWidth;
				auto wallEndSS = wallEndVS;
				wallEndSS.x.val   = wallEndSS.x.val * halfScreenWidth + halfScreenWidth;

				//initialise for inner loop

				unsigned const shift = 16;
				unsigned const shiftMult = 1<<shift;

				//treat the wall-space coordinate as 0-1, so one screen pixel is 1 / (width in pixels)
				auto d           = 1.0f / ((int)wallEndSS.x.val - (int)wallStartSS.x.val + 1);
				auto dWallTop    = (int)(d * (wallTopEnd - wallTopStart))       * shiftMult;
				auto wallTop     = (int)(wallTopStart - dWallTop)               * shiftMult;
				auto dWallBottom = (int)(d * (wallBottomEnd - wallBottomStart)) * shiftMult;
				auto wallBottom  = (int)(wallBottomStart - dWallBottom)         * shiftMult;

				for(auto x = static_cast<int>(wallStartSS.x.val); x < static_cast<int>(wallEndSS.x.val); ++x) {
					wallTop += dWallTop;
					wallBottom += dWallBottom;

					if(x < minX || x >= maxX) {
						continue;
					}

					{ // regular wall
						DrawWallSlice(
							ctx,
							x,
							wallTop >> shift, wallBottom >> shift,
							0, //U
							0, 0, //V0,Vmax
							wallRenderableTop[x], wallRenderableBottom[x],
							nullptr, //texToUse
							0 //invDist
						);

						wallRenderableTop[x] = ScreenHeight;
						wallRenderableBottom[x] = 0u;
					}
				}
			}
		}

		bool DrawWallSlice(
			Rendering::Context &ctx,
			int x,
			int wallTop, int wallBottom,
			btStorageType u,
			btStorageType vStart, btStorageType vEnd,
			int viewSlotTop, int viewSlotBottom,
			Rendering::Texture *tex,
			btStorageType colorScale,
			bool useAlpha /* = false */
		) {
			if(wallTop > viewSlotBottom || wallBottom < viewSlotTop)
				return false;

			wallTop = Maths::max(wallTop, viewSlotTop);
			wallBottom = Maths::max(wallBottom, viewSlotBottom);

			ctx.DrawVLine(x, wallTop, wallBottom, Rendering::Color{255, 0, 0, 0});
			return true;
		}
	}
}