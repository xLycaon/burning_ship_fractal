#include <complex.h>
#include <stddef.h>
#include <math.h>

#include "burning_ship.h"
#include "utils.h"

#define SCALEFACT (2.0f)
#define SCALERES(X, SIZE, RES) ((float) ((-((float) RES) + SCALEFACT * ((float) RES)) * ((float) X / (float) SIZE)))
#define BW_CLR(ITER, N) ( (ITER) > (N)/2 ? 0x00 : 0xff )

// We can show that lim sup n -> ∞ |zn| ≤ 2
// Therefore zr^2 + zi^2 ≤ 4
#define LIMIT 4

// c = cr + I * ci
// Z(n+1) = (|Re(Z(n))| + I * |Imag(Z(n))|)^2 + c = (|zr| + I * |zi|)^2 + (cr + I * ci)
// Re(Z(n+1)) = (zr)^2 - (zi)^2 + cr
// Img(Z(n+1)) = 2|zr||zi| + ci = 2|zrzi| + ci
void burning_ship(float complex start, size_t width, size_t height,
                  float res, unsigned n, unsigned char* img)
{
	float zr, zi;
	float cr, ci;

    float s_cr = crealf(start);
    float s_ci = cimagf(start);

	for (size_t h = 0; h < height; h++) {

        // ci := y-coord + offset
        ci = SCALERES(h, height, res) + s_ci;

		for (size_t w = 0; w < width; w++) {

			// cr := x-coord + offset
			cr = SCALERES(w, width, res) + s_cr;

            zr = 0.0f;
            zi = 0.0f;

			unsigned i = 0;
			for (; i < n && zr*zr + zi*zi <= LIMIT; i++) {
				float zrtmp = zr*zr - zi*zi + cr;
				zi = 2.0f * fabsf(zr*zi) + ci;
				zr = zrtmp;
			}

            size_t index = BMDIM(w) + h * BMDIM(width);
            img[index] = BW_CLR(i, n);
		}
	}
}
