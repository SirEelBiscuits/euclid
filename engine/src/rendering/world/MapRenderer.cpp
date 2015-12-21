#include "MapRenderer.h"

PRE_STD_LIB
#include <algorithm>
POST_STD_LIB

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
			Fix16 u,                                  ///< texture u start
			Fix16 vStart, Fix16 vEnd,                 ///< texture v start and end
			int viewSlotTop, int viewSlotBottom,      ///< viewslot, will be used to clip the render
			Rendering::Texture *tex,                  ///< texture to use
			btStorageType colorScale,                 ///< used to darken the texture
			uint8_t stencil,
			bool useAlpha = false                     ///< whether to use see-through rendering
		);

		bool ClipToView(btStorageType hFOVMult, PositionVec2 &wallStartVS, PositionVec2 &wallEndVS, float &uStart, float &uEnd);

		btStorageType DepthMultFromDistance(Mesi::Meters distance, btStorageType ambientStrength);

		MapRenderer::MapRenderer(Rendering::Context &ctx) 
			: ctx(ctx)
			, widthUsed(ctx.GetWidth())
			, heightUsed(ctx.GetHeight())
			, wallRenderableTop(new int[widthUsed])
			, wallRenderableBottom(new int[widthUsed])
			, floorRenderableTop(new int[widthUsed])
			, floorRenderableBottom(new int[widthUsed])
			, ceilRenderableTop(new int[widthUsed])
			, ceilRenderableBottom(new int[widthUsed])
			, distances(new Mesi::Meters[heightUsed])
		{
		
		}

		MapRenderer::~MapRenderer() {
			delete[] wallRenderableTop;
			delete[] wallRenderableBottom;
			delete[] floorRenderableTop;
			delete[] floorRenderableBottom;
			delete[] ceilRenderableTop;
			delete[] ceilRenderableBottom;
			delete[] distances;
		}

		void MapRenderer::Render(View const &view) {
			if(ctx.GetWidth() > widthUsed) {
				widthUsed = ctx.GetWidth();
				delete[] wallRenderableTop;
				delete[] wallRenderableBottom;
				delete[] floorRenderableTop;
				delete[] floorRenderableBottom;
				delete[] ceilRenderableTop;
				delete[] ceilRenderableBottom;
				wallRenderableTop     = new int[widthUsed];
				wallRenderableBottom  = new int[widthUsed];
				floorRenderableTop    = new int[widthUsed];
				floorRenderableBottom = new int[widthUsed];
				ceilRenderableTop     = new int[widthUsed];
				ceilRenderableBottom  = new int[widthUsed];
			}
			if(ctx.GetHeight() > heightUsed) {
				heightUsed = ctx.GetHeight();
				delete[] distances;
				distances = new Mesi::Meters[heightUsed];
			}

			for(auto x = 0u; x < ctx.GetWidth(); ++x) {
				wallRenderableTop[x] = 0;
				wallRenderableBottom[x] = ctx.GetHeight() - 1;
				floorRenderableTop[x] = 0;
				floorRenderableBottom[x] = ctx.GetHeight() - 1;
				ceilRenderableTop[x] = 0;
				ceilRenderableBottom[x] = ctx.GetHeight() - 1;
			}
			//distances doesn't get zeroed here

			RenderRoom(view, 0, ctx.GetWidth() - 1);
		}

		void MapRenderer::RenderRoom(View const &view, int minX, int maxX, int MaxPortalDepth, int portalDepth) {
			if(portalDepth == MaxPortalDepth || minX > maxX || view.sector == nullptr)
				return;
			ASSERT(maxX < (int)widthUsed);

			struct RoomRenderDefer {
				View view;
				int minX, maxX;
			};
			auto deferList = std::vector<RoomRenderDefer>{};
			deferList.reserve(4);

			struct CurtainRenderDefer{
				int                 x;
				btStorageType       curtainTop;
				btStorageType       curtainBottom;
				Fix16               u;
				Fix16               vStart;
				Fix16               vEnd;
				int                 viewSlotTop;
				int                 viewSlotBottom;
				Rendering::Texture *tex;
				btStorageType       invDist;
			};
			auto curtainDeferList = std::vector<CurtainRenderDefer>();

			auto &sec = *view.sector;

			auto ceilHeight  = sec.ceilHeight - view.eye.z;
			auto floorHeight = sec.floorHeight - view.eye.z;

			for(auto wi = 0u; wi < sec.GetNumWalls(); ++wi) {
				auto &wall = *sec.GetWall(wi);
				auto &nextWall = *sec.GetWall((wi+1) % sec.GetNumWalls());

				auto wallStartVS = view.ToViewSpace(*nextWall.start);
				auto wallEndVS   = view.ToViewSpace(*wall.start);

				auto vFOVMult = ctx.GetVFOVMult();
				auto hFOVMult = ctx.GetHFOVMult();

				auto uStart     = 0.f;
				auto uEnd       = 1.f;
				auto vEndMain   = wall.mainTex.uvStart.y + Fix16((ceilHeight - floorHeight).val);
				auto vEndTop    = 0_fp;
				auto vEndBottom = 0_fp;

				if(!ClipToView(hFOVMult, wallStartVS, wallEndVS, uStart, uEnd))
					continue;

				uStart *= wall.length.val;
				uEnd   *= wall.length.val;

				auto isPortal = wall.portal != nullptr;
				auto toSector = wall.portal;
				auto wasPortalDrawn = false;

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

				//in pixels
				auto wallTopStart    = ScreenHeight/2 - ceilHeight * wallScalarStart;
				auto wallTopEnd      = ScreenHeight/2 - ceilHeight * wallScalarEnd;
				auto wallBottomStart = ScreenHeight/2 - floorHeight * wallScalarStart;
				auto wallBottomEnd   = ScreenHeight/2 - floorHeight * wallScalarEnd;

				auto wallStartSS = wallStartVS;
				wallStartSS.x.val = wallStartSS.x.val * halfScreenWidth + halfScreenWidth;
				auto wallEndSS = wallEndVS;
				wallEndSS.x.val   = wallEndSS.x.val * halfScreenWidth + halfScreenWidth;

				//neighbouring sector, if relevant
				auto nWallTopStart    = .0f;
				auto nWallTopEnd      = .0f;
				auto nWallBottomStart = .0f;
				auto nWallBottomEnd   = .0f;
				if(isPortal) {
					auto nCeilHeight = toSector->ceilHeight - view.eye.z;
					auto nFloorHeight = toSector->floorHeight - view.eye.z;
					nWallTopStart    = ScreenHeight/2 - nCeilHeight * wallScalarStart;
					nWallTopEnd      = ScreenHeight/2 - nCeilHeight * wallScalarEnd;
					nWallBottomStart = ScreenHeight/2 - nFloorHeight * wallScalarStart;
					nWallBottomEnd   = ScreenHeight/2 - nFloorHeight * wallScalarEnd;

					vEndTop    = wall.topTex.uvStart.y + Fix16(ceilHeight.val - nCeilHeight.val);
					vEndMain   = wall.mainTex.uvStart.y + Fix16( Maths::min(ceilHeight.val, nCeilHeight.val) - Maths::max(floorHeight.val, nFloorHeight.val) );
					vEndBottom = wall.bottomTex.uvStart.y + Fix16(nFloorHeight.val - floorHeight.val);
				}

				//initialise for inner loop

				unsigned const shift = 16;
				unsigned const shiftMult = 1<<shift;

				//treat the wall-space coordinate as 0-1, so one screen pixel is 1 / (width in pixels)
				auto d              = Mesi::Scalar(1) / ((int)wallEndSS.x.val - (int)wallStartSS.x.val/* + 1*/);
				auto dFix16         = Fix16(d);
				auto dWallTop       = d * (wallTopEnd - wallTopStart); //todo fixed point?
				auto wallTop        = wallTopStart - dWallTop;
				auto dWallBottom    = d * (wallBottomEnd - wallBottomStart);
				auto wallBottom     = wallBottomStart - dWallBottom;
				auto dPortalTop     = d * (nWallTopEnd - nWallTopStart);
				auto portalTop      = nWallTopStart - dPortalTop;
				auto dPortalBottom  = d * (nWallBottomEnd - nWallBottomStart);
				auto portalBottom   = nWallBottomStart - dPortalBottom;

				auto dU = dFix16 * Fix16(uEnd - uStart);
				auto uAcc = Fix16(uStart) - dU;
				
				auto TwoOverStartDist = Mesi::Scalar(2) / wallStartSS.y;
				auto TwoOverEndDist   = Mesi::Scalar(2) / wallEndSS.y;
				auto TwoOverStartDistFix16 = 2_fp / Fix16(wallStartSS.y.val);
				auto TwoOverEndDistFix16   = 2_fp / Fix16(wallEndSS.y.val);

				auto uCache = uStart;

				const int gap = 1 << 3;

				auto dist = wallStartSS.y;
				auto dDist = d * (wallEndSS.y - wallStartSS.y);
				auto distCache = dist;

				for(auto x = static_cast<int>(wallStartSS.x.val); x < static_cast<int>(wallEndSS.x.val) && x <= maxX; ++x) {
					wallTop += dWallTop;
					wallBottom += dWallBottom;
					portalTop += dPortalTop;
					portalBottom += dPortalBottom;

					if( (x - static_cast<int>(wallStartSS.x.val) & gap - 1) == 0 ) {
						// if x + gap results in alpha > 1, we get weird lighting artefacts
						auto gapToUse = 
							x + gap >= static_cast<int>(wallEndSS.x.val)
							? static_cast<int>(wallEndSS.x.val) - x
							: gap;
						auto alpha = d * (x + gapToUse - (int)wallStartSS.x.val);
						auto uNext = ((1.f - alpha) * uStart * TwoOverStartDist + uEnd * alpha * TwoOverEndDist)
							/ ((1.f - alpha) * TwoOverStartDist + alpha * TwoOverEndDist);

						dU = Fix16((uNext - uCache) / gapToUse);
						uAcc = Fix16(uCache);
						uCache = uNext;

						auto distNext = ((1 - alpha) * wallStartSS.y * TwoOverStartDist + wallEndSS.y * alpha * TwoOverEndDist)
							/ ((1 - alpha) * TwoOverStartDist + alpha * TwoOverEndDist);
						dDist = (distNext - distCache) / (btStorageType)gapToUse;
						dist = distCache;
						distCache = distNext;
					} else {
						uAcc += dU;
						dist += dDist;
					}

					auto depthShadeVal = DepthMultFromDistance(dist, sec.lightLevel);

					if(x < minX || x > maxX) {
						continue;
					}

					//this is the same for both types of wall, portal and non-
					ceilRenderableTop[x]     = static_cast<int>(Maths::max(wallRenderableTop[x], ceilRenderableTop[x]));
					ceilRenderableBottom[x]  = static_cast<int>(
						Maths::min(
							wallRenderableBottom[x],
							Maths::max(wallTop, wallRenderableTop[x]) - 1
						)
					);
					
					floorRenderableTop[x]    = static_cast<int>(
						Maths::max(
							wallRenderableTop[x],
							Maths::min(wallBottom, wallRenderableBottom[x]) + 1
						)
					);
					floorRenderableBottom[x] = static_cast<int>(Maths::min(wallRenderableBottom[x], floorRenderableBottom[x]));

					if(isPortal && !(portalTop > wallBottom) && !(portalBottom < wallTop)) {
						bool topDrawn{false}, bottomDrawn{false};
						if(wallTop < portalTop) {
							topDrawn = wall.topTex.tex != nullptr;
							DrawWallSlice(
								ctx,
								x,
								(int)wallTop, (int)portalTop-1,
								wall.topTex.uvStart.x + uAcc,
								wall.topTex.uvStart.y, vEndTop,
								wallRenderableTop[x], wallRenderableBottom[x],
								wall.topTex.tex,
								depthShadeVal,
								portalDepth
							);
						}

						if(wall.mainTex.tex != nullptr) {
							curtainDeferList.emplace_back(CurtainRenderDefer{
									x,
									Maths::max(portalTop, wallTop),
									Maths::min(portalBottom, wallBottom),
									wall.mainTex.uvStart.x + uAcc,
									wall.mainTex.uvStart.y,
									vEndMain,
									wallRenderableTop[x],
									wallRenderableBottom[x],
									wall.mainTex.tex,
									depthShadeVal
								}
							);
						}
						wasPortalDrawn = true;

						if(wallBottom > portalBottom) {
							bottomDrawn = wall.bottomTex.tex != nullptr;
							DrawWallSlice(
								ctx,
								x,
								(int)portalBottom + 1, (int)wallBottom,
								wall.bottomTex.uvStart.x + uAcc,
								wall.bottomTex.uvStart.y, vEndBottom,
								wallRenderableTop[x], wallRenderableBottom[x],
								wall.bottomTex.tex,
								depthShadeVal,
								portalDepth
							);
						}

						if(topDrawn)
							wallRenderableTop[x] = static_cast<int>(Maths::min(ScreenHeight-1,
								Maths::max(Maths::max(portalTop, wallTop), wallRenderableTop[x])
							));
						if(bottomDrawn)
							wallRenderableBottom[x] = static_cast<int>(Maths::max(0,
								Maths::min(Maths::min(portalBottom, wallBottom), wallRenderableBottom[x])
							));
					} else { // regular wall
						DrawWallSlice(
							ctx,
							x,
							(int)wallTop, (int)wallBottom,
							wall.mainTex.uvStart.x + uAcc,
							wall.mainTex.uvStart.y, vEndMain,
							wallRenderableTop[x], wallRenderableBottom[x],
							wall.mainTex.tex,
							depthShadeVal,
							portalDepth
						);

						wallRenderableTop[x] = ScreenHeight;
						wallRenderableBottom[x] = 0u;
					}
				}

				if(wasPortalDrawn) {
					auto v2 = view;
					v2.sector = toSector;
					deferList.emplace_back(
						RoomRenderDefer{
							v2,
							Maths::max(static_cast<int>(wallStartSS.x.val), minX),
							Maths::min(static_cast<int>(wallEndSS.x.val - 1), maxX) //wallEndSS is an exclusive bound, because reasons
						}
					);
				}
			}

			DrawHorizontalPlanes(
				view, 
				minX, maxX,
				ceilHeight, floorHeight,
				sec.floor.tex,
				sec.ceiling.tex,
				sec.lightLevel
				,Rendering::Color{0, 0, 128, 255}
				,Rendering::Color{0, 0,  64, 255}
				, portalDepth
			);
			auto numSprites = sec.barrow.GetNumSprites();
			for(auto &dl : deferList) {
				RenderRoom(dl.view, dl.minX, dl.maxX, MaxPortalDepth, portalDepth + 1);
			}
			for(auto &dl : curtainDeferList)
				DrawWallSlice(
					ctx,
					dl.x,
					(int)dl.curtainTop,
					(int)dl.curtainBottom,
					dl.u,
					dl.vStart,
					dl.vEnd,
					dl.viewSlotTop,
					dl.viewSlotBottom,
					dl.tex,
					dl.invDist,
					portalDepth,
					true
				);
			DrawSprites(view, minX, maxX, sec.lightLevel, ceilHeight, floorHeight, portalDepth);
			sec.barrow.SetNumSprites(numSprites);
			//ASSERT(sec.barrow.GetNumSprites() < 2);
		}

		bool DrawWallSlice(
			Rendering::Context &ctx,
			int x,
			int wallTop, int wallBottom,
			Fix16 u,
			Fix16 vStart, Fix16 vEnd,
			int viewSlotTop, int viewSlotBottom,
			Rendering::Texture *tex,
			btStorageType colorScale,
			uint8_t stencil,
			bool useAlpha /* = false */
		) {
			if(wallTop > viewSlotBottom || wallBottom < viewSlotTop)
				return false;

			if(tex != nullptr) {
				auto const ppm = Fix16(Rendering::Texture::PixelsPerMeter);
				auto srcR = Rendering::UVRect(UVVec2(u * ppm, vStart * ppm), UVVec2(1_fp, vEnd * ppm));

				if(wallBottom > viewSlotBottom) {
					auto height = wallBottom - wallTop;
					wallBottom = viewSlotBottom;
					auto newHeight = wallBottom - wallTop;
					srcR.size.y = srcR.size.y * Fix16((float)newHeight/height);
				}

				if(wallTop < viewSlotTop) {
					auto height = wallBottom - wallTop;
					wallTop = viewSlotTop;
					auto newHeight = wallBottom - wallTop;
					auto oldSrcH = srcR.size.y;
					srcR.size.y = srcR.size.y * Fix16((float)newHeight/height);
					srcR.pos.y += oldSrcH - srcR.size.y;
				}

				if((wallBottom - wallTop) + 1 > 0) {
					Rendering::ScreenRect dstR(ScreenVec2{x, wallTop}, ScreenVec2{1, (wallBottom - wallTop) + 1});
					if(useAlpha)
						ctx.DrawRectAlpha(dstR, tex, srcR, colorScale, stencil);
					else
						ctx.DrawRect(dstR, tex, srcR, colorScale, stencil);
				} else {
					return false;
				}
			}
			return true;
		}
		
		void MapRenderer::DrawHorizontalPlanes(
			View view,
			int minX, int maxX,
			Mesi::Meters ceilHeight, Mesi::Meters floorHeight,
			Rendering::Texture *floorTex,
			Rendering::Texture *ceilTex,
			btStorageType lightlevel,
			Rendering::Color tmpceil,
			Rendering::Color tmpfloor,
			uint8_t stencil
		) {
			auto const ScreenHeight = (int)ctx.GetHeight();
			for(auto y = 0; y < ScreenHeight; ++y)
				distances[y] = 0_m;

			auto vfovm = ctx.GetVFOVMult();

			//todo: is it really worth doing this double loop?
			//for(auto x = minX; x <= maxX; ++x) {
				for(auto y = 0; y <= ScreenHeight; ++y) {
					if(distances[y] == 0_m) {
						auto p = y - ScreenHeight / 2.f;
						auto pp = -p / (ScreenHeight * vfovm);
						if(p < 0)
							distances[y] = ceilHeight/pp;
						else
							distances[y] = floorHeight/pp;
					}
				}

		
			auto reverseT = view.forward.Inverse();
			auto f = [
				forwardVec = reverseT * PositionVec2(0_m, 1_m),
				rightVec   = reverseT * PositionVec2(1_m, 0_m),
				halfWidth = ctx.GetWidth() / 2,
				halfHeight = ScreenHeight / 2,
				hfovm = ctx.GetHFOVMult(),
				pos = AsVec2(view.eye)
			] (float x, Mesi::Meters distance) {
				x -= halfWidth;
				x /= halfWidth;
				x /= hfovm;
				x *= distance.val;
				return pos + forwardVec * distance.val + rightVec * x;
			};

			auto ppmFix16 = Fix16(Rendering::Texture::PixelsPerMeter);

			auto y = 0;
			if(ceilTex) {
				for(; y < ScreenHeight/2; ++y) {
					auto stripStarted = 0u;
					auto stripActive = false;
					auto depthMult = DepthMultFromDistance(distances[y], lightlevel);
					for(auto x = minX; x <= maxX; ++x) {
						if(ceilRenderableBottom[x] >= y && y >= ceilRenderableTop[x]) {
							if(stripActive == false) {
								stripActive = true;
								stripStarted = x;
							}
						} else if(stripActive) {
							auto left = f((float)stripStarted, distances[y]);
							auto right = f((float)x - 1.0f, distances[y]);
							auto leftFix16 = UVVec2(Fix16(left.x.val), Fix16(left.y.val)) * ppmFix16;
							auto rightFix16 = UVVec2(Fix16(right.x.val), Fix16(right.y.val)) * ppmFix16;
							ctx.DrawHLine(
								stripStarted, x - 1,
								y,
								ceilTex,
								UVVec2(leftFix16.x, leftFix16.y),
								UVVec2(rightFix16.x, rightFix16.y),
								depthMult,
								stencil
							);
							stripActive = false;
						}
					}
					if(stripActive) {
						auto left = f((float)stripStarted, distances[y]);
						auto right = f((float)maxX, distances[y]);
						auto leftFix16 = UVVec2(Fix16(left.x.val), Fix16(left.y.val)) * ppmFix16;
						auto rightFix16 = UVVec2(Fix16(right.x.val), Fix16(right.y.val)) * ppmFix16;
						ctx.DrawHLine(
							stripStarted, maxX,
							y,
							ceilTex,
							UVVec2(leftFix16.x, leftFix16.y),
							UVVec2(rightFix16.x,rightFix16.y),
							depthMult,
							stencil
						);
					}
				}
			} else {
				y = ScreenHeight / 2;
			}
			if(floorTex) {
				for(; y < ScreenHeight; ++y) {
					auto stripStarted = 0u;
					auto stripActive = false;
					auto depthMult = DepthMultFromDistance(distances[y], lightlevel);
					for(auto x = minX; x <= maxX; ++x) {
						if(floorRenderableBottom[x] >= y && y >= floorRenderableTop[x]) {
							if(stripActive == false) {
								stripActive = true;
								stripStarted = x;
							}
						} else if(stripActive) {
							auto left = f((float)stripStarted, distances[y]);
							auto right = f((float)x - 1.0f, distances[y]);
							auto leftFix16 = UVVec2(Fix16(left.x.val), Fix16(left.y.val)) * ppmFix16;
							auto rightFix16 = UVVec2(Fix16(right.x.val), Fix16(right.y.val)) * ppmFix16;
							ctx.DrawHLine(
								stripStarted, x - 1,
								y,
								floorTex,
								UVVec2(leftFix16.x, leftFix16.y),
								UVVec2(rightFix16.x,rightFix16.y),
								depthMult,
								stencil
							);
							stripActive = false;
						}
					}
					if(stripActive) {
						auto left = f((float)stripStarted, distances[y]);
						auto right = f((float)maxX, distances[y]);
						auto leftFix16 = UVVec2(Fix16(left.x.val), Fix16(left.y.val)) * ppmFix16;
						auto rightFix16 = UVVec2(Fix16(right.x.val), Fix16(right.y.val)) * ppmFix16;
						ctx.DrawHLine(
							stripStarted, maxX,
							y,
							floorTex,
							UVVec2(leftFix16.x, leftFix16.y),
							UVVec2(rightFix16.x,rightFix16.y),
							depthMult,
							stencil
						);
					}
				}
			} else {
				y = ScreenHeight / 2;
			}

			for(auto x = minX; x <= maxX; ++x) {
				ceilRenderableTop[x] = ceilRenderableBottom[x];
				if(ceilTex != nullptr)
					wallRenderableTop[x] = Maths::max(wallRenderableTop[x], ceilRenderableTop[x]);
				floorRenderableBottom[x] = floorRenderableTop[x] - 1;
				if(floorTex != nullptr)
					wallRenderableBottom[x] = Maths::min(wallRenderableBottom[x], floorRenderableTop[x] - 1);
			}
		}

		void MapRenderer::DrawSprites(
			View view,
			int minX, int maxX,
			btStorageType lightLevel,
			Mesi::Meters ceilHeight, Mesi::Meters floorHeight,
			int portalDepth
		) {
			auto sprites = view.sector->barrow.GetSprites();
			
			auto vFOVMult = ctx.GetVFOVMult();
			auto hFOVMult = ctx.GetHFOVMult();
			auto ScreenHeight = ctx.GetHeight();
			auto ScreenWidth = ctx.GetWidth();

			std::sort(sprites.begin(), sprites.end(), [view](::World::Sprite *a, ::World::Sprite *b) {
				auto u = a->position - view.eye;
				auto v = b->position - view.eye;
				return u.LengthSquared() > v.LengthSquared();
			} );

			for(auto &sprite : sprites) {
				if(sprite->tex == nullptr)
					continue;
				auto height          = Mesi::Meters(static_cast<btStorageType>(sprite->tex->h / (btStorageType)Texture::PixelsPerMeter));
				auto width           = Mesi::Meters(static_cast<btStorageType>(sprite->tex->w / (btStorageType)Texture::PixelsPerMeter));
				auto posVS           = view.ToViewSpace(sprite->position);
				if(posVS.y < 0.001_m)
					continue;

				auto texSourceRect   = Rendering::UVRect(
					{0_fp, 0_fp}, 
					{Fix16(sprite->tex->w), Fix16(sprite->tex->h)}
				);
				auto xLeft  = posVS.x - width / 2.0f;
				auto xRight = posVS.x + width / 2.0f;

				//Need to check if the sprite crosses any boundaries.
				//Both left and right might hit a wall/portal
				//If they hit something, truncate the sprite
				//If they hit a portal, dupe the sprite into that sector
				for(auto i = 0u; i < view.sector->GetNumWalls(); ++i) {
					auto start = view.ToViewSpace(*view.sector->GetWall(i)->start);
					auto end   = view.ToViewSpace(*view.sector->GetWall((i+1) % view.sector->GetNumWalls())->start);

					//If the y crosses like this, then the wall is a possible intersect - but not definite!
					if((start.y < posVS.y) != (end.y < posVS.y)) {
						auto xIntersect = start.x
							+ (end.x - start.x) * (posVS.y - start.y) / (end.y - start.y);
						
						if(xIntersect > xLeft && xIntersect < xRight ) {
							float dummy;
							//an intersection means part of the sprite might need to be drawn on the far side of the portal,
							if(
								ClipToView(hFOVMult, start, end, dummy, dummy)
								&& (end.x / end.y > start.x / start.y || sprite->GetSector() != view.sector)
							) {
								auto toSec = view.sector->GetWall(i)->portal;
								toSec->barrow.AddSprite(*sprite);
								if(((end - start) ^ (PositionVec2(xLeft, posVS.y) - start)) < 0_m2) {
									// left end to move
									texSourceRect.pos.x  = Fix16((xIntersect - xLeft).val * Texture::PixelsPerMeter);
									texSourceRect.size.x = Fix16((xRight - xIntersect).val * Texture::PixelsPerMeter);
									xLeft = xIntersect;
								} else {
									// right end to move
									texSourceRect.size.x = Fix16((xIntersect - xLeft).val * Texture::PixelsPerMeter);
									xRight = xIntersect;
								}
							}
						}
					}
				}

				auto posSS = posVS;
				posSS.x.val = (posVS.x.val / posSS.y.val) * hFOVMult * ScreenWidth / 2 + ScreenWidth / 2;
				auto distScaleV      = vFOVMult * ScreenHeight / posVS.y;
				auto distScaleH      = hFOVMult * ScreenWidth  / posVS.y / 2.0f; // it horrifies me that I don't know why horizontal needs /2 but vertical doesn't
				auto spritebottom    = ScreenHeight/2 - posVS.z * distScaleV;
				auto spritetop       = ScreenHeight/2 - (posVS.z + height) * distScaleV;
				auto texDestRect     = Rendering::ScreenRect(
					{int(posSS.x.val + (xLeft - posVS.x)*distScaleH), int(spritetop)},
					{int((xRight - xLeft) * distScaleH + 1), int(spritebottom - spritetop)} //TODO this +1 is a hack
				);

				if(texDestRect.size.x > 0 && texDestRect.size.y > 0)
					ctx.DrawRectAlphaDepth(
						texDestRect,
						sprite->tex,
						texSourceRect,
						sprite->GetSector()->lightLevel,
						portalDepth,
						minX,
						maxX
					);
			}
		}

		bool ClipToView(btStorageType hFOVMult, PositionVec2 &start, PositionVec2 &end, float &uStart, float &uEnd) {
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
			auto qu          = uEnd - uStart;
			auto denominator = (q ^ right);
			auto t           = MesiType<btStorageType, 0, 0, 0>(0.f);
			if(denominator.val != 0.f) {
				t = (right ^ start) / denominator;
				if( t.val > 0 && t.val < 1) {
					if((right ^ start).val >= 0 ) {
						end = start + t * q;
						uEnd = uStart + t * qu;

						//q and qu become invalid when end or start change value
						q = end - start;
						qu = uEnd - uStart;
					} else {
						start = start + t * q;
						uStart = uStart + t * qu;
						q = end - start;
						qu = uEnd - uStart;
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
						uStart = uStart + t * qu;
					} else {
						end = start + t * q;
						uEnd = uStart + t * qu;
					}
				} else if((left ^ start).val > 0 && (left ^ end).val > 0) {
					return false;
				}
			}

			//clip to near plane
			auto const nearDist = 0.00001_m;
			if(start.y < nearDist) {
				auto p = (end.y - start.y) / (end.y - nearDist);
				start.y = nearDist;
				start.x = end.x + (start.x - end.x) / p;
			}
			if(end.y < nearDist) {
				auto p = (start.y - end.y) / (start.y - nearDist);
				end.y = nearDist;
				end.x = start.x + (end.x - start.x) / p;
			}

		return true;
		}

		btStorageType DepthMultFromDistance(Mesi::Meters distance, btStorageType ambientLight) {
			if(distance < 0.001_m)
				return 0.9f * (1 - ambientLight) + ambientLight;
			return Maths::clamp(
				.5f / distance.val,
				ambientLight,
				0.9f * (1 - ambientLight) + ambientLight);
		}
	}
}
