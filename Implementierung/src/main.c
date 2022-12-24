#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <complex.h>
#include <string.h>

#ifdef _WIN32
#include <malloc.h>
#define _alloca(x) alloca(x)
#define _malloca(x) alloca(x)
#else
#include <alloca.h>
#endif

#include "utils.h"

//TODO debugging
#define ATOI_nCC(var, arg, fcond) errno = 0; \
				  var = strtol(arg, &endptr, 10); \
				  if ( errno != 0 || endptr == arg || *endptr != '\0' || fcond ) \
				  	goto Lerr;

#define ATOF_S(var, arg) errno = 0; \
				  var = strtof(arg, &endptr); \
				  if ( errno != 0 || *endptr != '\0' || !var ) \
				  	goto Lerr;

extern void
burning_ship(float complex start, size_t width, size_t height,
		  float res, unsigned n, unsigned char* img);

int main(int argc, char* argv[argc]) {
	const char* const usage =
	"usage: prog [-h | --help] [-o <Dateiname>]\n"
	"[-V <Zahl>] [-B <Zahl>]\n"
	"[-s <Realteil>,<Imaginärteil>] [-d <Zahl>,<Zahl>] [-n <Zahl>] [-r <Floating Point Zahl>]\n"
	"[--bmpftest]";
	//TODO verbose man
	const char* const usage_v =
		"usage: NO PARAMETERS ARE ACCEPTED OTHER THAN OPTIONS\n"
		"-h | --help *** displays verbose help\n"
		"-d<width,height> *** Sets the dimensions of the output image. NO SPACE after , is allowed. Ex: -d100,200 or -d 100,200 but not d 100, 200.\n"
		"--bmpftest *** Runs tests checking the validity of the output format and ingoring all options except for dimensions.";

	// DEFAULT parameter values
	int impl_ind = 0;
	int time_cap = -1;
	float complex s_val = 0.0;
	unsigned long img_w = 5;
	unsigned long img_h = 5;
	int iter_n = 10;
	float incr = 1.0; //TODO
	char* file_name = "burning_ship";
	int is_test = 0;

	int opt;
	int l_optind;
	char* endptr;
	char* tkns[2];
	const char* optstr = "V:B:s:d:n:r:o:h";
	const struct option longopts[] = {
		{"help", no_argument, NULL, 0},
		{"bmpftest", no_argument, NULL, 0},
		{NULL, 0, NULL, 0}
	};

	// UPDATES parameters
	while ( (opt = getopt_long(argc, argv, optstr, longopts, &l_optind)) != -1 ) {
		switch (opt) {
			case 'V':
				ATOI_nCC(impl_ind, optarg, impl_ind < 0);
				break;
			case 'B': 
				ATOI_nCC(time_cap, optarg, time_cap < 1);
				break;
			case 's':
				float s_real;
				float s_img;

				if ( (tkns[0] = strtok(optarg, ",")) == NULL )
					goto Lerr;
				if ( (tkns[1] = strtok(NULL, ",")) == NULL )
					goto Lerr;

				ATOF_S(s_real, tkns[0]);
				ATOF_S(s_img, tkns[1]);

				s_val = s_real + I * s_img;
				break;
			case 'd':
				if ( (tkns[0] = strtok(optarg, ",")) == NULL )
					goto Lerr;
				if ( (tkns[1] = strtok(NULL, ",")) == NULL )
					goto Lerr;

				ATOI_nCC(img_w, tkns[0], img_w < 1);
				ATOI_nCC(img_h, tkns[1], img_h < 1);
				break;
			case 'n':
				ATOI_nCC(iter_n, optarg, iter_n < 1 );
				break;
			case 'r':
				ATOF_S(incr, optarg);
				break;
			case 'o':
				file_name = optarg; //TODO max file length
				break;
			case 0:
				if ( strncmp(longopts[l_optind].name, "bmpftest", 9) == 0 ) {
					is_test = 1;
					break;
				} else if ( strncmp(longopts[l_optind].name, "help", 5) != 0 ) {
					goto Lerr;
				}
			case 'h':
				printf("%s\n", usage_v);
				exit(EXIT_SUCCESS);
			case '?': 
				if ( optopt == 'B' || optopt == 'V' )
					break;
			default:
				goto Lerr;
		}
	}

	//TODO edge case for long options and non options in combination
	if ( optind != argc ) // no other elements than options in argv allowed
		goto Lerr;

	unsigned char* img;
	if ( (img = malloc(img_w*BYTESPP * img_h*BYTESPP + sizeof (BMP_H)) ) == NULL)
		goto Lerr;

	printf("Caculating results\n");
	//TODO
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
			//TODO run performance measurements
			printf("Measurements not implemented yet\n");
			free(img);
			return EXIT_FAILURE;
		} else {
			burning_ship(s_val, img_w, img_h, incr, iter_n, img);
		}
	}

	// FILE PATH for result
	char* fpath = alloca(strlen(file_name)+1 + 2 + 4);
	strncpy(fpath, "./", 3);
	strncat(fpath, file_name, strlen(file_name)+1);
	strncat(fpath, ".bmp", 5);

	printf("Writing image\n");
	BMP_H bmph = creat_bmph(img_w, img_h);
	writef_bmp(img, fpath, bmph);

	free(img);
	return EXIT_SUCCESS;
Lerr:
	printf("%s\n", usage);
	exit(EXIT_FAILURE);
}

#undef ATOI_nCC
#undef ATOF_S
