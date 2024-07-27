#ifndef __UTILS_H__
#define __UTILS_H__

#define CTRLMODE_FLOAT 0
#define CTRLMODE_FIXED 1
#define GLOBAL_LOCK 0
#define GLOBAL_UNLOCK 1

uint16_t mappingINT(uint16_t value, uint16_t rangeOldHigh, uint16_t rangeNewHigh);
float mappingSHIFT(float value, float rangeNewLow, float rangeNewHigh);

#endif