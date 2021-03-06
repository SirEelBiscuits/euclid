#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <memory>
POST_STD_LIB

#include "RenderingPrimitives.h"
#include "lib/BasicTypes.h"

#include "Textures.h"

namespace Rendering {
	struct Context {
		Context(unsigned Width, unsigned Height, unsigned Scale, bool shouldAllocateScreenBuffer);
		virtual ~Context();

		//inline because they need to be fast fast fast!
		unsigned       GetWidth()  const                         { return Width; };
		unsigned       GetHeight() const                         { return Height; };
		unsigned       GetWindowWidth()  const                   { return Width * Scale; };
		unsigned       GetWindowHeight() const                   { return Height * Scale; };
		unsigned       GetScale()  const                         { return Scale; };
		Color         &ScreenPixel(unsigned x, unsigned y)       { return screen[x + Width * y]; };
		Color         &ScreenPixel(ScreenVec2 pos)               { return ScreenPixel(pos.x, pos.y); };
		Color         &ScreenPixel_slow(unsigned x, unsigned y);
		Color         &ScreenPixel_slow(ScreenVec2 pos)          { return ScreenPixel_slow(pos.x, pos.y); };
		btStorageType &DepthPixel(unsigned x, unsigned y)        { return depth[x + Width * y]; };
		btStorageType &DepthPixel(ScreenVec2 pos)                { return DepthPixel(pos.x, pos.y); };
		btStorageType &DepthPixel_slow(unsigned x, unsigned y);
		btStorageType &DepthPixel_slow(ScreenVec2 pos)           { return DepthPixel_slow(pos.x, pos.y); };

		/**
			Clear the screen to the given colour
		 */
		virtual void Clear(Color c) = 0;
		/**
			Clear the depth buffer to the given depth
		 */
		virtual void ClearDepth() = 0;

		/**
			Set the resolution to the specified values
		*/
		virtual void SetResolution(unsigned Width, unsigned Height) = 0;

		virtual void SetRenderScale(unsigned Scale) = 0;

		/**
			Present the back buffer to the screen
		*/
		virtual void FlipBuffers() = 0;

		/////////////////
		//Draw commands//
		/////////////////
		//colours

		/** 
			Draw Vertical line.

			x, yTop, yBottom are inclusive screen co-ordinates
		*/
		void DrawVLine(unsigned x, unsigned yTop, unsigned yBottom, Color c);

		/**
			Draw Horizontal line.

			xLeft, xRight, y are inclusive screen co-ordinates
		*/
		void DrawHLine(unsigned xLeft, unsigned xRight, unsigned y, Color c, bool useAlpha = false);

		/**
			Draw a neat line

			start and end are inclusive screen co-ordinates
		*/
		void DrawLine(ScreenVec2 start, ScreenVec2 end, Color c);

		/**
			Draw a filled rectangle
		*/
		void DrawRect(ScreenRect dest, Color c, bool useAlpha = false);

		/**
			Draw a filled rectangle
			
			topLeft and bottomRight are inclusive screen co-ordinates
		*/
		void DrawRect(ScreenVec2 topLeft, ScreenVec2 bottomRight, Color c);

		/**
			Draw text using a bitmapped texture. The size the text appears is your problem

			todo: make a function which returns the size of a string when rendered
		*/
		void DrawText(ScreenVec2 topLeft, Texture *tex, char const* text);

		////////////
		//textures

		/**
			Draw horizontal line.
		*/
		void DrawHLine(
			unsigned xLeft,           // \ 
			unsigned xRight,          //  > Inclusive screen co-ordinates
			unsigned y,               // /
			Texture const *tex,       // texture to draw
			UVVec2 start, UVVec2 end, // inclusive texture co-ordinates
			btStorageType colorMult,  // will scale the color from the texture
			btStorageType stencil     // depth
		);
		
		/**
			Draw texture to screen rectangle
		 */
		void DrawRect(
			ScreenRect dest,         // screen rect to draw to
			Texture const *tex,      // texture to draw
			UVRect src,              // subtexture to be stretched over the region dest on screen
			btStorageType colorMult, // will scale the color from the texture
			btStorageType depth      // depth
		);

		/**
			Draw texture to screen rectangle, respecting the texture alpha.

		*/
		void DrawRectAlpha(
			ScreenRect dest,         // screen rect to draw to
			Texture const *tex,      // texture to draw
			UVRect src,              // subtexture to be stretched over the region dest on screen
			btStorageType colorMult, // will scale the color from the texture
			btStorageType depth      // depth
		);
		
		/**
			Draw a texture to screen rectangle, respecting the texture alpha, minx and maxx values,
			and only drawing over pixels whose stencil value is equal or greater than the listed value
		 */
		void DrawRectAlphaDepth( // TODO this is named badly
			ScreenRect dest,             // screen rect to draw to
			Texture const *tex,          // texture to draw
			UVRect src,                  // subtexture to be stretched over the region dest on screen
			btStorageType colorMult,     // will scale the color from the texture
			btStorageType depth          // depth
		);

		btStorageType GetVFOV() const { return vFOV; }
		btStorageType GetVFOVMult() const { return vFOVMult; };
		void SetvFov(btStorageType fovAngle) { vFOV = fovAngle; vFOVMult = tan(fovAngle * PI / 180); } //todo magic numbers and move to .cpp

		btStorageType GetHFOV() const { return hFOV; }
		btStorageType GetHFOVMult() const { return hFOVMult; };
		void SethFov(btStorageType fovAngle) { hFOV = fovAngle; hFOVMult = tan(fovAngle * PI / 180); } //todo magic numbers and move to .cpp

		void StartDebugRendering() { debugRendering = true; }

		//////////
		// DATA //
		//////////
	
	protected:
		Color         *screen{nullptr};
		btStorageType *depth {nullptr};
		unsigned       Height{0u};
		unsigned       Width {0u};
		unsigned       Scale {0u};

		btStorageType vFOV{45}, vFOVMult{1};
		btStorageType hFOV{45}, hFOVMult{1};

	private:
		//used to redirect bad writes to
		Color Dummy;
		float DummyDepth;

		bool debugRendering{false};
	};

	//This is to be implemented in platform-specific code
	/**
		Get a render context
	*/
	std::unique_ptr<Context> GetContext(
		unsigned Width,
		unsigned Height,
		unsigned Scale,
		bool FullScreen,
		char const *Title
	);
}