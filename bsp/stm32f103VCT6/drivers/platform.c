#include <rtthread.h>
#include "board.h"

#ifdef RT_USING_RTC

#endif /* RT_USING_RTC */

#ifdef RT_USING_SPI
#include "rt_stm32f10x_spi.h"
#include "nRF24L01.h"

/*
SPI2_MOSI: PB15
SPI2_MISO: PB14
SPI2_SCK : PB13

CS0: PG10  SPI FLASH
CS1: PB12  TOUCH
CS2: PG7   WIFI
*/
static void rt_hw_spi1_init(void)
{
    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        GPIO_InitTypeDef GPIO_InitStructure;
			
			  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
			
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
				GPIO_Init(GPIOA, &GPIO_InitStructure);  
			
        stm32_spi_register(SPI1, &stm32_spi, "spi1");
    }

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
}
#endif /* RT_USING_SPI */

void rt_platform_init(void)
{
#ifdef RT_USING_SPI
    rt_hw_spi1_init();
		rt_hw_nRF24L01_init("nRF24L01");
#endif /* RT_USING_SPI */
	
#ifdef RT_USING_DFS
   // w25qxx_init("flash0", "spi20");
#endif /* RT_USING_DFS */

#ifdef RT_USING_RTGUI
    /* initilize touch panel */
    //rtgui_touch_hw_init("spi21");
#endif /* RT_USING_RTGUI */


#ifdef RT_USING_USB_HOST
    /* register stm32 usb host controller driver */
   // rt_hw_susb_init();
#endif

#ifdef RT_USING_DFS
    /* initilize sd card */
#ifdef RT_USING_SDIO
   // rt_mmcsd_core_init();
  //  rt_mmcsd_blk_init();
   // stm32f4xx_sdio_init();
  //  rt_thread_delay(RT_TICK_PER_SECOND);
#else
   // rt_hw_sdcard_init();
#endif
#endif /* RT_USING_DFS */

#ifdef RT_USING_RTGUI
    /* initilize ra8875 lcd controller */
  //  ra8875_init();

    /* initilize key module */
  //  rt_hw_key_init();
#endif /* RT_USING_RTGUI */

#ifdef RT_USING_RTC
  //rt_hw_rtc_init();
#endif /* RT_USING_RTC */

}

