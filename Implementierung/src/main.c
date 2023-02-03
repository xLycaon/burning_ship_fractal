#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <complex.h>

#include "usage.h"
#include "utils.h"
#include "burning_ship.h"
#include "timer.h"

#define FAIL(...) do {fprintf(stderr, __VA_ARGS__); printf("%s\n", USAGE); exit(EXIT_FAILURE);} while(0)
#define FAIL_E() exit(EXIT_FAILURE);

// CONSTANTS
#define MAX_ITER 100 // TODO might also be higher
#define MIN_ITER 1

//#define MAX_N 10000
#define MAX_N (INT32_MAX)
#define MIN_N 1

#define MIN_W 100
#define MAX_W 10000 // -> (3*1E4)^2 -> 900MB image -> 11% of 8GB phys. memory //TODO might be higher

#define MIN_H 100
#define MAX_H 10000 // -> (3*1E4)^2 -> 900MB image -> 11% of 8GB phys. memory //TODO might be higher

#define MIN_ZOOM (4.0f)

#define RADIUS (2.0f)

#define DPATH_LEN 2
#define BMP_EXT_LEN 4

// END CONSTANTS

#define D_OPT (0x1)
#define N_OPT (0x2)

#define PARG_CHECK_ERRNO(OPT, OPTARG) \
if (errno == EINVAL) { \
FAIL("Param %s in -%c contains non-digit params!\n", (OPTARG), (OPT)); \
} else if (errno == ERANGE) { \
FAIL("Param %s in -%c overflows!\n", (OPTARG), (OPT)); \
}

//TODO header
extern void test_image_sanity(burning_ship_t bs1, burning_ship_t bs2, struct BS_Params params);

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

static inline void check_range_f(long double val, long double min, long double max) { //TODO double
    if (val < min || val > max) {
        FAIL("%Lf is out of bounds [%Lf, %Lf]!\n", val, min, max);
    }
}

extern unsigned is_gmp; //TODO

//TODO limit range of s_val and pres
int main(int argc, char* argv[argc]) {

	// DEFAULT parameters
	int impl_ind, time_cap, iter_n;
	long double s_real, s_imag, pres; //TODO double
	long double complex s_val; //TODO double
	long img_w, img_h;
	char file_name[FILENAME_MAX];

    // MP DEFAULT PARAMETERS //TODO
    // mpfr_t mp_real, mp_imag, mp_pres;

    //printf("%u\n", is_gmp); //TODO

	// DEFAULT VALUES for parameters
	impl_ind = 0;
	time_cap = -1;
	iter_n = -1;
	pres = 1.0f;
	s_val = 0.0;
	img_w = -1;
	img_h = -1;
    STRLCPY(file_name+DPATH_LEN, "bs", 2+DPATH_LEN);

    // DEFAULT VALUES for multiple precision parameters //TODO
    /*
    mpfr_init(mp_pres);
    mpfr_init(mp_real);
    mpfr_init(mp_imag);

    mpfr_set_d(mp_pres, 0, MPFR_RNDN);
    mpfr_set_d(mp_real, 0, MPFR_RNDN);
    mpfr_set_d(mp_imag, 0, MPFR_RNDN);

    mpfr_clear(mp_pres);
    mpfr_clear(mp_real);
    mpfr_clear(mp_imag);
     */

	// Other parameters required for getopt
	int opt, l_optind, optset = 0, is_test = 0/*, is_mp = 0*/; //TODO is_mp
	const char *optstr = ":V:B:s:d:n:r:o:h", *sep = ", ";
    char* tmp; //TODO
	const struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
        {"test", no_argument, NULL, 't'},
        //TODO user should be able to set accuracy //TODO not sure if include
        //{"gmp", optional_argument, NULL, 1}, // Enables Multiple Precision Calculations
		{NULL, 0, NULL, 0}
	};

	// List of burning_ship implementations
    //TODO
    /*
	const burning_ship_t burning_ship_impl[] = {
		burning_ship
        ,burning_ship_V1

#ifdef __AVX2__
        ,burning_ship_AVX256
#endif // __AVX2__
	};
     */

    const burning_ship_prec_t burning_ship_impl[] = {
            {.f = burning_ship}
            ,{.f = burning_ship_V1}
#ifdef __AVX2__
            ,{.f = burning_ship_AVX256}
#endif // __AVX2__
            ,{.d = burning_ship_d} //TODO macro
            ,{.ld = burning_ship_ld} //TODO macro
    };

    // TODO
    /*
    // List of multiple precision burning_ship implementations
    const burning_ship_MPt mp_burning_ship_impl[] = {
            burning_ship_MP
    };
     */

    //TODO case multiple precision //TODO not sure if include
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
                check_range(time_cap, MIN_ITER, MAX_ITER);
				break;
			case 's': //TODO multiple precision
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
                check_range_f(s_imag, -RADIUS, RADIUS);

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
                check_range(img_w, MIN_W, MAX_W); //TODO

                tmp = strtok(NULL, sep);
                if (tmp == NULL) {
                    FAIL("Could not parse second parameter of -d!\n");
                }
                img_h = atoi_s(tmp);
                PARG_CHECK_ERRNO(opt, tmp);
                check_range(img_h, MIN_H, MAX_H); //TODO
				break;
			case 'n':
                optset |= N_OPT;
                iter_n = atoi_s(optarg);
                PARG_CHECK_ERRNO(opt, optarg);
                check_range(iter_n, MIN_N, MAX_N);
				break;
			case 'r': //TODO multiple precision
                pres = atof_s(optarg);
                PARG_CHECK_ERRNO(opt, optarg);
                //check_range_f(pres, FLT_MIN, MIN_ZOOM);
				break;
            case 'o':
                STRLCPY(file_name+DPATH_LEN, optarg,FILENAME_MAX - BMP_EXT_LEN - DPATH_LEN - 1);
                break;
            case 't':
                is_test = 1;
                break;
                /*
            case 1: //TODO multiple precision
                is_mp = 1;
                mpfr_set_default_prec(53); //TODO
                break;
                 */
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

    // Deletes mpfr pointers if gmp flag was not specified
    /*
    if (!is_mp) { // TODO alternative?
        mpfr_clear(mp_pres);
        mpfr_clear(mp_real);
        mpfr_clear(mp_imag);
    }
     */

    //TODO
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
    if (is_test /*&& !is_mp*/) { //TODO
        printf("Running tests...\n");
        test_image_sanity(burning_ship_impl[impl_ind].f, burning_ship_impl[0].f, (struct BS_Params) { //TODO precision selection .f vs .d
                .start = s_val,
                .width = img_w,
                .height = img_h,
                .res = (float) pres,
                .n = iter_n,
                .img = NULL
        });
        exit(EXIT_SUCCESS);
    }

    unsigned char* img;

    // BENCHMARKING
    if (time_cap >= 1 /*&& !is_mp*/) { //TODO
        // Allocate image-memory necessary for benchmarks
        if ( (img = malloc(BMRS(img_w) * img_h) ) == NULL) {
            goto Lerr;
        }
        printf("Starting Benchmark...\n");
        time_fn(burning_ship_impl[impl_ind].f, (struct BS_Params) { //TODO precision
                        .start = s_val,
                        .width = img_w,
                        .height = img_h,
                        .res = (float) pres,
                        .n = iter_n,
                        .img = img
                }
                , time_cap);
        printf("Benchmark complete!\n");
        free(img);
        exit(EXIT_SUCCESS);
    }

    // Allocates memory to the final size of the .bmp output file
    ssize_t fsize = BMRS(img_w) * img_h + sizeof (BMP_H) + sizeof (struct COLOR_TB16);
    if ( (img = malloc(fsize)) == NULL)
		goto Lerr;

    // Normal execution
    printf("Calculating results...\n");
    burning_ship_impl[impl_ind].f(s_val, img_w, img_h, (float) pres, iter_n, img); //TODO precision
    /*
    if (is_mp) { // TODO
        mp_burning_ship_impl[impl_ind](mp_real, mp_imag, img_w, img_h, mp_pres, iter_n, img);
    } else {
        burning_ship_impl[impl_ind](s_val, img_w, img_h, (float) pres, iter_n, img);
    }
     */

    // frees mpfr pointers
    //TODO exception handling
    /*
    mpfr_clear(mp_pres);
    mpfr_clear(mp_real);
    mpfr_clear(mp_imag);
     */

	// FILE PATH for result
	memcpy(file_name, "./", DPATH_LEN);
	strncat(file_name, ".bmp", BMP_EXT_LEN);

	// WRITING IMAGE DATA INTO FILE
	printf("Creating image file...\n");
    ssize_t written;
	if ( (written = writef_bmp(img, file_name, (struct DIM) {.width = img_w, .height = img_h})) < 0 ) {
        free(img);
        goto Lerr;
    }
    if (written < fsize) {
        fprintf(stderr, "WARNING: Only %ld out of %ld bytes were written successfully!", written, fsize);
    }

	free(img);
	return EXIT_SUCCESS;
Lerr:
	FAIL_E();
}

