#pragma once

#define PROGNAME "prog"
#define USAGE \
"usage: " PROGNAME " [-h | --help] [-o <file name>]\n" \
"            [-V <Integer>] [-B <Integer>] [-r <Floating Point Number>]\n" \
"            [-s <Real part>,<Imaginary part>] [-d <Integer>,<Integer>] [-n <Integer>]\n" \
"            [-t | --test]\n"

//TODO
#define INVALID_CHARS ""
#define NUM_IMPL "2"
#define MAX_BENCH "100"

#define USAGE_V \
"Only the options below are accepted!" \
"\n\n" \
"Non-optional arguments:" \
"\n" \
"-n <Integer> --- Accepts integer values in range [1, 100]. The number indicates the maximum number of iterations that are used to determine the color of each Pixel." \
"\n" \
"-d <Integer>,<Integer> --- Accepts integer values in range [100, 10000]. The numbers indicate the width and height of the image in pixels." \
"\n\n" \
"Optional arguments:" \
"\n" \
"-o <file name> --- Sets a name for the output file. Any file name that contains invalid characters (" INVALID_CHARS ") is not allowed." \
"\n"            \
"-V <Integer> --- Accepts integer values in range [0," NUM_IMPL "]. The number indicates which implementation of the following "           \
"is used to create the image: \n" \
"\t0 -> standard implementation\n" \
"\t1 -> SIMD implementation\n"           \
"\tOnly ONE implementation can be selected at once!. If not set the standard implementation is used instead!"           \
"\n" \
"-s <Real part>,<Imaginary part> --- Sets the used coordinates (r, i) on the complex plane to be the center of the image. The default is (0, 0). "                                  \
"Because the burning-ship Fractal is located in the area of the complex plane that spans a vector space of [-2, 2] x [-2,2] only values between -2 and 2 are allowed."            \
"Values MUST be seperated by a comma with NO spaces in between them!"            \
"\n"            \
"-r <Floating Point Number> --- Zooms into the Fractal. Values > 1 Zoom out and 0 < values < 1 Zoom in. The precision for Zooming is at 1E-5." \
" Negative numbers have no meaning this context and thus are discarded." \
" Values MUST be seperated by a comma with NO spaces in between them!" \
"\n\n" \
"Testing arguments:" \
"\n" \
"--test --- Runs the program in test mode. This mode is used to test the correctness of the program by comparing the results of the given Implementation and the SIMD implementation " \
"\n" \
"-B <Integer> --- When this option is set, benchmarks of the specified implementation are run. The number in range [1," MAX_BENCH"] specifies how many times a benchmark is run." \
"\n\n"            \
"-h | --help --- Displays verbose information about options, their parameters and shows some examples on how to use them." 
