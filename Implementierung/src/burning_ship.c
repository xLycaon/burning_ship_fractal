#include <complex.h>
#include <stddef.h>
#include <math.h>
#include <emmintrin.h>
#include <tmmintrin.h>

#include "burning_ship.h"
#include "utils.h"

#define SCALEFACT (2.0f)
#define SCALERES(X, SIZE, RES) ( \
(float) (                        \
            (((float) -(RES)) + SCALEFACT * ((float) (RES))) * ((float) (X) / (float) (SIZE)) \
        ) \
)
#define BW_CLR(ITER, N) ( (ITER) > (N)/2 ? 0x00 : 0xff )
#define SCALE_CLR(ITER, N) ( (unsigned char) ((float)(ITER)/(float)(N) * (float) (TOTAL_COLORS-1)) )

//TODO alternative?
// TODO too expensive? Due to SEQUENCE
#define SCALERES_PS(X, L, RES, C) (_mm_setr_ps( \
                                                SCALERES(X, L, RES) + C, \
                                                SCALERES(X+1, L, RES) + C, \
                                                SCALERES(X+2, L, RES) + C, \
                                                SCALERES(X+3, L, RES) + C))

#define SCALE_CLR_PS(I_VEC, N) _mm_set_epi32( \
    SCALE_CLR(HI64(I_VEC[1]), N),                \
    SCALE_CLR(LO64(I_VEC[1]), N), \
    SCALE_CLR(HI64(I_VEC[0]), N),                \
    SCALE_CLR(LO64(I_VEC[0]), N) \
)
#define PSTEP 4

//  (.) (., ., ., x) -> (0) (0, 0, 0, x)
#define LO64(X) (0x00000000000000ff & X)

// (., ., ., x) (.)  -> (0) (0, 0, 0, x)
#define HI64(X) ((0x000000ff00000000 & X) >> 32)

// (0, 0, 0, x) (0, 0, 0, y) | (0, 0, 0, m) (0, 0, 0, n) -> (0) (0) | (0) (x, y, m, n)
#define HLH_LLH_LHL_MSK (0x0C080400) //TODO

// Sets a chunk to zero (.) -> (0)
#define Z_MSK (0x80808080)

#define IS_NALGND(PTR) ((size_t) (PTR) & 0xf)
#define FOUR_DIV(X) ((X) & ~3ul)

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
            img[index] = SCALE_CLR(i, n);
		}
	}
}

static inline void burning_ship_step(size_t index, float cr, float ci, unsigned n, unsigned char* img) {

    float zr = 0.0f;
    float zi = 0.0f;

    unsigned i = 0;
    for (; i < n && zr*zr + zi*zi <= LIMIT; i++) {
        float zrtmp = zr*zr + zi*zi + cr;
        zi = 2.0f * fabsf(zr*zi) + ci;
        zr = zrtmp;
    }

    img[index] = SCALE_CLR(i, n);
}

//TODO iterations 4 vs 8 vs 16
//TODO avoiding integer to float casting
//TODO segfault due to missing alignment

//TODO IDEA: Precomputing cr for a whole row then iterate through cis using precomputed crs
void burning_ship_V1(float complex start, size_t width, size_t height,
                  float res, unsigned n, unsigned char* img)
{
    float s_ci = cimagf(start);
    float s_cr = crealf(start);

    __m128 abs_msk = _mm_set1_ps(-0.0f);
    __m128 two = _mm_set1_ps(2.0f);
    __m128 limit = _mm_set1_ps(LIMIT);
    __m128i mv2lo32msk = _mm_set_epi32((int) Z_MSK,
                                       (int) Z_MSK,
                                       (int) Z_MSK,
                                       (int) HLH_LLH_LHL_MSK);

    size_t index;

    for (size_t h = 0; h < height; h++) {

        __m128 ci = _mm_set1_ps(SCALERES(h, height, res) + s_ci);

        size_t w = 0;
        for (; w < FOUR_DIV(width); w+=PSTEP) {

            // TODO
            __m128 cr = SCALERES_PS(w, width, res, s_cr);// TODO reusable over h iterations?

            __m128 zr = _mm_setzero_ps();
            __m128 zi = _mm_setzero_ps();

            __m128 zrzr = _mm_setzero_ps();
            __m128 zizi = _mm_setzero_ps();

            __m128 ngt_lim;

            __m128i one = _mm_set1_epi32(1);

            unsigned i = 0;
            __m128i i_vec = _mm_setzero_si128(); // i = 0
            do
            {
                __m128 zrtmp = zrzr - zizi + cr;                       // zr(n+1) := zr*zr - zi*zi + cr
                zi = two * _mm_andnot_ps(abs_msk, zr*zi) + ci;  // zi := 2 * |zr*zi| + ci
                zr = zrtmp;                                           // zr := zr(n+1)

                ngt_lim = _mm_cmpgt_ps(limit, zrzr + zizi); // zr^2+zi^2<=limit

                i_vec += _mm_castps_si128(ngt_lim) & one; // i++

                zrzr = zr * zr; // zr*zr := zr(n+1) * zr(n+1)
                zizi = zi * zi; // zi*zi := zi(n+1) * zi(n+1)

            } while( _mm_movemask_ps(ngt_lim) & (int) 0xf && i++ < n); //TODO i ?

            index = w + h * width;

            // Stores bytes in pval at the end of each 4 segments into to lower 32 bits of pvals
            __m128i pvals = SCALE_CLR_PS(i_vec, n);
            pvals = _mm_shuffle_epi8(pvals, mv2lo32msk);

            _mm_storeu_si32((__m128i_u *) (img+index), pvals);
        }

        for (; w < width; w++)
            burning_ship_step(index++, SCALERES(w, width, res), SCALERES(h, height, res), n, img);
    }
}
