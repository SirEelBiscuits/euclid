#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <cmath>
POST_STD_LIB

using btStorageType = float;
#define MESI_LITERAL_TYPE btStorageType
#include "Mesitype.h"
#include "Matrix.h"

using PositionVec2 = Vector<Mesi::Meters, 2, btStorageType>;
using PositionVec3 = Vector<Mesi::Meters, 3, btStorageType>;
using ScreenVec2   = Vector<int, 2>;
