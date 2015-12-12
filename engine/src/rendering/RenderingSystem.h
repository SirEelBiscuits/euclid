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
		unsigned  GetWidth()  const                         { return Width; };
		unsigned  GetHeight() const                         { return Height; };
		unsigned  GetWindowWidth()  const                   { return Width * Scale; };
		unsigned  GetWindowHeight() const                   { return Height * Scale; };
		unsigned  GetScale()  const                         { return Scale; };
		Color    &ScreenPixel(unsigned x, unsigned y)       { return screen[x + Width * y]; };
		Color    &ScreenPixel(ScreenVec2 pos)               { return ScreenPixel(pos.x, pos.y); };
		Color    &ScreenPixel_slow(unsigned x, unsigned y);
		Color    &ScreenPixel_slow(ScreenVec2 pos)          { return ScreenPixel_slow(pos.x, pos.y); };

		/**
			Clear the screen to the given colour
		 */
		virtual void Clear(Color c) = 0;

		/**
			Set the resolution to the specified values
		*/
		virtual void SetResolution(unsigned Width, unsigned Height) = 0;

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
		void DrawHLine(unsigned xLeft, unsigned xRight, unsigned y, Color c);

		/**
			Draw a neat line

			start and end are inclusive screen co-ordinates
		*/
		void DrawLine(ScreenVec2 start, ScreenVec2 end, Color c);

		/**
			Draw a filled rectangle
		*/
		void DrawRect(ScreenRect dest, Color c);

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

			xLeft, xRight, y are inclusive screen co-ordinates.
			start, end, are inclusive texture co-ordinates.
			colorMult will scale the color coming from the texture
		*/
		void DrawHLine(unsigned xLeft, unsigned xRight, unsigned y, Texture const *tex, UVVec2 start, UVVec2 end, btStorageType colorMult, uint8_t stencil);
		
		/**
			Draw texture to screen rectangle

			src represents the subtexture to be stretched over the region dest on screen.
		 */
		void DrawRect(ScreenRect dest, Texture const *tex, UVRect src, btStorageType colorMult, uint8_t stencil);

		/**
			Draw texture to screen rectangle, respecting the texture alpha.

			src represents the subtexture to be stretched over the region dest on screen.
		*/
		void DrawRectAlpha(ScreenRect dest, Texture const *tex, UVRect src, btStorageType colorMult, uint8_t stencil);
		
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
		Color*   screen{nullptr};
		unsigned Height{0u};
		unsigned Width {0u};
		unsigned Scale {0u};

		btStorageType vFOV{45}, vFOVMult{1};
		btStorageType hFOV{45}, hFOVMult{1};

	private:
		//used to redirect bad writes to
		Color Dummy;

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