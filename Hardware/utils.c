#include "stm32f10x.h"                  // Device header
#include <stdint.h>
#include "utils.h"

uint16_t mappingINT(uint16_t value, uint16_t rangeOldHigh, uint16_t rangeNewHigh){
    float mappedValue;
    mappedValue = ((float) value / (float) rangeOldHigh) * (float) rangeNewHigh;
    return (uint16_t) mappedValue;
}

float mappingSHIFT(float value, float rangeNewLow, float rangeNewHigh){
    // default range is 0~100
    if (rangeNewHigh < rangeNewLow){
        return 0;
    }
    float mappedValue = 0;
    float databuffer = 0;
    databuffer = value - rangeNewLow;
    if (databuffer < 0){
        return 0;
    }
    else{
        mappedValue = (databuffer / (rangeNewHigh - rangeNewLow));
    }
    return mappedValue * 100;
}

