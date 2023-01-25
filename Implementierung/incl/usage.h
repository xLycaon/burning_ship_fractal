#pragma once

#define PROGNAME "prog"
#define USAGE \
"usage: " PROGNAME " [-h | --help] [-o <file name>]\n" \
"            [-V <Integer>] [-B <Integer>] [-r <Floating Point Number>]\n" \
"            [-s <Real part>,<Imaginary part>] [-d <Integer>,<Integer>] [-n <Integer>]\n" \
"            [-t | --test]"

//TODO
#define INVALID_CHARS ""
#define NUM_IMPL "2"
#define MAX_BENCH "100"

#define USAGE_V \
"Only the options below are accepted!" \
"\n" \
"-h | --help --- Displays verbose information about options, their parameters and shows some examples on how to use them." \
"\n"            \
"-o <file name> --- Sets a name for the output file. Any file name that contains invalid characters (" INVALID_CHARS ") is not allowed." \
"\n"            \
"-V <Integer> --- Accepts integer values in range [0," NUM_IMPL "]. The number indicates which implementation of the following "           \
"is used to create the image: \n" \
"0 -> standard implementation\n" \
"1 -> SIMD implementation\n"           \
"Only ONE implementation can be selected at once!. If not set the standard implementation is used instead!"           \
"\n" \
"-B <Integer> --- When this option is set, benchmarks of the specified implementation are run. The number in range [1," MAX_BENCH"] specifies how many times a benchmark is run." \
"\n"            \
"-s <Real part>,<Imaginary part> --- Sets the used coordinates (r, i) on the complex plane to be the center of the image. The default is (0, 0). "                                  \
"Because the burning-ship Fractal is located in the area of the complex plane that spans a vector space of [-2, 2] x [-2,2] only values between -2 and 2 are allowed."            \
"Values MUST be seperated by a comma with NO spaces in between them!"            \
"\n"            \
"-r <Floating Point Number> --- Zooms into the Fractal. Values > 1 Zoom out and 0 < values < 1 Zoom in. The precision for Zooming is at 1E-5." \
" Negative numbers have no meaning this context and thus are discarded." \
" Values MUST be seperated by a comma with NO spaces in between them!"
