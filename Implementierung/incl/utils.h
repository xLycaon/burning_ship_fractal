#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define ARRAY_SIZE(arr) sizeof (arr) / sizeof (arr[0])

#define MAGIC 0x424D 
#define PAD_ALIGN 4
#define BYTESPP 3 // 24 (bpp) / 8 (bits)

typedef uint32_t uint32_2a __attribute__((__aligned__(2)));

// Mapping of BMP format which is 54 bytes long
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

// Creates a file in BMP-format and writes header and image data into file specified by path.
// Returns -1 on fail and number of bytes written on a successful write.
ssize_t
writef_bmp(unsigned char* img, const char* path, BMP_H bmph);

// Returns a mapped BMP_H struct with the specified width and height of the image
// and standard values required for the 24bmp format.
BMP_H
creat_bmph(size_t img_w, size_t img_h);

#endif
