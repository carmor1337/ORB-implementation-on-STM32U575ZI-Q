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

#define PATCH_SIZE 31


typedef struct {
	uint8_t pixel;
	uint8_t patch_size[PATCH_SIZE*PATCH_SIZE];
}ORB_patch_t;




void ORB_extract_and_match(void);

bool ORB_init(uint8_t *image_start);

#endif /* INC_ORB_H_ */
