#include <math.h>

//SIMD
#include <emmintrin.h>
#include <tmmintrin.h>
#include <xmmintrin.h>
#include <immintrin.h>

//AVX
#ifdef __AVX2__
#include <avx2intrin.h>

#define OCT_DIV(X) ((X) & ~0x7l)
#define AVX_STEP (8)
#endif // __AVX2__

#include "burning_ship.h"
#include "utils.h"

// We can show that lim sup n -> ∞ |zn| ≤ 2
// Therefore zr^2 + zi^2 ≤ 4
#define LIMIT 4

#define SCALE2RNG(POS, RNG, RES, TYPE) (((TYPE) (POS) / (TYPE) (RNG) - (TYPE) 0.5) * (RES))
#define SCALE_CLR(ITER, N, TYPE) ( (unsigned char) ((TYPE)(ITER)/(TYPE)(N) * (TYPE) (TOTAL_COLORS-1)))

#define SIMD_STEP (4)
#define FOUR_DIV(X) ((X) & ~3ul)

// Moves each lower byte value of 4 xmm 32-bit chunks to lower 32bit
#define MV_4_SEG32 (0x0c080400)

// Sets a 32-bit chunk to zero (.) -> (0)
#define Z_MSK (0x80808080)

static inline __attribute__((always_inline))
__m128 _mm_scale2rng_ps(__m128 pos, __m128 rng, __m128 res)
{
    return (pos * _mm_rcp_ps(rng) - _mm_set1_ps(0.5f)) * res; //TODO _mm_set1_ps
}

//TODO color scaling not right
static inline __attribute__((always_inline))
__m128i _mm_scaleclr_ps(__m128i iter, __m128 n)
{
    __m128 iter_f = _mm_cvtpi32x2_ps((__m64) iter[0], (__m64) iter[1]);
    __m128 res = iter_f * _mm_rcp_ps(n) * _mm_set1_ps((float) (TOTAL_COLORS-1));
    return _mm_cvtps_epi32(res);
}

#ifdef __AVX2__

static inline __attribute__((always_inline))
__m256 _mm256_scale2rng_ps(__m256 pos, __m256 rng, __m256 res)
{
    return _mm256_fmsub_ps(pos, _mm256_rcp_ps(rng), _mm256_set1_ps(0.5f)) * res; //TODO _mm_set1_ps
}

//TODO color scaling function accuracy
static inline __attribute((always_inline))
__m256i _mm256_scaleclr_ps(__m256i iter, __m256 n)
{
    __m256 res = _mm256_cvtepi32_ps(iter) * _mm256_rcp_ps(n) * _mm256_set1_ps((float) (TOTAL_COLORS-1)); // TODO _mm256_set1_ps
    return _mm256_cvtps_epi32(res);
}

#endif // __AVX2__

//TODO fabs for each type due to value promotion
#define BURNING_SHIP(START, WIDTH, HEIGHT, RES, N, IMG, TYPE)\
do \
{ \
TYPE s_cr = (TYPE) creall(START); \
TYPE s_ci = (TYPE) cimagl(START); \
\
for (size_t h = 0; h < HEIGHT; h++) \
{\
TYPE ci = SCALE2RNG(h, HEIGHT, RES, TYPE) + s_ci; \
\
for (size_t w = 0; w < WIDTH; w++) \
{ \
TYPE cr = SCALE2RNG(w, WIDTH, RES, TYPE) + s_cr; \
\
TYPE zr = 0.0f;\
TYPE zi = 0.0f; \
\
unsigned i = 0; \
while (i < N) \
{ \
TYPE tmp = zr*zr - zi*zi + cr; \
zi = (TYPE) 2.0 * (TYPE) fabs(zr*zi) + ci; \
zr = tmp; \
\
if (zr*zr + zi*zi > LIMIT) \
break; \
\
i++; \
}\
\
IMG[w + h * WIDTH] = SCALE_CLR(i, N, TYPE); \
}\
}\
} while(0)

static inline __attribute__((always_inline))
void burning_ship_step(size_t index, float cr, float ci, unsigned n, unsigned char* img)
{
    float zr = 0.0f;
    float zi = 0.0f;

    unsigned i = 0;
    while (i < n)
    {
        float tmp = zr*zr - zi*zi + cr;
        zi = 2.0f * fabsf(zr*zi) + ci;
        zr = tmp;

        if (zr*zr + zi*zi > LIMIT)
            break;

        i++;
    }

    img[index] = SCALE_CLR(i, n, float);
}

//TODO
#ifdef __AVX2__

static inline __attribute__((always_inline))
__m128i burning_ship_SIMD_step(__m128 cr, __m128 ci, unsigned n,
                               __m128 two, __m128 abs_msk, __m128 limit) {

    __m128 zr = _mm_setzero_ps();
    __m128 zi = _mm_setzero_ps();

    __m128i one = _mm_set1_epi32(1); //TODO

    // ESCAPE LOOP
    __m128i i_vec = {0};
    unsigned i = 0;
    while (i < n)
    {
        __m128 tmp = zr*zr - zi*zi + cr;
        zi = two * _mm_andnot_ps(abs_msk, zr*zi) + ci;
        zr = tmp;

        __m128 gt_lim = _mm_cmpgt_ps(zr*zr+zi*zi, limit);
        if (_mm_movemask_ps(gt_lim) == 0xf) // if every pixel exceeds the limit
            break;

        one = _mm_andnot_si128(_mm_castps_si128(gt_lim), one);
        i_vec += one;
        i++;
    }

    return i_vec;
}

#endif // __AVX2__

// c = cr + I * ci
// Z(n+1) = (|Re(Z(n))| + I * |Imag(Z(n))|)^2 + c = (|zr| + I * |zi|)^2 + (cr + I * ci)
// Re(Z(n+1)) = (zr)^2 - (zi)^2 + cr
// Img(Z(n+1)) = 2|zr||zi| + ci = 2|zrzi| + ci
//TODO shorter complex representation
//TODO macro
void burning_ship(float complex start, size_t width, size_t height,
                  float res, unsigned n, unsigned char* img)
{
    BURNING_SHIP(start, width, height, res, n, img, float);
}

void burning_ship_d(double complex start, size_t width, size_t height,
                  double res, unsigned n, unsigned char* img)
{
    BURNING_SHIP(start, width, height, res, n, img, double);
}

void burning_ship_ld(long double complex start, size_t width, size_t height,
                    long double res, unsigned n, unsigned char* img)
{
    BURNING_SHIP(start, width, height, res, n, img, long double);
}

/*
void burning_ship(float complex start, size_t width, size_t height,
                  float res, unsigned n, unsigned char* img)
{
    float s_cr = crealf(start);
    float s_ci = cimagf(start);

	for (size_t h = 0; h < height; h++) {

        // ci := y-coord + imaginary part offset
        float ci = SCALE2RNG(h, height, res, float) + s_ci;

		for (size_t w = 0; w < width; w++)
        {
			// cr := x-coord + real part offset
            float cr = SCALE2RNG(w, width, res, float) + s_cr;

            burning_ship_step(w + h * width, cr, ci, n, img);
		}
	}
}
 */

//TODO sanity
void burning_ship_V1(float complex start, size_t width, size_t height,
                     float res, unsigned n, unsigned char* img)
{
    float s_ci = cimagf(start);
    float s_cr = crealf(start);

    // FUNCTION PARAMETERS VECTORIZED
    //TODO stack?
    __m128 s_ci_vec = _mm_set1_ps(s_ci);
    __m128 s_cr_vec = _mm_set1_ps(s_cr);
    __m128 height_128 = _mm_set1_ps((float) height);
    __m128 width_128 = _mm_set1_ps((float) width);
    __m128 res_128 = _mm_set1_ps(res);
    __m128 n_vec = _mm_set1_ps((float) n);

    // FLOATING POINT VECTOR CONSTANTS
    __m128 two = _mm_set1_ps(2.0f);
    __m128 abs_msk = _mm_set1_ps(-0.0f);
    __m128 limit = _mm_set1_ps(LIMIT);
    __m128 incr = _mm_setr_ps(0.0f, 1.0f, 2.0f, 3.0f);

    // INTEGER VECTOR CONSTANTS
    __m128i one_msk = _mm_set1_epi32(0x1); //TODO simpler
    __m128i mv2lo32msk = _mm_set_epi32((int) Z_MSK,
                                       (int) Z_MSK,
                                       (int) Z_MSK,
                                       (int) MV_4_SEG32);

    for (size_t h = 0; h < height; h++) {

        // ci := y-coord + imaginary part offset
        __m128 ci = _mm_scale2rng_ps(_mm_set1_ps((float) h), height_128, res_128) + s_ci_vec;

        size_t w = 0;
        for (; w < FOUR_DIV(width); w += SIMD_STEP)
        {
            // cr := x-coord + real part offset
            __m128 cr = _mm_set1_ps((float) w) + incr;
            cr = _mm_scale2rng_ps(cr, width_128, res_128) + s_cr_vec;

            __m128 zr = _mm_setzero_ps();
            __m128 zi = _mm_setzero_ps();

            __m128i one = one_msk; //TODO

            // ESCAPE LOOP
            __m128i i_vec = {0};
            unsigned i = 0;
            while (i < n)
            {
                __m128 tmp = zr*zr - zi*zi + cr;
                zi = two * _mm_andnot_ps(abs_msk, zr*zi) + ci;
                zr = tmp;

                __m128 gt_lim = _mm_cmpgt_ps(zr*zr+zi*zi, limit);
                if (_mm_movemask_ps(gt_lim) == 0xf) // if every pixel exceeds the limit
                    break;

                one = _mm_andnot_si128(_mm_castps_si128(gt_lim), one);
                i_vec += one;
                i++;
            }

            // COLORING PIXELS
            __m128i pvals = _mm_scaleclr_ps(i_vec, n_vec);
            pvals = _mm_shuffle_epi8(pvals, mv2lo32msk);

            _mm_storeu_si32((__m128i_u *) (img + w + h * width), pvals); //TODO
        }

        // TODO
        // DEALING WITH REMAINING PIXELS
        for (; w < width; w++) {
            burning_ship_step(w + h * width, s_cr, s_ci, n, img);
        }
    }
}

#ifdef __AVX2__

//TODO fused multiplication, addition and subtraction
void burning_ship_AVX256(float complex start, size_t width, size_t height,
                         float res, unsigned n, unsigned char* img)
{
    float s_ci = cimagf(start);
    float s_cr = crealf(start);

    // FUNCTION PARAMETERS VECTORIZED
    __m256 s_cr_256ps = _mm256_set1_ps(s_cr);
    __m256 s_ci_256ps = _mm256_set1_ps(s_ci);
    __m256 height_256 = _mm256_set1_ps((float) height);
    __m256 width_256 = _mm256_set1_ps((float) width);
    __m256 res_vec = _mm256_set1_ps(res);
    __m256 n_vec = _mm256_set1_ps((float) n);

    // FLOATING POINT VECTOR CONSTANTS
    __m256 two = _mm256_set1_ps(2.0f);
    __m256 abs_msk = _mm256_set1_ps(-0.0f);
    __m256 limit = _mm256_set1_ps(LIMIT);
    __m256 incr = _mm256_setr_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);

    // INTEGER VECTOR CONSTANTS
    __m256i one_msk = _mm256_set1_epi32(1);
    __m256i store_msk = _mm256_set_epi32(0,0,0,0,0,0,-1,-1);
    __m256i mv2hilo32lolo32msk = _mm256_set_epi32(Z_MSK,
                                          Z_MSK,
                                          Z_MSK,
                                          MV_4_SEG32,
                                          Z_MSK,
                                          Z_MSK,
                                          Z_MSK,
                                          MV_4_SEG32);

    for (size_t h = 0; h < height; h++) {

        __m256 ci = _mm256_scale2rng_ps(_mm256_set1_ps((float) h), height_256, res_vec) + s_ci_256ps;

        size_t w = 0;
        for (; w < OCT_DIV(width); w+=AVX_STEP)
        {
            __m256 cr = _mm256_set1_ps((float) w) + incr;
            cr = _mm256_scale2rng_ps(cr, width_256, res_vec) + s_cr_256ps;

            __m256 zr = _mm256_setzero_ps();
            __m256 zi = _mm256_setzero_ps();

            __m256i one = one_msk;

            __m256i i_vec = {0};
            unsigned i = 0;
            while (i < n)
            {
                // tmp = (zr*zr) + (-(zi*zi) + cr)
                __m256 tmp = _mm256_fmadd_ps(zr, zr, _mm256_fnmadd_ps(zi, zi, cr));

                // zi = (2.0f * |zr*zi|) + ci
                zi = _mm256_fmadd_ps(two, _mm256_andnot_ps(abs_msk, zr*zi), ci); // TODO wait for instruction?
                zr = tmp;

                __m256 gt_lim = _mm256_cmp_ps(limit, _mm256_fmadd_ps(zr, zr, zi*zi), 2); //TODO instruction wait?
                if (_mm256_movemask_ps(gt_lim) == 0xff) // if every pixel exceeds the limit
                    break;

                one = _mm256_andnot_si256(_mm256_castps_si256(gt_lim), one);
                i_vec += one;
                i++;
            }

            __m256i pvals = _mm256_scaleclr_ps(i_vec, n_vec);
            pvals = _mm256_shuffle_epi8(pvals, mv2hilo32lolo32msk);
            pvals[0] |= pvals[2] << 32;

            _mm256_maskstore_epi32((int*) (img + w + h * width), store_msk,pvals); //TODO
        }

        //TODO
        for (; w < FOUR_DIV(width); w+=SIMD_STEP) {
            __m128 cr = _mm_set1_ps((float) w) + incr[0];
            cr = _mm_scale2rng_ps(cr, _mm256_castps256_ps128(width_256), _mm256_castps256_ps128(res_vec));

            //TODO
            __m128i i_vec = burning_ship_SIMD_step(cr, _mm256_castps256_ps128(ci), n,
                                                   _mm256_castps256_ps128(two),
                                                   _mm256_castps256_ps128(abs_msk),
                                                   _mm256_castps256_ps128(limit));

            //TODO
            __m128i pvals = _mm_scaleclr_ps(i_vec, _mm256_castps256_ps128(n_vec));
            pvals = _mm_shuffle_epi8(pvals, _mm256_castsi256_si128(mv2hilo32lolo32msk));

            _mm_storeu_si32((__m128i_u *) (img + w + h * width), pvals); //TODO
        }

        // TODO
        for (; w < width; w++) {
            burning_ship_step((w + h * width), s_cr, s_ci, n, img);
        }
    }
}

#endif // __AVX2__
