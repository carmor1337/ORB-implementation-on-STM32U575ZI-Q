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





bool ORB_init(uint8_t *image_start);

void ORB_extract_and_match(void);

void get_ORB_patch(ORB_t *orb_obj,ORB_keypoint_patch_t *patch);

#endif /* INC_ORB_H_ */
