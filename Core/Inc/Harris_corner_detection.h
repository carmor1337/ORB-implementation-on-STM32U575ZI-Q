#ifndef HARRIS_CORNER_DETECTION_H
#define HARRIS_CORNER_DETECTION_H

#include "common_includes.h"
#include "config.h"





typedef struct{
	int32_t Ixx;
	int32_t Ixy;
	int32_t Iyy;
}Matrix_values_t;


typedef struct{
	uint32_t pixel_index;
	uint8_t area[(HARRIS_WINDOW_SIZE + 2) *2]; // +2 is the offset for the area for the sobel operators to work
}Harris_t;




#endif
