#pragma once

#include <sys/types.h>
#include <complex.h>

typedef void
(*burning_ship_t) (float complex start, size_t width, size_t height,
                   float res, unsigned n, unsigned char* img);

typedef void
(*burning_ship_dt) (double complex start, size_t width, size_t height,
                   double res, unsigned n, unsigned char* img);

typedef void
(*burning_ship_ldt) (long double complex start, size_t width, size_t height,
                     long double res, unsigned n, unsigned char* img);

struct BS_Params { //TODO long double
    long double complex start;
    size_t width;
    size_t height;
    long double res;
    unsigned n;
    unsigned char* img;
};

#ifdef __GNU_MP_VERSION //TODO
#include <gmp.h>
#include <mpfr.h>
#include <mpf2mpfr.h>

typedef void
(*burning_ship_MPt) (mpfr_t real, mpfr_t imag, size_t width, size_t height,
                     mpfr_t res, unsigned n, unsigned char* img);
typedef union {
    burning_ship_t f;
    burning_ship_dt d;
    burning_ship_ldt ld;
    burning_ship_mpt mp; //TODO multiple precision
} burning_ship_prec_t;
#else
typedef union { //TODO
    burning_ship_t f;
    burning_ship_dt d;
    burning_ship_ldt ld;
} burning_ship_prec_t;
#endif // __GNU_MP_VERSION

