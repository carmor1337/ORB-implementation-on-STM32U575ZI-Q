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
#include "Benchmarking.h"
#include "Benchmarking_map.h"


// 31 x 31 area
// Get circle from the area
// Do the summation

static uint8_t u_max[16] = {
		15, // y = 0
		14,	// y = 1
		14, // y = 2
		14, // y = 3
		14, // y = 4
		14, // y = 5
		13, // y = 6
		13, // y = 7
		12, // y = 8
		12, // y = 9
		11, // y = 10
		10, // y = 11
		 9, // y = 12
		 7, // y = 13
		 5, // y = 14
		 0  // y = 15
};



// TODO: Remove the orb patch implementation
void compute_intensity_centroid(uint8_t *image, ORB_feature_point_t *feature_point ,uint32_t pixel_index){
#if CENTROID_PROFILING
	DWT_start(DWT_Lookup("Centroid:m01,m10"));
#endif
	const uint8_t * __restrict__ p_center_pointer = image + pixel_index;
	int32_t m10 = 0; // Sum of x*I for all x,y
	int32_t m01 = 0; // Sum of y*I for all x,y


	// Fist row
	for (int x = 1; x <= u_max[0]; x++){
		m10 +=x * ( p_center_pointer[x] - p_center_pointer[-x]);
	}
	for (int32_t y = 1; y <= 15; y++){
		const uint8_t *row_pointer_plus  = p_center_pointer + y*IMAGE_WIDTH;
		const uint8_t *row_pointer_minus = p_center_pointer - y*IMAGE_WIDTH;

				for (int x = 1; x <= u_max[y]; x++){
					uint8_t pos_x_pos_y = row_pointer_plus[  x];
					uint8_t neg_x_pos_y = row_pointer_plus[ -x];
					uint8_t pos_x_neg_y = row_pointer_minus[ x];
					uint8_t neg_x_neg_y = row_pointer_minus[-x];


					m10 += x * ((pos_x_pos_y - neg_x_pos_y) + (pos_x_neg_y - neg_x_neg_y));
					m01 += y * ((pos_x_pos_y + neg_x_pos_y) - (pos_x_neg_y + neg_x_neg_y));
				}
	}
#if CENTROID_PROFILING
	DWT_stop(DWT_Lookup("Centroid:m01,m10"));

	DWT_start(DWT_Lookup("Centroid:atan2"));
#endif

	feature_point->angle = atan2f((float)m01, (float)m10);

#if CENTROID_PROFILING
	DWT_stop(DWT_Lookup("Centroid:atan2"));
#endif
}

