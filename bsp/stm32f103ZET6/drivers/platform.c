#include <rtthread.h>
#include "board.h"

#ifdef RT_USING_SPI
#include "rt_stm32f10x_spi.h"

#ifdef RT_USING_NRF24L01
		#include "nRF24L01.h"
#endif /* RT_USING_NRF24L01 */	

static void rt_hw_spi2_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); 
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 				

				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_Init(GPIOB, &GPIO_InitStructure);
			
        stm32_spi_register(SPI2, &stm32_spi, "spi2");
    }
#ifdef RT_USING_NRF24L01
    /* attach nRF24L01 cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

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
#endif /* RT_USING_NRF24L01 */	

#ifdef RT_USING_SST25VFXX
    /* attach sst25vfxx cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
				GPIO_Init(GPIOB, &GPIO_InitStructure);

        spi_cs.GPIOx = GPIOB;
        spi_cs.GPIO_Pin = GPIO_Pin_12;

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "sst25vfxx", "spi2", (void*)&spi_cs);
    }
#endif /* RT_USING_NRF24L01 */
}
#endif /* RT_USING_SPI */

void rt_platform_init(void)
{
	
#ifdef RT_USING_SPI
    rt_hw_spi2_init();
#endif /* RT_USING_SPI */
	
    /* initilize key module */
    rt_hw_key_init();
	
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)		
		//rt_hw_sst25vfxx_init("sst25vfxx");
		sst25vfxx_init("spi0","sst25vfxx");
#endif  /* RT_USING_DFS */

#ifdef RT_USING_NRF24L01
		rt_hw_2401_init();
#endif /* RT_USING_NRF24L01 */	
}

