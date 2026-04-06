/*
 * Orientation.c
 *
 *  Created on: 6 apr. 2026
 *      Author: erika
 */


#include <stdbool.h>
#include <stdint.h>

#include "common_includes.h"
#include "config.h"




// 31 x 31 area
// Get circle from the area
// Do the summation



/*
void compute_intensity_centroid(void){
	uint32_t m10 = 0; // Sum of x*I for all x,y
	uint32_t m01 = 0; // Sum of y*I for all x,y
	cfor (int y = -15; y <= 15; y++){
    for (int x = -15; x <= 15; x++){
        if (x*x + y*y <= 15*15){  // inside circle
            m10 += x * image[center + y*IMAGE_WIDTH + x];
            m01 += y * image[center + y*IMAGE_WIDTH + x];

        }
    }
}
float angle = atan2f(m01, m10);
}
*/
