/*************************************************
Author: Stan
Created: 2019-6-4
Note: for debug info display in Screen.
************************************************/
#pragma once

namespace show {

#define CC_BREAK_IF(cond)           if(cond) break

enum {
	TGA_OK,
	TGA_ERROR_READING_FILE,
	TGA_ERROR_INDEXED_COLOR,
	TGA_ERROR_MEMORY,
	TGA_ERROR_COMPRESSED_FILE,
};

/** TGA format */
typedef struct sImageTGA {
	int status;
	unsigned char type, pixelDepth;

	/** map width */
	signed short width;

	/** map height */
	signed short height;

	/** raw data */
	unsigned char *imageData;
	int flipped;
} tImageTGA;

/// load the image header fields. We only keep those that matter!
bool tgaLoadHeader(unsigned char *buffer, unsigned long bufSize, tImageTGA *info);

/// loads the image pixels. You shouldn't call this function directly
bool tgaLoadImageData(unsigned char *buffer, unsigned long bufSize, tImageTGA *info);

/// this is the function to call when we want to load an image buffer.
tImageTGA* tgaLoadBuffer(unsigned char* buffer, long size);

/// releases the memory used for the image
void tgaDestroy(tImageTGA *info);

}