#include "stm32f10x.h"                  // Device header

void icInit(void){
	
	//TIM3 CH1 ->PA6
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  // Set the GPIO
	GPIO_InitTypeDef signalInput;
	signalInput.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	signalInput.GPIO_Pin = GPIO_Pin_6;
	signalInput.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &signalInput);
	
  // Set the time base
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef pwmTB;
	pwmTB.TIM_ClockDivision = TIM_CKD_DIV1;
	pwmTB.TIM_CounterMode = TIM_CounterMode_Up;
	pwmTB.TIM_Period = 65536 - 1;   // ARR, big ARR can avoid the overflow, the tested freq need to higher 15Hz
	pwmTB.TIM_Prescaler = 72 - 1;  // set fc 1M
	pwmTB.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &pwmTB);
	
	// Set the IC
	TIM_ICInitTypeDef signalIC;
	signalIC.TIM_Channel = TIM_Channel_1;
	signalIC.TIM_ICFilter = 0xF;
	signalIC.TIM_ICPolarity = TIM_ICPolarity_Rising;
	signalIC.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// 下面的句子确定了输入信号是直连通道TI1FP1还是交叉通道TI2FP1
	signalIC.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM3, &signalIC);
	

  // Set the slave operation
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset); // clear CNT

	
	TIM_Cmd(TIM3, ENABLE);
}

uint32_t IC_GetFreq(void){
	return 1000000/TIM_GetCapture1(TIM3); // fc/counter
}
