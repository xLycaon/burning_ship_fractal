#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

typedef struct {
	uint32_2a entry[2];
} COLORP;

//TODO newline appended at the end after fwrite
void
writef_bmp(unsigned char* img, const char* path, BMP_H bmph) { //TODO annahme 1 Pixel ist 1 Byte
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
	unsigned char npad = 32 - (bmph.img_width % 32);
	unsigned char zeros[npad];
	memset(zeros, 0, npad);
	for (size_t y = 0; y < bmph.img_height; y++) {
		fwrite(&img[y * bmph.img_width], 1, bmph.img_width, file);
		// ROW PADDING
		fwrite(zeros, 1, npad, file);
	}
}

BMP_H
creat_bmph(size_t img_w, size_t img_h) {
	BMP_H bmp;
	bmp.magic = MAGIC;
	bmp.size = img_w * img_h;
	bmp.reserved = 0;
	bmp.dib_offset = sizeof (BMP_H);
	bmp.header_size = sizeof (BMP_H) - 14;
	bmp.img_width = img_w;
	bmp.img_height = img_h;
	bmp.planes = 1;
	bmp.bpp = 8;
	bmp.compression = 0;
	bmp.img_size = 0;
	bmp.Xppm = 0;
	bmp.Yppm = 0;
	bmp.total_colors = 0;
	bmp.important_colors = 0;
	return bmp;
}
