/*
 * common_includes.h
 *
 *  Created on: 6 apr. 2026
 *      Author: erika
 */

#ifndef INC_COMMON_INCLUDES_H_
#define INC_COMMON_INCLUDES_H_

#include <stdint.h>
#include "config.h"

typedef struct{
	uint16_t x; // Column position in image
	uint16_t y; // Row position in image
	uint32_t pixel_index;
}Pixel_t;

typedef struct {

	Pixel_t  coords;
	float    score; 		 // Corner response strength
	float    angle;  		 // Orientation in radians for ORB rotation
	uint8_t  descriptor[32]; // 256 bit descriptor for ORB
}ORB_feature_point_t;

typedef struct {
	uint8_t *image; 		// Target image
	uint16_t width; 		// width of the image
	uint16_t height;		// The height of the image
	uint32_t pixel_index;	// Pixel index to keep track of current pixel
	ORB_feature_point_t *feature_point_list;
}ORB_t;

// The pach is usually 31 x 31 for the 256 bit descriptors
typedef struct {
	uint32_t pixel; // the pixel index of the patch
	uint8_t patch_data[PATCH_SIZE][PATCH_SIZE];
}ORB_keypoint_patch_t;


#endif /* INC_COMMON_INCLUDES_H_ */
