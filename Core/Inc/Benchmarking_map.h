/*
 * Benchmark_map.h
 *
 *  Created on: 2 apr. 2026
 *      Author: erika
 */

#ifndef INC_ORB_BENCHMARKING_MAP_H_
#define INC_ORB_BENCHMARKING_MAP_H_

#include <stdint.h>

#define DWT_MAP_SIZE 16  // Must be power of 2

typedef struct {
    const char *key;   // Profile label
    int8_t      idx;   // Index into orb_dwt_registry
} DWT_MapEntry_t;

typedef struct {
    DWT_MapEntry_t entries[DWT_MAP_SIZE];
} DWT_Map_t;

extern DWT_Map_t dwt_map;

void   DWT_MapInit(void);
void   DWT_MapInsert(const char *key, int8_t idx);
int8_t DWT_Lookup(const char *key);  // Returns -1 if not found
DWT_Map_t DWT_get_map(void);

#endif /* INC_ORB_BENCHMARKING_MAP_H_ */
