#include "stm32f10x.h"                  // Device header

void pwmInit(void){
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/*Remap Pin function. Shift the output pin from PA0 to PA15
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //release PA15,PB3,PB4 as GPIO, not JTAG
	*/
	
	// init GPIO to pwm output
	GPIO_InitTypeDef pwmPin;
	pwmPin.GPIO_Mode = GPIO_Mode_AF_PP;
	pwmPin.GPIO_Pin = GPIO_Pin_0; //GPIO_Pin_15
	pwmPin.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &pwmPin);
	
	// set the time base unit
  // 1KHz 50% duty
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef pwmTimer;
	pwmTimer.TIM_ClockDivision = TIM_CKD_DIV1;
	pwmTimer.TIM_CounterMode = TIM_CounterMode_Up;
	pwmTimer.TIM_Period = 100 - 1;  //ARR
	pwmTimer.TIM_Prescaler = 720 - 1; //72M/100/720 = 1000
	pwmTimer.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2,&pwmTimer);
	// Set the OC function. PWM function called OC
	TIM_OCInitTypeDef pwmOC;
	TIM_OCStructInit(&pwmOC);  // set default value for unused var
	pwmOC.TIM_OCMode = TIM_OCMode_PWM1;
	pwmOC.TIM_OCPolarity = TIM_OCPolarity_High;  // 有效电平是高还是低
	pwmOC.TIM_OutputState = TIM_OutputState_Enable;
	pwmOC.TIM_Pulse = 50;  // CCR
	
	TIM_OC1Init(TIM2, &pwmOC);
	
	TIM_Cmd(TIM2, ENABLE);
	
}

void PWM_SetCompare1(uint16_t Compare){
	TIM_SetCompare1(TIM2, Compare);
}
