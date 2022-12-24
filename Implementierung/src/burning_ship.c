#include <complex.h>
#include <stddef.h>
#include <math.h>

void burning_ship(float complex start, size_t width, size_t height,
		  float res, unsigned n, unsigned char* img);

static inline float
scale_coordx(float x, size_t width);

static inline float
scale_coordy(float y, size_t height);

//TODO res
void burning_ship(float complex start, size_t width, size_t height,
		  float res, unsigned n, unsigned char* img)
{
	float zx = creal(start);
	float zy = cimag(start);
	float xtmp;
	for (size_t h = 0; h < height; h++) {
		for (size_t w = 0; w < width; w++) {
			float x = scale_coordx(w, width);
			float y = scale_coordy(h, height);
			unsigned i = 0;
			for (; i < n && zx*zx + zy*zy < 4; i++) {
				xtmp = zx*zx - zy*zy + x;
				zy = fabs(2*zx*zy) + y;
				zx = xtmp;
			}
			if (i == n) {
				img[w + h * height] = 0xff;
			} else { //TODO color by number of iterations?
				img[w + h * height] = 0;
			}
		}
	}
}

static inline float
scale_coordx(float x, size_t width) {
	return -2.5 + 3.5 * (x / width);
}

static inline float
scale_coordy(float y, size_t height) {
	return -1 + 2 * (y / height);
}
