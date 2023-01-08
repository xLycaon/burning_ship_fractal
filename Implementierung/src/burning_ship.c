#include <complex.h>
#include <stddef.h>
#include <math.h>

#include "burning_ship.h"
#include "utils.h" //TODO BYTESPP

//TODO scaling
#define SCALEX 2.5f
#define SCALEY 2.5f
#define SCOFFX 1.0f
#define SCOFFY 1.0f
#define LIMIT 10000

// [0, 1]
static inline float
scale_coordx(float x, size_t width, float res) {
	return -SCALEX+ (SCALEX+SCOFFX) * (x / (float) width);
}

static inline float
scale_coordy(float y, size_t height, float res) {
	return -SCALEY + (SCALEY+SCOFFY) * (y / (float) height);
}

//TODO better color conversion
//TODO too lisp like
static unsigned char
scale_gscolor(unsigned iter_n, unsigned n) {
	return (unsigned char) (((float) 0xFF) - ( ((float) 0xFF) * ( (float) (iter_n % (n+1)) / (float) n )));
}

// c = cr + I * ci
// Z(n+1) = (|Re(Z(n))| + I * |Imag(Z(n))|)^2 + c = (|zr| + I * |zi|)^2 + (cr + I * ci)
// Re(Z(n+1)) = (zr)^2 - (zi)^2 + cr
// Img(Z(n+1)) = 2|zr||zi| + ci = 2|zrzi| + ci
//TODO scaling
void burning_ship(float complex start, size_t width, size_t height,
		  float res, unsigned n, unsigned char* img)
{
	(void)res;
	float zr, zi;
	float cr, ci;
	float complex zrtmp;
	for (size_t h = 0; h < height; h++) {
		for (size_t w = 0; w < width; w++) {
			// Z(0) = 0
			// Z(1) = (0+0)^2+c = cr + I * ci
			zr = 0.0f;
			zi = 0.0f;

			// cr := x-coord + offset
			// ci := y-coord + offset
			cr = scale_coordx(w, width, res) + creal(start);
			ci = scale_coordy(h, height, res) + cimag(start);

			unsigned i = 0;
			for (; i < n && zr*zr + zi*zi <= LIMIT; i++) {
				zrtmp = zr*zr - zi*zi + cr;
				zi = 2.0f * fabsf(zr*zi) + ci;
				zr = zrtmp;
			}

			// COLORING PIXELS
            unsigned char gscolor = scale_gscolor(i, n);
            size_t index = BMDIM(w) + h * BMDIM(width);
            img[index] = gscolor;
            img[index + 1] = gscolor;
            img[index + 2] = gscolor;
		}
	}
}

