#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "img.h"

#define PI 3.141592

int main(int argc, char** argv)
{
	int w0 = 36;
	int h0 = 50;
	int col0 = 28;
	char buf_name[80];

	int size_image = 1024;
	img image_out(size_image, size_image, NULL);
	for(int i=0;i<231;++i){
		int col = i % col0;
		int row = i / col0;
		sprintf(buf_name, "assets_angled_36x50/isometric_pixel_flat_%04d.png", i);
		img image;
		image.load(buf_name);
		image_out.setPart(col * w0, row * h0, &image, 0, 0, w0, h0);
	}
	image_out.save("a.bmp");
	return 0;
	/*
	img image(100, 100, NULL);
	image.box(0, 0, 100, 100, rgb(255, 255, 255), true);
	image.line(0, 10, 100, 10, rgb(0, 0, 0));
	image.line(10, 0, 10, 100, rgb(0, 0, 0));
	image.save("a.bmp");

	img b;
	b.load_3byte_file("./b.bmp");
	b.line(0, 10, 100, 10, rgb(0, 0, 0));
	b.line(10, 0, 10, 100, rgb(0, 0, 0));
	b.box(0, 0, 1000, 1000, rgb(255, 255, 255), true);
	b.save("c.bmp");
	return 0;
	*/
}
