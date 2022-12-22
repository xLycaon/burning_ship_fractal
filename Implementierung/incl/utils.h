#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

#define MAGIC 0x424D //TODO little endian format
#define COLORP_S 2
#define COLORP_ES 4
#define PAD_ALIGN 4

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

void
writef_bmp(unsigned char* img, const char* path, BMP_H bmph);

BMP_H
creat_bmph(size_t img_w, size_t img_h);

//TODO idea for converting big-endian to little-endian BMP-Format for file write
void
cvtbmph_bele(BMP_H* bmph);
void
cvtbmph_lebe(BMP_H* bmph);

#endif
