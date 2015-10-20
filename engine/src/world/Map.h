#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <list>
#include <vector>
POST_STD_LIB

#include "lib/BasicTypes.h"
#include "rendering/Textures.h"

namespace World {

	using Vert = PositionVec2;

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
		Wall           * GetWall          (unsigned ID);
		Wall     const * GetWall          (unsigned ID) const;

		Wall           * InsertWallAfter  (unsigned ID);
		Wall           * InsertWallBefore (unsigned ID);
		void             DeleteWall       (unsigned ID);

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
		btStorageType          LightLevel  {0.05f};
		Rendering::TextureInfo floor       {};
		Rendering::TextureInfo ceiling     {};

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
		Sector         * GetSector     (unsigned ID);
		Sector   const * GetSector     (unsigned ID) const;
		unsigned         GetSectorID   (Sector const * sec) const;
		Sector         * AddNewSector  ();
		void             RemoveSector  (Sector *s);

		unsigned         GetNumVerts   () const;
		Vert           * GetVert       (unsigned ID);
		Vert     const * GetVert       (unsigned ID) const;
		unsigned         GetVertID     (Vert const * v) const;
		Vert           * AddNewVert    ();
		void             RemoveVert    (Vert const * v);

	private:
		std::list<Sector> sectors  {};
		std::list<Vert>   verts    {};
		std::string       filename {};
	};
}
