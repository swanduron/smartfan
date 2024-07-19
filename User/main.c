#include "stm32f10x.h"                  // Device header
#include <stdint.h>
#include "encoder.h"
#include "key.h"
#include "systick.h"
#include "OLED.h"
#include "pwm.h"
#include "ic.h"

int main(void){
	uint8_t keyVal, encoderNum = 0;

	OLED_Init();
	encoderInit();
	pwmInit();
	icInit();
	
	OLED_ShowString(0,0,"hello!", OLED_8X16);
	OLED_Update();
	while(1){
		encoderNum += getEncoderValue();
		if (encoderNum < 0){
			encoderNum = 0;
		}
		if (encoderNum > 100){
			encoderNum = 100;
		}
		keyVal = GetKeyVal();
        switch (keyVal)
        {
        case KEY1_SHORT_PRESS:
            break;
        case KEY1_LONG_PRESS:
            break;
        case KEY2_SHORT_PRESS:
            break;
        case KEY2_LONG_PRESS:
            break;
        case KEY3_SHORT_PRESS:
            break;
        case KEY3_LONG_PRESS:
            break;
        default:
            break;
        }
    }
	
}
