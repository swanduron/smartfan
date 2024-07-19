#ifndef __SYSTICK_H__
#define __SYSTICK_H__
#include <stdint.h>

void SystickInit(void);
uint64_t GetSysRunTime(void);

#endif
