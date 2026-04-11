/*
 * config.h
 *
 *  Created on: 6 apr. 2026
 *      Author: erika
 */

#ifndef INC_CONFIG_H
#define INC_CONFIG_H

// Image specifics
// width x heigth
#define IMAGE_WIDTH  		   320 // Pixels
#define IMAGE_HEIGTH		   240 // Pixels

// Benchmarking configs
#define ORB_PROFILING 	   1
#define FAST_PROFILING 	   0
#define NMS_PROFILING      0
#define HARRIS_PROFILING   0
#define CENTROID_PROFILING 0
#define rBRIEF_PROFILING   0

// Define in your code
#if defined(__OPTIMIZE_SIZE__)
    #define OPT_LEVEL "Os"
#elif defined(__OPTIMIZE__)
    #define OPT_LEVEL "O1/O2/O3"  // GCC doesn't distinguish between these at runtime
#else
    #define OPT_LEVEL "O0"
#endif

// For the printing in SWV
#define COL_PART  17
#define COL_NUM   10

#define COL_OVERVIEW_SECTION_NAME  12
#define COL_OVERVIEW_NUM 6
#define COL_OVERVIEV_PERCENTAGE 5

// ORB Config
#define MAX_FEATURE_POINTS 700
#define PATCH_SIZE 		   31


// FAST Config
#define ILLUMINATION_THRESHOLD     60 // Is in uint8_t
#define CONTINUOUS_PIXEL_THRESHOLD 12 // Also called fast 12

// Harris Config
#define HARRIS_K           0.04
#define HARRIS_WINDOW_SIZE 5

// Centroid Config

// rBRIEF Config
#define DESCRIPTOR_BYTES 8

// Feature matching Config



// Misc
#define LED_BLINK_WAIT 500


#endif /* INC_CONFIG_H_ */
