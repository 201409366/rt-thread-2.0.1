#include <rtthread.h>
#include "board.h"

#ifdef RT_USING_SPI
#include "rt_stm32f10x_spi.h"
static int rt_hw_spi_init(void)
{
    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        GPIO_InitTypeDef GPIO_InitStructure;
			
			  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
			
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
				GPIO_Init(GPIOA, &GPIO_InitStructure);  
			
        stm32_spi_register(SPI1, &stm32_spi, "spi1");
    }
		return RT_EOK;
}
INIT_BOARD_EXPORT(rt_hw_spi_init);
#endif /* RT_USING_SPI */

	
#ifdef 	USING_NRF24L01
	#include "nRF24L01.h"
static	int _nRF24L01_init(void)
{
		extern rt_hw_nRF24L01_init(const char * name);
		/* attach nRF24L01 cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
				GPIO_Init(GPIOA, &GPIO_InitStructure);

        spi_cs.GPIOx = GPIOA;
        spi_cs.GPIO_Pin = GPIO_Pin_4;

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "nRF24L01", "spi1", (void*)&spi_cs);
    }		
		rt_hw_nRF24L01_init("nRF24L01");
		return RT_EOK;
}
	
INIT_DEVICE_EXPORT(_nRF24L01_init);
#endif /* RT_USING_SPI */

#ifdef 	RT_USING_ENC28J60
static	int _enc28j60_init(void)
{
		extern rt_err_t enc28j60_attach(const char * spi_device_name);
		/* attach nRF24L01 cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;
				
        GPIO_InitTypeDef GPIO_InitStructure;
			
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
			
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
				GPIO_Init(GPIOC, &GPIO_InitStructure);

        spi_cs.GPIOx = GPIOC;
        spi_cs.GPIO_Pin = GPIO_Pin_5;

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "enc_spi", "spi1", (void*)&spi_cs);
    }
		{ //config PA8  as the Interrupt pin of enc28j60
			NVIC_InitTypeDef NVIC_InitStructure;
			EXTI_InitTypeDef EXTI_InitStructure;

			/* Configure one bit for preemption priority */
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
			GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
	    /* Configure ENC28J60 EXTI Line to generate an interrupt on falling edge */
	    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	    EXTI_Init(&EXTI_InitStructure);
	    /* Enable the EXTI0 Interrupt */
	    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	    NVIC_Init(&NVIC_InitStructure);
			
			/* Clear the EXTI line pending bit */
			EXTI_ClearITPendingBit(EXTI_Line8);
		}
		{
			GPIO_InitTypeDef GPIO_InitStruct;
			//¸´Î»
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
			GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
			GPIO_Init(GPIOC,&GPIO_InitStruct);
			/* reset */
			GPIO_ResetBits(GPIOC,GPIO_Pin_4);
			rt_thread_delay(RT_TICK_PER_SECOND/100);
			/* release reset */
			GPIO_SetBits(GPIOC,GPIO_Pin_4);
		}
		enc28j60_attach("enc_spi");
		return RT_EOK;
}
	
INIT_DEVICE_EXPORT(_enc28j60_init);
#endif /* RT_USING_ENC28J60 */
