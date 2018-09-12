#pragma once
class color_capsule
{
public:
	color_capsule(unsigned char rf, unsigned char gf, unsigned char bf, unsigned char rt, unsigned char gt, unsigned char bt, float value_r);
	~color_capsule();
	unsigned char r_from_;
	unsigned char g_from_;
	unsigned char b_from_;
	unsigned char r_to_;
	unsigned char g_to_;
	unsigned char b_to_;
	float value_r_;
};
