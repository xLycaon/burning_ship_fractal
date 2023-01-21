#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "timer.h"
#include "utils.h"

void test_image_sanity(burning_ship_t bs1, burning_ship_t bs2, struct BS_Params params);

void test_image_sanity(burning_ship_t bs1, burning_ship_t bs2, struct BS_Params params) {
    unsigned char *img1, *img2;

    size_t i_size = BMRS(params.width) * params.height;

    if ( (img1 = malloc(i_size)) == NULL ) {
        exit(EXIT_FAILURE);
    }
    if ( (img2 = malloc(i_size)) == NULL ) {
        free(img1);
        exit(EXIT_FAILURE);
    }

    bs1(params.start, params.width, params.height, params.res, params.n, img1);
    bs2(params.start, params.width, params.height, params.res, params.n, img2);

    if (memcmp(img1, img2, i_size) != 0) {
        fprintf(stderr, "TEST: Image sanity DID NOT pass!\n");
    } else {
        printf("TEST: Image sanity passed!\n");
    }

    free(img1);
    free(img2);
    exit(EXIT_SUCCESS);
}
