#ifndef HARRIS_CORNER_DETECTION_H
#define HARRIS_CORNER_DETECTION_H

#include "common_includes.h"
#include "config.h"





typedef struct{
	int64_t Ixx;
	int64_t Ixy;
	int64_t Iyy;
}Matrix_values_t;


typedef struct{
	uint32_t pixel_index;
	uint8_t area[(HARRIS_WINDOW_SIZE + 2) *2]; // +2 is the offset for the area for the sobel operators to work
}Harris_t;

bool Harris_init(void);
float harris_score_compute(ORB_t *orb_obj);

#endif
