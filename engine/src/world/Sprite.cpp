#include "Sprite.h"

PRE_STD_LIB
#include <map>
#include <set>
#include <algorithm>
POST_STD_LIB

#include "world/Map.h"

namespace World {
	void Sprite::MoveSector(IDType toSector) {
		map->barrow.MoveSprite(*this, toSector);
	}

	void Sprite::MoveMapAndSector(Map *toMap, IDType toSector) {
		map->barrow.MoveSprite(*this, *toMap, toSector);
	}

	void Sprite::Deleter::operator()(Sprite * s) {
		s->map->barrow.DeleteSprite(*s);
	}

	SpriteBarrow::SpriteBarrow(Map &owner)
		: mapOwner(owner) 
	{}

	Sprite::Ptr SpriteBarrow::CreateSprite(IDType sectorID, PositionVec3 pos) {
		auto &list = sprites[sectorID];
		auto spr = Sprite::Ptr(new Sprite());
		spr->map = &mapOwner;
		spr->secID = sectorID;
		spr->position = pos;

		list.push_back(spr.get());

		return spr;
	}

	std::vector<Sprite*> SpriteBarrow::GetSprites(IDType sectorID) {
		return std::vector<Sprite*>(sprites[sectorID]);
	}

	void SpriteBarrow::DeleteSprite(Sprite &sprite) {
		ASSERT(sprite.map == &mapOwner);
		auto &list = sprites[sprite.secID];
		std::remove(list.begin(), list.end(), &sprite);
	}


	void SpriteBarrow::MoveSprite(Sprite &sprite, IDType toSector) {
		auto &list = sprites[sprite.secID];
		auto itr = std::find(list.begin(), list.end(), &sprite);

		if(itr != list.end()) {
			sprite.secID = toSector;
			sprites[toSector].push_back(*itr);
			std::remove(list.begin(), list.end(), &sprite);
		}
	}

	void SpriteBarrow::MoveSprite(Sprite &sprite, Map &toMap, IDType toSector) {
		auto &list = sprites[sprite.secID];
		auto itr = std::find(list.begin(), list.end(), &sprite);
		if(itr != list.end()) {
			sprite.map = &toMap;
			sprite.secID = toSector;
			toMap.barrow.sprites[toSector].push_back(*itr);
			std::remove(list.begin(), list.end(), &sprite);
		}
	}
}
