#pragma once

#include <time.h>
#include <sys/types.h>
#include <complex.h>

typedef void
(*burning_ship_t) (float complex start, size_t width, size_t height,
             float res, unsigned n, unsigned char* img);

struct BS_Params {
    float complex start;
    size_t width;
    size_t height;
    float res;
    unsigned n;
    unsigned char* img;
};

void time_fn(burning_ship_t fn, struct BS_Params params, unsigned n, int *err);
