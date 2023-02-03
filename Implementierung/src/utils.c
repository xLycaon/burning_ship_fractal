#include <stdio.h>
#include <string.h>

#include "utils.h"

const struct COLOR_TB16 BGW_EXTENDED_CTB = {
    		.entry[0] = {0xff, 0xff, 0xff, 0},
            .entry[1] = {0xee, 0xee, 0xee, 0},
            .entry[2] = {0xdd, 0xdd, 0xdd, 0},
            .entry[3] = {0xcc, 0xcc, 0xcc, 0},
            .entry[4] = {0xbb, 0xbb, 0xbb, 0},
            .entry[5] = {0xaa, 0xaa, 0xaa, 0},
            .entry[6] = {0x99, 0x99, 0x99, 0},
            .entry[7] = {0x88, 0x88, 0x88, 0},
            .entry[8] = {0x77, 0x77, 0x77, 0},
            .entry[9] = {0x66, 0x66, 0x66, 0},
            .entry[10] = {0x55, 0x55, 0x55, 0},
            .entry[11] = {0x44, 0x44, 0x44, 0},
            .entry[12] = {0x33, 0x33, 0x33, 0},
            .entry[13] = {0x22, 0x22, 0x22, 0},
            .entry[14] = {0x11, 0x11, 0x11, 0},
            .entry[15] = {0x00, 0x00, 0x00, 0}
};

const struct COLOR_TB16 RB_EXTENDED_CTB = {
    		.entry[0] = {0xff, 0x00, 0x00, 0},
            .entry[1] = {0xee, 0x00, 0x11, 0},
            .entry[2] = {0xdd, 0x00, 0x22, 0},
            .entry[3] = {0xcc, 0x00, 0x33, 0},
            .entry[4] = {0xbb, 0x00, 0x44, 0},
            .entry[5] = {0xaa, 0x00, 0x55, 0},
            .entry[6] = {0x99, 0x00, 0x66, 0},
            .entry[7] = {0x88, 0x00, 0x77, 0},
            .entry[8] = {0x77, 0x00, 0x88, 0},
            .entry[9] = {0x66, 0x00, 0x99, 0},
            .entry[10] = {0x55, 0x00, 0xaa, 0},
            .entry[11] = {0x44, 0x00, 0xbb, 0},
            .entry[12] = {0x33, 0x00, 0xcc, 0},
            .entry[13] = {0x22, 0x00, 0xdd, 0},
            .entry[14] = {0x11, 0x00, 0xee, 0},
            .entry[15] = {0x00, 0x00, 0xff, 0}
};

const struct COLOR_TB16 FIRE_CTB = {
    		.entry[0] = {0x00, 0x44, 0xff, 0},
            .entry[1] = {0x00, 0x51, 0xff, 0},
            .entry[2] = {0x00, 0x5e, 0xfe, 0},
            .entry[3] = {0x00, 0x6b, 0xfe, 0},
            .entry[4] = {0x00, 0x78, 0xfe, 0},
            .entry[5] = {0x00, 0x85, 0xfd, 0},
            .entry[6] = {0x00, 0x92, 0xfd, 0},
            .entry[7] = {0x00, 0x9f, 0xfd, 0},
            .entry[8] = {0x00, 0xac, 0xfc, 0},
            .entry[9] = {0x00, 0xb9, 0xfc, 0},
            .entry[10] = {0x00, 0xc6, 0xfb, 0},
            .entry[11] = {0x00, 0xd3, 0xfb, 0},
            .entry[12] = {0x00, 0xe0, 0xfb, 0},
            .entry[13] = {0x00, 0xed, 0xfa, 0},
            .entry[14] = {0x00, 0xfa, 0xfa, 0},
            .entry[15] = {0x00, 0x00, 0x00, 0}
};

const struct COLOR_TB16 OB_CTB = {
    		.entry[0] = {0xff, 0xff, 0xff, 0},
            .entry[1] = {0x00, 0x95, 0xf1, 0},
            .entry[2] = {0x13, 0x8f, 0xe3, 0},
            .entry[3] = {0x26, 0x89, 0xd4, 0},
            .entry[4] = {0x44, 0xb1, 0xbd, 0},
            .entry[5] = {0x55, 0xb8, 0xb0, 0},
            .entry[6] = {0x66, 0xbf, 0xa3, 0},
            .entry[7] = {0x77, 0xc6, 0x96, 0},
            .entry[8] = {0x88, 0xcd, 0x89, 0},
            .entry[9] = {0x99, 0xd4, 0x7c, 0},
            .entry[10] = {0xaf, 0x5d, 0x6b, 0},
            .entry[11] = {0xc1, 0x57, 0x5e, 0},
            .entry[12] = {0xd2, 0x51, 0x51, 0},
            .entry[13] = {0xe4, 0x4c, 0x43, 0},
            .entry[14] = {0xf5, 0x46, 0x36, 0},
            .entry[15] = {0x00, 0x00, 0x00, 0}
};

static inline BMP_H creat_bmph(struct DIM dim) {
    BMP_H bmp;
    bmp.magic = MAGIC;
    bmp.fsize = BMRS(dim.width) * dim.height + sizeof (BMP_H) + sizeof (struct COLOR_TB16);
    bmp.reserved = 0;
    bmp.dib_offset = sizeof (BMP_H) + sizeof (struct COLOR_TB16);
    bmp.header_size = sizeof (BMP_H) - 14;
    bmp.img_width = dim.width;
    bmp.img_height = dim.height;
    bmp.planes = 1;
    bmp.bpp = BYTESPP * 8;
    bmp.compression = 0;
    bmp.img_size = 0;
    bmp.Xppm = 0;
    bmp.Yppm = 0;
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
    written += (ssize_t) fwrite((char *) &bmph, sizeof (char), sizeof (BMP_H), file);

    // Write COLOR TABLE into file
    written += (ssize_t) fwrite(&OB_CTB, sizeof (char), sizeof (struct COLOR_TB16), file);

    // PADDING MEM
    unsigned char npad = BMP_ROW_PADDING(bmph.img_width);
	unsigned char zeros[npad];
	memset(zeros, 0, npad);

	// Write IMAGE DATA row-wise reversed into file
	for (size_t y = bmph.img_height-1; (y+1) > 0; y--) {
        written += (ssize_t) fwrite(&(img[y * BMDIM(bmph.img_width)]), sizeof (char), BMDIM(bmph.img_width), file);
        written += (ssize_t) fwrite(zeros, sizeof (char), npad, file);
	}

	fclose(file);
	return written;
}
