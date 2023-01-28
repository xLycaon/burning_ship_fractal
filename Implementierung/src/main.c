#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <complex.h>
#include <float.h>

#include "usage.h"
#include "utils.h"
#include "burning_ship.h"
#include "timer.h"

#define FAIL(...) do {fprintf(stderr, __VA_ARGS__); printf("%s\n", USAGE); exit(EXIT_FAILURE);} while(0)
#define FAIL_E() exit(EXIT_FAILURE);

// CONSTANTS
#define MAX_ITER 100 // TODO might also be higher
#define MIN_ITER 1

#define MAX_N 10000
#define MIN_N 1

#define MIN_W 100
#define MAX_W 10000 // -> (3*1E4)^2 -> 900MB image -> 11% of 8GB phys. memory //TODO might be higher

#define MIN_H 100
#define MAX_H 10000 // -> (3*1E4)^2 -> 900MB image -> 11% of 8GB phys. memory //TODO might be higher

#define MIN_ZOOM (4.0f)

#define RADIUS (2.0f)

#define DPATH_LEN 2
#define BMP_EXT_LEN 4

//benchmark constants
#define MAX_BENCH_ITER 5
#define MIN_BENCH_ITER 1

//epsilon for image sanity test
#define EPS 0.000001

// END CONSTANTS

#define D_OPT (0x1)
#define N_OPT (0x2)

#define PARG_CHECK_ERRNO(OPT, OPTARG) \
if (errno == EINVAL) { \
FAIL("Param %s in -%c contains non-digit params!\n", (OPTARG), (OPT)); \
} else if (errno == ERANGE) { \
FAIL("Param %s in -%c overflows!\n", (OPTARG), (OPT)); \
}


extern void test_image_sanity(burning_ship_t bs1, burning_ship_t bs2, struct BS_Params params, double epsilon);
extern void test_scaling(void);

static inline int atoi_s(const char* arg) {
    errno = 0;
    char* endptr;
    long res = strtol(arg, &endptr, 10);
    if ( *endptr != '\0' ) {
        errno = EINVAL;
        return 0;
    } else if ( errno != 0 ) {
        return 0;
    }
    return (int) res;
}

static inline float atof_s(const char* arg) {
    errno = 0;
    char* endptr;
    float res = strtof(arg, &endptr);
    if ( *endptr != '\0' ) {
        errno = EINVAL;
        return 0.0f;
    } else if ( errno != 0 ) {
        return 0.0f;
    }
    return res;
}

static inline void check_range(long val, long min, long max) {
    if (val < min || val > max) {
        FAIL("%ld is out of bounds [%ld, %ld]!\n", val, min, max);
    }
}

static inline void check_range_f(float val, float min, float max) {
    if (val < min || val > max) {
        FAIL("%f is out of bounds [%f, %f]!\n", val, min, max);
    }
}

//TODO limit range of s_val and pres
int main(int argc, char* argv[argc]) {

	// DEFAULT parameters
	int impl_ind, time_cap, iter_n;
	float s_real, s_imag, pres; //TODO pres double?
	float complex s_val;
	long img_w, img_h;
	char file_name[FILENAME_MAX];

	// DEFAULT VALUES for parameters
	impl_ind = 0;
	time_cap = -1;
	iter_n = -1;
	pres = 1.0f;
	s_val = 0.0;
	img_w = -1;
	img_h = -1;
    STRLCPY(file_name+DPATH_LEN, "bs", 2+DPATH_LEN);

	// Other parameters required for getopt
	int opt, l_optind, optset = 0, test = 0;
	const char *optstr = ":V:B:s:d:n:r:o:h", *sep = ", ";
    char* tmp;
	const struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
        {"test", no_argument, NULL, 't'},
        {"double", no_argument, NULL, 0}, //TODO double precision input
        {"L_double", no_argument, NULL, 0}, //TODO long double precision
        {"arbitrary", no_argument, NULL, 0}, //TODO extended precision
		{NULL, 0, NULL, 0}
	};

	// List of burning_ship implementations
	const burning_ship_t burning_ship_impl[] = {
		burning_ship
        ,burning_ship_V1
        ,burning_ship_AVX256
	};

	// UPDATES DEFAULT PARAMETERS
	while ( (opt = getopt_long(argc, argv, optstr, longopts, &l_optind)) != -1) {
		switch (opt) {
			case 'V':
                impl_ind = atoi_s(optarg);
                PARG_CHECK_ERRNO(opt, optarg);
                check_range(impl_ind, 0, (sizeof burning_ship_impl / sizeof burning_ship_impl[0]) - 1);
				break;
            case 'B':
                time_cap = atoi_s(optarg);
                PARG_CHECK_ERRNO(opt, optarg);
                check_range(time_cap, MIN_BENCH_ITER, MAX_BENCH_ITER);
				break;
			case 's':
                tmp = strtok(optarg, sep);
                if (tmp == NULL) {
                    FAIL("Could not parse first parameter of -s!\n");
                }
                s_real = atof_s(tmp);
                PARG_CHECK_ERRNO(opt, tmp);
                check_range_f(s_real, -RADIUS, RADIUS);

                tmp = strtok(NULL, sep);
                if (tmp == NULL) {
                    FAIL("Could not parse second parameter of -s!\n");
                }
                s_imag = atof_s(tmp);
                PARG_CHECK_ERRNO(opt, tmp);
                check_range_f(s_real, -RADIUS, RADIUS);

				s_val = s_real + I * s_imag;
				break;
			case 'd':
                optset |= D_OPT;

                tmp = strtok(optarg, sep);
                if (tmp == NULL) {
                    FAIL("Could not parse first parameter of -d!\n");
                }
                img_w = atoi_s(tmp);
                PARG_CHECK_ERRNO(opt, tmp);
                //check_range(img_w, MIN_W, MAX_W);

                tmp = strtok(NULL, sep);
                if (tmp == NULL) {
                    FAIL("Could not parse second parameter of -d!\n");
                }
                img_h = atoi_s(tmp);
                PARG_CHECK_ERRNO(opt, tmp);
                //check_range(img_h, MIN_H, MAX_H);
				break;
			case 'n':
                optset |= N_OPT;
                iter_n = atoi_s(optarg);
                PARG_CHECK_ERRNO(opt, optarg);
                check_range(iter_n, MIN_N, MAX_N);
				break;
			case 'r':
                pres = atof_s(optarg);
                PARG_CHECK_ERRNO(opt, optarg);
                check_range_f(pres, FLT_MIN, MIN_ZOOM);
				break;
            case 'o':
                STRLCPY(file_name+DPATH_LEN, optarg,FILENAME_MAX - BMP_EXT_LEN - DPATH_LEN - 1);
                break;
            case 't':
                test = 1;
                break;
			case 'h':
				printf("%s\n", USAGE);
				printf("%s\n", USAGE_V);
				exit(EXIT_SUCCESS);
			case ':':
                if(optopt == 'B'){
                    time_cap = 1;
                    break;
                }
				FAIL("%c requires argument(s)!\n", optopt);
            case '?':
				FAIL("Option %c unrecognized!\n", optopt);
            default:
                FAIL("Unknown ERROR!\n");
		}
	}

    // Checks if program arguments are only options
	if ( optind != argc ) {
		FAIL("No paramters other than options are allowed\n");
	}

    // Checks if required options are set
    if (!(optset & N_OPT)) {
        fprintf(stderr, "Option -n was not specified!\n");
    }
    if (!(optset & D_OPT)) {
        fprintf(stderr, "Option -d was not specified!\n");
    }
    if ( ((optset & N_OPT) | (optset & D_OPT)) != (N_OPT | D_OPT) ) {
        goto Lerr;
    }

    // Test Execution
    if (test) {
        printf("Running tests...\n");
        //add EPSILON as parameter
        test_image_sanity(burning_ship_impl[impl_ind], burning_ship_impl[0], (struct BS_Params) {
                .start = s_val,
                .width = img_w,
                .height = img_h,
                .res = pres,
                .n = iter_n,
                .img = NULL
        },EPS);
        exit(EXIT_SUCCESS);
    }

    unsigned char* img;

    // BENCHMARKING
    if (time_cap >= 1) {
        // Allocate image-memory necessary for benchmarks
        int err = 0;
        img = malloc(BMRS(img_w) * img_h);
        if (img == NULL) {
            perror("malloc failed");
            goto Lerr;
        }
        printf("Starting Benchmark...\n");
        time_fn(burning_ship_impl[impl_ind], (struct BS_Params) {
                .start = s_val,
                .width = img_w,
                .height = img_h,
                .res = pres,
                .n = iter_n,
                .img = img
        }, time_cap, &err);

        if (err != 0) {
            // Error handling here
            free(img);
            exit(EXIT_FAILURE);
        }

        printf("Benchmark complete!\n");
        free(img);
        exit(EXIT_SUCCESS);
    }

    // Allocates memory to the final size of the .bmp output file
    if ( (img = malloc(BMRS(img_w) * img_h + sizeof (BMP_H) + sizeof (struct COLOR_TB16)) ) == NULL)
		goto Lerr;

    // Normal execution
    printf("Calculating results...\n");
    burning_ship_impl[impl_ind](s_val, img_w, img_h, pres, iter_n, img);

	// FILE PATH for result
	memcpy(file_name, "./", DPATH_LEN);
	strncat(file_name, ".bmp", BMP_EXT_LEN);

	// WRITING IMAGE DATA INTO FILE
	printf("Creating image file...\n");
	if ( writef_bmp(img, file_name, (struct DIM) {.width = img_w, .height = img_h} ) < 0 ) {
        free(img);
        goto Lerr;
    }

	free(img);
	return EXIT_SUCCESS;
Lerr:
	FAIL_E();
}

