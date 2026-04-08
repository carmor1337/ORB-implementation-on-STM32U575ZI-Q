


#include <stdbool.h>
#include "Harris_corner_detection.h"
#include "common_includes.h"
#include <assert.h>
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
/*
static const int32_t HARRIS_AREA_OFFSETS_3x3[9] = {
	 -1 * IMAGE_WIDTH - 1, -1 * IMAGE_WIDTH + 0, -1 * IMAGE_WIDTH + 1,
	 0  * IMAGE_WIDTH - 1, 0  * IMAGE_WIDTH + 0, 0  * IMAGE_WIDTH + 1,
	 1  * IMAGE_WIDTH - 1, 1  * IMAGE_WIDTH + 0, 1  * IMAGE_WIDTH + 1,

	};
*/
static const int32_t HARRIS_AREA_OFFSETS_5x5[25] = {
		-2 * IMAGE_WIDTH - 2, -2 * IMAGE_WIDTH - 1, -2 * IMAGE_WIDTH + 0, -2 * IMAGE_WIDTH + 1, -2 * IMAGE_WIDTH + 2,
		-1 * IMAGE_WIDTH - 2, -1 * IMAGE_WIDTH - 1, -1 * IMAGE_WIDTH + 0, -1 * IMAGE_WIDTH + 1, -1 * IMAGE_WIDTH + 2,
		0  * IMAGE_WIDTH - 2, 0  * IMAGE_WIDTH - 1, 0  * IMAGE_WIDTH + 0, 0  * IMAGE_WIDTH + 1, 0  * IMAGE_WIDTH + 2,
		1  * IMAGE_WIDTH - 2, 1  * IMAGE_WIDTH - 1, 1  * IMAGE_WIDTH + 0, 1  * IMAGE_WIDTH + 1, 1  * IMAGE_WIDTH + 2,
		2  * IMAGE_WIDTH - 2, 2  * IMAGE_WIDTH - 1, 2  * IMAGE_WIDTH + 0, 2  * IMAGE_WIDTH + 1, 2  * IMAGE_WIDTH + 2

	};

bool Harris_init(void){
	return true;
}

static void compute_harris_matrix(uint8_t *image, int32_t index, Matrix_values_t* matrix_values ){
	int64_t Ix = 0;
	int64_t Iy = 0;
	int64_t Ixx = 0;
	int64_t Ixy = 0;
	int64_t Iyy = 0;
	int64_t idx = 0;
	// Sobel operator is a 3x3 matrix therefore the 3
	for (int i = 0; i < (HARRIS_WINDOW_SIZE*HARRIS_WINDOW_SIZE); i++){
		Ix = 0;
		Iy = 0;
		idx = (int32_t) index + HARRIS_AREA_OFFSETS_5x5[i];
		// Applies the sobel operator
		for (int j = 0; j< 9; j++){
			Ix += (int64_t)image[(int64_t)idx + SOBEL_OFFSETS[j]] * SOBEL_X_VALUES[j];
			Iy += (int64_t)image[(int64_t)idx + SOBEL_OFFSETS[j]] * SOBEL_Y_VALUES[j];
		}
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
	Matrix_values_t M= {0};
	compute_harris_matrix(orb_obj->image, (int32_t)orb_obj->pixel_index, &M);
	float det   = ((float)M.Ixx * (float)M.Iyy) - ((float)M.Ixy * (float)M.Ixy);
	float trace = (float)((float)M.Ixx + (float)M.Iyy);
	float score = (det - (float)HARRIS_K * (trace * trace));
	return score;
}
