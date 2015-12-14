#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <memory>
#include <map>
#include <vector>
#include <algorithm>
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

		Sector* GetSector() { return sector; }

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

		int                  GetNumSprites() const { return sprites.size(); }
		void                 SetNumSprites(int num) {
			if(num < sprites.size())
				sprites.resize(num);
		}
		void                 AddSprite(Sprite &sprite) {
			//TODO find out why this is still needed
			if(std::find(sprites.begin(), sprites.end(), &sprite) == sprites.end())
				sprites.push_back(&sprite);
		}
	private:
		Sector *owner;
		std::vector<Sprite*> sprites;
	};
}
