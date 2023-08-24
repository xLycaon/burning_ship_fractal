#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#ifdef __GNU_MP_VERSION
#include <gmp.h> // TODO
#endif // __GNU_MP_VERSION

//#define MAGIC 0x424d
#define MAGIC 0x4d42 // magic number for BMP format, for little endian
#define PAD_ALIGN 4 // 4 byte alignment
#define BYTESPP 1 /*3*/ // 24 (bpp) / 8 (bits) //BYTES PER PIXEL

// String length copy macro with length check and null termination at the end of the string
#define STRLCPY(SRC, DEST, LEN) \
strncpy((SRC), (DEST), (LEN)-1); \
(SRC)[(LEN)-1] = '\0'

//Padding is set to {0, 1, 2, 3}
#define PADDING_(N) ( (PAD_ALIGN) - ((N)*BYTESPP % PAD_ALIGN) )
// calculates the padding for a row of pixels
#define BMP_ROW_PADDING(N) (PADDING_(N) % PAD_ALIGN ? PADDING_(N) : 0)

// BMpDIMension
#define BMDIM(N) ((N)*BYTESPP)

// BMpRowSize 
#define BMRS(N) (BMDIM(N)+BMP_ROW_PADDING(N))

typedef uint32_t uint32_2a __attribute__((__aligned__(2)));

// Mapping of BMP format which is 54 bytes long.
// Each member of the struct is aligned on a 2 byte boundary
// to guarantee a consistent total size of 54 bytes.
typedef struct {
	uint16_t magic; // magic number for BMP format, for little endian
	uint32_2a fsize; // file size
	uint32_2a reserved; // reserved
	uint32_2a dib_offset; // offset to the DIB header
	uint32_2a header_size; // size of the DIB header
	uint32_2a img_width; // width of the image
	uint32_2a img_height; // height of the image
	uint16_t planes; // number of color planes
	uint16_t bpp; // bits per pixel
	uint32_2a compression; // compression type
	uint32_2a img_size; // size of the image data
	uint32_2a Xppm; // horizontal resolution (pixels per meter)
	uint32_2a Yppm; // vertical resolution (pixels per meter)
	uint32_2a total_colors; // number of colors in the color table
	uint32_2a important_colors; // number of important colors
} BMP_H;

typedef struct COLOR {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t reserved;
} color_t;

// Number of colors in the color table
#define TOTAL_COLORS 16
// Number of important colors in the color table
#define IMPORTANT_COLORS 0 // 0 means all colors are important

struct COLOR_TB16 {
    color_t entry[16];
};

//DIMension
struct DIM { //TODO
    size_t width;
    size_t height;
};

//Write BMP file
ssize_t
writef_bmp(unsigned char* img, const char* path, struct DIM dim);
