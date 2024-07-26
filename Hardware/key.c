#include "stm32f10x.h"                  // Device header
#include <stdint.h>
#include "systick.h"

static GPIO_InitTypeDef g_gpioList[] = {
    {GPIO_Pin_2, GPIO_Speed_10MHz, GPIO_Mode_IPU},
    {GPIO_Pin_3, GPIO_Speed_10MHz, GPIO_Mode_IPU},
    {GPIO_Pin_4, GPIO_Speed_10MHz, GPIO_Mode_IPU},
};

#define KEY_NUM_MAX (sizeof(g_gpioList) / sizeof(g_gpioList[0]))

void KeyDrvInit(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    for (uint8_t i; i < KEY_NUM_MAX; i++){
        GPIO_Init(GPIOA, &g_gpioList[i]);
    }
}

typedef enum{
    KEY_RELEASE = 0,
    KEY_CONFIRM,
    KEY_SHORTPRESS,
    KEY_LONGPRESS
} KEY_STATE;

#define CONFIRM_TIME 10
#define LONGPRESS_TIME 1000

typedef struct{
    KEY_STATE keyState;
    uint64_t lastOp;
} Key_Info_t;

static Key_Info_t g_keyInfo[KEY_NUM_MAX];

static uint8_t KeyScan(uint8_t keyIndex){

    uint64_t curSysTime;
    uint8_t keyPress;
    keyPress = GPIO_ReadInputDataBit(GPIOA, g_gpioList[keyIndex].GPIO_Pin);

    switch (g_keyInfo[keyIndex].keyState)
    {
        case KEY_RELEASE:
            if (!keyPress){
                g_keyInfo[keyIndex].keyState = KEY_CONFIRM;
                g_keyInfo[keyIndex].lastOp = GetSysRunTime();
            }
            break;
        case KEY_CONFIRM:
            if (!keyPress){
                curSysTime = GetSysRunTime();
                if ((curSysTime - g_keyInfo[keyIndex].lastOp) > CONFIRM_TIME){
                    g_keyInfo[keyIndex].keyState = KEY_SHORTPRESS;
                }
            }
            else{
                g_keyInfo[keyIndex].keyState = KEY_RELEASE;
            }
            break;
        case KEY_SHORTPRESS:
            if (keyPress){
                g_keyInfo[keyIndex].keyState = KEY_RELEASE;
                return (keyIndex + 1); // 0x1 0x2 0x3 .....
            }
            else{
                curSysTime = GetSysRunTime();
                if ((curSysTime - g_keyInfo[keyIndex].lastOp) > LONGPRESS_TIME){
                    g_keyInfo[keyIndex].keyState = KEY_LONGPRESS;
                }
            }
            break;
        case KEY_LONGPRESS:
            if (keyPress){
                g_keyInfo[keyIndex].keyState = KEY_RELEASE;
                return (keyIndex + 0x81); // 0x81 0x82 0x83 .....
            }
            break;
        default:
            g_keyInfo[keyIndex].keyState = KEY_RELEASE;
            break;
    }
    return 0;
}

uint8_t GetKeyVal(void){
    uint8_t res = 0;
    for (uint8_t i = 0; i < KEY_NUM_MAX; i++){
        res = KeyScan(i);
        if(res != 0){
            break;
        }
    }
    return res;
}
