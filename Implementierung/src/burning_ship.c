#include <complex.h>
#include <stddef.h>
#include <math.h>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <xmmintrin.h>

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

#define PSTEP16 0x10
#define PSTEP 4
#define VEC_COUNT 4

//  (.) (., ., ., x) -> (0) (0, 0, 0, x)
#define LO64(X) (0x00000000000000ff & X)

// (., ., ., x) (.)  -> (0) (0, 0, 0, x)
#define HI64(X) ((0x000000ff00000000 & X) >> 32)

#define MV_4_SEG32 (0x0c080400)

// Sets a chunk to zero (.) -> (0)
#define Z_MSK (0x80808080)

#define IS_NALGND(PTR) ((size_t) (PTR) & 0xf)
#define FOUR_DIV(X) ((X) & ~3ul)
#define HEX_DIV(X) ((X) & ~0xf) //TODO

#define IS_DIV4(X) ((int) (X) & 03)
#define IS_DIV16(X) ((int) (X) & 0xf)

// We can show that lim sup n -> ∞ |zn| ≤ 2
// Therefore zr^2 + zi^2 ≤ 4
#define LIMIT 4

static inline __attribute__((always_inline)) void burning_ship_step(size_t index, float cr, float ci, unsigned n, unsigned char* img) {

    float zr = 0.0f;
    float zi = 0.0f;

    unsigned i = 0;
    for (; i < n && zr*zr + zi*zi <= LIMIT; i++) {
        float zrtmp = zr*zr - zi*zi + cr;
        zi = 2.0f * fabsf(zr*zi) + ci;
        zr = zrtmp;
    }

    img[index] = SCALE_CLR(i, n);
}

// c = cr + I * ci
// Z(n+1) = (|Re(Z(n))| + I * |Imag(Z(n))|)^2 + c = (|zr| + I * |zi|)^2 + (cr + I * ci)
// Re(Z(n+1)) = (zr)^2 - (zi)^2 + cr
// Img(Z(n+1)) = 2|zr||zi| + ci = 2|zrzi| + ci
void burning_ship(float complex start, size_t width, size_t height,
                  float res, unsigned n, unsigned char* img)
{
	float cr, ci;

    float s_cr = crealf(start);
    float s_ci = cimagf(start);

	for (size_t h = 0; h < height; h++) {

        // ci := y-coord + offset
        ci = SCALERES(h, height, res) + s_ci;

		for (size_t w = 0; w < width; w++) {

			// cr := x-coord + offset
			cr = SCALERES(w, width, res) + s_cr;
            size_t index = BMDIM(w) + h * BMDIM(width);

            burning_ship_step(index, cr, ci, n, img);
		}
	}
}

//TODO incorrect results when width is not a multiple of 4
void burning_ship_V1(float complex start, size_t width, size_t height,
                     float res, unsigned n, unsigned char* img) {
    float s_ci = cimagf(start);
    float s_cr = crealf(start);

    __m128 abs_msk = _mm_set1_ps(-0.0f);
    __m128 two = _mm_set1_ps(2.0f);
    __m128 limit = _mm_set1_ps(LIMIT);
    __m128i mv2lo32msk = _mm_set_epi32((int) Z_MSK,
                                       (int) Z_MSK,
                                       (int) Z_MSK,
                                       (int) MV_4_SEG32);

    size_t index = 0;

    for (size_t h = 0; h < height; h++) {

        __m128 ci = _mm_set1_ps(SCALERES(h, height, res) + s_ci);

        size_t w = 0;
        for (; w < FOUR_DIV(width); w += PSTEP) {

            __m128 cr = SCALERES_PS(w, width, res, s_cr);
            __m128 zr = _mm_setzero_ps();
            __m128 zi = _mm_setzero_ps();

            __m128 zrzr = _mm_setzero_ps();
            __m128 zizi = _mm_setzero_ps();

            __m128 ngt_lim;

            __m128i one = _mm_set1_epi32(1);

            unsigned i = 0;
            __m128i i_vec = _mm_setzero_si128();
            do {
                __m128 zrtmp = zrzr - zizi + cr;
                zi = two * _mm_andnot_ps(abs_msk, zr * zi) + ci;
                zr = zrtmp;

                ngt_lim = _mm_cmpgt_ps(limit, zrzr + zizi);
                i_vec += _mm_castps_si128(ngt_lim) & one;

                zrzr = zr * zr;
                zizi = zi * zi;
            } while (_mm_movemask_ps(ngt_lim) & (int) 0xf && i++ < n);

            index = w + h * width;

            __m128i pvals = SCALE_CLR_PS(i_vec, n);
            pvals = _mm_shuffle_epi8(pvals, mv2lo32msk);

            _mm_storeu_si32((__m128i_u *) (img + index), pvals);
        }

        //TODO
        for (; w < width; w++)
            burning_ship_step(index++, SCALERES(w, width, res), SCALERES(h, height, res), n, img);
    }
}

//TODO incorrect results if width is not a multiple of 16
void burning_ship_V2(float complex start, size_t width, size_t height,
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
                                       (int) MV_4_SEG32);

    __m128i mv2hilo32msk = _mm_set_epi32((int) Z_MSK,
                                         (int) Z_MSK,
                                         (int) MV_4_SEG32,
                                         (int) Z_MSK);

    __m128i mv2lohi32msk = _mm_set_epi32((int) Z_MSK,
                                         (int) MV_4_SEG32,
                                         (int) Z_MSK,
                                         (int) Z_MSK);

    __m128i mv2hihi32msk = _mm_set_epi32((int) MV_4_SEG32,
                                         (int) Z_MSK,
                                         (int) Z_MSK,
                                         (int) Z_MSK);

    size_t index = 0;

    // TODO precalculate cr and ci ?
    for (size_t h = 0; h < height; h++) {

        __m128 ci = _mm_set1_ps(SCALERES(h, height, res) + s_ci);

        size_t w = 0;
        for (; w < HEX_DIV(width); w+=PSTEP16) {

            //TODO
            __m128 cr[VEC_COUNT] = {
                    SCALERES_PS(w, width, res, s_cr),
                    SCALERES_PS(w+VEC_COUNT, width, res, s_cr),
                    SCALERES_PS(w+VEC_COUNT*2, width, res, s_cr),
                    SCALERES_PS(w+VEC_COUNT*3, width, res, s_cr)
            };

            __m128 zr[VEC_COUNT] = {0};
            __m128 zi[VEC_COUNT] = {0};

            __m128 ngt_lim[VEC_COUNT];
            __m128i ngt_lim_i;

            __m128i one[4] = {
                    _mm_set1_epi32(1),
                    _mm_set1_epi32(1),
                    _mm_set1_epi32(1),
                    _mm_set1_epi32(1)
            };

            unsigned i = 0;
            __m128i i_vec[VEC_COUNT] = {0};

            do
            {
                for (size_t j = 0; j < VEC_COUNT; j++) {
                    __m128 zrtmp = zr[j]*zr[j] - zi[j]*zi[j] + cr[j];
                    zi[j] = two * _mm_andnot_ps(abs_msk, zr[j]*zi[j]) + ci;
                    zr[j] = zrtmp;

                    ngt_lim[j] = _mm_cmpgt_ps(limit, zr[j]*zr[j] + zi[j]*zi[j]);
                    i_vec[j] += _mm_castps_si128(ngt_lim[j]) & one[j]; //TODO can be optimised
                }

                //TODO
                ngt_lim_i = _mm_shuffle_epi8(_mm_castps_si128(ngt_lim[0]), mv2lo32msk);
                ngt_lim_i |= _mm_shuffle_epi8(_mm_castps_si128(ngt_lim[1]), mv2hilo32msk);
                ngt_lim_i |= _mm_shuffle_epi8(_mm_castps_si128(ngt_lim[2]), mv2lohi32msk);
                ngt_lim_i |= _mm_shuffle_epi8(_mm_castps_si128(ngt_lim[3]), mv2hihi32msk);

            } while( _mm_movemask_epi8(ngt_lim_i) & (int) 0xff && i++ < n);

            index = w + h * width;

            __m128i pvals[VEC_COUNT] = {
                    SCALE_CLR_PS(i_vec[0], n),
                    SCALE_CLR_PS(i_vec[1], n),
                    SCALE_CLR_PS(i_vec[2], n),
                    SCALE_CLR_PS(i_vec[3], n)
            };

            //TODO
            __m128i pvals_i = _mm_shuffle_epi8(pvals[0], mv2lo32msk);
            pvals_i |= _mm_shuffle_epi8(pvals[1], mv2hilo32msk);
            pvals_i |= _mm_shuffle_epi8(pvals[2], mv2lohi32msk);
            pvals_i |= _mm_shuffle_epi8(pvals[3], mv2hihi32msk);

            _mm_storeu_si128((__m128i_u *) (img+index), pvals_i);
        }

        //TODO
        for (; w < width; w++)
            burning_ship_step(index++, SCALERES(w, width, res), SCALERES(h, height, res), n, img);
    }
}
