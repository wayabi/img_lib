#include "img.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <list>
#include <algorithm>
#include <functional>


#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_STATIC
#include "stb_image.h"
#include "stb_image_write.h"
#include "jpge.h"



using namespace std;

img::img() :
	w_(0), h_(0), buf_(NULL)
{}

img::img(int w, int h, unsigned char* buf) :
	w_(w), h_(h)
{
	buf_ = new unsigned char[w_*h_ * 3];
	if (buf) {
		memcpy(buf_, buf, w_*h_ * 3);
	}
}

img::img(const img& image) :
	w_(image.w_),
	h_(image.h_)
{
	buf_ = new unsigned char[w_*h_ * 3];
	memcpy(buf_, image.buf_, w_*h_ * 3);
}

img::~img()
{
	if (buf_) delete[](buf_);
}

img& img::operator=(const img& image)
{
	w_ = image.w_;
	h_ = image.h_;
	if (buf_) delete[](buf_);
	buf_ = new unsigned char[w_*h_ * 3];
	memcpy(buf_, image.buf_, w_*h_ * 3);
	return *this;
}

void img::load_4byte(int w, int h, unsigned char* buf)
{
	w_ = w;
	h_ = h;
	buf_ = new unsigned char[w*h * 3];
	int index = 0;
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			int yy = h - y - 1;
			unsigned char* c = buf + w*(yy)* 4 + x * 4;
			*(buf_ + index) = *(c + 2);
			++index;
			*(buf_ + index) = *(c + 1);
			++index;
			*(buf_ + index) = *(c + 0);
			++index;
		}
	}
}

void img::output_4byte_y_ascending(int& size_buf, unsigned char*& buf)
{
	size_buf = w_*h_ * 4;
	buf = new unsigned char[size_buf];
	unsigned char* p = buf;
	for (int y = 0; y < h_; ++y) {
		for (int x = 0; x < w_; ++x) {
			*(p++) = *(buf_ + y*w_ * 3 + x * 3 + 2);
			*(p++) = *(buf_ + y*w_ * 3 + x * 3 + 1);
			*(p++) = *(buf_ + y*w_ * 3 + x * 3 + 0);
			*(p++) = 0;
		}
	}
}

rgb img::getRGB(int x, int y)
{
	rgb ret;
	unsigned char* c = buf_ + y*w_ * 3 + x * 3;
	ret.r_ = *(c + 0);
	ret.g_ = *(c + 1);
	ret.b_ = *(c + 2);
	return ret;
}

void img::setRGB(int r, int g, int b, int x, int y)
{
	if (x < 0 || x >= w_ || y < 0 || y >= h_) return;
	unsigned char* c = buf_ + y*w_ * 3 + x * 3;
	*(c + 0) = r;
	*(c + 1) = g;
	*(c + 2) = b;
}

void img::setRGB(const rgb& color, int x, int y)
{
	unsigned char* c = buf_ + y*w_ * 3 + x * 3;
	*(c + 0) = color.r_;
	*(c + 1) = color.g_;
	*(c + 2) = color.b_;
}

bool img::save(const char* path_file)
{
	unsigned char HEADER[] = {
		0x42, 0x4d, 0x76, 0xca, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
		0x00, 0x00, 0x16, 0x02, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x40, 0xca, 0x09, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};
	const int offset_file_size = 2;
	const int offset_w = 18;
	const int offset_h = 22;
	const int size_header = 54;
	unsigned char buf[size_header];
	memcpy(buf, HEADER, size_header);

	int w_padding = 4-((w_*3) % 4);
	if (w_padding == 4) w_padding = 0;
	
	int size = (w_*3+w_padding) * h_ + size_header;
	memcpy(buf + offset_file_size, &size, 4);
	memcpy(buf + offset_w, &w_, 4);
	memcpy(buf + offset_h, &h_, 4);
	FILE* f;
	if ((f = fopen(path_file, "wb")) == NULL) {
		return false;
	}
	fwrite(buf, 1, size_header, f);
	for (int y = 0; y < h_; ++y) {
		for (int x = 0; x < w_; ++x) {
			int yy = h_ - y - 1;
			fputc(*(buf_ + yy*w_ * 3 + x * 3 + 2), f);
			fputc(*(buf_ + yy*w_ * 3 + x * 3 + 1), f);
			fputc(*(buf_ + yy*w_ * 3 + x * 3 + 0), f);
		}
		for (int i = 0; i < w_padding; ++i) {
			fputc(0, f);
		}
	}

	fclose(f);
	return true;
}

std::vector<unsigned char> img::save_to_memory()
{
	vector<unsigned char> ret;
	unsigned char HEADER[] = {
		0x42, 0x4d, 0x76, 0xca, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
		0x00, 0x00, 0x16, 0x02, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x40, 0xca, 0x09, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};
	const int offset_file_size = 2;
	const int offset_w = 18;
	const int offset_h = 22;
	const int size_header = 54;
	unsigned char buf[size_header];
	memcpy(buf, HEADER, size_header);

	int w_padding = 4 - ((w_ * 3) % 4);
	if (w_padding == 4) w_padding = 0;

	int size = (w_ * 3 + w_padding) * h_ + size_header;
	memcpy(buf + offset_file_size, &size, 4);
	memcpy(buf + offset_w, &w_, 4);
	memcpy(buf + offset_h, &h_, 4);

	ret.resize(sizeof(HEADER));
	memcpy(&ret[0], buf, sizeof(HEADER));

	for (int y = 0; y < h_; ++y) {
		for (int x = 0; x < w_; ++x) {
			int yy = h_ - y - 1;
			ret.push_back(*(buf_ + yy*w_ * 3 + x * 3 + 2));
			ret.push_back(*(buf_ + yy*w_ * 3 + x * 3 + 1));
			ret.push_back(*(buf_ + yy*w_ * 3 + x * 3 + 0));
		}
		for (int i = 0; i < w_padding; ++i) {
			ret.push_back(0);
		}
	}

	return ret;
}

bool img::load_3byte_file(const char* path_file)
{
	FILE* f;
	if ((f = fopen(path_file, "rb")) == NULL) {
		return false;
	}
	const int offset_w = 18;
	const int offset_h = 22;
	const int size_header = 54;
	unsigned char* buf = new unsigned char[size_header];
	fread(buf, 1, size_header, f);
	memcpy(&w_, buf + offset_w, 4);
	memcpy(&h_, buf + offset_h, 4);
	delete[](buf);
	int ww = 4-((w_*3) % 4);
	if (ww == 4) ww = 0;
	buf = new unsigned char[(w_*3+ww)*h_];
	fread(buf, 1, (w_ * 3 + ww)*h_, f);
	fclose(f);
	buf_ = new unsigned char[w_*h_ * 3];
	int index = 0;
	for (int y = 0; y < h_; ++y) {
		for (int x = 0; x < w_; ++x) {
			int yy = h_ - y - 1;
			*(buf_ + index) = *(buf + (w_*3 + ww)*yy + x * 3 + 2);
			++index;
			*(buf_ + index) = *(buf + (w_*3 + ww)*yy + x * 3 + 1);
			++index;
			*(buf_ + index) = *(buf + (w_*3 + ww)*yy + x * 3 + 0);
			++index;
		}
	}
	delete[](buf);
	return true;
}

bool img::_load(const unsigned char* buf, int size)
{
	int w, h, components;
	int request_components = 3;
	unsigned char * ret = stbi_load_from_memory(buf, size, &w, &h, &components, request_components);
	w_ = w;
	h_ = h;
	if (components != request_components) return false;
	if (buf_) delete[](buf_);
	buf_ = new unsigned char[w_*h_ * 3];
	memcpy(buf_, ret, w_*h_ * 3);
	stbi_image_free(ret);
	return true;
}

bool img::load(const char* path_file)
{
	FILE* f;
	if((f = fopen(path_file, "rb")) == NULL){
		printf("file not exist:%s\n", path_file);
		return false;
	}

	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char* buf = new unsigned char[size];
	fread(buf, 1, size, f);
	bool ret = this->_load(buf, size);
	delete[](buf);
	if(ret == false){
		printf("file format is not any of png bmp jpg\n");
	}
	return ret;
}

void img::load_3byte(int w, int h, unsigned char* buf)
{
	if (buf_) delete[](buf_);
	w_ = w;
	h_ = h;
	buf_ = new unsigned char[w*h * 3];
	int index = 0;
	for (int y = 0; y < h_; ++y) {
		for (int x = 0; x < w_; ++x) {
			int yy = h_ - y - 1;
			*(buf_ + index) = *(buf + (w_ * 3)*yy + x * 3 + 2);
			++index;
			*(buf_ + index) = *(buf + (w_ * 3)*yy + x * 3 + 1);
			++index;
			*(buf_ + index) = *(buf + (w_ * 3)*yy + x * 3 + 0);
			++index;
		}
	}
}

bool img::match(img* target, int& x_ret, int& y_ret, bool flag_null_black)
{
	if (target->w_ == 0 || target->h_ == 0) return false;
	for (int y = 0; y < h_; ++y) {
		if (y + target->h_ > h_) break;
		for (int x = 0; x < w_; ++x) {
			if (x + target->w_ > w_) break;
			bool flag = true;
			for (int yy = 0; yy < target->h_; ++yy) {
				for (int xx = 0; xx < target->w_; ++xx) {
					rgb p = target->getRGB(xx, yy);
					if (flag_null_black && p.r_ == 0 && p.g_ == 0 && p.b_ == 0) continue;
					if (getRGB(x + xx, y + yy) != p) {
						flag = false;
						break;
					}
				}
				if (!flag) break;
			}
			if (flag) {
				x_ret = x;
				y_ret = y;
				return true;
			}
		}
	}
	return false;
}

bool img::match_max_diff(img* target, int& x_ret, int& y_ret, int& max_diff)
{
	int max = max_diff;
	if (target->w_ == 0 || target->h_ == 0) return false;
	for (int y = 0; y < h_; ++y) {
		if (y + target->h_ >= h_) break;
		for (int x = 0; x < w_; ++x) {
			if (x + target->w_ >= w_) break;
			bool flag = true;
			max_diff = 0;
			for (int yy = 0; yy < target->h_; ++yy) {
				for (int xx = 0; xx < target->w_; ++xx) {
					rgb p_s = getRGB(x + xx, y + yy);
					rgb p_t = target->getRGB(xx, yy);
					int diff_r = abs(p_s.r_ - p_t.r_);
					int diff_g = abs(p_s.g_ - p_t.g_);
					int diff_b = abs(p_s.b_ - p_t.b_);
					if(diff_r > max || diff_g > max || diff_b > max) {
						flag = false;
						break;
					}
					if (diff_r > max_diff) max_diff = diff_r;
					if (diff_g > max_diff) max_diff = diff_g;
					if (diff_b > max_diff) max_diff = diff_b;
				}
				if (!flag) break;
			}
			if (flag) {
				x_ret = x;
				y_ret = y;
				return true;
			}
		}
	}
	return false;
}

void get_next_dir(int& x, int& y)
{
	if (x == 0 && y == 0) {
		x = 1;
		y = 0;
	}
	else if (x == 1 && y == 0) {
		x = 0;
		y = 1;
	}
	else if (x == 0 && y == 1) {
		x = -1;
		y = 0;
	}
	else if (x == -1 && y == 0) {
		x = 0;
		y = -1;
	}
	else if (x == 0 && y == -1) {
		x = 1;
		y = 0;
	}
}

bool img::match_vortex(std::vector<img*> target, int&x, int&y, int& max_count, int& max_diff)
{
	int max = max_diff;
	int max_count_s = max_count;
	int dir_x = 0;
	int dir_y = 0;
	int count = 0;
	int count_line = 0;
	int max_count_line = 1;
	int w_target = target.at(0)->w_;
	int h_target = target.at(0)->h_;

	for (int i = 0; i < max_count_s; ++i) {
		x += dir_x;
		y += dir_y;
		++count_line;
		if (count_line >= max_count_line/2) {
			get_next_dir(dir_x, dir_y);
			++max_count_line;
			count_line = 0;
		}
		if (x < 0 || y < 0 || x + w_target >= w_ || y + h_target >= h_) {
			continue;
		}
		for (auto ite = target.begin(); ite != target.end(); ++ite) {
			max_diff = 0;
			bool flag = true;
			for (int yy = 0; yy < h_target; ++yy) {
				for (int xx = 0; xx < w_target; ++xx) {
					rgb p_s = getRGB(x + xx, y + yy);
					rgb p_t = (*ite)->getRGB(xx, yy);
					int diff_r = abs(p_s.r_ - p_t.r_);
					int diff_g = abs(p_s.g_ - p_t.g_);
					int diff_b = abs(p_s.b_ - p_t.b_);
					if (diff_r > max || diff_g > max || diff_b > max) {
						flag = false;
						break;
					}
					if (diff_r > max_diff) max_diff = diff_r;
					if (diff_g > max_diff) max_diff = diff_g;
					if (diff_b > max_diff) max_diff = diff_b;
				}
				if (!flag) break;
			}
			if (flag) {
				max_count = i;
				return true;
			}
		}
	}
	return false;
}

void img::scale_down(int n, bool flag_smoothing)
{
	int w = w_ / n;
	int h = h_ / n;
	unsigned char* buf = new unsigned char[w*h * 3];
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			int r = 0; int g = 0; int b = 0;
			if (flag_smoothing) {
				for (int yy = 0; yy < n; ++yy) {
					for (int xx = 0; xx < n; ++xx) {
						r += *(buf_ + (y*n + yy)*w_ * 3 + (x*n + xx) * 3 + 0);
						g += *(buf_ + (y*n + yy)*w_ * 3 + (x*n + xx) * 3 + 1);
						b += *(buf_ + (y*n + yy)*w_ * 3 + (x*n + xx) * 3 + 2);
					}
				}
				r /= n*n; g /= n*n; b /= n*n;
			}
			else {
				r = *(buf_ + y*n*w_ * 3 + x*n * 3 + 0);
				g = *(buf_ + y*n*w_ * 3 + x*n * 3 + 1);
				b = *(buf_ + y*n*w_ * 3 + x*n * 3 + 2);
			}
			*(buf + y*w * 3 + x * 3 + 0) = r;
			*(buf + y*w * 3 + x * 3 + 1) = g;
			*(buf + y*w * 3 + x * 3 + 2) = b;
		}
	}
	delete[](buf_);
	buf_ = buf;
	w_ = w;
	h_ = h;
}

void img::gray_scale()
{
	for (int y = 0; y < h_; ++y) {
		for (int x = 0; x < w_; ++x) {
			int sum = 0;
			for (int i = 0; i < 3; ++i) {
				sum += *(buf_ + y*w_ * 3 + x * 3 + i);
			}
			sum /= 3;
			for (int i = 0; i < 3; ++i) {
				*(buf_ + y*w_ * 3 + x * 3 + i) = sum;
			}
		}
	}
}

void img::hog(float*& buf, int size_cell, int num_dir)
{
	int w_cell = w_ / size_cell + ((w_%size_cell) == 0 ? 0 : 1);
	int h_cell = h_ / size_cell + ((h_%size_cell) == 0 ? 0 : 1);
	buf = new float[w_cell*h_cell*num_dir];

}

float _getSquDotLineDistance(float ax, float ay, float az, float bx, float by, float bz, float x, float y, float z) {
	float dx, dy, dz, a, b, t, tx, ty, tz;
	float distance;
	dx = (bx - ax); dy = (by - ay); dz = (bz - az);
	a = dx*dx + dy*dy + dz*dz;
	b = dx * (ax - x) + dy * (ay - y) + dz * (az - z);
	if (a == 0.0f) {
		return (x - ax)*(x - ax) + (y - ay)*(y - ay) + (z - az)*(z - az);
	}
	t = -b / a;
	if (t < 0) t = 0;
	if (t > 1) t = 1;
	tx = ax + dx * t;
	ty = ay + dy * t;
	tz = az + dz * t;
	distance = (x - tx)*(x - tx) + (y - ty)*(y - ty) + (z - tz)*(z - tz);
	return distance;
}

unsigned char* img::makeColorCapsuleMap(int rf, int gf, int bf, int rt, int gt, int bt, float value_r, unsigned char* buf_base)
{
	unsigned char* buf;
	if (buf_base) {
		buf = buf_base;
	}
	else {
		buf = new unsigned char[1024 * 32];
	}
	int index = 0;
	unsigned char bit = 0;;
	for (int b = 0; b < 64; ++b) {
		for (int g = 0; g < 64; ++g) {
			for (int r = 0; r < 64; ++r) {
				float rr = _getSquDotLineDistance((float)rf, (float)gf, (float)bf, (float)rt, (float)gt, (float)bt, (float)r * 4, (float)g * 4, (float)b * 4);
				if (rr <= value_r*value_r) {
					unsigned char a = 0x01;
					int index_8 = index % 8;
					a = a << (7 - index_8);
					bit |= a;
				}
				if ((index % 8) == 7) {
					if (buf_base) {
						*(buf + index / 8) |= bit;
					}
					else {
						*(buf + index / 8) = bit;
					}
					bit = 0;
				}
				++index;
			}
		}
	}
	return buf;
}

bool checkColorCapsuleMap(unsigned char* buf, int r, int g, int b)
{
	int rr = r / 4;
	int gg = g / 4;
	int bb = b / 4;
	int index = rr + gg * 64 + bb * 64 * 64;
	unsigned char bit = *(buf + index / 8);
	unsigned char a = 0x01;
	a = a << (7 - (index % 8));
	if (bit & a) {
		return true;
	}
	else {
		return false;
	}
}

void img::binarizationColorMap(std::vector<unsigned char*>& buf)
{
	for (vector<unsigned char*>::iterator ite = buf.begin(); ite != buf.end(); ++ite) {
		for (int y = 0; y < h_; ++y) {
			for (int x = 0; x < w_; ++x) {
				rgb a = getRGB(x, y);
				if (checkColorCapsuleMap(*ite, a.r_, a.g_, a.b_)) {
					//setRGB(a.r_, a.g_, a.b_, x, y);
					setRGB(0, 255, 0, x, y);
				}
				else {
					//setRGB(255, 255, 255, x, y);
					setRGB(a.r_, a.g_, a.b_, x, y);
				}
			}
		}
	}
}

void img::fillNeighbor(int size_window, int num_neighbor, rgb color)
{
	unsigned char* buf = new unsigned char[w_*h_ * 3];
	for (int y = 0; y < h_; ++y) {
		for (int x = 0; x < w_; ++x) {
			int num = 0;
			for (int yy = y - size_window / 2; yy <= y + (size_window - 1) / 2; ++yy) {
				for (int xx = x - size_window / 2; xx <= x + (size_window - 1) / 2; ++xx) {
					if (xx < 0 || yy < 0 || xx >= w_ || yy >= h_ || (xx == 0 && yy == 0)) continue;
					if (
						*(buf_ + yy*w_ * 3 + xx * 3 + 0) == color.r_ &&
						*(buf_ + yy*w_ * 3 + xx * 3 + 1) == color.g_ &&
						*(buf_ + yy*w_ * 3 + xx * 3 + 2) == color.b_) {
						++num;
					}
				}
			}
			if (num >= num_neighbor) {
				*(buf + y*w_ * 3 + x * 3 + 0) = color.r_;
				*(buf + y*w_ * 3 + x * 3 + 1) = color.g_;
				*(buf + y*w_ * 3 + x * 3 + 2) = color.b_;
			}
			else {
				*(buf + y*w_ * 3 + x * 3 + 0) = *(buf_ + y*w_ * 3 + x * 3 + 0);
				*(buf + y*w_ * 3 + x * 3 + 1) = *(buf_ + y*w_ * 3 + x * 3 + 1);
				*(buf + y*w_ * 3 + x * 3 + 2) = *(buf_ + y*w_ * 3 + x * 3 + 2);
			}
		}
	}
	delete[](buf_);
	buf_ = buf;
}

void img::filtering3x3(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9)
{
	unsigned char* buf = new unsigned char[w_*h_ * 3];
	for (int y = 0; y < h_; ++y) {
		for (int x = 0; x < w_; ++x) {
			int value = 0;
			if (x == 0 || y == 0 || x == w_ - 1 || y == h_ - 1) {
				;
			}
			else {
				value += (int)(a1 * *(buf_ + (y - 1)*w_ * 3 + (x - 1) * 3 + 0));
				value += (int)(a2 * *(buf_ + (y - 1)*w_ * 3 + (x - 0) * 3 + 0));
				value += (int)(a3 * *(buf_ + (y - 1)*w_ * 3 + (x + 1) * 3 + 0));
				value += (int)(a4 * *(buf_ + (y - 0)*w_ * 3 + (x - 1) * 3 + 0));
				value += (int)(a5 * *(buf_ + (y - 0)*w_ * 3 + (x - 0) * 3 + 0));
				value += (int)(a6 * *(buf_ + (y - 0)*w_ * 3 + (x + 1) * 3 + 0));
				value += (int)(a7 * *(buf_ + (y + 1)*w_ * 3 + (x - 1) * 3 + 0));
				value += (int)(a8 * *(buf_ + (y + 1)*w_ * 3 + (x - 0) * 3 + 0));
				value += (int)(a9 * *(buf_ + (y + 1)*w_ * 3 + (x + 1) * 3 + 0));
			}
			if (value <= 0) value = 0;
			if (value >= 256) value = 255;
			*(buf + y*w_ * 3 + x * 3 + 0) = value;
			*(buf + y*w_ * 3 + x * 3 + 1) = value;
			*(buf + y*w_ * 3 + x * 3 + 2) = value;
		}
	}
	delete[](buf_);
	buf_ = buf;
}

int img::haarlike(img* image_haarlike, int x, int y)
{
	if (image_haarlike == NULL || x < 0 || y < 0 || x >= w_ - image_haarlike->w_ || y >= h_ - image_haarlike->h_) return -1;
	int sum = 0;
	for (int yy = 0; yy < image_haarlike->h_; ++yy) {
		for (int xx = 0; xx < image_haarlike->w_; ++xx) {
			int g = image_haarlike->getRGB(xx, yy).g_;
			if (g == 200) {
				continue;
			}
			else if (g == 0) {
				sum -= getRGB(x + xx, y + yy).g_;
			}
			else if (g == 255) {
				sum += getRGB(x + xx, y + yy).g_;
			}
		}
	}
	return sum;
}

void img::haarlike(img* image_haarlike, int& x, int& y, int& sum, float& a)
{
	int max = -1;
	int xxx = 0, yyy = 0;
	float aaa = 1.0f;
	for (float aa = 1.0f; aa < a; aa *= 1.1f) {
		img image = *image_haarlike;
		image.scale(aa);
		for (int yy = 0; yy <= h_ - image.h_; ++yy) {
			for (int xx = 0; xx <= w_ - image.w_; ++xx) {
				int v = haarlike(&image, xx, yy);
				if (v > max) {
					max = v;
					xxx = xx;
					yyy = yy;
					aaa = aa;
				}
			}
		}
	}
	x = xxx;
	y = yyy;
	sum = max;
	a = aaa;
}

void img::line(int x1, int y1, int x2, int y2, rgb color)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = 1;
	int sy = 1;
	if (x1 > x2) sx = -1;
	if (y1 > y2) sy = -1;
	int err = dx - dy;

	while (1) {
		setRGB(color.r_, color.g_, color.b_, x1, y1);
		if (x1 == x2 && y1 == y2) return;
		int e2 = 2 * err;
		if (e2 > -dy) {
			err = err - dy;
			x1 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y1 += sy;
		}
	}
}

void img::box(int x1, int y1, int x2, int y2, rgb color, bool fill)
{
	if (fill) {
		int sx = (x1 > x2 ? -1 : 1);
		int sy = (y1 > y2 ? -1 : 1);
		for (int y = y1; y != y2; y += sy) {
			for (int x = x1; x != x2; x += sx) {
				setRGB(color.r_, color.g_, color.b_, x, y);
			}
		}
	}
	else {
		line(x1, y1, x1, y2, color);
		line(x1, y1, x2, y1, color);
		line(x2, y2, x2, y1, color);
		line(x2, y2, x1, y2, color);
	}

}

void img::scale(float a)
{
	int w = (int)(w_*a);
	int h = (int)(h_*a);
	unsigned char* buf = new unsigned char[w*h * 3];
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			int yy = y*h_ / h;
			int xx = x*w_ / w;
			unsigned char* c = (buf_ + yy*w_ * 3 + xx * 3);
			*(buf + y*w* 3 + x * 3 + 0) = *(c + 0);
			*(buf + y*w* 3 + x * 3 + 1) = *(c + 1);
			*(buf + y*w* 3 + x * 3 + 2) = *(c + 2);
		}
	}
	w_ = w;
	h_ = h;
	delete[](buf_);
	buf_ = buf;
}

void img::expandHistgram()
{
	int min = 255;
	int max = 0;
	for (int y = 0; y < h_; ++y) {
		for (int x = 0; x < w_; ++x) {
			rgb c = getRGB(x, y);
			if (min > c.r_) min = c.r_;
			if (max < c.r_) max = c.r_;
		}
	}
	int a = max - min;
	for (int y = 0; y < h_; ++y) {
		for (int x = 0; x < w_; ++x) {
			rgb c = getRGB(x, y);
			int v = (c.r_ - min) * 255 / a;
			setRGB(v, v, v, x, y);
		}
	}
}

img* img::getPart(int x, int y, int w, int h)
{
	int ww = w;
	int hh = h;
	if (w_ - x + 1 < ww) ww = w_ - x + 1;
	if (h_ - y + 1 < hh) hh = h_ - y + 1;

	unsigned char* buf = new unsigned char[ww*hh * 3];
	for (int yy = 0; yy < hh; ++yy) {
		for (int xx = 0; xx < ww; ++xx) {
			rgb c = getRGB(xx + x, yy + y);
			*(buf + yy*ww * 3 + xx * 3 + 0) = c.r_;
			*(buf + yy*ww * 3 + xx * 3 + 1) = c.g_;
			*(buf + yy*ww * 3 + xx * 3 + 2) = c.b_;
		}
	}
	img* image = new img(ww, hh, buf);
	delete[](buf);
	return image;
}

void img::setPart(int x, int y, img* source, int x_s, int y_s, int w_s, int h_s)
{
	for (int yy = 0; yy < h_s; ++yy) {
		if (yy + y_s >= source->h_ || y + yy >= h_) break;
		for (int xx = 0; xx < w_s; ++xx) {
			if (xx + x_s >= source->w_ || x + xx >= w_) break;
			setRGB(source->getRGB(xx + x_s, yy + y_s), xx + x, yy + y);
		}
	}
}

int img::getMatchValueSquare(int x, int y, img* image_smaller)
{
	int ret = 0;
	if (x + image_smaller->w_ > w_ || y + image_smaller->h_ > h_) return -1;
	for (int yy = 0; yy < image_smaller->h_; ++yy) {
		for (int xx = 0; xx < image_smaller->w_; ++xx) {
			rgb c1 = getRGB(x + xx, y + yy);
			rgb c2 = image_smaller->getRGB(xx, yy);
			ret += (c1.r_ - c2.r_)*(c1.r_ - c2.r_) + (c1.g_ - c2.g_)*(c1.g_ - c2.g_) + (c1.b_ - c2.b_)*(c1.b_ - c2.b_);
		}
	}
	return ret;
}

void img::gaussian_filter(double sig)
{
	double* sum_mat = new double[w_*h_*3];
	int window_size = (int)round((3.0*sig) * 2 + 1) | 1;   //ëãïù
	double *mask = new double[window_size];
	double sum = -1.0;
	sig = -0.5 / (sig*sig);
	for (int i = 0; i <= window_size / 2; i++) {
		double t = exp(sig*i*i);
		mask[(window_size / 2 + i)] = t;
		sum += mask[(window_size / 2 + i)] * 2;
	}
	//ê≥ãKâª
	sum = 1.0 / sum;
	for (int i = 0; i <= window_size / 2; i++) {
		mask[(window_size / 2 + i)] = mask[(window_size / 2 - i)] = mask[(window_size / 2 + i)] * sum;
	}
	//êÇíºï˚å¸
	for (int x = 0; x<w_; x++) {
		for (int y = 0; y<h_; y++) {
			sum_mat[y*w_ * 3 + x * 3 + 0] = 0.0;
			sum_mat[y*w_ * 3 + x * 3 + 1] = 0.0;
			sum_mat[y*w_ * 3 + x * 3 + 2] = 0.0;
			for (int i = 0; i<window_size; i++) {
				int yy = y + i - window_size / 2;
				if (yy < 0 || yy >= h_) continue;
				sum_mat[y*w_*3 + x*3 + 0] += mask[i] * getRGB(x, yy).r_;
				sum_mat[y*w_*3 + x*3 + 1] += mask[i] * getRGB(x, yy).g_;
				sum_mat[y*w_*3 + x*3 + 2] += mask[i] * getRGB(x, yy).b_;
			}
		}
	}
	//êÖïΩï˚å¸
	for (int x = 0; x<w_; x++) {
		for (int y = 0; y<h_; y++) {
			double r = 0.0;
			double g = 0.0;
			double b = 0.0;
			for (int i = 0; i<window_size; i++) {
				int xx = x + i - window_size / 2;
				if (xx < 0 || xx >= w_) continue;
				r += mask[i] * sum_mat[y*w_ * 3 + xx * 3 + 0];
				g += mask[i] * sum_mat[y*w_ * 3 + xx * 3 + 1];
				b += mask[i] * sum_mat[y*w_ * 3 + xx * 3 + 2];
			}
			*(buf_ + y*w_ * 3 + x * 3 + 0) = r;
			*(buf_ + y*w_ * 3 + x * 3 + 1) = g;
			*(buf_ + y*w_ * 3 + x * 3 + 2) = b;
		}
	}
	delete[](mask);
	delete[](sum_mat);
}

img_calc* img::subtract(img* a)
{
	img_calc* ret = new img_calc(w_, h_, NULL);
	for (int y = 0; y < h_; ++y) {
		for (int x = 0; x < w_; ++x) {
			ret->set(x, y, getRGB(x, y).r_ - a->getRGB(x, y).r_);
		}
	}
	return ret;
}

img_calc::img_calc() :
	w_(0), h_(0), buf_(NULL)
{

}

img_calc::img_calc(int w, int h, float* buf) :
	w_(w), h_(h)
{
	buf_ = new float[w_*h_];
	if (buf) {
		memcpy(buf_, buf, w_*h_);
	}
}

img_calc::~img_calc()
{
	if (buf_) delete[](buf_);
}

void img_calc::set(int x, int y, float v)
{
	*(buf_ + y*w_ + x) = v;
}std::vector<unsigned char> save_to_memory();

float img_calc::get(int x, int y)
{
	return *(buf_ + y*w_ + x);
}

void img::output_jpeg(unsigned char* buf, int* size)
{
	jpge::compress_image_to_jpeg_file_in_memory(buf, *size, w_, h_, 3, buf_);
}

void rotate_t(int& x, int& y, float sheta)
{
	double xx = x*cos(sheta) - y*sin(sheta);
	double yy = x*sin(sheta) + y*cos(sheta);
	x = (int)round(xx);
	y = (int)round(yy);
}

void img::rotate(float sheta, bool flag_scale, rgb color_fill)
{
	if (buf_ == NULL) return;
	int w = 0;
	int h = 0;
	if (flag_scale) {
		int vx0 = -w_ / 2.0f;
		int vy0 = -h_ / 2.0f;
		int vx1 = w_ / 2.0f;
		int vy1 = -h_ / 2.0f;
		int vx2 = -w_ / 2.0f;
		int vy2 = h_ / 2.0f;
		int vx3 = w_ / 2.0f;
		int vy3 = h_ / 2.0f;
		rotate_t(vx0, vy0, sheta);
		rotate_t(vx1, vy1, sheta);
		rotate_t(vx2, vy2, sheta);
		rotate_t(vx3, vy3, sheta);

		int min_x = min(min(min(vx0, vx1), vx2), vx3);
		int min_y = min(min(min(vy0, vy1), vy2), vy3);

		w = -2 * min_x;
		h = -2 * min_y;
	}
	else {
		w = w_;
		h = h_;
	}

	rgb color_null(0, 200, 0);
	unsigned char* buf = new unsigned char[w*h * 3];
	for (int y = 0; y < h; ++y) {
		rgb color_fill_w1 = color_null;
		rgb color_fill_w2 = color_null;
		for (int x = 0; x < w; ++x) {
			rgb color = color_null;
			int vx = x - w / 2;
			int vy = y - h / 2;
			rotate_t(vx, vy, -sheta);
			int xx = vx + w / 2;
			int yy = vy + h / 2;
			if (xx >= 0 && xx < w_ && yy >= 0 && yy < h_) {
				color = getRGB(xx, yy);
				if (color_fill_w1 == color_null) {
					color_fill_w1 = color;
				}
				color_fill_w2 = color;
			}
			*(buf + y*w * 3 + x * 3 + 0) = color.r_;
			*(buf + y*w * 3 + x * 3 + 1) = color.g_;
			*(buf + y*w * 3 + x * 3 + 2) = color.b_;
		}

		for (int x = 0; x < w; ++x) {
			if (
				*(buf + y*w * 3 + x * 3 + 0) == color_null.r_ &&
				*(buf + y*w * 3 + x * 3 + 1) == color_null.g_ &&
				*(buf + y*w * 3 + x * 3 + 2) == color_null.b_) {
				*(buf + y*w * 3 + x * 3 + 0) = color_fill_w1.r_;
				*(buf + y*w * 3 + x * 3 + 1) = color_fill_w1.g_;
				*(buf + y*w * 3 + x * 3 + 2) = color_fill_w1.b_;
			}
			else {
				break;
			}
		}
		for (int x = w-1; x >= 0; --x) {
			if (
				*(buf + y*w * 3 + x * 3 + 0) == color_null.r_ &&
				*(buf + y*w * 3 + x * 3 + 1) == color_null.g_ &&
				*(buf + y*w * 3 + x * 3 + 2) == color_null.b_) {
				*(buf + y*w * 3 + x * 3 + 0) = color_fill_w2.r_;
				*(buf + y*w * 3 + x * 3 + 1) = color_fill_w2.g_;
				*(buf + y*w * 3 + x * 3 + 2) = color_fill_w2.b_;
			}
			else {
				break;
			}
		}
	}

	delete[](buf_);
	buf_ = buf;
	w_ = w;
	h_ = h;
}
