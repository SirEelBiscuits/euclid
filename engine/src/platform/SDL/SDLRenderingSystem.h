#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <memory>
POST_STD_LIB

#include "rendering/RenderingSystem.h"

struct SDL_Renderer;
struct SDL_Window;
struct SDL_Texture;

namespace Rendering {

	template<typename T>
	struct sdlDeleter {
		void operator()(T *p);
	};

	using winType = std::unique_ptr<SDL_Window,   sdlDeleter<SDL_Window>>;
	using renType = std::unique_ptr<SDL_Renderer, sdlDeleter<SDL_Renderer>>;

	struct SDLSystem {
		SDLSystem();
		~SDLSystem();
		SDLSystem& operator=(SDLSystem&) = delete;
	};

	class SDLContext : public Context {
	public:
		SDLContext(unsigned Width, unsigned Height, unsigned Scale, bool Fullscreen, char const *Title);
		virtual ~SDLContext();

		virtual void Clear(Color c) override;
		virtual void SetResolution(unsigned Width, unsigned Height) override;
		virtual void FlipBuffers() override;

	private:

		int ScreenWidth;
		int ScreenHeight;

		winType win{nullptr};
		renType ren{nullptr};

		std::shared_ptr<SDLSystem>  sys{nullptr};
		SDL_Texture                *t  {nullptr};
	};
}