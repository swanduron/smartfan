#include "stm32f10x.h"                  // Device header

int8_t encoderDirection = 0;

void encoderInit(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef encoderPin;
	encoderPin.GPIO_Mode = GPIO_Mode_IPU;
	encoderPin.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	encoderPin.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &encoderPin);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	
	EXTI_InitTypeDef encoderEXTI;
	encoderEXTI.EXTI_Line = EXTI_Line0 | EXTI_Line1;
	encoderEXTI.EXTI_LineCmd = ENABLE;
	encoderEXTI.EXTI_Mode = EXTI_Mode_Interrupt;
	encoderEXTI.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&encoderEXTI);
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef encoderNVIC;
	encoderNVIC.NVIC_IRQChannel = EXTI0_IRQn;
	encoderNVIC.NVIC_IRQChannelCmd = ENABLE;
	encoderNVIC.NVIC_IRQChannelPreemptionPriority = 1;
	encoderNVIC.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&encoderNVIC);
	
	encoderNVIC.NVIC_IRQChannel = EXTI1_IRQn;
	encoderNVIC.NVIC_IRQChannelCmd = ENABLE;
	encoderNVIC.NVIC_IRQChannelPreemptionPriority = 1;
	encoderNVIC.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&encoderNVIC);
	
}

void EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0) == SET){
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0){
			encoderDirection--;
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void EXTI1_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line1) == SET){
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0){
			encoderDirection++;
		}
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

int8_t getEncoderValue(void){
	int8_t currentValue = encoderDirection;
	encoderDirection = 0;
	return currentValue;
}
