#ifndef __18B20_H__
#define __18B20_H__
#include <stdint.h>

/*
uint8_t DS18B20_Reset(void);
void DS18B20_Write(uint8_t data);
uint8_t DS18B20_Read(void);
*/

void DS18B20_portInit(void);
uint8_t DS18B20_Reset(void);
void DS18B20_Write(uint8_t data);
uint8_t DS18B20_Read(void);
float ds18b20GetTemp(void);

#endif
