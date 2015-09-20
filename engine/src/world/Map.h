#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <list>
#include <vector>
POST_STD_LIB

#include "lib/BasicTypes.h"

namespace World {

	using Vert = PositionVec2;

	struct Wall {
		Vert          *start {nullptr};
		struct Sector *portal{nullptr};
		Mesi::Meters   length{0};
	};

	struct Sector {
		Sector() = default;
		Sector(std::vector<Wall> const &walls)
			: walls(walls)
		{}

		Vert centroid{};
		Mesi::Meters floorHeight{0};
		Mesi::Meters ceilHeight {2};

		std::vector<Wall> walls{};
	};

	class Map {
	
		std::list<Sector> sectors{};
		std::list<Vert> verts{};

		std::string filename;
	};
}