#include "stm32l4xx.h"
#include "notes.h"
/*
 *
 * 	pwmTune
 * 	T.Lloyd 2017
 * 	Plays a basic tune using the PWM functions
 *
 */

volatile char length=8;
volatile int tempo=80;
volatile char i;
//c major scale frequencies
int freq[8]={c5,d5,e5,f5,g5,a5,b5,c6};
//note lengths
char value[8]={2,2,2,2,2,2,2,1};


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
void updateFreq(int freq){
	TIM1->ARR=1000000/(freq);
}
void updateValue(char val){
	TIM2->ARR=1000000/(tempo/(60/val));
}
void TIM2_IRQHandler(void){
	//acknowledge interrupt
	TIM2->SR&=~(TIM_SR_UIF);
	//used to debug interrupt speed
	toggleB(3);
	//update pwm frequency
	updateFreq(freq[i]);
	//update note length
	updateValue(value[i++]);
	//move to next note
	i=i%length;
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
	TIM1->ARR=1000000/1000;
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
	//main output enable
	TIM1->BDTR |= TIM_BDTR_MOE;
	//start
	//TIM1->CR1|=(TIM_CR1_CEN);
}

void timerSetup(){
	//enable timer 2 clock
	RCC->APB1ENR1|=RCC_APB1ENR1_TIM2EN;
	//Set interrupt priority
	NVIC_SetPriority(TIM2_IRQn,0);
	//Enable interrupt
	NVIC_EnableIRQ(TIM2_IRQn);
	//Frequency
	TIM2->ARR=1000000/(tempo/60);
	//prescalar
	TIM2->PSC=SystemCoreClock/1000000 -1;
	//enable interrupts
	TIM2->DIER|=TIM_DIER_UIE;
}
void start(){
	//start pwm
	TIM1->CR1|=(TIM_CR1_CEN);
	//start timer
	TIM2->CR1|=TIM_CR1_CEN;
}
int main(void){
	i=0;
	//pin setup
	pinSetup();
	//pwm setup
	pwmSetup();
	//timer setup
	timerSetup();
	//start playback
	start();
	while(1);
}
