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
		if(s->sector)
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
		auto num = sprites.size();
		sprites.erase(std::remove(sprites.begin(), sprites.end(), &sprite), sprites.end());
		sprite.sector = nullptr;
		ASSERT(sprites.size() < num);
	}

	void SpriteBarrow::MoveSprite(Sprite &sprite, Map &toMap, IDType toSector) {
		auto &targetBarrow = toMap.GetSector(toSector)->barrow;
		if(&targetBarrow == this)
			return;
		auto itr = std::find(sprites.begin(), sprites.end(), &sprite);
		if(itr != sprites.end()) {
			auto num = targetBarrow.sprites.size() + sprites.size();
			sprite.sector = targetBarrow.owner;
			targetBarrow.sprites.push_back(*itr);
			sprites.erase(std::remove(sprites.begin(), sprites.end(), &sprite), sprites.end());
			ASSERT(num == targetBarrow.sprites.size() + sprites.size());
		} else ASSERT(false);
	}
}
