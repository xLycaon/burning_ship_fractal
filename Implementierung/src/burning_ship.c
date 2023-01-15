#include <complex.h>
#include <stddef.h>
#include <math.h>
#include <emmintrin.h>
#include <tmmintrin.h>

#include "burning_ship.h"
#include "utils.h"

#define SCALEFACT (2.0f)
#define SCALERES(X, SIZE, RES) ((float) ((-((float) RES) + SCALEFACT * ((float) RES)) * ((float) X / (float) SIZE)))
#define BW_CLR(ITER, N) ( (ITER) > (N)/2 ? 0x00 : 0xff )
#define SCALE_CLR(ITER, N) ( (unsigned char) ((float)ITER/(float)N * (TOTAL_COLORS-1)) )

// TODO too expensive?
#define SCALERES_PS(X, L, RES, C) (_mm_setr_ps( \
                                                SCALERES(X, L, RES) + C, \
                                                SCALERES(X+1, L, RES) + C, \
                                                SCALERES(X+2, L, RES) + C, \
                                                SCALERES(X+3, L, RES) + C))

#define PSTEP 4

//  (.) (., ., ., x) -> (0) (0, 0, 0, x)
#define LO64(X) (0x00000000000000ff & X) //TODO

// (., ., ., x) (.)  -> (0) (0, 0, 0, x)
#define HI64(X) ((0x000000ff00000000 & X) >> 32) //TODO

// (0, 0, 0, x) (0, 0, 0, y) | (0, 0, 0, m) (0, 0, 0, n) -> (0) (0) | (0) (x, y, m, n)
#define HLH_LLH_LHL_MSK (0x0C080400) //TODO

// Preserves a chunk (.) -> (.)
#define PRS_MSK (0x80808080)

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
    __m128 limit = _mm_set1_ps(4.0f);
    __m128i mv2lo32msk = _mm_set_epi32((int) PRS_MSK,
                                       (int) PRS_MSK,
                                       (int) PRS_MSK,
                                       (int) HLH_LLH_LHL_MSK);

    for (size_t h = 0; h < height; h++) {

        __m128 ci = _mm_set1_ps(SCALERES(h, height, res) + s_ci);

        for (size_t w = 0; w < width; w+=PSTEP) {

            __m128 cr = SCALERES_PS(w, width, res, s_cr);// TODO reusable over h iterations?

            __m128 zr = _mm_setzero_ps();
            __m128 zi = _mm_setzero_ps();

            __m128 zrzr = _mm_setzero_ps();
            __m128 zizi = _mm_setzero_ps();

            __m128 iter_cmp;

            unsigned i = 0;
            __m128i i_vec = _mm_setzero_si128(); // i = 0
            __m128i one = _mm_set1_epi32(1);
            do {

                // zr := zr*zr - zi*zi + cr
                __m128 zrtmp = zrzr - zizi + cr;

                // zi := 2 * |zr*zi| + ci
                zi = _mm_mul_ps(zi, zr);
                zi = _mm_andnot_ps(abs_msk, zi);
                zi = _mm_mul_ps(zi, two);
                zi = _mm_add_ps(zi, ci);

                zr = zrtmp;

                zrzr = _mm_mul_ps(zr, zr);
                zizi = _mm_mul_ps(zi, zi);

                // zrzr + zizi ≤ LIMIT ?
                __m128 iter_val = _mm_add_ps(zrzr, zizi);
                iter_cmp = _mm_cmpgt_ps(limit, iter_val);
                __m128i iter_cmpi = _mm_castps_si128(iter_cmp); //TODO avoidable type casting?

                // i++
                one = _mm_and_si128(iter_cmpi, one); //TODO avoidable type casting?
                i_vec = _mm_add_epi32(i_vec,one);

            } while( _mm_movemask_ps(iter_cmp) & (int) 0xf && i++ < n);

            size_t index = w + h * width;
            //TODO expensive and naive
            // (0, 0, 0, x) (0, 0, 0, y) (0, 0, 0, z) (0, 0, 0, b) -> (x, y, z, b) gehts auch mit 8? (a, b, c, d, e, f, g, h)
            int ione = BW_CLR(HI64(i_vec[1]), n);
            int itwo = BW_CLR(LO64(i_vec[1]), n);
            int ithree = BW_CLR(HI64(i_vec[0]), n);
            int ifour = BW_CLR(LO64(i_vec[0]), n);
            __m128i pvals = _mm_set_epi32(ione,
                                           itwo,
                                           ithree,
                                           ifour);
            pvals = _mm_shuffle_epi8(pvals, mv2lo32msk); //TODO
            _mm_storeu_si32((__m128i_u *) &img[index], pvals);
        }
    }
}
//Optimised SIMD with __m128d vector type, test, not sure if it's allowed
void burning_ship_V2(float complex start, size_t width, size_t height,
                     float res, unsigned n, unsigned char* img){
    __m128d zr, zi, cr, ci;
    __m128d limit = _mm_set1_pd(LIMIT);

    for (size_t h = 0; h < height; h++) {
        ci = _mm_set1_pd(SCALERES(h, height, res) + s_ci);

        for (size_t w = 0; w < width; w += 2) {
            cr = _mm_add_pd(_mm_set_pd(SCALERES(w + 1, width, res) + s_cr, SCALERES(w, width, res) + s_cr), ci);
            zr = _mm_setzero_pd();
            zi = _mm_setzero_pd();

            unsigned i = 0;
            for (; i < n; i++) {
                __m128d zr_tmp = _mm_sub_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zi, zi));
                zi = _mm_add_pd(_mm_mul_pd(_mm_set1_pd(2.0), _mm_mul_pd(zr, zi)), ci);
                zr = _mm_add_pd(zr_tmp, cr);

                __m128d cmp = _mm_cmpgt_pd(_mm_add_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zi, zi)), limit);
                if (_mm_movemask_pd(cmp) == 0x3) break;
            }

            for (size_t j = 0; j < 2; j++) {
                size_t index = BMDIM(w + j) + h * BMDIM(width);
                img[index] = BW_CLR(i, n);
            }
        }
    }
}
//This version improves cache performance, uses pragma to parallelize the execution of the outer loop, which can significantly improve performance on multi-core processors.
void burning_ship_V2_1(float complex start, size_t width, size_t height,
                       float res, unsigned n, unsigned char* img){
    __m128d zr, zi, cr, ci;
    __m128d limit = _mm_set1_pd(LIMIT);
    __m128d two = _mm_set1_pd(2.0);

#pragma omp parallel for schedule(dynamic)
    for (size_t h = 0; h < height; h++) {
        ci = _mm_set1_pd(SCALERES(h, height, res) + s_ci);

        for (size_t w = 0; w < width; w += 2) {
            cr = _mm_add_pd(_mm_set_pd(SCALERES(w + 1, width, res) + s_cr, SCALERES(w, width, res) + s_cr), ci);
            zr = _mm_setzero_pd();
            zi = _mm_setzero_pd();

            unsigned i = 0;
            for (; i < n; i++) {
                __m128d zr_tmp = _mm_sub_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zi, zi));
                zi = _mm_add_pd(_mm_mul_pd(two, _mm_mul_pd(zr, zi)), ci);
                zr = _mm_add_pd(zr_tmp, cr);

                __m128d cmp = _mm_cmpgt_pd(_mm_add_pd(_mm_mul_pd(zr, zr), _mm_mul_pd(zi, zi)), limit);
                if (_mm_movemask_pd(cmp) == 0x3) break;
            }
            for (size_t j = 0; j < 2; j++) {
                size_t index = BMDIM(w + j) + h * BMDIM(width);
                __builtin_prefetch(img + index);
                img[index] = BW_CLR(i, n);
            }
        }
    }
}