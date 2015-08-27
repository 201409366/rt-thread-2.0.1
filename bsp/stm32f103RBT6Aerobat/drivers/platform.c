#include <rtthread.h>
#include "board.h"
#include "drv_mpu6050.h"

#ifdef RT_USING_SPI
#include "rt_stm32f10x_spi.h"

#ifdef RT_USING_NRF24L01
		#include "nRF24L01.h"
#endif /* RT_USING_NRF24L01 */	

static void rt_hw_spi_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 				

				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_Init(GPIOA, &GPIO_InitStructure);
			
        stm32_spi_register(SPI1, &stm32_spi, "spi1");
    }
		
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
			
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 				
			
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

#ifdef RT_USING_W25QXX
    /* attach W25QXX cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
				GPIO_Init(GPIOA, &GPIO_InitStructure);

        spi_cs.GPIOx = GPIOA;
        spi_cs.GPIO_Pin = GPIO_Pin_2;

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "w25qxx", "spi1", (void*)&spi_cs);
    }
#endif /* RT_USING_W25QXX */
		
		
}
#endif /* RT_USING_SPI */

void rt_platform_init(void)
{
	
#ifdef RT_USING_SPI
    rt_hw_spi_init();
#endif /* RT_USING_SPI */
	
    /* initilize key module */
    //rt_hw_key_init();
	
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)		
		//rt_hw_sst25vfxx_init("sst25vfxx");
		w25qxx_init("spi0","w25qxx");
#endif  /* RT_USING_DFS */

#ifdef RT_USING_NRF24L01
		rt_hw_nRF24L01_init("nRF24L01");
#endif /* RT_USING_NRF24L01 */	
	
#ifdef RT_USING_I2C
		//rt_hw_i2c_init();
	//	rt_hw_mpu6050_init("i2c1",0xD0);
#endif /* RT_USING_I2C */	
		
}

