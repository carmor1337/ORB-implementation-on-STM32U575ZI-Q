// defines the macros needed for bencmarking with cycles

#include "stdint.h"
#include "main.h"
#include "Benchmarking.h"
#include "Benchmarking_map.h"

DWT_Registry_t dwt_registry;

static DWT_timed_pair_t dwt_timed_registry[DWT_MAX_PROFILES];

uint16_t profile_count = 0;

// Call once at startup
void DWT_init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    for (uint8_t i = 0; i < DWT_MAX_PROFILES; i++) {
        dwt_registry.profiles[i].start     = 0;
        dwt_registry.profiles[i].elapsed   = 0;
        dwt_registry.profiles[i].max 	   = 0;
        dwt_registry.profiles[i].min       = UINT32_MAX;
        dwt_registry.profiles[i].avg       = 0;
        dwt_registry.profiles[i].runs      = 0;
        dwt_registry.profiles[i].aggregate = 0;
        //dwt_registry.profiles[i].load_store_stalls = 0;
    }

}

// Returns index, or -1 if registry is full
int8_t DWT_register(const char *label) {
    if (dwt_registry.count >= DWT_MAX_PROFILES) return (-1);
    int8_t idx = (int8_t)dwt_registry.count++;
    dwt_registry.profiles[idx].label = label;
    DWT_MapInsert(label, idx);
    profile_count++;
    return ((int8_t)idx);
}


void DWT_process_data(int8_t idx){
	DWT_Profile_t *p = &dwt_registry.profiles[idx];

	if (p->elapsed < p->min) p->min = p->elapsed;
	if (p->elapsed > p->max) p->max = p->elapsed;
	p->avg = (uint32_t)(((uint64_t)p->avg * p->runs + p->elapsed) / (p->runs + 1));
	p->runs++;
	p->aggregate += p->elapsed;
}
// Get a pointer to a profile by index
DWT_Profile_t *DWT_get(int8_t idx) {
    return (&dwt_registry.profiles[idx]);
}



DWT_timed_pair_t *DWT_get_timed(int8_t idx){
	 return (&dwt_timed_registry[idx]);
}

DWT_profile_timed_t DWT_convert_cycles_to_us(int8_t idx){
	DWT_Profile_t *p = &dwt_registry.profiles[idx];
	DWT_profile_timed_t profile_us;
	double clock_factor =  (SystemCoreClock / 1000000.0);
	profile_us.label = p->label;
	profile_us.min = p->min/ clock_factor;
	profile_us.max = p->max/ clock_factor;
	profile_us.avg = p->avg/ clock_factor;
	profile_us.aggregate = p->aggregate/ clock_factor;
	return profile_us;
}


DWT_profile_timed_t DWT_convert_cycles_to_ms(int8_t idx){

	DWT_Profile_t *p = &dwt_registry.profiles[idx];
	DWT_profile_timed_t profile_ms;
	double clock_factor =  (SystemCoreClock / 1000.0);
	profile_ms.label = p->label;
	profile_ms.min = p->min/ clock_factor;
	profile_ms.max = p->max/ clock_factor;
	profile_ms.avg = p->avg/ clock_factor;
	profile_ms.aggregate = p->aggregate/ clock_factor;
	return profile_ms;
}



void DWT_aggregate_reset(int8_t idx){
	DWT_Profile_t *p = &dwt_registry.profiles[idx];
	p->aggregate = 0;
}
// Resets all the aggregates for the profiles
void DWT_aggregate_reset_all(void){
	for (int8_t i = 0; i < dwt_registry.count; i++){
			dwt_registry.profiles[i].aggregate = 0;
		}
}


void DWT_reset(int8_t idx){
	DWT_Profile_t *p = &dwt_registry.profiles[idx];
	p->start     = 0;
	p->elapsed   = 0;
	p->max 	     = 0;
	p->min       = UINT32_MAX;
	p->avg       = 0;
	p->runs      = 0;
	p->aggregate = 0;
}

DWT_Registry_t* DWT_get_registry(void){
	return (&dwt_registry);
}

uint16_t DWT_get_profile_count(void){
	return (profile_count);
}

DWT_timed_pair_t* DWT_get_timed_registry(void){
	return (dwt_timed_registry);
}

DWT_timed_pair_t DWT_convert_to_timed(int8_t idx){
	DWT_timed_pair_t res;
	res.profiles_us = DWT_convert_cycles_to_us(idx);
	res.profiles_ms = DWT_convert_cycles_to_ms(idx);
	return (res);
}

void DWT_convert_all_profiles_to_timed(void){
	for (int8_t i = 0; i < dwt_registry.count; i++){
		dwt_timed_registry[i] = DWT_convert_to_timed(i);
	}
}

