#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "timer.h"
#include "utils.h"

void test_image_sanity(burning_ship_t bs1, burning_ship_t bs2, struct BS_Params params, double epsilon);

// Tests the similarity of the given implementation for the given parameters to the reference implementation
void test_image_sanity(burning_ship_t bs1, burning_ship_t bs2, struct BS_Params params, double epsilon) {
    unsigned char *img1, *img2;
    size_t i_size = params.width * params.height;

    size_t different_pixels = 0;
    double difference_percentage;

    if ( (img1 = malloc(i_size)) == NULL ) {
        exit(EXIT_FAILURE);
    }
    if ( (img2 = malloc(i_size)) == NULL ) {
        free(img1);
        exit(EXIT_FAILURE);
    }

    bs1(params.start, params.width, params.height, (float) params.res, params.n, img1);
    bs2(params.start, params.width, params.height, (float) params.res, params.n, img2);

    for (size_t i = 0; i < i_size; i++) {
        if(img1[i] != img2[i])
            different_pixels++;
    }
    difference_percentage = (double) different_pixels / i_size * 100;

    if(difference_percentage > epsilon)
        printf("TEST: Image difference is %lf%%\n", difference_percentage);
    else
        printf("TEST: Images are equal!\n");

    free(img1);
    free(img2);
}
