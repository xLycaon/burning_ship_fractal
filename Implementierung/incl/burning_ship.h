#pragma once

#include <complex.h>
#include <sys/types.h>

void burning_ship(float complex start, size_t width, size_t height,
                  float res, unsigned n, unsigned char* img);

// SIMD-Implementation of burning_ship with 4 Steps per Iteration
void burning_ship_V1(float complex start, size_t width, size_t height,
                  float res, unsigned n, unsigned char* img);

// TODO signature
// AVX-Implementation of burning_ship
void burning_ship_AVX256(float complex start, size_t width, size_t height,
                     float res, unsigned n, unsigned char* img);
