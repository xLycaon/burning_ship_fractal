#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <complex.h>
#include <string.h>
#include <malloc.h>

#ifndef _WIN32
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

int main(int argc, char* argv[argc]) {
	const char* const usage =
	"usage: prog [-h | --help] [-o <Dateiname>] <file>\n"
	"[-V <Zahl>] [-B <Zahl>]\n"
	"[-s <Realteil>,<Imaginärteil>] [-d <Zahl>,<Zahl>] [-n <Zahl>] [-r <Floating Point Zahl>]";
	const char* const usage_v = "TEST"; // TODO verbose usage man

	/*
	if (argc == 1)
		goto Lerr;
		*/

	// DEFAULT parameter values
	int impl_ind = 0;
	int time_cap = -1;
	float complex s_val;
	int img_w = 2;
	int img_h = 2;
	int iter_n = 10;
	float incr;
	char* file_name = "burning_ship";

	int opt;
	int l_optind;
	char* endptr;
	char* tkns[2];
	const char* optstr = "V:B:s:d:n:r:o:h";
	const struct option longopts[] = {
		{"help", no_argument, NULL, 0},
		{NULL, 0, NULL, 0}
	};

	// UPDATES parameters
	// TODO parameters that must be set
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
			case 0: // with fallthrough to 'h' due to no other long options
				if ( strncmp(longopts[l_optind].name, "help", 5) != 0 )
					goto Lerr;
			case 'h':
				printf("%s\n", usage);
				printf("%s\n", usage_v);
				exit(EXIT_SUCCESS);
			case '?': 
				if ( optopt == 'B' || optopt == 'V' )
					break;
			default:
				goto Lerr;
		}
	}

	unsigned char* img;
	if ( (img = malloc(img_w * img_h + sizeof (BMP_H) + COLORP_S * COLORP_ES) ) == NULL)
		goto Lerr;

	printf("Caculating results\n");
	//burning_ship(s_val, img_w, img_h, incr, iter_n, img);
	//TODO just for testing
	img[0] = 0;
	img[1] = 1;
	img[2] = 0;
	img[3] = 1;


	char* path;
#ifndef _WIN32
	path = alloca(strlen(file_name)+1 + 2 + 4);
#else
	path = _malloca(strlen(file_name)+1 + 2 + 4);
#endif
	strncpy(path, "./", 3);
	strncat(path, file_name, strlen(file_name)+1);
	strncat(path, ".bmp", 5);

	printf("Writing image\n");
	BMP_H bmph = creat_bmph(img_w, img_h);
	writef_bmp(img, path, bmph);

	free(img);
	return EXIT_SUCCESS;
Lerr:
	printf("%s\n", usage);
	exit(EXIT_FAILURE);
}

#undef ATOI_nCC
#undef ATOF_S
