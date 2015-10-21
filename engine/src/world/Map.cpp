#include "Map.h"

namespace World {
	Sector::Sector(std::vector<Wall> const & walls)
		: walls(walls)
	{}
	
	unsigned Sector::GetNumWalls() const {
		return walls.size();
	}

	Wall * Sector::GetWall(unsigned ID) {
		return &walls[ID];
	}

	Wall const * Sector::GetWall(unsigned ID) const {
		return &walls[ID];
	}

	Wall * Sector::InsertWallAfter(unsigned ID) {
		using difType = std::vector<World::Wall>::iterator::difference_type;
		return &*walls.insert(walls.begin() + static_cast<difType>(ID) + 1, Wall{});
	}

	Wall * Sector::InsertWallBefore(unsigned ID) {
		using difType = std::vector<World::Wall>::iterator::difference_type;
		return &*walls.insert(walls.begin() + static_cast<difType>(ID), Wall{});
	}

	void Sector::DeleteWall(unsigned ID) {
		using difType = std::vector<World::Wall>::iterator::difference_type;
		walls.erase(walls.begin() + static_cast<difType>(ID));
	}

	bool Sector::IsConvex() const {
		if(GetNumWalls() < 3)
			return true;
		for(auto li = 0u; li < GetNumWalls(); ++li) {
			auto &l = walls[li];
			auto &nl = walls[(li + 1) % GetNumWalls()];
			auto &nnl = walls[(li + 2) % GetNumWalls()];

			auto val = (*nl.start - *l.start) ^ (*nnl.start - *l.start);

			if(val.val < -0.01)
				return false;
		}
		return true;
	}

	void Sector::UpdateCentroid() {
		centroid = PositionVec2{};
		for(auto &w : walls) 
			centroid += *w.start;
		centroid /= static_cast<btStorageType>(GetNumWalls());
	}

	void Sector::UpdateLineLengths() {
		for(auto wi = 0u; wi < GetNumWalls(); ++wi) {
			auto w = GetWall(wi);
			auto nw = GetWall((wi + 1) % GetNumWalls());
			w->length = (*w->start - *nw->start).Length();
		}
	}

	void Sector::FixWinding() {
		if(!IsConvex())
			ReverseWinding();
	}

	void Sector::ReverseWinding() {
		std::vector<Wall> vec(walls.rbegin(), walls.rend());

		auto vert = vec[vec.size()-1].start;
		for(auto wi = vec.size() - 1; wi > 0; --wi) {
			vec[wi].start = vec[wi-1].start;
		}
		vec[0].start = vert;

		walls = vec;
	}

	Map::Map() {
	}

	Map::~Map() {
	}

	Map::Map(Map const & other) {
		*this = other;
	}

	Map & Map::operator=(Map const & other) {
		verts.clear();
		sectors.clear();
		sectors.resize(other.GetNumSectors());
		//todo skyboxes

		verts = other.verts;

		auto thisSec = sectors.begin();
		auto otherSec = other.sectors.cbegin();
		for(; thisSec != sectors.end(); ++thisSec, ++otherSec) {
			*thisSec = *otherSec;

			for(auto wi = 0u; wi < thisSec->GetNumWalls(); ++ wi) {
				auto thisWall = thisSec->GetWall(wi);
				auto otherWall = otherSec->GetWall(wi);
				thisWall->start = GetVert(other.GetVertID(otherWall->start));
				if(thisWall->portal != nullptr)
					thisWall->portal = GetSector(other.GetSectorID(otherWall->portal));
			}
		}

		return *this;
	}

	unsigned Map::GetNumSectors() const {
		return sectors.size();
	} 

	Sector * Map::GetSector(unsigned ID) {
		auto cur = sectors.begin();
		for(auto i = 0u; i < sectors.size() && i < ID; ++i, ++cur);
		return &*cur;
	}

	Sector const * Map::GetSector(unsigned ID) const {
		auto cur = sectors.cbegin();
		for(auto i = 0u; i < sectors.size() && i < ID; ++i, ++cur);
		return &*cur;
	}

	unsigned Map::GetSectorID(Sector const * sec) const {
		auto si = 0u;
		for(auto cur = sectors.cbegin(); si < GetNumSectors() && &*cur != sec; ++si, ++cur);
		return si;
	}

	Sector * Map::AddNewSector() {
		sectors.emplace_back();
		return &sectors.back();
	}

	void Map::RemoveSector(Sector * s) {
		for(auto it = sectors.begin(); it != sectors.end(); ++it)
			if(&*it == s) {
				sectors.erase(it);
				break;
			}

		for(auto &sec : sectors)
			for(auto wi = 0u; wi < sec.GetNumWalls(); ++wi) {
				auto wall = sec.GetWall(wi);
				wall->portal = wall->portal == s? nullptr : wall->portal;
			}

		std::vector<Vert*> vertsToKill;

		auto vi = 0u;
		for(auto vit = verts.begin(); vit != verts.end(); ++vit, ++vi) {
			auto v = &*vit;
			bool found = false;
			for(auto sit = sectors.begin(); sit != sectors.end() && !found; ++sit) {
				for(auto wi = 0u; wi < sit->GetNumWalls(); ++wi) {
					auto w = sit->GetWall(wi);
					found |= w->start == v;
				}
			}
			if(!found)
				vertsToKill.push_back(&*vit);
		}

		for(auto it = vertsToKill.rbegin(); it != vertsToKill.rend(); ++it)
			RemoveVert(*it);
	}

	unsigned Map::GetNumVerts() const {
		return verts.size();
	}

	Vert * Map::GetVert(unsigned ID) {
		auto cur = verts.begin();
		for(auto i = 0u; i < verts.size() && i < ID; ++i, ++cur);
		return &*cur;
	}

	Vert const * Map::GetVert(unsigned ID) const {
		auto cur = verts.cbegin();
		for(auto i = 0u; i < verts.size() && i < ID; ++i, ++cur);
		return &*cur;
	}

	unsigned Map::GetVertID(Vert const * v) const {
		auto ret = 0u;
		auto it = verts.cbegin();
		for(; it != verts.cend() && &*it != v; ++it, ++ret);
		if(it == verts.cend())
			ret = (unsigned)-1;
		return ret;
	}

	Vert * Map::AddNewVert() {
		verts.push_back(Vert{});
		return &verts.back();
	}

	void Map::RemoveVert(Vert const * v) {
		for(auto it = verts.begin(); it != verts.end(); ++it)
			if(&*it == v) {
				verts.erase(it);
				break;
			}
	}

	void Map::RegisterTexture(std::shared_ptr<Rendering::Texture> ptr) {
		if(ptr.get() == nullptr)
			return;
		for(auto & t : TexturesUsed)
			if(t.get() == ptr.get())
				return;
		 TexturesUsed.push_back(ptr);
	}

	void Map::RegisterAllTextures() {
		using Rendering::TextureStore::GetTexture;
		TexturesUsed.clear();
		for(auto &s : sectors) {
			RegisterTexture(GetTexture(s.floor.tex));
			RegisterTexture(GetTexture(s.ceiling.tex));
			for(auto wi = 0u; wi < s.GetNumWalls(); ++wi) {
				auto w = s.GetWall(wi);
				RegisterTexture(GetTexture(w->bottomTex.tex));
				RegisterTexture(GetTexture(w->mainTex.tex));
				RegisterTexture(GetTexture(w->topTex.tex));
			}
		}
	}
}
