#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define _OPEN_SYS_ITOA_EXT

int _width, _height, _filesize, _imagesize;
uint8_t *image;

uint8_t bmpfileheader[14] = {
    'B', 'M',				// Magic number
    00, 00, 00, 00,			// File size in bytes
    00, 00,					// Reserved
    00, 00, 54, 00, 00,		// Offset to image data
};

uint8_t bmpinfoheader[40] = {
    40, 00, 00, 00,		// Header size
    00, 00, 00, 00,		// Image _width
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
	
	_width = W&(int)~0x3;
	_height = H&(int)~0x3;
	_filesize = 54+_width*_height*3;
	_imagesize = _width*_height*3;
	
	uint8_t *tmpimage = (uint8_t *) malloc(_imagesize*sizeof(uint8_t));
	if(tmpimage == NULL) {
		perror("malloc\n");
		exit(-1);
	}
	
	image = tmpimage;
	
    bmpfileheader[2] = (uint8_t) (_filesize);
    bmpfileheader[3] = (uint8_t) (_filesize >> 8);
    bmpfileheader[4] = (uint8_t) (_filesize >> 16);
    bmpfileheader[5] = (uint8_t) (_filesize >> 24);
	
    bmpinfoheader[4] = (uint8_t) (_width);
    bmpinfoheader[5] = (uint8_t) (_width >> 8);
    bmpinfoheader[6] = (uint8_t) (_width >> 16);
    bmpinfoheader[7] = (uint8_t) (_width >> 24);
	
    bmpinfoheader[8] = (uint8_t) (_height);
    bmpinfoheader[9] = (uint8_t) (_height >> 8);
    bmpinfoheader[10]= (uint8_t) (_height >> 16);
    bmpinfoheader[11]= (uint8_t) (_height >> 24);
}

int createFile(const char *path) {
	FILE *file;
    file = fopen(path, "wb");
    if (file == NULL) {
        fprintf(stderr, "bitmap.c: Failed to create bitmap.bmp file.\n");
        return EXIT_FAILURE;
    }
    fwrite(bmpfileheader,1,14,file);
    fwrite(bmpinfoheader,1,40,file);
    fwrite(image,1,_imagesize,file);
	fclose(file);
	return EXIT_SUCCESS;
}

void createFileNumber(int n) {
	FILE *file;
	char fname[64];
	sprintf(fname,"bitmap%d.bmp",n);
    file = fopen(fname, "wb");
    fwrite(bmpfileheader,1,14,file);
    fwrite(bmpinfoheader,1,40,file);
    fwrite(image,1,_imagesize,file);
	fclose(file);
}

int greyScale(uint8_t n) {
	return n*256*256+n*256+n;
}

void pixelRGB(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
	if(x>=_width || y>=_height) return;
	x*=3; y*=3;
	image[x+_width*y] 	= b;
	image[x+_width*y+1] = g;
	image[x+_width*y+2] = r;
}

void pixelColor(int x, int y, uint32_t color) {
	if(x>=_width || y>=_height) return;
	x*=3; y*=3;
	image[x+_width*y] 	= (color>>0)  & 0xff;
	image[x+_width*y+1] = (color>>8)  & 0xff;
	image[x+_width*y+2] = (color>>16) & 0xff;
}

void pixel(int x, int y, uint8_t grey) {
	if(x>=_width || y>=_height) return;
	x*=3; y*=3;
	image[x+_width*y] 	= grey;
	image[x+_width*y+1] = grey;
	image[x+_width*y+2] = grey;
}

uint8_t getPixel(int x, int y) {
	if(x>=_width || y>=_height) return 0;
	x*=3; y*=3;
	return image[x+_width*y];
}

void positionColor(int pos, uint32_t color) {
	pos*=3;
	image[pos] 	 = (color>>0)  & 0xff;
	image[pos+1] = (color>>8)  & 0xff;
	image[pos+2] = (color>>16) & 0xff;
}

void position(int pos, uint8_t grey) {
	pos*=3;
	image[pos] 	 = grey;
	image[pos+1] = grey;
	image[pos+2] = grey;
}
