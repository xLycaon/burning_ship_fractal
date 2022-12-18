#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <complex.h>
#include <string.h>

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

	if (argc == 1)
		goto Lerr;

	// DEFAULT parameter values
	int impl_ind = 0;
	int time_cap = -1;
	float complex s_val;
	int img_w;
	int img_h;
	int iter_n;
	float incr;
	char* file_name = "prog";

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
				ATOI_nCC(img_h, tkns[0], img_h < 1);
				break;
			case 'n':
				ATOI_nCC(iter_n, optarg, (iter_n > 0) );
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
				if ( optopt == 'B' )
					break;
			default:
				goto Lerr;
		}
	}
	return EXIT_SUCCESS;
Lerr:
	printf("%s\n", usage);
	exit(EXIT_FAILURE);
}

#undef ATOI_nCC
#undef ATOF_S
