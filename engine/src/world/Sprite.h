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
		Mesi::Meters height;
		PositionVec3 position;
		btStorageType angle;

		struct angleTexture {
			Rendering::Texture *tex;
			btStorageType angle;
		};

		std::vector<angleTexture> textures {};

		Sector* GetSector() { return sector; }

		class Deleter {
		public:
			void operator() (Sprite * s);
		};

		using Ptr = std::unique_ptr<Sprite, Deleter>;

		void Move(Map *map, IDType toSector);

		void SetTexture(Rendering::Texture *tex) {
			textures.resize(1);
			textures[0].tex = tex;
			textures[0].angle = 0;
		}

		void SetTextureAngle(Rendering::Texture *tex, btStorageType angle) {
			auto it = textures.cbegin();
			for(; it < textures.cend(); ++it)
				if(it->angle > angle)
					break;
			textures.insert(it, {tex, angle});
		}

		Rendering::Texture * GetTexture(btStorageType angle = 0) {
			if(textures.size() == 0)
				return nullptr;
			if(textures.size() == 1)
				return textures[0].tex;
			for(auto i = textures.size() - 1u; i >= 0u ; --i)
				if(angle >= textures[i].angle)
					return textures[i].tex;
			return textures[textures.size() - 1].tex;
		}

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
		void                 AddSprite(Sprite &sprite) {
			//TODO find out why this is still needed
			if(std::find(sprites.begin(), sprites.end(), &sprite) == sprites.end())
				sprites.push_back(&sprite);
			else
				ASSERT(false);
		}
	private:
		Sector *owner;
		std::vector<Sprite*> sprites;
	};
}
