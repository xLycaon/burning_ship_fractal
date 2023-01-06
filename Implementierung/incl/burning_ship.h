#pragma once

#include <complex.h>
#include <sys/types.h>

void burning_ship(float complex start, size_t width, size_t height,
                  float res, unsigned n, unsigned char* img);
