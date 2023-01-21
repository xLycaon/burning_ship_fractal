#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <complex.h>

#include "utils.h"
#include "burning_ship.h"
#include "timer.h"

#define PROGNAME "prog"
#define USAGE \
"usage: " PROGNAME " [-h | --help] [-o <Dateiname>]\n" \
"            [-V <Zahl>] [-B <Zahl>] [-r <Floating Point Zahl>]\n" \
"            [-s <Realteil>,<Imaginärteil>] [-d <Zahl>,<Zahl>] [-n <Zahl>]" \

//TODO
#define USAGE_V \
"\n" \
"NO PARAMETERS ARE ACCEPTED OTHER THAN OPTIONS\n" \
"\n" \
"-h | --help *** displays verbose help\n"

#define FAIL(...) do {fprintf(stderr, __VA_ARGS__); printf("%s\n", USAGE); exit(EXIT_FAILURE);} while(0)
#define FAIL_E() exit(EXIT_FAILURE);

// CONSTANTS
#define MAX_ITER 100 // TODO might also be higher
#define MIN_ITER 1

#define MAX_N 200 // TODO values > 200 are Impractical due to how colors are calculated see SCALE_CLR
#define MIN_N 1

#define MIN_W 100
#define MAX_W 10000 // -> (3*1E4)^2 -> 900MB image -> 11% of 8GB phys. memory //TODO might be higher

#define MIN_H 100
#define MAX_H 10000 // -> (3*1E4)^2 -> 900MB image -> 11% of 8GB phys. memory //TODO might be higher

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

//TODO limit range of s_val and pres
int main(int argc, char* argv[argc]) {

	// DEFAULT parameters
	int impl_ind, time_cap, iter_n;
	float s_real, s_imag, pres;
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
	const char *optstr = ":V:B:s:d:n:r:o:h", *sep = ",";
    char* tmp;
	const struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
        {"test", no_argument, NULL, 't'},
		{NULL, 0, NULL, 0}
	};

	// List of burning_ship implementations
	const burning_ship_t burning_ship_impl[] = {
		burning_ship
        ,burning_ship_V1
        ,burning_ship_V2
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
                check_range(time_cap, MIN_ITER, MAX_ITER);
				break;
			case 's':
                tmp = strtok(optarg, sep);
                if (tmp == NULL) {
                    FAIL("Could not parse first parameter of -s!\n");
                }
                s_real = atof_s(tmp);
                PARG_CHECK_ERRNO(opt, tmp);

                tmp = strtok(NULL, sep);
                if (tmp == NULL) {
                    FAIL("Could not parse second parameter of -s!\n");
                }
                s_imag = atof_s(tmp);
                PARG_CHECK_ERRNO(opt, tmp);

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
				FAIL("%c requires argument(s)!\n", optopt);
			case '?':
				FAIL("Option %c unrecognized!\n", optopt);
            default:
                FAIL("Unknown ERROR!\n");
		}
	}

    //TEST
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
        test_image_sanity(burning_ship_impl[impl_ind], burning_ship_impl[0], (struct BS_Params) {
                .start = s_val,
                .width = img_w,
                .height = img_h,
                .res = pres,
                .n = iter_n,
                .img = NULL
        });
    }

    // Allocates memory to the final size of the .bmp output file
	unsigned char* img;
    if ( (img = malloc(BMRS(img_w) * img_h + sizeof (BMP_H) + sizeof (struct COLOR_TB16)) ) == NULL)
		goto Lerr;

    // Check if Benchmark is going to be runned.
    if (time_cap < 1) {
        printf("Calculating results...\n");
        burning_ship_impl[impl_ind](s_val, img_w, img_h, pres, iter_n, img);
    } else {
        printf("Starting Benchmark...\n");
        time_fn(burning_ship_impl[impl_ind], (struct BS_Params) { // TODO catching errors -> free(img) ?
                        .start = s_val,
                        .width = img_w,
                        .height = img_h,
                        .res = pres,
                        .n = iter_n,
                        .img = img
            }
                , time_cap);
        printf("Benchmark complete!\n");
    }

	// FILE PATH for result
	memcpy(file_name, "./", DPATH_LEN);
	strncat(file_name, ".bmp", BMP_EXT_LEN);

    // CREATE NO IMAGE when benchmarking
    if (time_cap >= 1) {
        free(img);
        return EXIT_SUCCESS;
    }

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

