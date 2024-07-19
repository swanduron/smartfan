#ifndef __KEY_DRV_H__
#define __KEY_DRV_H__
#include <stdint.h>

#define KEY1_SHORT_PRESS 0x01
#define KEY2_SHORT_PRESS 0x02
#define KEY3_SHORT_PRESS 0x03
#define KEY1_LONG_PRESS 0x81
#define KEY2_LONG_PRESS 0x82
#define KEY3_LONG_PRESS 0x83

void KeyDrvInit(void);
uint8_t GetKeyVal(void);

#endif
