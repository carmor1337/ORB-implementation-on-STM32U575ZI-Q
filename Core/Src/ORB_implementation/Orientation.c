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
#include "profiling_config.h"
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
	DWT_start(idx_Centroid_momentums);
#endif
	const uint8_t * __restrict__ p_center_pointer = image + pixel_index;
	int32_t m10 = 0; // Sum of x*I for all x,y
	int32_t m01 = 0; // Sum of y*I for all x,y


	// Fist row special case Only contributes to m10 since y = 0
	for (int x = 1; x <= u_max[0]; x++){
		m10 +=x * ( p_center_pointer[x] - p_center_pointer[-x]);
	}
	for (int32_t y = 1; y <= 15; y++){
		const uint8_t *row_pointer_plus  = p_center_pointer + y*IMAGE_WIDTH;
		const uint8_t *row_pointer_minus = p_center_pointer - y*IMAGE_WIDTH;
		int32_t y_sum = 0;
			for (int x = 1; x <= u_max[y]; x++){
				uint8_t px_py = row_pointer_plus[  x];
				uint8_t nx_py = row_pointer_plus[ -x];
				uint8_t px_ny = row_pointer_minus[ x];
				uint8_t nx_ny = row_pointer_minus[-x];

				m10   += x * ((px_py - nx_py) + (px_ny - nx_ny));
				y_sum +=     ((px_py + nx_py) - (px_ny + nx_ny));
			}
	    m01 += y * y_sum;
	}

#if CENTROID_PROFILING
	DWT_stop(idx_Centroid_momentums);
#endif

#if CENTROID_PROFILING
	DWT_start(idx_Centroid_atan2);
#endif

	feature_point->angle = atan2f((float)m01, (float)m10);

#if CENTROID_PROFILING
	DWT_stop(idx_Centroid_atan2);
#endif
}

