/*
 * common_includes.h
 *
 *  Created on: 6 apr. 2026
 *      Author: erika
 */

#ifndef INC_COMMON_INCLUDES_H_
#define INC_COMMON_INCLUDES_H_

#include <stdint.h>


typedef struct{
	uint16_t x; // Column position in image
	uint16_t y; // Row position in image
}Pixel_t;

typedef struct {

	Pixel_t coords;
	float score; 		// Corner response strength
	float  angle;  			// Orientation in degrees for ORB rotation
	uint8_t  descriptor[32];// 256 bit descriptor for ORB
}ORB_feature_point_t;

typedef struct {
	uint8_t *image; 		// Target image
	uint16_t width; 		// width of the image
	uint16_t height;		// The height of the image
	uint32_t pixel_index;	// Pixel index to keep track of current pixel
	ORB_feature_point_t *feature_point_list;
}ORB_t;

#endif /* INC_COMMON_INCLUDES_H_ */
