#include "stm32f10x.h"                  // Device header
#include <stdint.h>

static uint64_t g_sysRunTime = 0;

void SystickInit(void){
    RCC_ClocksTypeDef getFreq;
    RCC_GetClocksFreq(&getFreq);
    if (SysTick_Config(getFreq.SYSCLK_Frequency / 1000)){
        while (1);
    }
}


void SysTick_Handler(void){
    g_sysRunTime ++;
}

uint64_t GetSysRunTime(void){
    return g_sysRunTime;
}
