#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <memory>
#include <map>
#include <vector>
POST_STD_LIB

#include "lib/BasicTypes.h"

namespace Rendering {
	class Texture;
}

namespace World {
	class Map;
	class Sector;

	using IDType = unsigned;

	class Sprite {
	public:
		Rendering::Texture *tex;
		Mesi::Meters height;
		PositionVec3 position;

		class Deleter {
		public:
			void operator() (Sprite * s);
		};

		using Ptr = std::unique_ptr<Sprite, Deleter>;

		void Move(Map *map, IDType toSector);

		private:
			Sector *sector;

			friend class SpriteBarrow;
	};

	class SpriteBarrow {
	public:
		SpriteBarrow(Sector &owner);

		Sprite::Ptr          CreateSprite(PositionVec3 position);
		std::vector<Sprite*> GetSprites  () const;
		void                 DeleteSprite(Sprite &sprite);
		void                 MoveSprite  (Sprite &sprite, Map &toMap, IDType toSector);
	private:
		Sector *owner;
		std::vector<Sprite*> sprites;
	};
}
