#include "stm32f10x.h"                  // Device header
#include <stdint.h>
#include <stdbool.h>
#include "encoder.h"
#include "key.h"
#include "systick.h"
#include "OLED.h"
#include "pwm.h"
#include "ic.h"
#include "18b20.h"

int main(void){
	uint8_t keyVal, encoderNum = 0;
    bool lockFlag = false;
    float result = 0;

	OLED_Init();
	encoderInit();
	pwmInit();
	icInit();
    DS18B20_portInit();
	
	while(1){
		encoderNum += getEncoderValue();
		if (encoderNum > 100){
			encoderNum = 100;
		}
        OLED_ShowNum(0,0,encoderNum, 3, 6);
        result = ds18b20GetTemp();
        if (result != 9999.0f){
            OLED_ShowFloatNum(0, 10, result, 3, 2, 6);
        }
        OLED_ShowNum(0, 20, GetSysRunTime(), 18, 6);
		keyVal = GetKeyVal();
        switch (keyVal)
        {
        case KEY1_SHORT_PRESS:
            // key 1
            break;
        case KEY1_LONG_PRESS:
            break;
        case KEY2_SHORT_PRESS:
            // key 2
            break;
        case KEY2_LONG_PRESS:
            break;
        case KEY3_SHORT_PRESS:
            break;
        case KEY3_LONG_PRESS:
            // lock key/ rotary key
            break;
        default:
            break;
        }
        OLED_Update();
    }
	
}
