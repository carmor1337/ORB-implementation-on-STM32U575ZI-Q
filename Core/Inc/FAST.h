#ifndef FAST_H
#define FAST_H


#include <stdbool.h>
#include <stdint.h>
#include "common_includes.h"
// Variables to have
// Image heigth and width
// Number of gaussian pyrramids
// The number of sampling for gaussian pyramids
// Theshold for illumination
// Threshold for number of continous in order to be a feature






typedef struct {
	uint8_t pixel;
	uint8_t circle[16];
}FAST_circle_t;

typedef enum{
	FAST_PIXEL_BRIGHTER,
	FAST_PIXEL_DARKER,
	FAST_PIXEL_SIMILAR
}FAST_pixel_class_t ;



bool FAST_init(uint8_t *image_start);


bool FAST_assign_image(uint8_t *image_start);

__attribute__((hot)) bool FAST_detect(ORB_t *orb_obj);

// expose read access via functions
uint16_t FAST_get_feature_count(void);

ORB_feature_point_t* FAST_get_feature_points(void);


#endif
