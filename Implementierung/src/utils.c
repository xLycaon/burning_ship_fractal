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

	// Set padding to MAX 3 (PAD_ALIGN - 1)
	unsigned char npad = (PAD_ALIGN - (bmph.img_width*BYTESPP % PAD_ALIGN));
	npad = npad % PAD_ALIGN ? npad : 0;
	unsigned char zeros[npad];
	memset(zeros, 0, npad);

	// Write IMAGE DATA row-wise reversed into file
	for (size_t y = bmph.img_height-1; (y+1) > 0; y--) {
		written += fwrite(&(img[y * bmph.img_width*BYTESPP]), 1, bmph.img_width*BYTESPP, file);
		written += fwrite(zeros, 1, npad, file); // ROW PADDING
	}

	fclose(file);
	return written;
}

//TODO endianess
BMP_H
creat_bmph(size_t img_w, size_t img_h) { 
	BMP_H bmp;
	bmp.magic = bswap_16(MAGIC); // bswap for big endian -> little endian
	bmp.fsize = img_w*BYTESPP * img_h*BYTESPP + sizeof (BMP_H);
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
