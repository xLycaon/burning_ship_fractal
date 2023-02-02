
unsigned is_gmp = 0; //TODO

#ifdef __GNU_MP_VERSION
is_gmp = 1;

#include "utils.h" //TODO total colors
#include "bs_types.h"

static unsigned char
scale_clr_mp(unsigned i, unsigned n)
{
    mpfr_t tmp;
    mpfr_init(tmp);

    mpfr_set_d(tmp, (double) i, MPFR_RNDN);
    mpfr_div_d(tmp, tmp, (double) n, MPFR_RNDN);
    mpfr_mul_d(tmp, tmp, (double) TOTAL_COLORS-1, MPFR_RNDN);

    unsigned char result = (unsigned char) mpfr_get_d(tmp, MPFR_RNDN);

    mpfr_clear(tmp);

    return result;
}

static void
scale2rng_mp(mpfr_ptr result, mpfr_srcptr pos, mpfr_srcptr rng, mpfr_srcptr res, mpfr_srcptr half)
{
    mpfr_div(result, pos, rng, MPFR_RNDN);
    mpfr_sub(result, result, half, MPFR_RNDN);
    mpfr_mul(result, result, res, MPFR_RNDN);
}

void burning_ship_MP(mpfr_t real, mpfr_t imag, size_t width, size_t height,
                     mpfr_t res, unsigned n, unsigned char* img)
{
    mpfr_t cr, ci, zr, zi, // Vals for arithmetic
    mp_width, mp_height, mp_w, mp_h, mp_half, // Constants
    tmp, tmp2; // Temporary Vals

    mpfr_init(mp_width);
    mpfr_init(mp_height);

    mpfr_set_d(mp_width, (double) width, MPFR_RNDN);
    mpfr_set_d(mp_height, (double) height, MPFR_RNDN);

    mpfr_init(mp_half);

    mpfr_set_d(mp_half, 0.5, MPFR_RNDN);

    mpfr_init(cr);
    mpfr_init(ci);

    mpfr_init(zr);
    mpfr_init(zi);

    mpfr_init(mp_w);
    mpfr_init(mp_h);

    mpfr_init(tmp);
    mpfr_init(tmp2);

    for (size_t h = 0; h < height; h++)
    {
        mpfr_set_d(mp_h, (double) h, MPFR_RNDN);

        // ci := y-coord + offset
        scale2rng_mp(ci, mp_h, mp_height, res, mp_half);
        mpfr_add(ci, ci, imag, MPFR_RNDN);

        for (size_t w = 0; w < width; w++)
        {
            mpfr_set_d(mp_w, (double) w, MPFR_RNDN);

            // cr := x-coord + offset
            scale2rng_mp(cr, mp_w, mp_width, res, mp_half);
            mpfr_add(cr, cr, real, MPFR_RNDN);

            mpfr_zero_p(zr);
            mpfr_zero_p(zi);

            unsigned i = 0;
            while (i < n)
            {
                // tmp := zr*zr - zi_zi + cr = (zr + zi) * (zr - zi) + cr

                // tmp := (zr + zi)
                mpfr_add(tmp, zr, zi, MPFR_RNDN);

                // tmp2 := (zr - zi)
                mpfr_sub(tmp2, zr, zi, MPFR_RNDN);

                // tmp := (zr + zi) * (zr - zi) = tmp * tmp2
                mpfr_mul(tmp, tmp, tmp2, MPFR_RNDN);

                // tmp := (zr + zi) * (zr - zi) + cr = tmp + cr
                mpfr_add(tmp, tmp, cr, MPFR_RNDN);


                // zi := 2 * |zr*zi| + ci
                mpfr_mul(zi, zr, zi, MPFR_RNDN);
                mpfr_abs(zi, zi, MPFR_RNDN);
                mpfr_mul_d(zi, zi, 2, MPFR_RNDN);
                mpfr_add(zi, zi, ci, MPFR_RNDN);

                // tmp := zr*zr + zi*zi
                mpfr_mul(tmp, zr, zr, MPFR_RNDN);
                mpfr_mul(tmp2, zi, zi, MPFR_RNDN);
                mpfr_add(tmp, tmp, tmp2, MPFR_RNDN);

                // if ||z|| exceeds the limit
                if (mpfr_cmp_d(tmp, LIMIT) == 1)
                {
                    break;
                }

                i++;
            }

            img[w + h * width] = scale_clr_mp(i, n);
        }
    }

    mpfr_clear(tmp2);
    mpfr_clear(tmp);

    mpfr_clear(mp_w);
    mpfr_clear(mp_h);

    mpfr_clear(mp_half);

    mpfr_clear(mp_height);
    mpfr_clear(mp_width);

    mpfr_clear(zi);
    mpfr_clear(zr);

    mpfr_clear(ci);
    mpfr_clear(cr);

    //TODO
    mpfr_free_cache();
}

#endif // __GNU_MP_VERSION
