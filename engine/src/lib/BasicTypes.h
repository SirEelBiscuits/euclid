#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <cmath>
#include <cfloat>
POST_STD_LIB

using btStorageType = float;
#define BTST_MAX FLT_MAX
#define MESI_LITERAL_TYPE btStorageType
#include "Mesitype.h"
#include "Matrix.h"

#include "FixedPoint.h"

using Fix16 = Fixed16x16;

using PositionVec2  = Vector<Mesi::Meters, 2, btStorageType>;
using PositionVec3  = Vector<Mesi::Meters, 3, btStorageType>;
using ScreenVec2    = Vector<int, 2>;
using UVVec2        = Vector<Fix16, 2>;
using TransformMat2 = Matrix<btStorageType, 2, 2>;

PositionVec2 AsVec2(PositionVec3 const &vec);

TransformMat2 RotationMatrix(btStorageType angle);
