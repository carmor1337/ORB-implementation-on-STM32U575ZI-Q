/*
 * config.h
 *
 *  Created on: 6 apr. 2026
 *      Author: erika
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

// Image specifics
// width x heigth
#define IMAGE_WIDTH  		   320 // Pixels
#define IMAGE_HEIGTH		   240 // Pixels


// Benchmarking configs

// ORB Config
#define MAX_FEATURE_POINTS 700
#define PATCH_SIZE 31

// FAST Config
#define ILLUMINATION_THRESHOLD 50 // Is in uint8_t
#define CONTINUOUS_PIXEL_THRESHOLD 12 // Also called fast 12

// Harris Config
#define HARRIS_K 0.04
#define HARRIS_WINDOW_SIZE 5
// rBRIEF Config

// Feature matching Config

#endif /* INC_CONFIG_H_ */
