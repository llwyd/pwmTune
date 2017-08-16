#include "stm32l4xx.h"
/*
 *
 * 	pwmTune
 * 	T.Lloyd 2017
 * 	Plays a basic tune using the PWM functions
 *
 */
void pinSetup(void){
	//GPIO Clock
	RCC->AHB2ENR|=(1<<1);
	//Configure Pin PB3 (on board LED) for output
	GPIOB->MODER |=(1<<6);
	GPIOB->MODER &=~(1<<7);
}

void toggleB(char pin){
	GPIOB->ODR ^= (1<<pin);
}
void TIM1_IRQHandler(void){
	TIM1->SR&=~(TIM_SR_UIF);
}

void pwmSetup(){
	//enable Timer 1
	RCC->APB2ENR|=RCC_APB2ENR_TIM1EN;
	//enable gpio A
	RCC->AHB2ENR|=RCC_AHB2ENR_GPIOAEN;
	//Set PA8 for alternative function mode (TIM1_CH1)
	GPIOA->MODER|=GPIO_MODER_MODER8_1;
	GPIOA->MODER&=~GPIO_MODER_MODER8_0;
	//Set which alternative function to use (AF1)
	GPIOA->AFR[1]|=(1<<0);


	//Timer configuration
	//Set period
	TIM1->ARR=250;
	//set prescalar
	TIM1->PSC=SystemCoreClock/1000000 -1;
	//Enable PWM
	TIM1->CCMR1|=TIM_CCMR1_OC1M_1|TIM_CCMR1_OC1M_2;
	//Enable Preload
	TIM1->CCMR1|=TIM_CCMR1_OC1PE;
	//Auto reload
	TIM1->CR1|=TIM_CR1_ARPE;
	//Event generation register
	TIM1->EGR|=TIM_EGR_UG;
	//Set duty cycle
	TIM1->CCR1=TIM1->ARR/2;
	//active high
	TIM1->CCER&=~(TIM_CCER_CC1P);
	//output enable
	TIM1->CCER|=TIM_CCER_CC1E;
	//TIM1->DIER|= TIM_DIER_UIE;
	//main output enable
	TIM1->BDTR |= TIM_BDTR_MOE;
	//start
	TIM1->CR1|=(TIM_CR1_CEN);
}

int main(void){
	pinSetup();
	pwmSetup();
	while(1);
}
