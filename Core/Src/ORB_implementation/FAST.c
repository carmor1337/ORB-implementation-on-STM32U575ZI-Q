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

	return (true);
}




__attribute__((hot)) bool FAST_detect(ORB_t *orb_obj){
	// Compute 16 points and score
	// Use the counters here
	// decide to append point if not
	// 16 comes from the number of pixels to compare with

	int32_t idx = (int32_t)orb_obj->pixel_index;
	const uint8_t * __restrict__ img = orb_obj->image + idx;





	uint8_t origin_value = img[0];
	//uint8_t origin_value = orb_obj->image[idx];
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
	uint32_t high_speed_test_packed =((uint32_t)img[CIRCLE_OFFSETS[0]]  << 24) |
									 ((uint32_t)img[CIRCLE_OFFSETS[4]]  << 16) |
									 ((uint32_t)img[CIRCLE_OFFSETS[8]]  <<  8) |
									 ((uint32_t)img[CIRCLE_OFFSETS[12]] <<  0);


	// Checking if greater than
	// __USUBB8 Sets the bits in a register that __SEL then reads from and gets the greater than values
	// __USUB8 is used because it can never underflow, while __UADD8 can overflow
	__USUB8(high_speed_test_packed, origin_value_plus_threshold_packed);
	uint32_t bright_result = __SEL(0x01010101, 0);

	__USUB8(origin_value_minus_threshold_packed, high_speed_test_packed);
	uint32_t dark_result = __SEL(0x01010101, 0);

	bool is_bright = __builtin_popcount(bright_result) >= 3;
	bool is_dark   = __builtin_popcount(dark_result)   >= 3;

	if (__builtin_expect(!is_bright && !is_dark, 1)) return (false);

	// Uses is bright to check which of the 2 computed possibilities are used
	bright_or_dark         = is_bright;
	high_speed_test_result = is_bright ? bright_result : dark_result;

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
	uint16_t result_total = 0;

	/***************************************************************************************/

// Already have index 0,4,8,12
// Packing all the bits for maths
	uint32_t packed1 = ((uint32_t)img[CIRCLE_OFFSETS[1]] << 24) |
					   ((uint32_t)img[CIRCLE_OFFSETS[2]] << 16) |
					   ((uint32_t)img[CIRCLE_OFFSETS[3]] <<  8) |
					   ((uint32_t)img[CIRCLE_OFFSETS[5]] <<  0);

	uint32_t packed2 = ((uint32_t)img[CIRCLE_OFFSETS[6]] << 24) |
					   ((uint32_t)img[CIRCLE_OFFSETS[7]] << 16) |
					   ((uint32_t)img[CIRCLE_OFFSETS[9]] <<  8) |
					   ((uint32_t)img[CIRCLE_OFFSETS[10]]<<  0);

	uint32_t packed3 =((uint32_t)img[CIRCLE_OFFSETS[11]] << 24) |
					  ((uint32_t)img[CIRCLE_OFFSETS[13]] << 16) |
					  ((uint32_t)img[CIRCLE_OFFSETS[14]] <<  8) |
					  ((uint32_t)img[CIRCLE_OFFSETS[15]] <<  0);

	/***************************************************************************************/

	uint32_t result_1 = 0;
	uint32_t result_2 = 0;
	uint32_t result_3 = 0;

	if (bright_or_dark){
		__USUB8(packed1, origin_value_plus_threshold_packed);
		result_1 = __SEL(0x01010101, 0);

		__USUB8(packed2, origin_value_plus_threshold_packed);
		result_2 = __SEL(0x01010101, 0);

		__USUB8(packed3, origin_value_plus_threshold_packed);
		result_3 = __SEL(0x01010101, 0);


	}
	else{
		__USUB8(origin_value_minus_threshold_packed, packed1);
		result_1 = __SEL(0x01010101, 0);

		__USUB8(origin_value_minus_threshold_packed, packed2);
		result_2 = __SEL(0x01010101, 0);

		__USUB8(origin_value_minus_threshold_packed, packed3);
		result_3 = __SEL(0x01010101, 0);

	}

	/***************************************************************************************/
	// High speed test
	result_total |= ((uint16_t)((high_speed_test_result >> 24) & 0xFF) << 0) |
					((uint16_t)((high_speed_test_result >> 16) & 0xFF) << 4) |
					((uint16_t)((high_speed_test_result >> 8)  & 0xFF) << 8) |
					((uint16_t)((high_speed_test_result >> 0)  & 0xFF) << 12);
	// Package 1
	result_total  |= ((uint16_t)((result_1 >> 24) & 0xFF) << 1) |
			         ((uint16_t)((result_1 >> 16) & 0xFF) << 2) |
					 ((uint16_t)((result_1 >> 8)  & 0xFF) << 3) |
					 ((uint16_t)((result_1 >> 0)  & 0xFF) << 5);

	// Package 2
	result_total  |= ((uint16_t)((result_2 >> 24) & 0xFF) << 6) |
					 ((uint16_t)((result_2 >> 16) & 0xFF) << 7) |
					 ((uint16_t)((result_2 >> 8)  & 0xFF) << 9) |
					 ((uint16_t)((result_2 >> 0)  & 0xFF) << 10);

	// Package 3
	result_total  |= ((uint16_t)((result_3 >> 24) & 0xFF) << 11) |
					 ((uint16_t)((result_3 >> 16) & 0xFF) << 13) |
					 ((uint16_t)((result_3 >> 8)  & 0xFF) << 14) |
					 ((uint16_t)((result_3 >> 0)  & 0xFF) << 15);



	/***************************************************************************************/

	/*
	 ********************************************************
	 *													    *
	 *				Checking 12 consecutive bits	        *
	 *													    *
	 ********************************************************
	 *
	 *
	 */
	uint32_t x = ((uint32_t)result_total << 16) | result_total;

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

