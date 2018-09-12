#pragma once

#include <vector>

#include "color_capsule.h"

class rgb {
public:
	rgb() :
	r_(0), g_(0), b_(0){}
	rgb(unsigned char r, unsigned char g, unsigned char b) :
		r_(r), g_(g), b_(b) {}

	bool operator!=(const rgb& a) {
		if (r_ != a.r_) return true;
		if (g_ != a.g_) return true;
		if (b_ != a.b_) return true;
		return false;
	}

	bool operator==(const rgb& a) {
		return !(*this != a);
	}

	unsigned char r_;
	unsigned char g_;
	unsigned char b_;
};

class SIFTpoint {
public:
	int num_angle_;// 2*pi/num_angle
	int size_window_;
	int x_;
	int y_;
	std::vector<float> histgram_;
};

class img_calc
{
public:
	int w_;
	int h_;
	float* buf_;

	img_calc();
	img_calc(int x, int y, float* buf);
	~img_calc();
	void set(int x, int y, float v);
	float get(int x, int y);
};

class img
{
public:
	img();
	img(const img& image);
	img(int w, int h, unsigned char* buf);
	~img();
	img& operator=(const img& image);
	rgb getRGB(int x, int y);
	void setRGB(int r, int g, int b, int x, int y);
	void setRGB(const rgb& color, int x, int y);
	void load_4byte(int w, int h, unsigned char* buf);
	void load_3byte(int w, int h, unsigned char* buf);
	bool load(const char* path_file);
	void output_4byte_y_ascending(int& size_buf, unsigned char*& buf);
	void output_jpeg(unsigned char* buf, int* size);
	bool load_3byte_file(const char* path_file);
	bool save(const char* path_file);
	std::vector<unsigned char> save_to_memory();
	bool match(img* target, int& x, int& y, bool flag_null_black);
	bool match_max_diff(img* target, int& x, int& y, int& max_diff);
	bool match_vortex(std::vector<img*> target, int&x, int&y, int& max_count, int& max_diff);
	int getMatchValueSquare(int x, int y, img* image_smaller);
	void scale_down(int n, bool flag_smoothing);
	void gray_scale();
	void hog(float*& buf, int size_cell, int num_dir);
	void binarizationColorMap(std::vector<unsigned char*>& buf);
	void fillNeighbor(int size_window, int num_neighbor, rgb color);
	void filtering3x3(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9);
	int haarlike(img* image_haarlike, int x, int y);
	void haarlike(img* image_haarlike, int& x, int& y, int& sum, float& a);
	void scale(float a);
	void expandHistgram();
	img* getPart(int x, int y, int w, int h);
	void setPart(int x, int y, img* source, int x_s, int y_s, int w_s, int h_s);
	void gaussian_filter(double sig);
	img_calc* subtract(img* a);
	void rotate(float sheta, bool flag_scale, rgb color_fill);

	void line(int x1, int y1, int x2, int y2, rgb color);
	void box(int x1, int y1, int x2, int y2, rgb color, bool fill);

public:
	static unsigned char* makeColorCapsuleMap(int rf, int gf, int bf, int rt, int gt, int bt, float value_r, unsigned char* buf_base);
public:
	int w_;
	int h_;
	unsigned char* buf_;

private:
	bool _load(const unsigned char* buf, int size);
};

class sift {
public:
	int size_block_;
	float* buf_;
};

namespace {
	void rotate_(int& x, int& y, float sheta);
};
