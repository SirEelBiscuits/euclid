#include "Sprite.h"

PRE_STD_LIB
#include <map>
#include <set>
#include <algorithm>
POST_STD_LIB

#include "world/Map.h"

namespace World {
	SpriteBarrow::SpriteBarrow(Map &owner)
		: mapOwner(owner) 
	{}

	Sprite* SpriteBarrow::CreateSprite(IDType sectorID, PositionVec3 pos) {
		auto &list = sprites[sectorID];
		auto spr = std::make_unique<Sprite>();
		spr->map = &mapOwner;
		spr->secID = sectorID;
		spr->position = pos;

		list.push_back(std::move(spr));

		return list.back().get();
	}

	std::vector<Sprite*> SpriteBarrow::GetSprites(IDType sectorID) {
		std::vector<Sprite*> ret;
		for(auto &e : sprites[sectorID])
			ret.push_back(e.get());
		return ret;
	}

	void SpriteBarrow::DeleteSprite(Sprite &sprite) {
		ASSERT(sprite.map == &mapOwner);

		std::remove_if(
			sprites[sprite.secID].begin(),
			sprites[sprite.secID].end(),
			[sprite](Sprite::Ptr &a){return a.get() == &sprite;}
		);
	}


	void SpriteBarrow::MoveSprite(Sprite &sprite, IDType toSector) {
		auto &list = sprites[sprite.secID];
		auto itr = std::find_if(
			list.begin(),
			list.end(),
			[sprite](Sprite::Ptr &a){return a.get() == &sprite;}
		);

		if(itr != list.end()) {
			auto spriteUP = std::move(*itr);
			std::remove_if(list.begin(), list.end(), [](Sprite::Ptr &a){return a.get() == nullptr;});

			auto &newList = sprites[toSector];
			spriteUP->secID = toSector;
			newList.push_back(std::move(spriteUP));
		}
	}

	void SpriteBarrow::MoveSprite(Sprite &sprite, Map &toMap, IDType toSector) {
		CRITICAL_ASSERT(false && "not yet implemented" );
	}
}