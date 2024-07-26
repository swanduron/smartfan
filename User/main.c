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

uint16_t mappingINT(uint16_t value, uint16_t rangeOldHigh, uint16_t rangeNewHigh);

int main(void){
	uint8_t encoderNum = 0;
    uint8_t thresholdLow = 20, thresholdHigh = 75;
    uint8_t keyVal = 0;
    bool lockFlag = false;
    float result = 0;
    KeyDrvInit();
	OLED_Init();
	encoderInit();
	pwmInit();
	icInit();
    SystickInit();
    DS18B20_portInit();
	bool boxed = 0;

	while(1){

        // Read the encoder value
		encoderNum += getEncoderValue();
		if (encoderNum > 100){
			encoderNum = 100;
		}
        // Set the PWM duty
        PWM_SetCompare1(encoderNum);
        OLED_ShowNum(0,0,encoderNum, 3, 6);

        result = ds18b20GetTemp();
        if (result != 9999.0f){
            OLED_ShowFloatNum(0, 10, result, 3, 2, 6);
        }
        OLED_ShowNum(0, 20, GetSysRunTime(), 18, 6);
        OLED_ClearArea(0, 53, 127, 63);
        OLED_DrawRectangle(0, 53, mappingINT(encoderNum, 100, 127), 63, OLED_FILLED);

        // key management
		keyVal = GetKeyVal();
        switch (keyVal)
        {
            case KEY1_SHORT_PRESS:
                OLED_ShowString(0, 30, "button1", OLED_6X8);
                break;
            case KEY1_LONG_PRESS:
                break;
            case KEY2_SHORT_PRESS:
                // key 2
                OLED_ShowString(0, 30, "button2", OLED_6X8);
                break;
            case KEY2_LONG_PRESS:
                break;
            case KEY3_SHORT_PRESS:
                OLED_ShowString(0, 30, "button3", OLED_6X8);
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


uint16_t mappingINT(uint16_t value, uint16_t rangeOldHigh, uint16_t rangeNewHigh){
    float mappedValue;
    mappedValue = ((float) value / (float) rangeOldHigh) * (float) rangeNewHigh;
    return (uint16_t) mappedValue;
}
