#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <complex.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <malloc.h>
#define alloca(x) _alloca(x)
#else
#include <alloca.h>
#endif

#include "utils.h"

#define PROGNAME "prog"
#define USAGE \
"usage: " PROGNAME " [-h | --help] [-o <Dateiname>]\n" \
"            [-V <Zahl>] [-B <Zahl>] [-r <Floating Point Zahl>]\n" \
"            [-s <Realteil>,<Imaginärteil>] [-d <Zahl>,<Zahl>] [-n <Zahl>]\n" \
"            [--bmpftest]"

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
"--bmpftest *** Runs tests checking the validity of the output format and ingoring all options except for dimensions." \
"\n" \

#define FAIL(...) do {fprintf(stderr, __VA_ARGS__); printf("%s\n", USAGE); exit(EXIT_FAILURE);} while(0)
#define FAIL_E() exit(EXIT_FAILURE);

//OPTION MASKS 0011
#define D_MSK 0x1 // -d option 0001
#define N_MSK 0x2 // -n option 0010
#define ARO_MSK 0x3 // All required options 0011
#define SET_ROPT(optv, msk) optv &= msk ^ ARO_MSK

// BOUND VALUES
#define MAX_ITER 1000

#define MAX_N 200
#define MIN_N 1

#define MIN_W 100
#define MAX_W 10000 // -> (3*1E4)^2 -> 900MB image -> 11% of 8GB phys. memory

#define MIN_H 100
#define MAX_H 10000 // -> (3*1E4)^2 -> 900MB image -> 11% of 8GB phys. memory

/* redefinitions for atoi from stdlib.h */
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#define ATOT_S(VAR, ARG, ENDPTR, T) \
errno = 0; \
VAR = T; \
if ( *ENDPTR != '\0' ) { \
	FAIL("%c is not a digit!\n", *ENDPTR); \
} else if ( errno != 0 ) { \
	perror("OVERFLOW ERROR"); \
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

#define CHECK_RANGE(OPT, VAL, MIN_V, MAX_V)
//TODO
/*
#define CHECK_RANGE(OPT, VAL, MIN_V, MAX_V) \
if ( VAL < MIN_V || VAL > MAX_V ) { \
	FAIL("ERROR in -%c: %d is OUT OF RANGE %d to %d inclusive!\n",
	OPT, VAL, MIN_V, MAX_V); \
} 
*/

extern void
burning_ship(float complex start, size_t width, size_t height,
	     float res, unsigned n, unsigned char* img);

// Function type of burning_ship
typedef void (*bs_impl) (float complex start, size_t width, size_t height,
		 	 float res, unsigned n, unsigned char* img);

int main(int argc, char* argv[argc]) {

	// DEFAULT parameters
	int impl_ind, time_cap, iter_n, is_test;
	float s_real, s_imag, pres;
	float complex s_val;
	unsigned long img_w, img_h;
	char* file_name;

	// DEFAULT VALUES for parameters
	impl_ind = 0;
	time_cap = -1;
	iter_n = 0;
	is_test = 0;
	pres = 1.0;
	s_val = 0.0;
	img_w = 0;
	img_h = 0;
	file_name = "bs";

	// Other parameters required for getopt
	int opt, l_optind;
	char *endptr, *tkns[2], ropt_nset = ARO_MSK;
	const char* optstr = ":V::B::s:d:n:r:o:h";
	const struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
		{"bmpftest", no_argument, NULL, 0},
		{NULL, 0, NULL, 0}
	};

	// List of burning_ship implementations
	const bs_impl bs[] = {
		burning_ship
	};

	// UPDATES DEFAULT PARAMETERS
	while ( (opt = getopt_long(argc, argv, optstr, longopts, &l_optind)) != -1) {
		switch (opt) {
			case 'V':
				ATOI_S(impl_ind, optarg, endptr);
				CHECK_RANGE(opt, impl_ind, 0, ARRAY_SIZE (bs) - 1);
				break;
			case 'B':
				ATOI_S(time_cap, optarg, endptr);
				CHECK_RANGE(opt, time_cap, 1, MAX_ITER);
				break;
			case 's':
				TOKENIZE_TWO(tkns, optarg, ",");
				ATOF_S(s_real, tkns[0], endptr);
				ATOF_S(s_imag, tkns[1], endptr);
				s_val = s_real + I * s_imag;
				break;
			case 'd': //TODO handle great file sizes (couple of GBs)
				SET_ROPT(ropt_nset, D_MSK);
				TOKENIZE_TWO(tkns, optarg, ",");
				ATOI_S(img_w, tkns[0], endptr);
				ATOI_S(img_h, tkns[1], endptr);
				CHECK_RANGE(opt, img_w, MIN_W, MAX_W);
				CHECK_RANGE(opt, img_h, MIN_H, MAX_H);
				break;
			case 'n':
				SET_ROPT(ropt_nset, N_MSK);
				ATOI_S(iter_n, optarg, endptr);
				CHECK_RANGE(opt, iter_n, MIN_N, MAX_N);
				break;
			case 'r':
				ATOF_S(pres, optarg, endptr);
				break;
			case 'h':
				printf("%s\n", USAGE);
				printf("%s\n", USAGE_V);
				exit(EXIT_SUCCESS);
			case 0:
				if ( l_optind == 1 ) {
					is_test = 1;
				}
				break;
			case ':':
				FAIL("%c requires argument(s)!\n", optopt);
			case '?':
				FAIL("Option %c unrecognized!\n", optopt);
		}
	}

	if ( optind != argc ) {
		FAIL("No paramters other than options are allowed\n");
	}

	if ( ropt_nset ) {
		if ( ropt_nset & D_MSK ) {
			fprintf(stderr, "Option -d is required but was not set!\n");
		}

		if ( (ropt_nset & N_MSK) && !is_test ) {
			fprintf(stderr, "Option -n is required but was not set!\n");
		}

		if ( !is_test ) {
			goto Lerr;
		}
	}

	unsigned char* img;
	if ( (img = malloc(img_w*BYTESPP * img_h*BYTESPP + sizeof (BMP_H)) ) == NULL)
		goto Lerr;

	printf("Caculating results\n");
	//TODO more simple
	if ( is_test ) {
		for (size_t i = 0; i < img_w*img_h; i++) {
			//img[i] = 0xff;
			if ( i % 2 == 0 ) {
				img[i] = 0xff;
			} else {
				img[i] = 0;
			}
		}
	} else {
		if (time_cap > 0) {
			struct timespec start;
			struct timespec end;

			clock_gettime(CLOCK_MONOTONIC, &start);
			for (unsigned long i = 0; i < (unsigned) time_cap; i++) {
				bs[impl_ind](s_val, img_w, img_h, pres, iter_n, img);
			}
			clock_gettime(CLOCK_MONOTONIC, &end);
			
			double time = end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);
			double avg_time = time / time_cap;

			printf("Width: %ld Height: %ld Iterations: %d\nTime spent: %f Average Time spent: %f\n",
					img_w, img_h, time_cap, time, avg_time);
		} else {
			bs[impl_ind](s_val, img_w, img_h, pres, iter_n, img);
		}
	}

	// FILE PATH for result
	char* fpath = alloca(strlen(file_name)+1 + 2 + 4);
	strncpy(fpath, "./", 3);
	strncat(fpath, file_name, strlen(file_name)+1);
	strncat(fpath, ".bmp", 5);

	// WRITING IMAGE DATA INTO FILE
	printf("Writing image\n");
	BMP_H bmph = creat_bmph(img_w, img_h);
	if ( writef_bmp(img, fpath, bmph) < 0 ) //TODO
		goto Lerr;

	free(img);
	return EXIT_SUCCESS;
Lerr:
	FAIL_E();
}

#undef ATOT_S
#undef STRTOLT
#undef STRTOFT
#undef ATOI_S
#undef ATOLL_S
#undef ATOF_S
#undef ATOD_S

