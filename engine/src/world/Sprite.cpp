#include "Sprite.h"

PRE_STD_LIB
#include <map>
#include <set>
#include <algorithm>
POST_STD_LIB

#include "world/Map.h"

namespace World {

	void Sprite::Move(Map *toMap, IDType toSector) {
		sector->barrow.MoveSprite(*this, *toMap, toSector);
	}

	void Sprite::Deleter::operator()(Sprite * s) {
		s->sector->barrow.DeleteSprite(*s);
	}

	SpriteBarrow::SpriteBarrow(Sector &owner)
		: owner(&owner) 
	{}

	Sprite::Ptr SpriteBarrow::CreateSprite(PositionVec3 pos) {
		auto spr = Sprite::Ptr(new Sprite());
		spr->sector = owner;
		spr->position = pos;

		sprites.push_back(spr.get());

		return spr;
	}

	std::vector<Sprite*> SpriteBarrow::GetSprites() const {
		return std::vector<Sprite*>(sprites);
	}

	void SpriteBarrow::DeleteSprite(Sprite &sprite) {
		std::remove(sprites.begin(), sprites.end(), &sprite);
	}

	void SpriteBarrow::MoveSprite(Sprite &sprite, Map &toMap, IDType toSector) {
		auto itr = std::find(sprites.begin(), sprites.end(), &sprite);
		if(itr != sprites.end()) {
			auto targetBarrow = toMap.GetSector(toSector)->barrow;
			sprite.sector = targetBarrow.owner;
			targetBarrow.sprites.push_back(*itr);
			std::remove(sprites.begin(), sprites.end(), &sprite);
		}
	}
}