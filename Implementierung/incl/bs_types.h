#pragma once

#include <sys/types.h>
#include <complex.h>

typedef void
(*burning_ship_t) (long double complex start, size_t width, size_t height,
                   long double res, unsigned n, unsigned char* img);

struct BS_Params {
    long double complex start;
    size_t width;
    size_t height;
    long double res;
    unsigned n;
    unsigned char* img;
};
