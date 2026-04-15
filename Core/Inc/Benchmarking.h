/*
 * Benchmarking.h
 *
 *  Created on: 2 apr. 2026
 *      Author: erika
 */

#ifndef INC_BENCHMARKING_H
#define INC_BENCHMARKING_H

#include "main.h"
#include <stdint.h>
#define DWT_MAX_PROFILES 10

typedef struct {
    volatile const char *label;      		// Name of the measured section
    volatile uint32_t    start;      		// DWT->CYCCNT snapshot at start
    volatile uint32_t    elapsed;    		// Cycles taken (last run)
    volatile uint32_t    min;        		// Minimum cycles recorded
    volatile uint32_t    max;        		// Maximum cycles recorded
    volatile uint32_t    avg;        		// Running average (cycles)
    volatile uint32_t    runs;       		// Number of measurements taken
    volatile uint32_t    aggregate;  		// Total time spent in the profiler
    //uint32_t 	load_store_stalls;  // Number of memory access delays
} DWT_Profile_t;


typedef struct {
	volatile const char *label;
	volatile double    min;        // Minimum time in recorded
	volatile double    max;        // Maximum time in recorded
	volatile double    avg;        // Running
	volatile double    aggregate;	// total time
} DWT_profile_timed_t;

typedef struct {
    DWT_Profile_t profiles[DWT_MAX_PROFILES];
    uint8_t       count;
} DWT_Registry_t;

typedef struct {
	volatile DWT_profile_timed_t profiles_us;
	volatile DWT_profile_timed_t profiles_ms;
}DWT_timed_pair_t;


extern DWT_Registry_t dwt_registry;

void DWT_init(void);

int8_t DWT_register(const char *label);




__attribute__((always_inline)) inline void DWT_start(int8_t idx) {

    dwt_registry.profiles[idx].start = DWT->CYCCNT;

}
__attribute__((always_inline)) inline  void DWT_stop(int8_t idx) {
    DWT_Profile_t *p = &dwt_registry.profiles[idx];
    p->elapsed = DWT->CYCCNT - p->start;;
}
DWT_Profile_t *DWT_get(int8_t idx);

DWT_timed_pair_t *DWT_get_timed(int8_t idx);

DWT_profile_timed_t DWT_convert_cycles_to_us(int8_t idx);

DWT_profile_timed_t DWT_convert_cycles_to_ms(int8_t idx);

void DWT_aggregate_reset(int8_t idx);

void DWT_aggregate_reset_all(void);

DWT_timed_pair_t DWT_convert_to_timed(int8_t idx);

void DWT_convert_all_profiles_to_timed(void);

uint16_t DWT_get_profile_count(void);

DWT_Registry_t* DWT_get_registry(void);

DWT_timed_pair_t* DWT_get_timed_registry(void);

void DWT_process_data(int8_t idx);

#endif /* INC_BENCHMARKING_H_ */
