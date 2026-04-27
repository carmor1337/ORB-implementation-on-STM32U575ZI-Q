


#include <stdbool.h>
#include <assert.h>
#include "Harris_corner_detection.h"
#include "common_includes.h"

#include "Benchmarking.h"
#include "Benchmarking_map.h"
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


static const int32_t SOBEL_X_VALUES[9] = {
	    -1, 0, 1,
		-2, 0, 2,
		-1, 0, 1

	};
static const int32_t SOBEL_Y_VALUES[9] = {
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
			Ix = derrivative_buff[row - 1][col] +
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
	DWT_start(DWT_Lookup("HARRIS:matrix"));
#endif

	compute_harris_matrix(orb_obj->image, (int32_t)orb_obj->pixel_index, &M);

#if HARRIS_PROFILING
	DWT_stop(DWT_Lookup("HARRIS:matrix"));
#endif
	float det   = ((float)M.Ixx * (float)M.Iyy) - ((float)M.Ixy * (float)M.Ixy);
	float trace = (float)((float)M.Ixx + (float)M.Iyy);
	float score = (det - (float)HARRIS_K * (trace * trace));
	return score;
}
