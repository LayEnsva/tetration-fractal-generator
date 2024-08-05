#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define _OPEN_SYS_ITOA_EXT

int bitmap_width, bitmap_height, bitmap_filesize, bitmap_imagesize;
uint8_t *bitmap_image;

uint8_t bitmap_fileheader[14] = {
    'B', 'M',				// Magic number
    00, 00, 00, 00,			// File size in bytes
    00, 00,					// Reserved
    00, 00, 54, 00, 00,		// Offset to bitmap_image data
};

uint8_t bitmap_infoheader[40] = {
    40, 00, 00, 00,		// Header size
    00, 00, 00, 00,		// Image width
    00, 00, 00, 00,		// Image height
    01, 00,        		// Number of color planes
    24, 00,        		// Bits per pixel
    00, 00, 00, 00,		// Compression method
    00, 00, 00, 00,		// Image size
    00, 00, 00, 00,		// Horizontal resolution
    00, 00, 00, 00,		// Vertical resolution
    00, 00, 00, 00,		// Number of colors
    00, 00, 00, 00,		// Number of important colors
};

void drawInit(int W, int H) {
	
	bitmap_width     = W & (int) ~0x3;
	bitmap_height    = H & (int) ~0x3;
	bitmap_filesize  = 54 + bitmap_width * bitmap_height * 3;
	bitmap_imagesize = bitmap_width * bitmap_height * 3;
	
	uint8_t *tmpimage = (uint8_t *) malloc(bitmap_imagesize*sizeof(uint8_t));
	if(tmpimage == NULL) {
		perror("malloc\n");
		exit(-1);
	}
	
	bitmap_image = tmpimage;
	
    bitmap_fileheader[2] = (uint8_t) (bitmap_filesize);
    bitmap_fileheader[3] = (uint8_t) (bitmap_filesize >> 8);
    bitmap_fileheader[4] = (uint8_t) (bitmap_filesize >> 16);
    bitmap_fileheader[5] = (uint8_t) (bitmap_filesize >> 24);
	
    bitmap_infoheader[4] = (uint8_t) (bitmap_width);
    bitmap_infoheader[5] = (uint8_t) (bitmap_width >> 8);
    bitmap_infoheader[6] = (uint8_t) (bitmap_width >> 16);
    bitmap_infoheader[7] = (uint8_t) (bitmap_width >> 24);
	
    bitmap_infoheader[8] = (uint8_t) (bitmap_height);
    bitmap_infoheader[9] = (uint8_t) (bitmap_height >> 8);
    bitmap_infoheader[10]= (uint8_t) (bitmap_height >> 16);
    bitmap_infoheader[11]= (uint8_t) (bitmap_height >> 24);
}

int createFile(const char *path) {
	FILE *file;
    file = fopen(path, "wb");
    if (file == NULL) {
        fprintf(stderr, "bitmap.c: createFile: Failed to create bitmap.bmp file.\n");
        return EXIT_FAILURE;
    }
    fwrite(bitmap_fileheader, 1, 14, file);
    fwrite(bitmap_infoheader, 1, 40, file);
    fwrite(bitmap_image, 1, bitmap_imagesize, file);
	fclose(file);
	return EXIT_SUCCESS;
}

void createFileNumber(int n) {
	FILE *file;
	char fname[64];
	sprintf(fname,"bitmap%d.bmp",n);
    file = fopen(fname, "wb");
    fwrite(bitmap_fileheader, 1, 14, file);
    fwrite(bitmap_infoheader, 1, 40, file);
    fwrite(bitmap_image, 1, bitmap_imagesize,file);
	fclose(file);
}

//int upscale(char *image, int n) {
//    FILE *file = fopen(image, "rb");
//    if (file == NULL) {
//        fprintf(stderr, "bitmap.c: upscale: Failed to read file.\n");
//        return EXIT_FAILURE;
//    }
//    
//    fread(bitmap_fileheader, 1, 14, file);
//    fread(bitmap_infoheader, 1, 40, file);
//    
//    fseek(file, 0, SEEK_END);
//    long filesize  = ftell(file);
//    long imagesize = filesize - 54;
//    fseek(file, 54, SEEK_SET);
//}

int greyScale(uint8_t n) {
	return n*256*256+n*256+n;
}

void pixelRGB(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
	if(x>=bitmap_width || y>=bitmap_height) return;
	x*=3; y*=3;
	bitmap_image[x+bitmap_width*y] 	= b;
	bitmap_image[x+bitmap_width*y+1] = g;
	bitmap_image[x+bitmap_width*y+2] = r;
}

void pixelColor(int x, int y, uint32_t color) {
	if(x>=bitmap_width || y>=bitmap_height) return;
	x*=3; y*=3;
	bitmap_image[x+bitmap_width*y] 	= (color>>0)  & 0xff;
	bitmap_image[x+bitmap_width*y+1] = (color>>8)  & 0xff;
	bitmap_image[x+bitmap_width*y+2] = (color>>16) & 0xff;
}

void pixel(int x, int y, uint8_t grey) {
	if(x>=bitmap_width || y>=bitmap_height) return;
	x*=3; y*=3;
	bitmap_image[x+bitmap_width*y] 	= grey;
	bitmap_image[x+bitmap_width*y+1] = grey;
	bitmap_image[x+bitmap_width*y+2] = grey;
}

uint8_t getPixel(int x, int y) {
	if(x>=bitmap_width || y>=bitmap_height) return 0;
	x*=3; y*=3;
	return bitmap_image[x+bitmap_width*y];
}

void positionColor(int pos, uint32_t color) {
	pos*=3;
	bitmap_image[pos] 	 = (color>>0)  & 0xff;
	bitmap_image[pos+1] = (color>>8)  & 0xff;
	bitmap_image[pos+2] = (color>>16) & 0xff;
}

void position(int pos, uint8_t grey) {
	pos*=3;
	bitmap_image[pos] 	 = grey;
	bitmap_image[pos+1] = grey;
	bitmap_image[pos+2] = grey;
}
