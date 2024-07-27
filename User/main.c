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
#include "utils.h"


int main(void){
	int8_t encoderNum = 0;
    uint8_t thresholdLow = 20, thresholdHigh = 70;
    uint8_t keyVal = 0;
    uint8_t pwmValue = 50;
    uint8_t encoderValue = 0;
    uint8_t controlMode = CTRLMODE_FIXED;
    uint8_t lockFlag = GLOBAL_UNLOCK;
    float tempBuffer = 45.5;
    float result = 0;
    bool boxed = 0;

    // Initial functions
    KeyDrvInit();
	OLED_Init();
	encoderInit();
	pwmInit();
	icInit();
    SystickInit();
    DS18B20_portInit();

	while(1){

        // update temp and time
        // test temperature
        result = ds18b20GetTemp();
        // the 9999.0f means the system is under test
        if (result != 9999.0f){
            tempBuffer = result;
            OLED_ShowFloatNum(0, 10, result, 3, 2, 6);
        }
        OLED_ShowNum(0, 20, GetSysRunTime(), 18, 6);

        // check key value and change the control byte
        keyVal = GetKeyVal();
        
        switch (keyVal)
        {
            case KEY1_SHORT_PRESS:
                // control flag
                controlMode = !controlMode;
                if (controlMode == CTRLMODE_FIXED){
                    OLED_ShowImage(93, 0, 16, 16, Mtmode);
                }
                else{
                    OLED_ShowImage(93, 0, 16, 16, Atmode);
                }
                
                break;
            case KEY1_LONG_PRESS:
                break;
            case KEY2_SHORT_PRESS:
                // lock flag
                lockFlag = !lockFlag;
                if (lockFlag == GLOBAL_LOCK){
                    OLED_ShowImage(110, 0, 16, 16, Locker);
                }
                else{
                    OLED_ClearArea(110, 0, 16, 16);
                }
                break;
            case KEY2_LONG_PRESS:
                break;
            case KEY3_SHORT_PRESS:
                // change modify target
                // TBD
                OLED_ClearArea(0, 30, 127, 38);
                OLED_ShowString(0, 30, "button3", OLED_6X8);
                break;
            case KEY3_LONG_PRESS:
                // lock key/ rotary key
                break;
            default:
                break;
        }

        // Read the encoder value
        encoderValue = getEncoderValue();
        if (lockFlag == GLOBAL_UNLOCK){
            encoderNum += encoderValue;
            if (encoderNum > 100){
                encoderNum = 100;
            }
            if (encoderNum < 0){
                encoderNum = 0;
            }
        }
		else{
            // if in lock mode, any operation will be ignored
        }
        // Set the PWM duty
        if (controlMode == CTRLMODE_FIXED){
            PWM_SetCompare1(encoderNum);
            OLED_ShowNum(0,0,encoderNum, 3, 6);
            OLED_ClearArea(0, 53, 127, 63);
            OLED_DrawRectangle(0, 53, mappingINT(encoderNum, 100, 127), 63, OLED_FILLED);
        }
        else{
            // control mode == float mode
            float pwmDutyBuffer = mappingSHIFT(tempBuffer, thresholdLow, thresholdHigh);
            uint8_t pwmDuty = pwmDutyBuffer;
            // PWM_SetCompare1(pwmDuty);
            OLED_ShowNum(40, 0, pwmDuty, 4, OLED_6X8);
            OLED_ClearArea(0, 53, 127, 63);
            OLED_DrawRectangle(0, 53, pwmDuty, 63, OLED_FILLED);
        }
        

		
        OLED_Update();
    }
	
}

