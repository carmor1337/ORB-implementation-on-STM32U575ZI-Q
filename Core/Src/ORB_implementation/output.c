/*
 * output.c
 *
 *  Created on: 10 apr. 2026
 *      Author: erika
 */

#include <stdbool.h>
#include <stdint.h>
#include <memory.h>
#include <stdio.h>


#include "output.h"
#include "Benchmarking.h"
#include "Benchmarking_map.h"
#include "profiling_config.h"
#include "common_includes.h"
#include "config.h"
#include "main.h"
#include "ORB.h"


static double g_total_us = 0;
static double g_total_ms = 0;
static double g_convert_to_percent = 100.0;

//static const int32_t kitti_num_pixels = 1241 * 376;

static double output_commit_overview(const char* feature_message,const char* performance_message){


	uint16_t feature_count = ORB_get_feature_count();


	double other_total_ms = 0;

	// Commit overview
	printf("================== Overview ===============\n");
	printf("feat: %s\n", feature_message);
	printf("perf: %s\n\n", performance_message);
	printf("Performance baseline (%u keypoints, -%s, %luMHz)\n",
			feature_count,
			OPT_LEVEL,
			(SystemCoreClock/1000000U));
	printf("- %-*s %*.0fms\n",
			COL_OVERVIEW_SECTION_NAME, "Total:",
			COL_OVERVIEW_NUM, (double)g_total_ms);

#if ORB_PROFILING

	int num_of_sections = 4;
	int8_t sections[] = {idx_FAST_total,idx_HARRIS_total,idx_Centroid_total,idx_rBRIEF_total };
	for (int i= 0; i < num_of_sections; i++){
		DWT_timed_pair_t* times_profile = DWT_get_timed(sections[i]);
		char label[COL_OVERVIEW_SECTION_NAME + 2];
		snprintf(label, sizeof(label), "%s:", times_profile->profiles_ms.label);

		printf("- %-*s %*.0fms (%*.1f%%)\n",
			COL_OVERVIEW_SECTION_NAME, label,          // left align string to fixed width
			COL_OVERVIEW_NUM,          (double)times_profile->profiles_ms.aggregate,
			COL_OVERVIEV_PERCENTAGE,   (double)(times_profile->profiles_ms.aggregate / g_total_ms * g_convert_to_percent));
		other_total_ms += times_profile->profiles_ms.aggregate;

	}


	printf("- %-*s %*.0fms (%*.1f%%)\n",
				COL_OVERVIEW_SECTION_NAME, "unaccounted",          // left align string to fixed width
				COL_OVERVIEW_NUM,          (double)(g_total_ms- other_total_ms),
				COL_OVERVIEV_PERCENTAGE,   (double)((g_total_ms- other_total_ms) / g_total_ms * g_convert_to_percent));

#endif
	printf("\n");
	printf("=== Configs ====\n");
	printf("FAST:     threshold (t) %d, n = %d\n",ILLUMINATION_THRESHOLD,CONTINUOUS_PIXEL_THRESHOLD);
	printf("Harris:   K %f, harris window size = %d\n",HARRIS_K, HARRIS_WINDOW_SIZE);
	printf("Centroid: None\n");
	printf("rBRIEF:   None\n");
	return (other_total_ms);
}






static void setup_table_header(const char* header){
	printf("===  %s  ===\n",header);
	printf("| %-*s | %*s | %*s | %*s | %*s | %*s\n",
		COL_PART, "Part",
		COL_NUM,  "min",
		COL_NUM,  "max",
		COL_NUM,  "average",
		COL_NUM,  "aggregate",
		COL_NUM,  "% total");

	// Separator
	printf("|%-*s-|-%-*s-|-%-*s-|-%-*s-|-%-*s-|-%-*s-|-%-*s\n",
		COL_PART, "----------------------------",
		COL_NUM,  "----------",
		COL_NUM,  "----------",
		COL_NUM,  "----------",
		COL_NUM,  "----------",
		COL_NUM,  "----------",
		COL_NUM,  "----------");
}








void output_table(int8_t idx, int stat){
	DWT_timed_pair_t* time_profile =  DWT_get_timed(idx);
	// Data rows
	// us
	switch (stat){
		case 0:
			printf("| %-*s | %*.2f | %*.2f | %*.2f | %*.2f | %*.2f\n",
				COL_PART, time_profile->profiles_us.label,
				COL_NUM,  (double)time_profile->profiles_us.min,
				COL_NUM,  (double)time_profile->profiles_us.max,
				COL_NUM,  (double)time_profile->profiles_us.avg,
				COL_NUM,  (double)time_profile->profiles_us.aggregate,
				COL_NUM,  (double)time_profile->profiles_us.aggregate/g_total_us * g_convert_to_percent);
				break;

		case 1:
			printf("| %-*s | %*.2f | %*.2f | %*.2f | %*.2f | %*.2f\n",
				COL_PART, time_profile->profiles_ms.label,
				COL_NUM,  (double)time_profile->profiles_ms.min,
				COL_NUM,  (double)time_profile->profiles_ms.max,
				COL_NUM,  (double)time_profile->profiles_ms.avg,
				COL_NUM,  (double)time_profile->profiles_ms.aggregate,
				COL_NUM,  (double)time_profile->profiles_ms.aggregate/g_total_ms * g_convert_to_percent);
			break;
		default:
			printf("| %-*s | %-*s | %-*s | %-*s | %-*s | %-*sf\n",
						COL_PART, "-",
						COL_NUM,  "-",
						COL_NUM,  "-",
						COL_NUM,  "-",
						COL_NUM,  "-",
						COL_NUM,  "-");
	}
}








static void output_commit_table(void){
	const char *keys[] = {"us", "ms"};
	int different_measurment_types = 2;
	uint16_t profile_count = DWT_get_profile_count();
	uint16_t feature_count = ORB_get_feature_count();


	// Header
	// sets up the table headers per statistic
	printf("================== ORB Results ===============\n");
	printf("number of keypoints: %d\n",feature_count);
	for ( int stat= 0; stat < different_measurment_types; stat++){

		setup_table_header(keys[stat]);

		// Adds the table data

		for (uint16_t i= 0; i < profile_count; i++){
			output_table((int8_t)i,stat);


		}
		printf("\n");
	}
}



void output_commit_message(const char* feature_message,const char* performance_message){
	DWT_convert_all_profiles_to_timed();
	DWT_timed_pair_t* orb_times_profile= DWT_get_timed(idx_ORB_total);

	g_total_us = orb_times_profile->profiles_us.aggregate;
	g_total_ms = orb_times_profile->profiles_ms.aggregate;


	output_commit_overview(feature_message,performance_message);
	output_commit_table();
	printf("=====================================================================\n\n");

}

