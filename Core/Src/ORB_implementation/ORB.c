/*
 * ORB.c
 *
 *  Created on: 22 mars 2026
 *      Author: erika
 */


#include <stdbool.h>
#include <stdint.h>
#include <memory.h>
#include <stdio.h>

#include "Benchmarking.h"
#include "Benchmarking_map.h"
#include "common_includes.h"
#include "config.h"
#include "profiling_config.h"
#include "main.h"

#include "ORB.h"
#include "FAST.h"
#include "Harris_corner_detection.h"
#include "Orientation.h"
#include "rBrief.h"



static ORB_t g_orb_obj;
static ORB_feature_point_t g_feature_points[MAX_FEATURE_POINTS];
static uint16_t g_feature_count = 0;


bool ORB_assign_image(uint8_t *image_start){
	// Takes the image
	// return true if all good
	if (image_start == NULL){
		return (false);
	}
	g_orb_obj.image = image_start;

	return (true);
}


// Read access to count
uint16_t ORB_get_feature_count(void) {
    return (g_feature_count);
}

ORB_feature_point_t* FAST_get_feature_points(void) {
    return (g_feature_points);
}

bool ORB_init(uint8_t *image_start){
	if (!ORB_assign_image(image_start)){
			return (false);
		}

	g_orb_obj.pixel_index = 0;
	g_orb_obj.height = IMAGE_HEIGTH;
	g_orb_obj.width = IMAGE_WIDTH;
	g_orb_obj.feature_point_list = g_feature_points;
	FAST_init();
	Harris_init();
	return (true);
}

static bool append_feature_point(uint16_t x, uint16_t y, float score ){
	if (g_feature_count >= MAX_FEATURE_POINTS) {
		__BKPT(0);
			return (false);
		}

		g_feature_points[g_feature_count].coords.x     = x;
		g_feature_points[g_feature_count].coords.y     = y;
		g_feature_points[g_feature_count].score = score;

		g_feature_count++;
		return (true);
}

// Assume that current pixel is the pixel we want
void get_ORB_patch(ORB_t *orb_obj,ORB_keypoint_patch_t *patch){
	patch->pixel = orb_obj->pixel_index;
	for (int y = 0; y < 31; y++){
		for (int x = 0; x < 31; x++){
			int img_idx = (-15 + y)*IMAGE_WIDTH + (-15 + x);

			patch->patch_data[x][y] = orb_obj->image[img_idx];
		}

	}
	return;
}


/// Extracts the feature points and match
void ORB_extract_and_match(void){
	g_feature_count = 0;
	uint16_t current_index = g_feature_count;
	for (int row=3; row <g_orb_obj.height-3; row++){
		for (int col = 3; col < g_orb_obj.width - 3; col++) {
			g_orb_obj.pixel_index = (uint32_t)(row * IMAGE_WIDTH + col);
/*************************  FAST Start *******************************************/

#if ORB_PROFILING
			DWT_start(idx_FAST_total);
#endif

			bool is_feature_point = FAST_detect(&g_orb_obj);

#if ORB_PROFILING
			DWT_stop(idx_FAST_total);
			DWT_process_data(idx_FAST_total);

/*************************  FAST Stop *******************************************/

/*************************  Check feature point Start *******************************************/

			DWT_start(idx_is_feature_point_and_coords);
#endif

			if (!is_feature_point){ continue;}
			uint16_t x = (uint16_t)g_orb_obj.pixel_index % IMAGE_WIDTH;
			uint16_t y = (uint16_t)g_orb_obj.pixel_index / IMAGE_WIDTH;

#if ORB_PROFILING
			DWT_stop(idx_is_feature_point_and_coords);
			DWT_process_data(idx_is_feature_point_and_coords);

/*************************  Check feature point Stop *******************************************/

/************************* Harris Start  *******************************************/
			DWT_start(idx_HARRIS_total);
#endif

				// Then Harris score
			float score = harris_score_compute(&g_orb_obj);

#if ORB_PROFILING

			DWT_stop(idx_HARRIS_total);
			DWT_process_data(idx_HARRIS_total);
/*************************  Harris Stop *******************************************/

/*************************  Get ORB patch Start *******************************************/
			DWT_start(idx_get_ORB_patch);
#endif
			if (!append_feature_point(x, y, score)) { continue; }
			ORB_keypoint_patch_t patch;
			ORB_feature_point_t *fp = &g_orb_obj.feature_point_list[current_index];

			get_ORB_patch(&g_orb_obj, &patch);

#if ORB_PROFILING

			DWT_stop(idx_get_ORB_patch);
			DWT_process_data(idx_get_ORB_patch);
/*************************  Get ORB patch Stop *******************************************/

/*************************  Centroid Start *******************************************/
			DWT_start(idx_Centroid_total);
#endif

			compute_intensity_centroid(&g_orb_obj, fp, g_orb_obj.pixel_index );

#if CENTROID_PROFILING
			DWT_process_data(idx_Centroid_momentums);
			DWT_process_data(idx_Centroid_atan2);
#endif
#if ORB_PROFILING
			DWT_stop(idx_Centroid_total);
			DWT_process_data(idx_Centroid_total);

/*************************  Centroid Stop *******************************************/

/*************************  rBRIEF Start *******************************************/

			DWT_start(idx_rBRIEF_total);
#endif

			rBRIEF_compute(fp, &patch);

#if ORB_PROFILING
			DWT_stop(idx_rBRIEF_total);
			DWT_process_data(idx_rBRIEF_total);
#endif
/*************************  rBRIEF Stop *******************************************/
			}
	}

}


/*	Profiling
 * 	Get profiles with cycle's
 * 	Get profile with time
 * 	Send it to where i needs to be.
 *
 */

/* Keypoints
 * Grab the keypoints in array
 * Send them via uart
 *
 *
 *
 */

// TODO: implement so that I can have nice visuals in python
void send_benchmark_and_keypoints_uart(void){

}
