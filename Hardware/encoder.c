#include "stm32f10x.h"                  // Device header

int8_t encoderValue = 0;

void encoderInit(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef encoderPin;
	encoderPin.GPIO_Mode = GPIO_Mode_IPU;
	encoderPin.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	encoderPin.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &encoderPin);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	
	EXTI_InitTypeDef encoderEXTI;
	encoderEXTI.EXTI_Line = EXTI_Line1;
	encoderEXTI.EXTI_LineCmd = ENABLE;
	encoderEXTI.EXTI_Mode = EXTI_Mode_Interrupt;
	encoderEXTI.EXTI_Trigger = EXTI_Trigger_Falling;
	
	EXTI_Init(&encoderEXTI);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef encoderNVIC;
	encoderNVIC.NVIC_IRQChannel = EXTI1_IRQn;
	encoderNVIC.NVIC_IRQChannelCmd = ENABLE;
	encoderNVIC.NVIC_IRQChannelPreemptionPriority = 1;
	encoderNVIC.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&encoderNVIC);
	
}


void EXTI1_IRQHandler(void){
	
	if(EXTI_GetITStatus(EXTI_Line1) == SET){
		
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 1){
			encoderValue++;
		}
		else{
			encoderValue--;
		}

		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

int8_t getEncoderValue(void){
	
	int8_t dataBuffer = encoderValue;
	encoderValue = 0;
	return dataBuffer;
	
}
