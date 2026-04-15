/*
 * profiling_congif.c
 *
 *  Created on: 12 apr. 2026
 *      Author: erika
 */


#include "profiling_config.h"
#include "Benchmarking.h"

// --- Always present ---
int8_t idx_ORB_total = -1;

// --- ORB sub-profiles ---
#if ORB_PROFILING
int8_t idx_FAST_total    = -1;
int8_t idx_is_feature_point_and_coords  = -1;
int8_t idx_HARRIS_total  = -1;
int8_t idx_get_ORB_patch  = -1;
int8_t idx_Centroid_total = -1;
int8_t idx_rBRIEF_total  = -1;
#endif

// --- FAST profiles ---
#if FAST_PROFILING
int8_t idx_FAST_setup            = -1;
int8_t idx_FAST_HSP              = -1;
int8_t idx_FAST_prep_calc        = -1;
int8_t idx_FAST_do_calc          = -1;
int8_t idx_FAST_get_result       = -1;
int8_t idx_FAST_consecutive_check = -1;
#endif

// --- Harris profiles ---
#if HARRIS_PROFILING
int8_t idx_HARRIS_compute_matrix = -1;
#endif

// --- Centroid profiles ---
#if CENTROID_PROFILING
int8_t idx_Centroid_momentumsl = -1;
int8_t idx_Centroid_atan2      = -1;
#endif

// --- rBRIEF profiles ---
#if rBRIEF_PROFILING
int8_t idx_rBRIEF_rotation = -1;
int8_t idx_rBRIEF_sample   = -1;
#endif

// -------------------------------------------------------

void profiling_init(void) {
    DWT_init();   // hardware init first

    idx_ORB_total = DWT_register("ORB");

#if ORB_PROFILING
    idx_FAST_total     = DWT_register("FAST");
    idx_is_feature_point_and_coords = DWT_register("Is feature point");
    idx_HARRIS_total   = DWT_register("Harris");
    idx_get_ORB_patch  = DWT_register("Get ORB patch");
    idx_Centroid_total = DWT_register("Centroid");
    idx_rBRIEF_total   = DWT_register("rBRIEF");
#endif

#if FAST_PROFILING
    idx_FAST_setup             = DWT_register("FAST: Setup");
    idx_FAST_HSP               = DWT_register("FAST: HST");
    idx_FAST_prep_calc         = DWT_register("FAST: Prepare calculations");
    idx_FAST_do_calc           = DWT_register("FAST: Do calculations");
    idx_FAST_get_result        = DWT_register("FAST: Aggregate result");
    idx_FAST_consecutive_check = DWT_register("FAST: Consecutive check");
#endif

#if HARRIS_PROFILING
    idx_HARRIS_compute_matrix = DWT_register("HARRIS:matrix");
#endif

#if CENTROID_PROFILING
    idx_Centroid_momentumsl = DWT_register("Centroid:m01,m10");
    idx_Centroid_atan2      = DWT_register("Centroid:atan2");
#endif

#if rBRIEF_PROFILING
    idx_rBRIEF_rotation = DWT_register("rBRIEF:rotation");
    idx_rBRIEF_sample   = DWT_register("rBRIEF:sample");
#endif
}
