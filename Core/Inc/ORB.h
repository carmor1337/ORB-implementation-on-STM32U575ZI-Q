/*
 * ORB.h
 *
 *  Created on: 6 apr. 2026
 *      Author: erika
 */

#ifndef INC_ORB_H_
#define INC_ORB_H_

#include <stdbool.h>
#include <stdint.h>
#include "common_includes.h"



typedef struct __attribute__((packed))  {
	//
		uint16_t  x;
		uint16_t  y;
		float    score; 		 // Corner response strength
		float    angle;  		 // Orientation in radians for ORB rotation
		uint8_t  descriptor[DESCRIPTOR_BYTES];
}ORB_keypoint_package;

typedef struct __attribute__((packed))  {
	//
	const char *label;
    uint32_t    min_cycles;        // Minimum cycles recorded
    uint32_t    max_cycles;        // Maximum cycles recorded
    uint32_t    avg_cycles;        // Running average (cycles)
    uint32_t    runs_cycles;       // Number of measurements taken
    uint32_t    aggregate_cycles;  // Total time spent in the profiler
    uint32_t    clock_frequency;
}ORB_DWT_package;


bool ORB_init(uint8_t *image_start);

void ORB_extract_and_match(void);

void get_ORB_patch(ORB_t *orb_obj,ORB_keypoint_patch_t *patch);

void send_benchmark_and_keypoints_commit(void);

#endif /* INC_ORB_H_ */
