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

		void MoveSector(IDType toSector);
		void MoveMapAndSector(Map *map, IDType toSector);

		private:
			Map *map;
			IDType secID;

			friend class SpriteBarrow;
	};

	class SpriteBarrow {
	public:
		SpriteBarrow(Map &owner);

		Sprite::Ptr          CreateSprite(IDType sectorID, PositionVec3 position);
		std::vector<Sprite*> GetSprites  (IDType sectorID);
		void                 DeleteSprite(Sprite &sprite);
		void                 MoveSprite  (Sprite &sprite, IDType toSector);
		void                 MoveSprite  (Sprite &sprite, Map &toMap, IDType toSector);
	private:
		Map &mapOwner;
		std::map<IDType, std::vector<Sprite*>> sprites;
	};
}
