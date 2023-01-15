#include <stdio.h>
#include <string.h>

#include "utils.h"

//TODO
#define WARN(W, E, N) ("WARNING: Only "#W" out of "#E" bytes were able to be written in TOTAL ("#N")!")

const struct COLOR_TB BW_CTB = {
        .entry[0] = {0, 0, 0, 0},
        .entry[1] = {0xff, 0xff, 0xff, 0}
};

const struct COLOR_TB BGW_CTB = {
        .entry[4] = {31, 31, 31, 0},
        .entry[3] = {84, 84, 84, 0},
		.entry[2] = {137, 137, 137, 0},
		.entry[1] = {189, 190, 190, 0},
		.entry[0] = {242, 243, 243, 0},
};

static inline BMP_H creat_bmph(struct DIM dim) {
    BMP_H bmp;
    bmp.magic = MAGIC;
    bmp.fsize = BMRS(dim.width) * dim.height + sizeof (BMP_H) + sizeof BGW_CTB;
    bmp.reserved = 0;
    bmp.dib_offset = sizeof (BMP_H) + sizeof BGW_CTB;
    bmp.header_size = sizeof (BMP_H) - 14;
    bmp.img_width = dim.width;
    bmp.img_height = dim.height;
    bmp.planes = 1;
    bmp.bpp = BYTESPP * 8;
    bmp.compression = 0;
    bmp.img_size = 0; //TODO
    bmp.Xppm = 0; //TODO scaling?
    bmp.Yppm = 0; //TODO scaling?
    bmp.total_colors = TOTAL_COLORS;
    bmp.important_colors = IMPORTANT_COLORS;
    return bmp;
}

ssize_t
writef_bmp(unsigned char* img, const char* path, struct DIM dim) {
	FILE* file;
	if ( (file = fopen(path, "w")) == NULL ) 
		return -1;

	ssize_t written = 0;

	// Write BMP HEADER into file
    BMP_H bmph = creat_bmph(dim);
    written += fwrite((char *) &bmph, sizeof (char), sizeof (BMP_H), file);
	if ( written != sizeof (BMP_H) ) {
	    fprintf(stderr, "WARNING: ONLY %lu/%lu Bytes of the Header were able to be written!\n",
	            written, sizeof (BMP_H));
	}

    // Write COLOR TABLE into file
    written += fwrite(&BGW_CTB, sizeof (char), sizeof BGW_CTB, file);
    if ( written != sizeof (BMP_H) + sizeof BW_CTB ) {
        fprintf(stderr, "WARNING: ONLY %lu/%lu Bytes of the COLOR TABLE were able to be written!\n",
                written - sizeof (BMP_H), sizeof BGW_CTB);
    }

    // PADDING MEM
    unsigned char npad = BMP_ROW_PADDING(bmph.img_width);
	unsigned char zeros[npad];
	memset(zeros, 0, npad);

	// Write IMAGE DATA row-wise reversed into file
	for (size_t y = bmph.img_height-1; (y+1) > 0; y--) {
        written += fwrite(&(img[y * BMDIM(bmph.img_width)]), sizeof (char), BMDIM(bmph.img_width), file);
        written += fwrite(zeros, sizeof (char), npad, file);
	}
	if ( written != bmph.fsize ) {
	    fprintf(stderr, "WARNING: ONLY %lu/%u BYTES were able to be written in total!\n",
	            written, bmph.fsize);
	}

	fclose(file);
	return written;
}
