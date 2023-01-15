#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

//#define MAGIC 0x424d
#define MAGIC 0x4d42
#define PAD_ALIGN 4
#define BYTESPP 1/*3*/ // 24 (bpp) / 8 (bits)

#define STRLCPY(SRC, DEST, LEN) \
strncpy((SRC), (DEST), (LEN)-1); \
(SRC)[(LEN)-1] = '\0'

//Padding is set to {0, 1, 2, 3}
#define PADDING_(N) ( (PAD_ALIGN) - ((N)*BYTESPP % PAD_ALIGN) )
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
	uint16_t magic;
	uint32_2a fsize;
	uint32_2a reserved;
	uint32_2a dib_offset;
	uint32_2a header_size;
	uint32_2a img_width;
	uint32_2a img_height;
	uint16_t planes;
	uint16_t bpp;
	uint32_2a compression;
	uint32_2a img_size;
	uint32_2a Xppm;
	uint32_2a Yppm;
	uint32_2a total_colors;
	uint32_2a important_colors;
} BMP_H;

typedef struct COLOR {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t reserved;
} color_t;

#define TOTAL_COLORS 16
#define IMPORTANT_COLORS 0 // 0 means all colors are important

struct COLOR_TB {
    color_t entry[TOTAL_COLORS];
};

extern const struct COLOR_TB BW_CTB;

extern const struct COLOR_TB BGW_CTB;


struct DIM { //TODO
    size_t width;
    size_t height;
};

// Creates a new .bmp file with name as in path by:
// 1) Writing the BITMAP header (bmph) in correct order first.
// 2) Writing IMAGE DATA accordingly.
ssize_t
writef_bmp(unsigned char* img, const char* path, struct DIM dim);
