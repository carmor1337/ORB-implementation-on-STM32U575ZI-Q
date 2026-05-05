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
	for (int16_t y = 1; y <= 15; y++){
		const uint8_t *p_row_plus  = p_center_pointer + y*IMAGE_WIDTH;
		const uint8_t *p_row_minus = p_center_pointer - y*IMAGE_WIDTH;
		int32_t y_sum = 0;
		int16_t u = u_max[y];
		int16_t x = 1;

			for (; x + 3 <= u; x += 4){
				// Fetch the bytes to be compared
				uint32_t pos_pos_word	= __UNALIGNED_UINT32_READ(p_row_plus  + x);
				uint32_t pos_neg_word	= __UNALIGNED_UINT32_READ(p_row_minus + x);
				// __REV reverses the position so that they are aligned with the positive values
				uint32_t neg_pos_word	= __REV(__UNALIGNED_UINT32_READ(p_row_plus  - x - 3));
				uint32_t neg_neg_word	= __REV(__UNALIGNED_UINT32_READ(p_row_minus - x - 3));

		        // Extract into 16-bit lanes
		        // _20 = bytes 0 & 2 → pixels at offset 0 and 2
		        // _31 = bytes 1 & 3 → pixels at offset 1 and 3
		        uint32_t pp_20 = __UXTB16(pos_pos_word);
		        uint32_t np_20 = __UXTB16(neg_pos_word);
		        uint32_t pn_20 = __UXTB16(pos_neg_word);
		        uint32_t nn_20 = __UXTB16(neg_neg_word);

				uint32_t pp_31 = __UXTB16(__ROR(pos_pos_word, 8));
				uint32_t np_31 = __UXTB16(__ROR(pos_neg_word, 8));
		        uint32_t pn_31 = __UXTB16(__ROR(pos_neg_word, 8));
				uint32_t nn_31 = __UXTB16(__ROR(neg_neg_word, 8));


				/********************************   M10 calculation ********************/
		        // m10: x * ((I(x,+y)-I(-x,+y)) + (I(x,-y)-I(-x,-y)))
		        uint32_t delta_x_py_20 = __SSUB16(pp_20, np_20);
		        uint32_t delta_x_py_31 = __SSUB16(pp_31, np_31);
		        uint32_t delta_x_ny_20 = __SSUB16(pn_20, nn_20);
		        uint32_t delta_x_ny_31 = __SSUB16(pn_31, nn_31);

		        uint32_t delta_x_20 = __SADD16(delta_x_py_20, delta_x_ny_20);
			    uint32_t delta_x_31 = __SADD16(delta_x_py_31, delta_x_ny_31);

			    uint32_t x_packed_20 = ((uint32_t)(x + 2) << 16) | (uint32_t)(x);
				uint32_t x_packed_31 = ((uint32_t)(x + 3) << 16) | (uint32_t)(x + 1);

				m10 = (int32_t)__SMLAD(delta_x_20, x_packed_20, (uint32_t)m10);
				m10 = (int32_t)__SMLAD(delta_x_31, x_packed_31, (uint32_t)m10);

				/********************************   M01 calculation ********************/
				// m01: y * ((I(x,+y) + I(-x,+y)) - (I(x,-y) + I(-x,-y)))
				uint32_t delta_y_py_20 = __SADD16(pp_20, np_20);
				uint32_t delta_y_py_31 = __SADD16(pp_31, np_31);
				uint32_t delta_y_ny_20 = __SADD16(pn_20, nn_20);
				uint32_t delta_y_ny_31 = __SADD16(pn_31, nn_31);

				uint32_t delta_y_20 = __SSUB16(delta_y_py_20, delta_y_ny_20);
				uint32_t delta_y_31 = __SSUB16(delta_y_py_31, delta_y_ny_31);

				uint32_t y_packed_20 = ((uint32_t)(y + 2) << 16) | (uint32_t)(y);
				uint32_t y_packed_31 = ((uint32_t)(y + 3) << 16) | (uint32_t)(y + 1);

				y_sum = (int32_t)__SMLAD(delta_y_20, y_packed_20, (uint32_t)y_sum);
				y_sum = (int32_t)__SMLAD(delta_y_31, y_packed_31, (uint32_t)y_sum);

				//y_sum +=     ((px_py + nx_py) - (px_ny + nx_ny));
			}
			for (; x <= u; x++) {
				uint8_t pp = p_row_plus[x];
				uint8_t np = p_row_plus[-x];
				uint8_t pn = p_row_minus[x];
				uint8_t nn = p_row_minus[-x];
				m10   += x * ((pp - np) + (pn - nn));
				y_sum +=     ((pp + np) - (pn + nn));
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

