#pragma once

#include "bs_types.h"

void burning_ship(long double complex start, size_t width, size_t height,
                  long double res, unsigned n, unsigned char* img);

//TODO
void burning_ship_ld(long double complex start, size_t width, size_t height,
                    long double res, unsigned n, unsigned char* img);

// SIMD-Implementation of burning_ship with 4 Steps per Iteration
void burning_ship_V1(long double complex start, size_t width, size_t height,
                  long double res, unsigned n, unsigned char* img);

#ifdef __AVX2__

// AVX-Implementation of burning_ship
void burning_ship_AVX256(long double complex start, size_t width, size_t height,
                     long double res, unsigned n, unsigned char* img);

#endif // __AVX2__
