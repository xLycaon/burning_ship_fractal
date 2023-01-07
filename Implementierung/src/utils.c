#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <libkern/OSByteOrder.h>
#define bswap_16(x) OSSwapInt16(x)
#else
#include <byteswap.h>
#endif

#include "utils.h"

ssize_t
writef_bmp(unsigned char* img, const char* path, BMP_H bmph) {
	FILE* file;
	if ( (file = fopen(path, "w")) == NULL ) 
		return -1;
	ssize_t written = 0;

	// Write BMP HEADER into file
	written += fwrite((char *) &bmph, 1, sizeof (BMP_H), file);
    if ( written != sizeof (BMP_H) ) {
        fprintf(stderr, "WARNING: ONLY %lu/%lu Bytes of the Header were able to be written!\n",
                written, sizeof (BMP_H));
    }

    unsigned char npad = BMP_ROW_PADDING(bmph.img_width);
	unsigned char zeros[npad];
	memset(zeros, 0, npad);

	// Write IMAGE DATA row-wise reversed into file
	for (size_t y = bmph.img_height-1; (y+1) > 0; y--) {
        written += fwrite(&(img[y * BMDIM(bmph.img_width)]), 1, BMDIM(bmph.img_width), file);
		written += fwrite(zeros, 1, npad, file); // ROW PADDING
	}
    if ( written != bmph.fsize ) { //TODO
        fprintf(stderr, "WARNING: ONLY %lu/%u BYTES were able to be written in total!\n",
                written, bmph.fsize);
    }

	fclose(file);
	return written;
}

//TODO endianess
BMP_H
creat_bmph(size_t img_w, size_t img_h) { 
	BMP_H bmp;
	bmp.magic = bswap_16(MAGIC); // bswap for big endian -> little endian
	bmp.fsize = BMRS(img_w) * img_h + sizeof (BMP_H);
	bmp.reserved = 0;
	bmp.dib_offset = sizeof (BMP_H);
	bmp.header_size = sizeof (BMP_H) - 14;
	bmp.img_width = img_w;
	bmp.img_height = img_h;
	bmp.planes = 1;
	bmp.bpp = BYTESPP * 8;
	bmp.compression = 0;
	bmp.img_size = 0;
	bmp.Xppm = 0;
	bmp.Yppm = 0;
	bmp.total_colors = 0;
	bmp.important_colors = 0;
	return bmp;
}
