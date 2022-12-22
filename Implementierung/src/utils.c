#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <byteswap.h>

#include "utils.h"

typedef struct {
	uint32_2a entry[2];
} COLORP;

//TODO reading out values from BMP_H
void
writef_bmp(unsigned char* img, const char* path, BMP_H bmph) { //TODO annahme 1 Pixel ist 1 Byte
	//TODO val little endian to val big endian
	uint32_t width = bswap_32(bmph.img_width);
	uint32_t height = bswap_32(bmph.img_height);

	FILE* file;
	if ( (file = fopen(path, "w")) == NULL )
		exit(EXIT_FAILURE);

	// Write bmp header into file
	fwrite((char *) &bmph, 1, sizeof (BMP_H), file);

	// Write color pallet data into file
	COLORP cp = { .entry[0] = 0xFFFFFF00, .entry[1] = 0x00000000};
	printf("Size of color pallet: %ld\n", sizeof (COLORP));
	fwrite(&cp, 1, COLORP_S * COLORP_ES, file); //TODO better solution for color pallet

	// Write image data into file
	unsigned char npad = 32 - (width % 32);
	unsigned char zeros[npad];
	memset(zeros, 0, npad);
	size_t y = 0;
	for (; y < height; y++) {
		fwrite(&img[y * width], 1, width, file);
		// ROW PADDING
		fwrite(zeros, 1, npad, file);
	}
}

BMP_H
creat_bmph(size_t img_w, size_t img_h) { // bswap for big endian -> little endian
	// TODO CAUTION: bswap_x is a macro not a function
	uint32_2a file_size = img_w * img_h + sizeof (BMP_H) + sizeof (COLORP);
	uint32_2a dib_off = sizeof (BMP_H) + sizeof (COLORP);
	uint32_2a h_size = sizeof (BMP_H) - 14;

	BMP_H bmp;
	bmp.magic = bswap_16(MAGIC);
	bmp.fsize = bswap_32(file_size);
	bmp.reserved = 0;
	bmp.dib_offset = bswap_32(dib_off);
	bmp.header_size = bswap_32(h_size);
	bmp.img_width = bswap_32(img_w);
	bmp.img_height = bswap_32(img_h);
	bmp.planes = bswap_16(1);
	bmp.bpp = bswap_16(8);
	bmp.compression = 0;
	bmp.img_size = 0;
	bmp.Xppm = 0;
	bmp.Yppm = 0;
	bmp.total_colors = bswap_32(2);
	bmp.important_colors = 0;
	return bmp;
}
