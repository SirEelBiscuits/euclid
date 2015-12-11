#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <list>
#include <vector>
POST_STD_LIB

#include "lib/BasicTypes.h"
#include "rendering/Textures.h"

#include "Sprite.h"

namespace World {

	using Vert = PositionVec2;
	using IDType = unsigned;

	struct Wall {
	public:
		Vert                   *start     {nullptr};
		class Sector           *portal    {nullptr};
		Mesi::Meters            length    {0};
		Rendering::TextureInfo  topTex    {};
		Rendering::TextureInfo  mainTex   {};
		Rendering::TextureInfo  bottomTex {};
	};

	class Sector {
	public:
		Sector() = default;
		Sector(std::vector<Wall> const &walls);

		unsigned         GetNumWalls      () const;
		Wall           * GetWall          (IDType ID);
		Wall     const * GetWall          (IDType ID) const;

		Wall           * InsertWallAfter  (IDType ID);
		Wall           * InsertWallBefore (IDType ID);
		void             DeleteWall       (IDType ID);

		bool             IsConvex         () const;

		void             UpdateCentroid   ();
		void             UpdateLineLengths();
		void             FixWinding       ();
		void             ReverseWinding   ();

	//////////
	// DATA //
	//////////
	public:
		Vert                   centroid    {};
		Mesi::Meters           floorHeight {0};
		Mesi::Meters           ceilHeight  {2};
		btStorageType          lightLevel  {0.05f};
		Rendering::TextureInfo floor       {};
		Rendering::TextureInfo ceiling     {};

		SpriteBarrow           barrow   {*this};

	private:
		std::vector<Wall>      walls       {};
	};

	class Map {
	public:
		Map();
		~Map();
		Map(Map const &other);
		Map& operator=(Map const &other);
		//todo move operations

		unsigned         GetNumSectors () const;
		Sector         * GetSector     (IDType ID);
		Sector   const * GetSector     (IDType ID) const;
		IDType           GetSectorID   (Sector const * sec) const;
		Sector         * AddNewSector  ();
		void             RemoveSector  (Sector *s);

		unsigned         GetNumVerts   () const;
		Vert           * GetVert       (IDType ID);
		Vert     const * GetVert       (IDType ID) const;
		IDType           GetVertID     (Vert const * v) const;
		Vert           * AddNewVert    ();
		void             RemoveVert    (Vert const * v);

		void             RegisterTexture(std::shared_ptr<Rendering::Texture> ptr);
		void             RegisterAllTextures();

	private:
		std::list<Sector> sectors  {};
		std::list<Vert>   verts    {};

		std::vector<std::shared_ptr<Rendering::Texture>> TexturesUsed{};
	};
}
