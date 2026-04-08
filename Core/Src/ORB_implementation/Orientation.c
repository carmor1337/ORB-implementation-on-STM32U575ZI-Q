/*
 * Orientation.c
 *
 *  Created on: 6 apr. 2026
 *      Author: erika
 */


#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "common_includes.h"
#include "config.h"



// 31 x 31 area
// Get circle from the area
// Do the summation



void compute_intensity_centroid(ORB_t *orb_obj, ORB_feature_point_t *feature_point ,uint32_t pixel_center){

	float m10 = 0; // Sum of x*I for all x,y
	float m01 = 0; // Sum of y*I for all x,y
	for (int32_t y = -15; y <= 15; y++){
		for (int32_t x = -15; x <= 15; x++){
			if (x*x + y*y <= 15*15){  // inside circle
				m10 += (float)(x * orb_obj->image[(int32_t)pixel_center + y*IMAGE_WIDTH + x]);
				m01 += (float)(y * orb_obj->image[(int32_t)pixel_center + y*IMAGE_WIDTH + x]);

			}
		}
	}
	feature_point->angle = atan2f(m01, m10);
}

