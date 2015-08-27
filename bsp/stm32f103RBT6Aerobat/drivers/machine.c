#include <stm32f10x.h>
#include "machine.h"

/* Private variables ---------------------------------------------------------*/

//#define TimerPeriod (24000000/15600)
//#define TimerPeriod (24000000/64000)
#define TimerPeriod 	375

//uint16_t CCR1_Val = TimerPeriod * 0.08;
//uint16_t CCR2_Val = TimerPeriod * 0.08;
//uint16_t CCR3_Val = TimerPeriod * 0.08;
//uint16_t CCR4_Val = TimerPeriod * 0.08;

uint16_t CCR1_Val = 0;
uint16_t CCR2_Val = 0;
uint16_t CCR3_Val = 0;
uint16_t CCR4_Val = 0;

uint16_t PrescalerValue = 0;

void rt_hw_machine_init(void) {

	/* System Clocks Configuration */
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	/* GPIO Configuration */
	{
		GPIO_InitTypeDef GPIO_InitStructure;	
		  /*GPIOA Configuration: TIM2 channel1, 2, 3 and 4 */
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(GPIOA, &GPIO_InitStructure);
			
	}
	
	//TIM2 Configuration
	{
				/* -----------------------------------------------------------------------
			TIM2 Configuration: generate 4 PWM signals with 4 different duty cycles:
			The TIM2CLK frequency is set to SystemCoreClock (Hz), to get TIM2 counter
			clock at 24 MHz the Prescaler is computed as following:
			 - Prescaler = (TIM2CLK / TIM2 counter clock) - 1
			SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
			and Connectivity line devices and to 24 MHz for Low-Density Value line and
			Medium-Density Value line devices

			The TIM2 is running at 36 KHz: TIM2 Frequency = TIM2 counter clock/(ARR + 1)
																										= 24 MHz / 666 = 36 KHz
			TIM2 Channel1 duty cycle = (TIM2_CCR1/ TIM2_ARR)* 100 = 50%
			TIM2 Channel2 duty cycle = (TIM2_CCR2/ TIM2_ARR)* 100 = 37.5%
			TIM2 Channel3 duty cycle = (TIM2_CCR3/ TIM2_ARR)* 100 = 25%
			TIM2 Channel4 duty cycle = (TIM2_CCR4/ TIM2_ARR)* 100 = 12.5%
		----------------------------------------------------------------------- */
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		
		/* Compute the prescaler value */
		PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
		//PrescalerValue = (uint16_t) (SystemCoreClock / 2400) - 1;
		/* Time base configuration */
		TIM_TimeBaseStructure.TIM_Period = TimerPeriod - 1;
		//TIM_TimeBaseStructure.TIM_Period = 2400;
		TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

		/* PWM1 Mode configuration: Channel1 */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC1Init(TIM2, &TIM_OCInitStructure);

		TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);

		/* PWM1 Mode configuration: Channel2 */
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

		TIM_OC2Init(TIM2, &TIM_OCInitStructure);

		TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

		/* PWM1 Mode configuration: Channel3 */
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = CCR3_Val;

		TIM_OC3Init(TIM2, &TIM_OCInitStructure);

		TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

		/* PWM1 Mode configuration: Channel4 */
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = CCR4_Val;

		TIM_OC4Init(TIM2, &TIM_OCInitStructure);

		TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

		TIM_ARRPreloadConfig(TIM2, ENABLE);

		/* TIM2 enable counter */
		TIM_Cmd(TIM2, ENABLE);
	}
	
}

void rt_hw_setMathine1PWM(rt_uint8_t pwm)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	CCR1_Val = pwm / 100.0 * TimerPeriod;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	
	
	rt_kprintf("set machine 1 pwm = %d\n",pwm);
}
void rt_hw_setMathine2PWM(rt_uint8_t pwm)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	CCR2_Val = pwm / 100.0 * TimerPeriod;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	rt_kprintf("set machine 2 pwm = %d\n",pwm);
}
void rt_hw_setMathine3PWM(rt_uint8_t pwm)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	CCR3_Val = pwm / 100.0 * TimerPeriod;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	
	
	rt_kprintf("set machine 3 pwm = %d\n",pwm);
}
void rt_hw_setMathine4PWM(rt_uint8_t pwm)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	CCR4_Val = pwm / 100.0 * TimerPeriod;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC4Init(TIM2, &TIM_OCInitStructure);
		
	rt_kprintf("set machine 4 pwm = %d\n",pwm);
}
