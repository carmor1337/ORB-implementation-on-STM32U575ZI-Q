/*
 * Benchmarking_map.c
 *
 *  Created on: 2 apr. 2026
 *      Author: erika
 */

#include "Benchmarking_map.h"
#include <string.h>

DWT_Map_t dwt_map;

// Simple djb2 hash
static int8_t hash(const char *key) {
    uint32_t h = 5381;
    while (*key) h = ((h << 5) + h) ^ (uint8_t)*key++;
    return (int8_t)(h & (DWT_MAP_SIZE - 1));
}

void DWT_MapInit(void) {
    memset(&dwt_map, 0, sizeof(dwt_map));
    for (int8_t i = 0; i < DWT_MAP_SIZE; i++)
        dwt_map.entries[i].idx = -1;  // Mark all slots empty
}

void DWT_MapInsert(const char *key, int8_t idx) {
    int8_t slot = hash(key);
    // Linear probing on collision
    for (int8_t i = 0; i < DWT_MAP_SIZE; i++) {
        int8_t probe = (slot + i) & (DWT_MAP_SIZE - 1);
        if (dwt_map.entries[probe].idx == -1) {
            dwt_map.entries[probe].key = key;
            dwt_map.entries[probe].idx = idx;
            return;
        }
    }
}

int8_t DWT_Lookup(const char *key) {
    int8_t slot = hash(key);
    for (int8_t i = 0; i < DWT_MAP_SIZE; i++) {
        int8_t probe = (slot + i) & (DWT_MAP_SIZE - 1);
        if (dwt_map.entries[probe].idx == -1) return -1;
        if (dwt_map.entries[probe].key == NULL) continue;
        if (strcmp(dwt_map.entries[probe].key, key) == 0)
            return dwt_map.entries[probe].idx;
    }
    return -1;
}

DWT_Map_t DWT_get_map(void){
	return (dwt_map);
}








