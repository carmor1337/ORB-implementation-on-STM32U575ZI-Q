


#include <stdbool.h>
#include <assert.h>
#include "Harris_corner_detection.h"
#include "common_includes.h"

#include "Benchmarking.h"
#include "Benchmarking_map.h"
#include "profiling_config.h"
/*
 * apply the 3x3 Sobel operator to each pixel within a 7x7 window around fast feature point
 *
 *
 *
 *
 *
 */
#include <stdint.h>
// At compile time:

// regular sobel vs separable sobel
/*
// Sobel area is 3x3 = 9
static const int32_t SOBEL_OFFSETS[9] = {
	    -1 * IMAGE_WIDTH -1, -1 * IMAGE_WIDTH + 0, -1 * IMAGE_WIDTH +1,
		0  * IMAGE_WIDTH -1, 0  * IMAGE_WIDTH + 0, 0  * IMAGE_WIDTH +1,
		1  * IMAGE_WIDTH -1, 1  * IMAGE_WIDTH + 0, 1  * IMAGE_WIDTH +1
	};


static const int32_t SOBEL_X_HORIZONTAL[3] ={
	-1 , 0 , 1
};

static const int32_t SOBEL_X_VERTICAL[3] = {
	1,
	2,
	1
};

static const int32_t SOBEL_Y_HORIZONTAL[3] = {
		1, 2, 1
};

static const int32_t SOBEL_Y_VERTICAL[3] = {
		-1,
		0,
		1
};



static const int16_t SOBEL_X_VALUES[9] = {
	    -1, 0, 1,
		-2, 0, 2,
		-1, 0, 1

	};
static const int16_t SOBEL_Y_VALUES[9] = {
	    -1, -2,-1,
		0,   0, 0,
		1,   2, 1

	};
static const int32_t HARRIS_AREA_OFFSETS_3x3[9] = {
	 -1 * IMAGE_WIDTH - 1, -1 * IMAGE_WIDTH + 0, -1 * IMAGE_WIDTH + 1,
	 0  * IMAGE_WIDTH - 1, 0  * IMAGE_WIDTH + 0, 0  * IMAGE_WIDTH + 1,
	 1  * IMAGE_WIDTH - 1, 1  * IMAGE_WIDTH + 0, 1  * IMAGE_WIDTH + 1,

	};


static const int32_t HARRIS_AREA_OFFSETS_5x5[25] = {
		-2 * IMAGE_WIDTH - 2, -2 * IMAGE_WIDTH - 1, -2 * IMAGE_WIDTH + 0, -2 * IMAGE_WIDTH + 1, -2 * IMAGE_WIDTH + 2,
		-1 * IMAGE_WIDTH - 2, -1 * IMAGE_WIDTH - 1, -1 * IMAGE_WIDTH + 0, -1 * IMAGE_WIDTH + 1, -1 * IMAGE_WIDTH + 2,

		0  * IMAGE_WIDTH - 2, 0  * IMAGE_WIDTH - 1, 0  * IMAGE_WIDTH + 0, 0  * IMAGE_WIDTH + 1, 0  * IMAGE_WIDTH + 2,

		1  * IMAGE_WIDTH - 2, 1  * IMAGE_WIDTH - 1, 1  * IMAGE_WIDTH + 0, 1  * IMAGE_WIDTH + 1, 1  * IMAGE_WIDTH + 2,
		2  * IMAGE_WIDTH - 2, 2  * IMAGE_WIDTH - 1, 2  * IMAGE_WIDTH + 0, 2  * IMAGE_WIDTH + 1, 2  * IMAGE_WIDTH + 2

	};
*/
static const int32_t HARRIS_AREA_OFFSETS_7x7[49] = {

		-3 * IMAGE_WIDTH - 3,-3 * IMAGE_WIDTH - 2, -3 * IMAGE_WIDTH - 1, -3 * IMAGE_WIDTH + 0, -3 * IMAGE_WIDTH + 1, -3 * IMAGE_WIDTH + 2, -3 * IMAGE_WIDTH + 3,
		-2 * IMAGE_WIDTH - 3,-2 * IMAGE_WIDTH - 2, -2 * IMAGE_WIDTH - 1, -2 * IMAGE_WIDTH + 0, -2 * IMAGE_WIDTH + 1, -2 * IMAGE_WIDTH + 2, -2 * IMAGE_WIDTH + 3,
		-1 * IMAGE_WIDTH - 3,-1 * IMAGE_WIDTH - 2, -1 * IMAGE_WIDTH - 1, -1 * IMAGE_WIDTH + 0, -1 * IMAGE_WIDTH + 1, -1 * IMAGE_WIDTH + 2, -1 * IMAGE_WIDTH + 3,

		 0 * IMAGE_WIDTH - 3, 0 * IMAGE_WIDTH - 2, 0  * IMAGE_WIDTH - 1, 0  * IMAGE_WIDTH + 0,  0 * IMAGE_WIDTH + 1, 0  * IMAGE_WIDTH + 2,  0 * IMAGE_WIDTH + 3,

		 1 * IMAGE_WIDTH - 3, 1 * IMAGE_WIDTH - 2, 1  * IMAGE_WIDTH - 1, 1  * IMAGE_WIDTH + 0,  1 * IMAGE_WIDTH + 1, 1  * IMAGE_WIDTH + 2,  1 * IMAGE_WIDTH + 3,
		 2 * IMAGE_WIDTH - 3, 2 * IMAGE_WIDTH - 2, 2  * IMAGE_WIDTH - 1, 2  * IMAGE_WIDTH + 0,  2 * IMAGE_WIDTH + 1, 2  * IMAGE_WIDTH + 2,  2 * IMAGE_WIDTH + 3,
		 3 * IMAGE_WIDTH - 3, 3 * IMAGE_WIDTH - 2, 3  * IMAGE_WIDTH - 1, 3  * IMAGE_WIDTH + 0,  3 * IMAGE_WIDTH + 1, 3  * IMAGE_WIDTH + 2,  3 * IMAGE_WIDTH + 3,

	};
*/
//static uint16_t g_patch_size = HARRIS_WINDOW_SIZE + (HARRIS_SOBEL_SIZE -1);
static int32_t g_first_patch_pixel_offset = 0;


bool Harris_init(void){
	// Calculated the mid point manually
	int32_t mid_value = 0;
	if (HARRIS_WINDOW_SIZE == 3)      mid_value = 2;
	else if (HARRIS_WINDOW_SIZE == 5) mid_value = 3;
	else if (HARRIS_WINDOW_SIZE == 7) mid_value = 4;
	else if (HARRIS_WINDOW_SIZE == 9) mid_value = 5;
	else {
		printf("Harris window size is not allowed");
		return (false);
	}
	g_first_patch_pixel_offset = (-mid_value) + (-mid_value*IMAGE_WIDTH);
	return (true);
}

static void compute_harris_matrix(uint8_t *image, int32_t index, Matrix_values_t* matrix_values ){
	int32_t Ix  = 0;
	int32_t Iy  = 0;
	int64_t Ixx = 0;
	int64_t Ixy = 0;
	int64_t Iyy = 0;
	int16_t  smooth_buff       [HARRIS_WINDOW_SIZE + 2][HARRIS_WINDOW_SIZE];	// Sobel [ 1, 2, 1 ]
	int16_t  derrivative_buff  [HARRIS_WINDOW_SIZE + 2][HARRIS_WINDOW_SIZE]; // Sobel [-1, 0, 1 ]

	const uint8_t * __restrict__ row_ptr  = image + index + g_first_patch_pixel_offset;


	// Step 1 do horizontal passes of all pixels
	for (int row = 0; row < HARRIS_WINDOW_SIZE + 2; row++){
		for (int col = 0; col < HARRIS_WINDOW_SIZE; col++){
			derrivative_buff[row][col] = -row_ptr[col] + row_ptr[col + 2];
			smooth_buff[row][col]     = (int16_t)(row_ptr[col] +
									  row_ptr[col+1]*2 +
									  row_ptr[col+2]);
		}
		row_ptr += IMAGE_WIDTH;
		}
	// Step 2 - Do vertical passes and add to Ix
	// Only rows 1-7 are valid
	// Only cols 0-6 are valid
	for (int row = 1; row < 1 + HARRIS_WINDOW_SIZE; row++){
		for (int col = 0; col < HARRIS_WINDOW_SIZE ; col++){
			// smoothing [ 1, 2, 1 ]
			Ix =  derrivative_buff[row - 1][col] +
				  derrivative_buff[row][col] * 2 +
				  derrivative_buff[row + 1][col];

			// Derivateive [-1, 0, 1 ]
			Iy = -smooth_buff[row - 1][col]
				 + smooth_buff[row + 1][col];

			Ixx += (int64_t)Ix * Ix;
			Ixy += (int64_t)Ix * Iy;
			Iyy += (int64_t)Iy * Iy;
		}
	}

	int32_t Ix = 0;
	int32_t Iy = 0;
	int32_t Ixx = 0;
	int32_t Ixy = 0;
	int32_t Iyy = 0;
	int32_t idx = 0;

	// Sobel operator is a 3x3 matrix therefore the 3
	// Applies the sobel operator
		//for (int j = 0; j< 9; j++){
		//Ix += (int64_t)image[(int64_t)idx + SOBEL_OFFSETS[j]] * SOBEL_X_VALUES[j];
		//Iy += (int64_t)image[(int64_t)idx + SOBEL_OFFSETS[j]] * SOBEL_Y_VALUES[j];
		//}

		/*   SIMD
		 *
		 * __SMLALD() - Dual 16-bit signed multiply with single 64-bit accumulator.
		 *
		 *
		 *
		 *
		 *
		 */

		/*
		 * 1. convert uint8 to int16
		 * 2. Pack the int16 into a uint32 with the weights as well
		 * 3. Do the simd calculation
		 */
	for (int i = 0; i < (HARRIS_WINDOW_SIZE*HARRIS_WINDOW_SIZE); i++){
#if HARRIS_PROFILING
		DWT_start(idx_HARRIS_loop_setup);
#endif
		Ix = 0;
		Iy = 0;
		// Index to follow
		idx = (int32_t)index + HARRIS_AREA_OFFSETS_7x7[i];

		//********************************  Extract values for current pixel  ********************************
		// Gets the next four pixels from the current position -1 column to get all 3 values correctly
		uint32_t packed_top = __UNALIGNED_UINT32_READ(image + idx - IMAGE_WIDTH - 1); // 4 pixels from row above
		uint32_t packed 	= __UNALIGNED_UINT32_READ(image + idx -1 );
		uint32_t packed_bot = __UNALIGNED_UINT32_READ(image + idx + IMAGE_WIDTH - 1);

		// ****Getting values ****
		/* Pixel notation
		 * p1 p2 p3 xx
		 * p4 p5 p6 xx
		 * p7 p8 p9 xx
		 *
		 * xx is for dont care values
		 *
		 */
		// Getting corners
		// Flips the order, so (p1,p2,p3,xx) = (p3,p1)(xx,p2)
		uint32_t p3_p1 =__UXTB16(packed_top);
		uint32_t p6_p4 =__UXTB16(packed);
		uint32_t p9_p7 =__UXTB16(packed_bot);

		// getting centers
		uint32_t xx_p2 = __UXTB16(__ROR(packed_top, 8));
		//uint32_t xx_p5 = __UXTB16(__ROR(packed,     8)); never uese
		uint32_t xx_p8 = __UXTB16(__ROR(packed_bot, 8));

#if HARRIS_PROFILING
			DWT_stop(idx_HARRIS_loop_setup);
			DWT_process_data(idx_HARRIS_loop_setup);
#endif


		//********************************  Calculate Sobel values  ********************************
#if HARRIS_PROFILING
		DWT_start(idx_HARRIS_calculate_x);
#endif
		// **** X values ****
		// Corner mask is  [31-16] = 1 and [15-0] = -1
		uint32_t corner_mask_for_x = 0x0001FFFF; //
		Ix = (int32_t) __SMLAD(p3_p1, corner_mask_for_x, 0);
		Ix = (int32_t) __SMLAD(p9_p7, corner_mask_for_x,(uint32_t)Ix);
		// P6*2-P4*2 = (P6-P4) * 2
		Ix += (int32_t)__SMLAD(p6_p4, 0x0001FFFF, 0) * 2;

#if HARRIS_PROFILING
		DWT_stop(idx_HARRIS_calculate_x);
		DWT_process_data(idx_HARRIS_calculate_x);
#endif

#if HARRIS_PROFILING
		DWT_start(idx_HARRIS_calculate_y);
#endif
		// **** Y values ****
		// Calculating corners ( 1 coefficients)
		uint32_t diff_corners = __SSUB16(p9_p7, p3_p1);

		// Calculating center (2 coefficients)
		// P8 *2 - P2 *  = (P8-P2) *2 where SADD16 is the *2
		uint32_t diff_center_y    = __SSUB16(xx_p8,xx_p2);

		// Since SMLAD is multiplication 0x00010001 is just both values mulitplied with 1 and then added.
		Iy = (int32_t)__SMLAD(diff_corners, 0x00010001, 0);
		Iy += (int16_t)(diff_center_y & 0xFFFF) * 2;

#if HARRIS_PROFILING
		DWT_stop(idx_HARRIS_calculate_y);
		DWT_process_data(idx_HARRIS_calculate_y);

#endif

#if HARRIS_PROFILING
		DWT_start(idx_HARRIS_accumulate);
#endif
		//********************************  Add to Ixx, Ixy, Iyy  ********************************
		Ixx += Ix * Ix;
		Ixy += Ix * Iy;
		Iyy += Iy * Iy;

#if HARRIS_PROFILING
		DWT_stop(idx_HARRIS_accumulate);
		DWT_process_data(idx_HARRIS_accumulate);
#endif
	}


	matrix_values->Ixx = Ixx;
	matrix_values->Ixy = Ixy;
	matrix_values->Iyy = Iyy;


}


float harris_score_compute(ORB_t *orb_obj){
	// At compile time:
	//_Static_assert(HARRIS_WINDOW_SIZE == 5, "Offset table must match window size");
	// Use sobel on all pixels in the window
	// Sum up Ix^2, Iy^2 and Ixy
	// Construct M matrix
	// score = R = det(M) - k*trace(M)^2
	// return score
	Matrix_values_t M = {0};
#if HARRIS_PROFILING
	DWT_start(idx_HARRIS_compute_matrix);
#endif

	compute_harris_matrix(orb_obj->image, (int32_t)orb_obj->pixel_index, &M);

#if HARRIS_PROFILING
	DWT_stop(idx_HARRIS_compute_matrix);
#endif
	float det   = ((float)M.Ixx * (float)M.Iyy) - ((float)M.Ixy * (float)M.Ixy);
	float trace = (float)((float)M.Ixx + (float)M.Iyy);
	float score = (det - (float)HARRIS_K * (trace * trace));
	return score;
}
