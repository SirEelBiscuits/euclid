#include "PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB

POST_STD_LIB

#include "TypeMagic.h"

#define X_TO_POWER(x, power) (TypeMagic::pow<power>::calculate(x))
#define X_TO_POWER_TYPE(x, power) decltype(X_TO_POWER(x, power))
#define SAFISH_INVERT(in) (TypeMagic::invert(in))
#define INVERSE_TYPE(in) decltype(SAFISH_INVERT(in))
