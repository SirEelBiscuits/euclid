#include "Maths.h"

int Maths::nextBiggestPow2(int val) {
	--val;
	val |= val >> 1;
	val |= val >> 2;
	val |= val >> 4;
	val |= val >> 8;
	val |= val >> 16;
	++val;
	return val;
}
