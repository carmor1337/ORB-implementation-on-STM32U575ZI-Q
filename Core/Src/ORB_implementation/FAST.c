/*
 * FAST.c
 *
 *  Created on: 22 mars 2026
 *      Author: erika
 */


#include <stdbool.h>
#include <stdint.h>
#include <memory.h>
#include "main.h"

#include "FAST.h"
#include "common_includes.h"
#include "config.h"

#include "Benchmarking.h"
#include "Benchmarking_map.h"

// Link to site which has how to add the DSP lib
// https://community.st.com/t5/stm32-mcus/how-to-integrate-cmsis-dsp-libraries-on-a-stm32-project/ta-p/666790
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

#ifndef __ARM_FEATURE_DSP
  #error "DSP extension not enabled! Add -march=armv8-m.main+dsp to compiler flags"
#endif

#ifndef __ARM_ARCH_8M_MAIN__
  #error "Wrong architecture detected - should be ARMv8-M Main"
#endif

volatile uint32_t g_high_speed_test_rejections = 0;

static const int32_t CIRCLE_OFFSETS[16] = {
	    -3*IMAGE_WIDTH+0, -3*IMAGE_WIDTH+1, -2*IMAGE_WIDTH+2, -1*IMAGE_WIDTH+3,
	     0*IMAGE_WIDTH+3,  1*IMAGE_WIDTH+3,  2*IMAGE_WIDTH+2,  3*IMAGE_WIDTH+1,
	     3*IMAGE_WIDTH+0,  3*IMAGE_WIDTH-1,  2*IMAGE_WIDTH-2,  1*IMAGE_WIDTH-3,
	     0*IMAGE_WIDTH-3, -1*IMAGE_WIDTH-3, -2*IMAGE_WIDTH-2, -3*IMAGE_WIDTH-1
	};

// This is a wrap for checking for concecutive pixels
// For FAST 12
/*
static const uint8_t wrap[28] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    0,1,2,3,4,5,6,7,8,9,10,11
};
*/




bool FAST_init(void){
	//init_consecutive_table();

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
__attribute__((hot)) static inline  void FAST_get_circle(ORB_t *orb_obj,FAST_circle_t *pixel_circle ){
	// Grabs the 16 points around the pixel
	// pixel 1 is directly north

	uint32_t idx = orb_obj->pixel_index;


	pixel_circle->pixel = orb_obj->image[orb_obj->pixel_index];


	pixel_circle->circle[0]  = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[0]];
	pixel_circle->circle[1]  = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[1]];
	pixel_circle->circle[2]  = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[2]];
	pixel_circle->circle[3]  = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[3]];
	pixel_circle->circle[4]  = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[4]];
	pixel_circle->circle[5]  = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[5]];
	pixel_circle->circle[6]  = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[6]];
	pixel_circle->circle[7]  = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[7]];
	pixel_circle->circle[8]  = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[8]];
	pixel_circle->circle[9]  = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[9]];
	pixel_circle->circle[10] = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[10]];
	pixel_circle->circle[11] = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[11]];
	pixel_circle->circle[12] = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[12]];
	pixel_circle->circle[13] = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[13]];
	pixel_circle->circle[14] = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[14]];
	pixel_circle->circle[15] = orb_obj->image[(int32_t)idx + CIRCLE_OFFSETS[15]];

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





__attribute__((hot)) bool FAST_detect(ORB_t *orb_obj){
	// Compute 16 points and score
	// Use the counters here
	// decide to append point if not
	// 16 comes from the number of pixels to compare with


#if FAST_PROFILING
	DWT_start(DWT_Lookup("FAST: Circle"));
#endif

	int32_t idx = (int32_t)orb_obj->pixel_index;





#if FAST_PROFILING
	DWT_start(DWT_Lookup("FAST: Circle"));
#endif



	uint8_t origin_value = orb_obj->image[idx];
	// Clamping to avid overflows
	uint8_t origin_value_plus_threshold = (origin_value + ILLUMINATION_THRESHOLD > 255)
	                                       ? 255
	                                       : origin_value + ILLUMINATION_THRESHOLD;

	uint8_t origin_value_minus_threshold = (origin_value < ILLUMINATION_THRESHOLD)
	                                        ? 0
	                                        : origin_value - ILLUMINATION_THRESHOLD;

	// Adding calculated thresholds
	uint32_t origin_value_plus_threshold_packed = ((uint32_t)origin_value_plus_threshold << 24) |
												  ((uint32_t)origin_value_plus_threshold << 16) |
												  ((uint32_t)origin_value_plus_threshold <<  8) |
												  ((uint32_t)origin_value_plus_threshold <<  0);

	uint32_t origin_value_minus_threshold_packed = ((uint32_t)origin_value_minus_threshold << 24) |
												   ((uint32_t)origin_value_minus_threshold << 16) |
												   ((uint32_t)origin_value_minus_threshold <<  8) |
												   ((uint32_t)origin_value_minus_threshold <<  0);

	//
	    /*
	     ********************************************************
	     *													    *
	     *				   High Speed Test					    *
	     *													    *
	     ********************************************************
	     *
	     *
	     */
	// Check pixels 1,5,9,13 (paper notation) = indices 0,4,8,12 (0-based)
#if FAST_PROFILING
	DWT_start(DWT_Lookup("FAST: HST"));
#endif
	bool bright_or_dark = 0; // 1 for bright and 0 for dark
	uint32_t high_speed_test_result = 0;
	uint32_t high_speed_test_packed =((uint32_t)(orb_obj->image[idx + CIRCLE_OFFSETS[0]]) << 24) |
									 ((uint32_t)(orb_obj->image[idx + CIRCLE_OFFSETS[4]]) << 16) |
									 ((uint32_t)orb_obj->image[idx + CIRCLE_OFFSETS[8]]   <<  8) |
									 ((uint32_t)orb_obj->image[idx + CIRCLE_OFFSETS[12]]  <<  0);
	// Checking if greater than
	// __USUBB8 Sets the bits in a register that __SEL then reads from and gets the greater than values
	// __USUB8 is used because it can never underflow, while __UADD8 can overflow
	__USUB8(high_speed_test_packed,origin_value_plus_threshold_packed);
	uint32_t bright_result = __SEL(0x01010101, 0);
	bool is_bright = __builtin_popcount(bright_result) >= 3;

	if (__builtin_expect(is_bright == 0, 1)){

		__USUB8(origin_value_minus_threshold_packed , high_speed_test_packed);
		uint32_t dark_result = __SEL(0x01010101, 0);
		bool is_dark = __builtin_popcount(dark_result) >= 3;
		if((__builtin_expect(is_dark == 0, 1))) {
			//g_high_speed_test_rejections +=1;
			return (false);
		}
		bright_or_dark = 0;
		high_speed_test_result = dark_result;
	}
	else{

	bright_or_dark = 1;
	high_speed_test_result = bright_result;
	}

#if FAST_PROFILING
	DWT_stop(DWT_Lookup("FAST: HST"));
#endif
	//
    /*
     ********************************************************
     *													    *
     *				All pixel threshold caluclations        *
     *													    *
     ********************************************************
     *
     *
     */
	// Start from index 1 (pixel 2) since high speed test gets the 1,5,9,13 pixels
	uint16_t result = 0;

	result = ((uint16_t)((high_speed_test_result >> 24) & 0xFF) << 0) |
			 ((uint16_t)((high_speed_test_result >> 16) & 0xFF) << 4) |
			 ((uint16_t)((high_speed_test_result >> 8)  & 0xFF) << 8) |
			 ((uint16_t)((high_speed_test_result >> 0)  & 0xFF) << 12);

	for (int i = 1; i < 9; i += 4){
		uint32_t packed =((uint32_t)orb_obj->image[idx + CIRCLE_OFFSETS[i]]   << 24) |
						 ((uint32_t)orb_obj->image[idx + CIRCLE_OFFSETS[i+1]] << 16) |
						 ((uint32_t)orb_obj->image[idx + CIRCLE_OFFSETS[i+2]] <<  8) |
						 ((uint32_t)orb_obj->image[idx + CIRCLE_OFFSETS[i+3]] <<  0);
		if (bright_or_dark){
			__USUB8(packed, origin_value_plus_threshold_packed);
		}
		else if(!bright_or_dark){
			__USUB8(origin_value_minus_threshold_packed, packed);
		}
			uint32_t temp_result = __SEL(0x01010101, 0);
			// Need to get the relevant bits in result to their respective position
			result |= ((uint16_t)((temp_result >> 24) & 0xFF) << i   ) |
					 ((uint16_t)((temp_result >> 16) & 0xFF) <<(i+1)) |
					 ((uint16_t)((temp_result >> 8)  & 0xFF) <<(i+2)) |
					 ((uint16_t)((temp_result >> 0)  & 0xFF) <<(i+3));
	}
	// Last 3 pixels (13, 14, 15) — packed with 0 as dummy 4th byte
	uint32_t packed_last =
	    ((uint32_t)orb_obj->image[idx + CIRCLE_OFFSETS[13]] << 24) |
	    ((uint32_t)orb_obj->image[idx + CIRCLE_OFFSETS[14]] << 16) |
	    ((uint32_t)orb_obj->image[idx + CIRCLE_OFFSETS[15]] <<  8) |
	    0;  // dummy byte, result discarded

	if (bright_or_dark)
	    __USUB8(packed_last, origin_value_plus_threshold_packed);
	else
	    __USUB8(origin_value_minus_threshold_packed, packed_last);

	uint32_t temp_result = __SEL(0x01010101, 0);
	result |= ((uint16_t)((temp_result >> 24) & 0xFF) << 13) |
	          ((uint16_t)((temp_result >> 16) & 0xFF) << 14) |
	          ((uint16_t)((temp_result >>  8) & 0xFF) << 15);


	/*
	 ********************************************************
	 *													    *
	 *				Checking 12 consecutive bits	        *
	 *													    *
	 ********************************************************
	 *
	 *
	 */
	uint32_t x = ((uint32_t)result << 16) | result;
/*
	for (int i = 0; i < 11; i++) {
			x &= (x >> 1);
		}


*/

	// Loop unrolling
	x &= (x >> 1);
	x &= (x >> 1);
	x &= (x >> 1);
	x &= (x >> 1);
	x &= (x >> 1);
	x &= (x >> 1);
	x &= (x >> 1);
	x &= (x >> 1);
	x &= (x >> 1);
	x &= (x >> 1);
	x &= (x >> 1);

	if (x!=0 ) return (true);

	return (false);
}

