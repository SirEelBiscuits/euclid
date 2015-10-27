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
		bool ClipToView(btStorageType hFOVMult, PositionVec2 &wallStartVS, PositionVec2 &wallEndVS);

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

			for(auto x = 0u; x < ctx.GetWidth(); ++x) {
				wallRenderableTop[x] = 0;
				wallRenderableBottom[x] = ctx.GetHeight() - 1;
			}
			

			RenderRoom(view, 0, ctx.GetWidth());
		}

		void MapRenderer::RenderRoom(View const &view, int minX, int maxX, int portalDepth) {
			if(portalDepth == 0 || minX > maxX || view.sector == nullptr)
				return;
			auto &sec = *view.sector;

			for(auto wi = 0u; wi < sec.GetNumWalls(); ++wi) {
				auto &wall = *sec.GetWall(wi);
				auto &nextWall = *sec.GetWall((wi+1) % sec.GetNumWalls());

				auto wallStartVS = view.forward * (AsVec2(view.eye) - *nextWall.start);
				auto wallEndVS   = view.forward * (AsVec2(view.eye) - *wall.start);

				auto vFOVMult = ctx.GetVFOVMult();
				auto hFOVMult = ctx.GetHFOVMult();

				if(!ClipToView(hFOVMult, wallStartVS, wallEndVS))
					continue;

				//project
				wallStartVS.x /= wallStartVS.y.val;
				wallEndVS.x   /= wallEndVS.y.val;

				//cull backfacing
				//todo should this be > or >= ?
				if(wallStartVS.x > wallEndVS.x)
					continue;

				wallStartVS.x.val *= hFOVMult;
				wallEndVS.x.val   *= hFOVMult;
				
				auto ScreenHeight = ctx.GetHeight();
				auto ScreenWidth  = ctx.GetWidth();
				auto halfScreenWidth  = ScreenWidth/2;

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
				auto dWallTop    = (d * (wallTopEnd - wallTopStart)); //todo fixed point?
				auto wallTop     = (wallTopStart - dWallTop);
				auto dWallBottom = (d * (wallBottomEnd - wallBottomStart));
				auto wallBottom  = (wallBottomStart - dWallBottom);

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
							(int)wallTop, (int)wallBottom,
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
			wallBottom = Maths::min(wallBottom, viewSlotBottom);

			ctx.DrawVLine(x, wallTop, wallBottom, Rendering::Color{255, 0, 0, 0});
			return true;
		}

		bool ClipToView(btStorageType hFOVMult, PositionVec2 &start, PositionVec2 &end) {
			if(start.y < 0_m && end.y < 0_m)
				return false;

			/*
			for clip to left/right, we need to clip against O->[1,1] and O->[-1,1]
			intersection of two lines, one from O->e, one from a->b is at a point X

			hereon, ^ gives the length of the cross product
				that is: m ^ n = m.x n.y - m.y n.x

			X = se = a + t(b-a), for unknown s, and t
			q = b - a

			se = a + tq
			se ^ e = (a + tq) ^ e
					 0 =  a ^ e + t(q ^ e)
			e ^ a  = t(q ^ e)

			t = (e ^ a) / (q ^ e)

			for one clip plane, e = [1, 1] giving:
			t = (a.y - a.x) / (q.x - q.y)

			for the other, e = [-1, 1] giving:
			t = (-a.y - a.x) / (q.x + q.y)

			for a generic plane of e [ e.x, e.y]
			t = (e.x a.y - e.y a.x) / (q.x e.y - q.y e.x);

			note that if t is not in the range [0, 1] then the intersection is not within the line segment

			*/

			//the clip lines
			PositionVec2 const left (-1_m / hFOVMult, 1_m);
			PositionVec2 const right((1_m / hFOVMult) * 1.01f , 1_m); //width multiplier is a hack

			auto q           = end - start;
			//auto qu          = uEnd - uStart;
			auto denominator = (q ^ right);
			auto t           = MesiType<btStorageType, 0, 0, 0>(0.f);
			if(denominator.val != 0.f) {
				t = (right ^ start) / denominator;
				if( t.val > 0 && t.val < 1) {
					if((right ^ start).val >= 0 ) {
						end = start + t * q;
						//uEnd = uStart + t * qu;

						//q and qu become invalid when end or start change value
						q = end - start;
						//qu = uEnd - uStart;
					} else {
						start = start + t * q;
						//uStart = uStart + t * qu;
						q = end - start;
						//qu = uEnd - uStart;
					}
				} else if((right ^ start).val < 0 && (right ^ end).val < 0) {
					return false;
				}
			}

			denominator = (q ^ left);
			if(denominator.val != 0.f) {
				t = (left ^ start) / denominator;
				if( t.val > 0 && t.val < 1) {
					if((left ^ start).val > 0 ) {
						start = start + t * q;
						//uStart = uStart + t * qu;
					} else {
						end = start + t * q;
						//uEnd = uStart + t * qu;
					}
				} else if((left ^ start).val > 0 && (left ^ end).val > 0) {
					return false;
				}
			}

			//clip to near plane
			auto const nearDist = 0.001_m;
			if(start.y < 0_m) {
				auto p = (end.y - start.y) / (end.y - nearDist);
				start.y = nearDist;
				start.x = end.x + (start.x - end.x) / p;
			}
			if(end.y < 0_m) {
				auto p = (start.y - end.y) / (start.y - nearDist);
				end.y = nearDist;
				end.x = start.x + (end.x - start.x) / p;
			}

		return true;
		}
	}
}