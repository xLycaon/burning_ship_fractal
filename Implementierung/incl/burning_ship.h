#pragma once

#include "bs_types.h"

void burning_ship(float complex start, size_t width, size_t height,
                  float res, unsigned n, unsigned char* img);

//TODO
void burning_ship_d(double complex start, size_t width, size_t height,
                  double res, unsigned n, unsigned char* img);

//TODO
void burning_ship_ld(long double complex start, size_t width, size_t height,
                    long double res, unsigned n, unsigned char* img);

// SIMD-Implementation of burning_ship with 4 Steps per Iteration
void burning_ship_V1(float complex start, size_t width, size_t height,
                  float res, unsigned n, unsigned char* img);

#ifdef __AVX2__

// AVX-Implementation of burning_ship
void burning_ship_AVX256(float complex start, size_t width, size_t height,
                     float res, unsigned n, unsigned char* img);

#endif // __AVX2__

#ifdef __GNU_MP_VERSION

// Multiple Precision Implementation of burning_ship
void burning_ship_MP(mpfr_t real, mpfr_t imag, size_t width, size_t height,
                     mpfr_t res, unsigned n, unsigned char* img);

#endif // __GNU_MP_VERSION
