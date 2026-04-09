/*
 * Benchmarking.h
 *
 *  Created on: 2 apr. 2026
 *      Author: erika
 */

#ifndef INC_BENCHMARKING_H
#define INC_BENCHMARKING_H



#define DWT_MAX_PROFILES 10

typedef struct {
    const char *label;      		// Name of the measured section
    uint32_t    start;      		// DWT->CYCCNT snapshot at start
    uint32_t    elapsed;    		// Cycles taken (last run)
    uint32_t    min;        		// Minimum cycles recorded
    uint32_t    max;        		// Maximum cycles recorded
    uint32_t    avg;        		// Running average (cycles)
    uint32_t    runs;       		// Number of measurements taken
    uint32_t    aggregate;  		// Total time spent in the profiler
    //uint32_t 	load_store_stalls;  // Number of memory access delays
} DWT_Profile_t;


typedef struct {
	const char *label;
    double    min;        // Minimum time in recorded
    double    max;        // Maximum time in recorded
    double    avg;        // Running
    double    aggregate;	// total time
} DWT_profile_timed_t;

typedef struct {
    DWT_Profile_t profiles[DWT_MAX_PROFILES];
    uint8_t       count;
} DWT_Registry_t;

typedef struct {
	DWT_profile_timed_t profiles_us;
	DWT_profile_timed_t profiles_ms;
}DWT_timed_pair_t;


void DWT_init(void);

int8_t DWT_register(const char *label);


void DWT_start(int8_t idx);

void DWT_stop(int8_t idx);

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

#endif /* INC_BENCHMARKING_H_ */
