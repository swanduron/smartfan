#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "systick.h"

#define SENSOR_CONVERSE_TIME 950

void DS18B20_portInit(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef sensorPin;
    sensorPin.GPIO_Mode = GPIO_Mode_Out_OD;
    sensorPin.GPIO_Pin = GPIO_Pin_7;
    sensorPin.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &sensorPin);

}

uint8_t DS18B20_Reset(void){
    uint8_t sensorExist = 1;

    GPIO_WriteBit(GPIOA, GPIO_Pin_7, Bit_RESET);
    Delay_us(480); // 480~960
    GPIO_WriteBit(GPIOA, GPIO_Pin_7, Bit_SET);
    Delay_us(80); // maximum delay 60

    sensorExist = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
    Delay_us(400);  // 400 + 80

    return sensorExist ? 0 : 1; // if 18b20 pull the bus, it returns "exists" or 1, else 0
}

void DS18B20_Write(uint8_t data){

    for (uint8_t i = 0; i < 8; i ++){
        GPIO_WriteBit(GPIOA, GPIO_Pin_7, Bit_RESET);
        Delay_us(2);
        GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction) ((data >> i) & 0x01));
        Delay_us(60);
        GPIO_WriteBit(GPIOA, GPIO_Pin_7, Bit_SET); // release the bus
    }
}

uint8_t DS18B20_Read(void){
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i ++){
        data = data >> 1;

        GPIO_WriteBit(GPIOA, GPIO_Pin_7, Bit_RESET); // pull the bus
        Delay_us(2);
        GPIO_WriteBit(GPIOA, GPIO_Pin_7, Bit_SET);

        uint8_t dataBuffer = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
        if (dataBuffer){
            data = data | 0x80;
        }
        Delay_us(60);

        GPIO_WriteBit(GPIOA, GPIO_Pin_7, Bit_SET);
    }
    return data;
}


typedef enum {
    SENSOR_RELEASE = 0,
    SENSOR_BUSY,
} SENSOR_STATE;

typedef struct {
    SENSOR_STATE sensorState;
    uint64_t lastOP;
} Sensor_Info_t;

static Sensor_Info_t SensorInfo = {0};

float ds18b20GetTemp(void){
    uint8_t tempL;
    uint16_t tempH;
    uint64_t curSysTime;
    float result;

    switch (SensorInfo.sensorState){
        case SENSOR_RELEASE:
            DS18B20_Reset();
            DS18B20_Write(0xCC);
            DS18B20_Write(0x44);
            SensorInfo.sensorState = SENSOR_BUSY;
            SystickInit();
            SensorInfo.lastOP = GetSysRunTime();
            break;
        case SENSOR_BUSY:
            curSysTime = GetSysRunTime();
            if (curSysTime - SensorInfo.lastOP > SENSOR_CONVERSE_TIME){
                DS18B20_Reset();
                DS18B20_Write(0xCC);
                DS18B20_Write(0xBE);
                tempL = DS18B20_Read();
                tempH = DS18B20_Read();
                SystickInit();
                tempH = (tempH << 8) | tempL;
                if ((tempH & 0xF800) == 0xF800){
                    tempH = ~tempH + 1;
                }
                result = tempH * 0.0625;
                SensorInfo.sensorState = SENSOR_RELEASE;
                return result;
            }
            break;
        default:
            SensorInfo.sensorState = SENSOR_RELEASE;
            break;
    }
    return 9999.0f;
}


