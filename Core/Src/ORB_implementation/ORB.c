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
uint16_t FAST_get_feature_count(void) {
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
	for (int row=3; row <g_orb_obj.height-3; row++){
			for (int col = 3; col < g_orb_obj.width - 3; col++) {
				g_orb_obj.pixel_index = (uint32_t)(row * IMAGE_WIDTH + col);

				// First FAST
				// 	Should FAST be run once or chained together?
#if ORB_PROFILING
				DWT_start(DWT_Lookup("FAST"));
#endif
				bool is_feature_point = FAST_detect(&g_orb_obj);
#if ORB_PROFILING
				  DWT_stop(DWT_Lookup("FAST"));
#endif
				if (!is_feature_point){ continue;}
				uint16_t x = (uint16_t)g_orb_obj.pixel_index % IMAGE_WIDTH;
				uint16_t y = (uint16_t)g_orb_obj.pixel_index / IMAGE_WIDTH;
#if ORB_PROFILING
				DWT_start(DWT_Lookup("Harris"));
#endif
				// Then Harris score
				float score = harris_score_compute(&g_orb_obj);

#if ORB_PROFILING
				  DWT_stop(DWT_Lookup("Harris"));
#endif

			    append_feature_point(x, y, score);
				ORB_keypoint_patch_t patch;
				ORB_feature_point_t *fp = &g_orb_obj.feature_point_list[g_feature_count];

				get_ORB_patch(&g_orb_obj, &patch);
#if ORB_PROFILING
				DWT_start(DWT_Lookup("Centroid"));
#endif

				compute_intensity_centroid(&g_orb_obj,fp,g_orb_obj.pixel_index );

#if ORB_PROFILING
			    DWT_stop(DWT_Lookup("Centroid"));

				DWT_start(DWT_Lookup("rBRIEF"));
#endif

				rBRIEF_compute(fp, &patch);

#if ORB_PROFILING
				DWT_stop(DWT_Lookup("rBRIEF"));
#endif

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






	//

}


// TODO: Add the below as a first stage commit message
/*
 * feat: add ORB profiling instrumentation

	Performance baseline (459 keypoints, O0, 160MHz):
	- Total:    1129ms
	- FAST:      380ms (33.6%)
	- Harris:     97ms  (8.6%)
	- Centroid:  184ms (16.3%)
	- rBRIEF:    184ms (16.3%)
	- Unknown:   287ms (25.2%) <- under investigation

	Full profile: <paste table here>
 *
 *
 */
void send_benchmark_and_keypoints_commit(void){
	DWT_convert_all_profiles_to_timed();
	uint16_t profile_count = DWT_get_profile_count();
	// TODO: Implement overhead/leftover and Kitti calculations
	const char *keys[] = {"us", "ms"};
	int different_measurment_types = 2;

	DWT_timed_pair_t* orb_times_profile= DWT_get_timed(DWT_Lookup("ORB"));
	double total_us = orb_times_profile->profiles_us.aggregate;
	double total_ms = orb_times_profile->profiles_ms.aggregate;
	double to_percent = 100.0;

	// Commit overview
	printf("feat: xxxxxx");
	printf("\n\n");
	printf("Performance baseline (%u keypoints, -%s, %luMHz)\n",
			g_feature_count,
			OPT_LEVEL,
			(SystemCoreClock/1000000U));

	// Header
	printf("=== ORB Results ===\n");
	printf("number of keypoints: %d\n",g_feature_count);
	for ( int stat= 0; stat < different_measurment_types; stat++){

		// TODO: Adjust so that it is good align
		printf("===  %s  ===\n",keys[stat]);
		printf("| %-*s | %*s | %*s | %*s | %*s | %*s\n",
			COL_PART, "Part",
			COL_NUM,  "min",
			COL_NUM,  "max",
			COL_NUM,  "average",
			COL_NUM,  "aggregate",
			COL_NUM,  "% total");

		// Separator
		printf("|%-*s-|-%-*s-|-%-*s-|-%-*s-|-%-*s-|-%-*s\n",
			COL_PART, "------------------",
			COL_NUM,  "----------",
			COL_NUM,  "----------",
			COL_NUM,  "----------",
			COL_NUM,  "----------",
			COL_NUM,  "----------");


		for (uint16_t i= 0; i < profile_count; i++){
			DWT_timed_pair_t* time_profile =  DWT_get_timed((int8_t)i);
			// Data rows
			if (stat == 0){

			printf("| %-*s | %*.2f| %*.2f | %*.2f | %*.2f | %*.2f\n",
				COL_PART, time_profile->profiles_us.label,
				COL_NUM,  (double)time_profile->profiles_us.min,
				COL_NUM,  (double)time_profile->profiles_us.max,
				COL_NUM,  (double)time_profile->profiles_us.avg,
				COL_NUM,  (double)time_profile->profiles_us.aggregate,
				COL_NUM,  (double)time_profile->profiles_us.aggregate/total_us * to_percent);
			}
			if (stat == 1){

				printf("| %-*s | %*.2f | %*.2f | %*.2f | %*.2f | %*.2f\n",
					COL_PART, time_profile->profiles_ms.label,
					COL_NUM,  (double)time_profile->profiles_ms.min,
					COL_NUM,  (double)time_profile->profiles_ms.max,
					COL_NUM,  (double)time_profile->profiles_ms.max,
					COL_NUM,  (double)time_profile->profiles_ms.aggregate,
					COL_NUM,  (double)time_profile->profiles_ms.aggregate/total_ms * to_percent);
			}
		}
		printf("\n");


	}




}

