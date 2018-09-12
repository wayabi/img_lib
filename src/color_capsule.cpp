#include "color_capsule.h"

color_capsule::color_capsule(unsigned char rf, unsigned char gf, unsigned char bf, unsigned char rt, unsigned char gt, unsigned char bt, float value_r) :
	r_from_(rf),
	g_from_(gf),
	b_from_(bf),
	r_to_(rt),
	g_to_(gt),
	b_to_(bt),
	value_r_(value_r)
{
}


color_capsule::~color_capsule()
{
}
