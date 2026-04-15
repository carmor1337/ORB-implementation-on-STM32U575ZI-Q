


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

// regular sobel vs seuparable sobel

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

/*
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

bool Harris_init(void){
	return (true);
}

static void compute_harris_matrix(uint8_t *image, int32_t index, Matrix_values_t* matrix_values ){
	int64_t Ix = 0;
	int64_t Iy = 0;
	int64_t Ixx = 0;
	int64_t Ixy = 0;
	int64_t Iyy = 0;
	int64_t gx[3] = {0};
	int64_t gy[3] = {0};
	const uint8_t * __restrict__ img = image + index;

	// Sobel operator is a 3x3 matrix therefore the 3
	for (int i = 0; i < (HARRIS_WINDOW_SIZE*HARRIS_WINDOW_SIZE); i++){
		Ix = 0;
		Iy = 0;



		// Applies the sobel operator
		// 1. Apply vertical Sobel to the 3 columns
		// 2. Give a smoothened row
		// 3. Take that smoothened row and multiply by the Vertical values
		// 4. add to Ix
		for (int j = 0; j< 3; j++){
		// Vertical calculation
		gx[j] = img[i + SOBEL_OFFSETS[j]]   * SOBEL_X_VERTICAL[0] +
				img[i + SOBEL_OFFSETS[j+3]] * SOBEL_X_VERTICAL[1] +
				img[i + SOBEL_OFFSETS[j+6]] * SOBEL_X_VERTICAL[2];

		gy[j] = img[i + SOBEL_OFFSETS[j]]   * SOBEL_Y_VERTICAL[0] +
				img[i + SOBEL_OFFSETS[j+3]] * SOBEL_Y_VERTICAL[1] +
				img[i + SOBEL_OFFSETS[j+6]] * SOBEL_Y_VERTICAL[2];
		}
		Ix = gx[0] * SOBEL_X_HORIZONTAL[0] +
			 gx[1] * SOBEL_X_HORIZONTAL[1] +
			 gx[2] * SOBEL_X_HORIZONTAL[2];

		Ix = gy[0] * SOBEL_Y_HORIZONTAL[0] +
			 gy[1] * SOBEL_Y_HORIZONTAL[1] +
			 gy[2] * SOBEL_Y_HORIZONTAL[2];
			//Ix += (int64_t)image[(int64_t)idx + SOBEL_OFFSETS[j]] * SOBEL_X_VALUES[j];
			//Iy += (int64_t)image[(int64_t)idx + SOBEL_OFFSETS[j]] * SOBEL_Y_VALUES[j];
		Ixx += Ix*Ix;
		Ixy += Ix*Iy;
		Iyy += Iy*Iy;
	}
	matrix_values->Ixx = Ixx;
	matrix_values->Ixy = Ixy;
	matrix_values->Iyy = Iyy;


}


float harris_score_compute(ORB_t *orb_obj){
	// At compile time:
	_Static_assert(HARRIS_WINDOW_SIZE == 5, "Offset table must match window size");
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
