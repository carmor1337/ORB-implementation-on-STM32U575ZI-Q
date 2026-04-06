/*
 * FAST.c
 *
 *  Created on: 22 mars 2026
 *      Author: erika
 */


#include "FAST.h"
#include <stdbool.h>
#include <stdint.h>
#include <memory.h>
#include "main.h"

#include "Benchmarking.h"
#include "Benchmarking_map.h"

/*  Fast algorithm
 *
 * 	For every pixel
 * 		Check against threshold
 *
 *
 *
 *
 *
 *
 *
 */


#define PROFILING 1



static FAST_t g_fast_obj;
static ORB_feature_point_t g_feature_points[MAX_FEATURE_POINTS];
static uint16_t g_feature_count = 0;


static const int32_t CIRCLE_OFFSETS[16] = {
	    -3*IMAGE_WIDTH+0, -3*IMAGE_WIDTH+1, -2*IMAGE_WIDTH+2, -1*IMAGE_WIDTH+3,
	     0*IMAGE_WIDTH+3,  1*IMAGE_WIDTH+3,  2*IMAGE_WIDTH+2,  3*IMAGE_WIDTH+1,
	     3*IMAGE_WIDTH+0,  3*IMAGE_WIDTH-1,  2*IMAGE_WIDTH-2,  1*IMAGE_WIDTH-3,
	     0*IMAGE_WIDTH-3, -1*IMAGE_WIDTH-3, -2*IMAGE_WIDTH-2, -3*IMAGE_WIDTH-1
	};

// This is a wrap for checking for concecutive pixels
// For FAST 12
static const uint8_t wrap[28] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    0,1,2,3,4,5,6,7,8,9,10,11
};

bool FAST_assign_image(uint8_t *image_start){
	// Takes the image
	// Removes everything by 3
	// Assign the struct the processed image
	// return true if all good
	if (image_start == NULL){
		return false;
	}
	g_fast_obj.image = image_start;

	return true;
}



bool FAST_init(uint8_t *image_start){

	if (!FAST_assign_image(image_start)){
		return false;
	}
	// Should heigth and width be orignal or fast trimmed in?
	g_fast_obj.pixel_index = 0;
	g_fast_obj.height = IMAGE_HEIGTH;
	g_fast_obj.width = IMAGE_WIDTH;

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	return true;
}

static inline FAST_pixel_class_t FAST_test_against_threshold(uint8_t origin, uint8_t compare){
	// Test the original pixels vs the compare for brighter and darker
	int16_t brighter = (int16_t)( compare - origin);
	int16_t darker = (int16_t)( origin - compare);

	if (brighter > ILLUMINATION_THRESHOLD) { return FAST_PIXEL_BRIGHTER; }
	else if (darker > ILLUMINATION_THRESHOLD) { return FAST_PIXEL_DARKER; }
	else {return FAST_PIXEL_SIMILAR; }


}


// Input: the index for the pixel
__attribute__((hot)) static inline  FAST_circle_t FAST_get_circle(void){
	// Grabs the 16 points around the pixel
	// pixel 1 is directly north
	FAST_circle_t pixel_circle;
	uint32_t idx = g_fast_obj.pixel_index;


	pixel_circle.pixel = g_fast_obj.image[g_fast_obj.pixel_index];


	pixel_circle.circle[0] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[0]];
	pixel_circle.circle[1] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[1]];
	pixel_circle.circle[2] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[2]];
	pixel_circle.circle[3] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[3]];
	pixel_circle.circle[4] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[4]];
	pixel_circle.circle[5] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[5]];
	pixel_circle.circle[6] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[6]];
	pixel_circle.circle[7] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[7]];
	pixel_circle.circle[8] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[8]];
	pixel_circle.circle[9] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[9]];
	pixel_circle.circle[10] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[10]];
	pixel_circle.circle[11] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[11]];
	pixel_circle.circle[12] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[12]];
	pixel_circle.circle[13] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[13]];
	pixel_circle.circle[14] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[14]];
	pixel_circle.circle[15] = g_fast_obj.image[(int32_t)idx + CIRCLE_OFFSETS[15]];



	return pixel_circle;

}


__attribute__((hot)) static inline bool FAST_high_speed_test(FAST_circle_t *pixel_circle, FAST_pixel_class_t *result_storage, uint32_t *score){

	uint8_t brighter_counter = 0;
	uint8_t darker_counter   = 0;
	uint8_t similar_counter  = 0;

	uint8_t origin = pixel_circle->pixel;
	uint32_t* score_ptr = score;
	uint32_t temp_score = *score_ptr;
	// Check pixels 1,5,9,13 (paper notation) = indices 0,4,8,12 (0-based)


	 for (int i = 0; i<16;i=i+4){
	        int16_t brighter = (int16_t)( pixel_circle->circle[i] - origin);
			int16_t darker = (int16_t)( origin - pixel_circle->circle[i]);

			if (brighter > ILLUMINATION_THRESHOLD) {
				result_storage[i]= FAST_PIXEL_BRIGHTER;
				temp_score += (uint32_t)brighter;
				brighter_counter++;
			}
			else if (darker > ILLUMINATION_THRESHOLD) {
				result_storage[i]= FAST_PIXEL_DARKER;
				temp_score += (uint32_t)darker;
				darker_counter++;
			}
			else {
				result_storage[i]= FAST_PIXEL_SIMILAR;
				temp_score += (uint32_t)(brighter > 0 ? brighter : darker);
				similar_counter++;
			}
			if (similar_counter > 2) {
				*score_ptr = temp_score;
				return false;
			}
	 }
	 *score_ptr = temp_score;
	 return (brighter_counter >= 3 || darker_counter >= 3);
}



static bool append_feature_point(uint16_t x, uint16_t y, uint32_t score){
	if (g_feature_count >= MAX_FEATURE_POINTS) {
	        return false;
	    }

	g_feature_points[g_feature_count].x     = x;
	g_feature_points[g_feature_count].y     = y;
	g_feature_points[g_feature_count].score = score;

	g_feature_count++;
	return true;
}


__attribute__((hot)) static bool FAST_compute_and_score(FAST_circle_t *pixels){
	// Compute 16 points and score
	// Use the counters here
	// decide to append point if not
	// 16 comes from the number of pixels to compare with
	uint8_t origin = pixels->pixel;
	uint32_t score = 0;


	FAST_pixel_class_t pixel_result[16];

	// High speed test
	// Check pixels 1,5,9,13 (paper notation) = indices 0,4,8,12 (0-based)
#if PROFILING
	DWT_start(DWT_Lookup("FAST: high speed test"));
#endif

	if ( __builtin_expect(FAST_high_speed_test(pixels, pixel_result, &score) == false, 1)) return false;

#if PROFILING
	DWT_stop(DWT_Lookup("FAST: high speed test"));
#endif


	/*
	for (int i =1; i < 16; i++){

		// These pixels computed in high speed and does not need to recomputed
		if (i == 4 || i == 8 ||i == 12) { continue;}

		int16_t brighter = (int16_t)(origin - pixels->circle[i]);
		int16_t darker   = (int16_t)(pixels->circle[i] - origin);

		if (brighter > ILLUMINATION_THRESHOLD) {
			brighter_counter++;
			pixel_result[i]= FAST_PIXEL_BRIGHTER;
		}
		else if (darker > ILLUMINATION_THRESHOLD) {
			darker_counter++;
			pixel_result[i]= FAST_PIXEL_DARKER;
		}
		else {
			similar_counter++;
			// Not enough pixels of either brigther of darker to be a feature point return early

			if (similar_counter > 16-CONTINUOUS_PIXEL_THRESHOLD) { return false; }
			pixel_result[i]= FAST_PIXEL_SIMILAR;
		}
		// If both are above the required amount maximum number of diffrent pixels allowed for either case it cannot be a feature point
		if (brighter_counter > 16 - CONTINUOUS_PIXEL_THRESHOLD &&
			darker_counter > 16 - CONTINUOUS_PIXEL_THRESHOLD)
		{return false;}

		score += (uint32_t)(brighter > 0 ? brighter : darker);
	}

	int bright_run = 0;
	int dark_run   = 0;
	// checking for concecutive runs

	for (int i = 0; i < 16 + CONTINUOUS_PIXEL_THRESHOLD; i++){
		switch (pixel_result[wrap[i]]){
		case FAST_PIXEL_BRIGHTER:
			bright_run++;
			dark_run = 0;
			break;
		case FAST_PIXEL_DARKER:
			dark_run++;
			bright_run = 0;
			break;
		case FAST_PIXEL_SIMILAR:
			bright_run = 0;
			dark_run = 0;
			break;
		default:
			bright_run = 0;
			dark_run = 0;
			}

	    if (bright_run >= CONTINUOUS_PIXEL_THRESHOLD || dark_run >= CONTINUOUS_PIXEL_THRESHOLD){
			uint16_t x = (uint16_t)g_fast_obj.pixel_index % IMAGE_WIDTH;   // column
			uint16_t y = (uint16_t)g_fast_obj.pixel_index / IMAGE_WIDTH;   // row
			append_feature_point(x, y, score);
			return true;
		}
	}
	return false;
	*/
	int bright_run = 0;
	int dark_run   = 0;

	for (int i = 0; i < 16 + CONTINUOUS_PIXEL_THRESHOLD; i++) {
	    uint8_t c = pixels->circle[wrap[i]];
	    int16_t brighter = (int16_t)(origin - c);
	    int16_t darker   = (int16_t)(c - origin);

	    // accumulate score only for first 16 pixels
	    if (i < 16) score += (uint32_t)(brighter > 0 ? brighter : darker);

	    if      (brighter > ILLUMINATION_THRESHOLD) { bright_run++; dark_run = 0; }
	    else if (darker   > ILLUMINATION_THRESHOLD) { dark_run++; bright_run = 0; }
	    else                                         { bright_run = 0; dark_run = 0; }

	    if (bright_run >= CONTINUOUS_PIXEL_THRESHOLD ||
	        dark_run   >= CONTINUOUS_PIXEL_THRESHOLD) {
	        uint16_t x = (uint16_t)g_fast_obj.pixel_index % IMAGE_WIDTH;
	        uint16_t y = (uint16_t)g_fast_obj.pixel_index / IMAGE_WIDTH;
	        append_feature_point(x, y, score);
	        return true;
	    }
	}
	return false;
}


inline bool FAST_detect(void){
	if (g_fast_obj.image == NULL){
		return false;
	}
	//uint8_t *image = g_fast_obj.image;
	g_feature_count = 0;
	// To calculate the compute of all 16 points

	// Minus 3 for every corner so that no pixel is never not reached
	for (int row=3; row <g_fast_obj.height-3; row++){
		for (int col = 3; col < g_fast_obj.width - 3; col++) {

			// Calculates the correct pixel to be used in get circle

			g_fast_obj.pixel_index = (uint32_t)(row * IMAGE_WIDTH + col);

#if PROFILING

			DWT_start(DWT_Lookup("FAST: get circle"));

#endif

		FAST_circle_t curr_circle = FAST_get_circle();

#if PROFILING
			DWT_stop(DWT_Lookup("FAST: get circle"));
#endif


#if PROFILING
			DWT_start(DWT_Lookup("FAST: compute and score"));
#endif

			// 16 comes from the number of pixels to compare with
			//if (!FAST_compute_and_score(&curr_circle)) {continue;}
			FAST_compute_and_score(&curr_circle);

#if PROFILING
			DWT_stop(DWT_Lookup("FAST: compute and score"));
#endif
		}
	}

	return true;
}


void get_prev_feature_coords(uint16_t *x,uint16_t *y){
	if (g_feature_count == 0){
		return;
	}
	*x = g_feature_points[g_feature_count -1].x;
	*y = g_feature_points[g_feature_count -1].y;
}

// Read access to count
uint16_t FAST_get_feature_count(void) {
    return g_feature_count;
}

ORB_feature_point_t* FAST_get_feature_points(void) {
    return g_feature_points;
}


