/*
 * ORB.c
 *
 *  Created on: 22 mars 2026
 *      Author: erika
 */


#include <stdbool.h>
#include <stdint.h>
#include <memory.h>

#include "Benchmarking.h"
#include "Benchmarking_map.h"
#include "common_includes.h"
#include "config.h"


#include "ORB.h"
#include "FAST.h"
#include "Harris_corner_detection.h"
#include "rBrief.h"

#define Profiling 0


static ORB_t g_orb_obj;
static ORB_feature_point_t g_feature_points[MAX_FEATURE_POINTS];
static uint16_t g_feature_count = 0;


bool ORB_assign_image(uint8_t *image_start){
	// Takes the image
	// Removes everything by 3
	// Assign the struct the processed image
	// return true if all good
	if (image_start == NULL){
		return false;
	}
	g_orb_obj.image = image_start;

	return true;
}


// Read access to count
uint16_t FAST_get_feature_count(void) {
    return g_feature_count;
}

ORB_feature_point_t* FAST_get_feature_points(void) {
    return g_feature_points;
}

bool ORB_init(uint8_t *image_start){
	if (!ORB_assign_image(image_start)){
			return false;
		}

	g_orb_obj.pixel_index = 0;
	g_orb_obj.height = IMAGE_HEIGTH;
	g_orb_obj.width = IMAGE_WIDTH;
	g_orb_obj.feature_point_list = g_feature_points;
	FAST_init();
	Harris_init();
	return true;
}

static bool append_feature_point(uint16_t x, uint16_t y, float score ){
	if (g_feature_count >= MAX_FEATURE_POINTS) {
		        return false;
		    }

		g_feature_points[g_feature_count].coords.x     = x;
		g_feature_points[g_feature_count].coords.y     = y;
		g_feature_points[g_feature_count].score = score;

		g_feature_count++;
		return true;
}

/// Extracts the feature points and match
void ORB_extract_and_match(void){
	for (int row=3; row <g_orb_obj.height-3; row++){
			for (int col = 3; col < g_orb_obj.width - 3; col++) {
				g_orb_obj.pixel_index = (uint32_t)(row * IMAGE_WIDTH + col);

				// First FAST
				// 	Should FAST be run once or chained together?
#if Profiling
				DWT_start(idx_FAST_total);
#endif
				bool is_feature_point = FAST_detect(&g_orb_obj);
#if Profiling
				  DWT_stop(idx_FAST_total);
#endif
				if (!is_feature_point){ continue;}
				uint16_t x = (uint16_t)g_orb_obj.pixel_index % IMAGE_WIDTH;
				uint16_t y = (uint16_t)g_orb_obj.pixel_index / IMAGE_WIDTH;
#if Profiling
				DWT_start(idx_HARRIS_total);
#endif
				// Then Harris score
				float score = harris_score_compute(&g_orb_obj);
#if Profiling
				  DWT_stop(idx_HARRIS_total);
#endif
				append_feature_point(x, y, score);
				// Then Centroid and angle
				// Then rotation
				// Then feature matching
			}
	}
}




