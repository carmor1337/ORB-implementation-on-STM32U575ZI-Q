


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

	const uint32_t sobel_x_mask  = 0x0001FFFF;
	const uint8_t * __restrict__ row_ptr  = image + index + g_first_patch_pixel_offset;

	int32_t  Ix  = 0;
	int32_t  Iy  = 0;
	int32_t  Ixx = 0;
	int32_t  Ixy = 0;
	int32_t  Iyy = 0;
	// HARRIS_WINDOW_SIZE+1 is to align the bytes since HARRIS_WINDOW_SIZE is always odd
	int16_t  smooth_buff       [HARRIS_SOBEL_SIZE][HARRIS_WINDOW_SIZE+1] __attribute__((aligned(4))); // Sobel [ 1, 2, 1 ]
	int16_t  derrivative_buff  [HARRIS_SOBEL_SIZE][HARRIS_WINDOW_SIZE+1]  __attribute__((aligned(4)));// Sobel [-1, 0, 1 ]


	// ****Getting values ****
	/* Pixel notation
	 * p1 p2  p3  p4
	 * p5 p6  p7  p8
	 * p9 p10 p11 p12
	 * p6 is the pixel of intrest position
	 */
	//TODO: Implement the rotating buffer for the images.
	// Step 1 do horizontal passes of all pixels
	for (int row = 0; row < HARRIS_WINDOW_SIZE + 2; row++){
		for (int col = 0; col < HARRIS_WINDOW_SIZE; col++){

			// Grabbing pixels for horizontal pass
			uint32_t packed = __UNALIGNED_UINT32_READ(row_ptr + col);
			uint32_t p7_p5  = __UXTB16(packed);
			uint32_t p8_p6  = __UXTB16(__ROR(packed, 8));


			//derrivative_buff[row][col] = -row_ptr[col] + row_ptr[col + 2];
			//smooth_buff[row][col]     = (int16_t)(row_ptr[col] +row_ptr[col+1]*2 + row_ptr[col+2]);

			// -1 * p5 + p7*1
			derrivative_buff[row][col] = (int16_t) __SMLAD(p7_p5, sobel_x_mask, 0);
			// p7*1 + p5*1 = p7+p5
			smooth_buff[row][col]      = (int16_t) __SMLAD(p7_p5,0x00010001,0);
			 // 2*p6 + p5 + p7
			smooth_buff[row][col]     += (int16_t)(__SMLAD(p8_p6, 0x00010002, 0) & 0xFFFF);
		}
		row_ptr += IMAGE_WIDTH;
		}

	// Step 2 - Do vertical passes and add to Ix
	// Only rows 1-7 are valid
	// Only cols 0-6 are valid
	for (int row = 1; row < 1 + HARRIS_WINDOW_SIZE; row++){
		for (int col = 0; col < HARRIS_WINDOW_SIZE -1 ; col+=2){


			//********************************  Extract values for current pixel  ********************************
			// since derrivative_buff[y][x] is int16 each uint32_t is 2 int16

			uint32_t packed_top_derr = __UNALIGNED_UINT32_READ(&derrivative_buff[row - 1][col]);
			uint32_t packed_derr     = __UNALIGNED_UINT32_READ(&derrivative_buff[row][col]);
			uint32_t packed_bot_derr = __UNALIGNED_UINT32_READ(&derrivative_buff[row + 1][col]);

			uint32_t packed_top_smooth = __UNALIGNED_UINT32_READ(&smooth_buff[row - 1][col]);
			uint32_t packed_bot_smooth = __UNALIGNED_UINT32_READ(&smooth_buff[row + 1][col]);


			/*
			Ix =  derrivative_buff[row - 1][col] +
				  derrivative_buff[row][col] * 2 +
				  derrivative_buff[row + 1][col];


			Iy = - smooth_buff[row - 1][col]
				 + smooth_buff[row + 1][col];

			 Ixx += (int64_t)Ix * Ix;
			Ixy += (int64_t)Ix * Iy;
			Iyy += (int64_t)Iy * Iy;
			*/
			//********************************  Calculate Sobel values  ********************************

			// smoothing [ 1, 2, 1 ]T
			uint32_t corner = __SADD16(packed_top_derr,packed_bot_derr); // Add the corners together
			uint32_t middle = __SADD16(packed_derr,packed_derr);		 // Adds the middle together
			Ix	= (int32_t)__SADD16(corner,middle);		 // adds the 2 values together

			// Derrivative [-1, 0, 1 ]T
			Iy = (int32_t)__SSUB16(packed_bot_smooth,packed_top_smooth);

			//********************************  Add to Ixx, Ixy, Iyy  ********************************
			Ixx += (int32_t)__SMUAD((uint32_t)Ix, (uint32_t)Ix);
			Ixy += (int32_t)__SMLAD((uint32_t)Ix, (uint32_t)Iy, 0);
			Iyy += (int32_t)__SMUAD((uint32_t)Iy, (uint32_t)Iy);

		}
		//********************************  Doing the pass of the last pixel ********************************
		// Doing the last iteration by itself
		int last_pixel_index = HARRIS_WINDOW_SIZE -1;
		int16_t Ix_last =  (int16_t)(derrivative_buff[row - 1][last_pixel_index] +
			  derrivative_buff[row][last_pixel_index] * 2 +
			  derrivative_buff[row + 1][last_pixel_index]);


		int16_t Iy_last = (int16_t)(- smooth_buff[row - 1][last_pixel_index]
			 + smooth_buff[row + 1][last_pixel_index]);

		//********************************  Add to Ixx, Ixy, Iyy  ********************************
		Ixx += (int32_t)Ix_last * Ix_last;
		Ixy += (int32_t)Ix_last * Iy_last;
		Iyy += (int32_t)Iy_last * Iy_last;

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
