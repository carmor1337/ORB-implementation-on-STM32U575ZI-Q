/*
 * profiling_config.h
 *
 *  Created on: 12 apr. 2026
 *      Author: erika
 */

#ifndef INC_PROFILING_CONFIG_H
#define INC_PROFILING_CONFIG_H
#include <stdint.h>
#include "config.h"

// --- Always present ---
extern int8_t idx_ORB_total;

// --- ORB sub-profiles ---
#if ORB_PROFILING
extern int8_t idx_FAST_total;
extern int8_t idx_HARRIS_total;
extern int8_t idx_Centroid_total;
extern int8_t idx_rBRIEF_total;
#endif

// --- FAST profiles ---
#if FAST_PROFILING
extern int8_t idx_FAST_setup;
extern int8_t idx_FAST_HSP;
extern int8_t idx_FAST_prep_calc;
extern int8_t idx_FAST_do_calc;
extern int8_t idx_FAST_get_result;
extern int8_t idx_FAST_consecutive_check;
#endif

// --- Harris profiles ---
#if HARRIS_PROFILING
extern int8_t idx_HARRIS_compute_matrix;
#endif

// --- Centroid profiles ---
#if CENTROID_PROFILING
extern int8_t idx_Centroid_momentumsl;
extern int8_t idx_Centroid_atan2;
#endif

// --- rBRIEF profiles ---
#if rBRIEF_PROFILING
extern int8_t idx_rBRIEF_rotation;
extern int8_t idx_rBRIEF_sample;
#endif

void profiling_init(void);


#endif /* INC_PROFILING_CONFIG_H_ */
