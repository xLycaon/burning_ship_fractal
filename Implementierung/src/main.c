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
"            [-s <Realteil>,<Imaginärteil>] [-d <Zahl>,<Zahl>] [-n <Zahl>]\n" \

//TODO
#define USAGE_V \
"\n" \
"NO PARAMETERS ARE ACCEPTED OTHER THAN OPTIONS\n" \
"\n" \
"-h | --help *** displays verbose help\n" \
"\n" \
"-d<width,height> *** Sets the dimensions of the output image. NO SPACE after , is allowed.\n" \
"Ex: -d100,200 or -d 100,200 but not -d100, 200.\n" \
"\n" \

#define FAIL(...) do {fprintf(stderr, __VA_ARGS__); printf("%s\n", USAGE); exit(EXIT_FAILURE);} while(0)
#define FAIL_E() exit(EXIT_FAILURE);

//OPTION MASKS 0011
#define D_MSK 0x1 // -d option 0001
#define N_MSK 0x2 // -n option 0010
#define ARO_MSK 0x3 // All required options 0011
#define SET_ROPT(optv, msk) optv &= msk ^ ARO_MSK

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

/* redefinitions for atoi from stdlib.h */
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

//TODO
#define ATOT_S(VAR, ARG, ENDPTR, T) \
errno = 0; \
VAR = T; \
if ( *ENDPTR != '\0' ) { \
	FAIL("%c could not be converted!\n", *ENDPTR); \
} else if ( errno != 0 ) { \
	perror("OVER-/UNDERFLOW ERROR"); \
	FAIL_E(); \
}

#define STRTOLT(NPTR, ENDPTR, T) T(NPTR, &ENDPTR, 10)

#define STRTOFT(NPTR, ENDPTR, T) T(NPTR, &ENDPTR)

#define ATOI_S(VAR, ARG, ENDPTR) ATOT_S(VAR, ARG, ENDPTR, STRTOLT(ARG, ENDPTR, strtol))
#define ATOF_S(VAR, ARG, ENDPTR) ATOT_S(VAR, ARG, ENDPTR, STRTOFT(ARG, ENDPTR, strtof))

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#define TOKENIZE_TWO(TKNS, S_VAL, SEP) \
TKNS[0] = strtok(S_VAL, SEP); \
if ( (TKNS[1] = strtok(NULL, SEP)) == NULL ) { \
	FAIL("ERROR: Space after , or no argument!\n"); \
}

#define CHECK_RANGE(OPT, VAL, MIN_V, MAX_V) \
if ( VAL < MIN_V || VAL > MAX_V ) {  \
	FAIL("ERROR in -%c: %d is OUT OF RANGE %d to %d inclusive!\n", OPT, VAL, MIN_V, MAX_V); \
}
#define CHECK_RANGE_F(OPT, VAL, MIN_V, MAX_V) //TODO ??? for floats
//TODO
/*
#define CHECK_RANGE(OPT, VAL, MIN_V, MAX_V) \
if ( VAL < MIN_V || VAL > MAX_V ) { \
	FAIL("ERROR in -%c: %d is OUT OF RANGE %d to %d inclusive!\n",
	OPT, VAL, MIN_V, MAX_V); \
} 
*/

static void check_range() {
}

//TODO limit range of s_val and pres
int main(int argc, char* argv[argc]) {

	// DEFAULT parameters
    unsigned impl_ind;
	int time_cap, iter_n;
	float s_real, s_imag, pres;
	float complex s_val;
	unsigned long img_w, img_h;
	char file_name[FILENAME_MAX];

	// DEFAULT VALUES for parameters
	impl_ind = 0;
	time_cap = -1;
	iter_n = 0;
	pres = 1.0f;
	s_val = 0.0;
	img_w = 100; //TODO Range check
	img_h = 100; //TODO Range check
    STRLCPY(file_name+DPATH_LEN, "bs", 2+DPATH_LEN);

	// Other parameters required for getopt
	int opt, l_optind;
	char *endptr, *tkns[2], ropt_nset = ARO_MSK;
	const char* optstr = ":V:B:s:d:n:r:o:h";
	const struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
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
				ATOI_S(impl_ind, optarg, endptr)
				CHECK_RANGE(opt, impl_ind, 0, sizeof(burning_ship_impl)/sizeof(burning_ship_impl[0]));
				break;
			case 'B':
				ATOI_S(time_cap, optarg, endptr);
				CHECK_RANGE(opt, time_cap, 1, MAX_ITER);
				break;
			case 's':
				TOKENIZE_TWO(tkns, optarg, ",")
				ATOF_S(s_real, tkns[0], endptr)
				ATOF_S(s_imag, tkns[1], endptr)
				s_val = s_real + I * s_imag;
				break;
			case 'd': //TODO handle great file sizes (couple of GBs)
				SET_ROPT(ropt_nset, D_MSK);
				TOKENIZE_TWO(tkns, optarg, ",")
				ATOI_S(img_w, tkns[0], endptr)
				ATOI_S(img_h, tkns[1], endptr)
				CHECK_RANGE(opt, img_w, MIN_W, MAX_W);
				CHECK_RANGE(opt, img_h, MIN_H, MAX_H);
				break;
			case 'n':
				SET_ROPT(ropt_nset, N_MSK);
				ATOI_S(iter_n, optarg, endptr)
				CHECK_RANGE(opt, iter_n, MIN_N, MAX_N);
				break;
			case 'r':
				ATOF_S(pres, optarg, endptr);
				break;
            case 'o':
                STRLCPY(file_name+DPATH_LEN, optarg, FILENAME_MAX - BMP_EXT_LEN - DPATH_LEN - 1);
                break;
			case 'h':
				printf("%s\n", USAGE);
				printf("%s\n", USAGE_V);
				exit(EXIT_SUCCESS);
			case ':':
				FAIL("%c requires argument(s)!\n", optopt);
			case '?':
				FAIL("Option %c unrecognized!\n", optopt);
		}
	}

    // Checks if program arguments are only options
	if ( optind != argc ) {
		FAIL("No paramters other than options are allowed\n");
	}

    // Checks if required options are set
	if ( ropt_nset ) {
		if ( ropt_nset & D_MSK ) {
			fprintf(stderr, "Option -d is required but was not set!\n");
		}

		if (ropt_nset & N_MSK) {
			fprintf(stderr, "Option -n is required but was not set!\n");
		}
        goto Lerr;
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
    }

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

#undef ATOT_S
#undef STRTOLT
#undef STRTOFT
#undef ATOI_S
#undef ATOF_S

