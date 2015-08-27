#include <board.h>
#include <rtthread.h>
#include "nRF24L01.h"
#include "led.h"

static void app_2401_thread_entry(void* parameter)
{
	u8 status = RT_EOK;	
	u8 txbuf[TX_PLOAD_WIDTH]="yz01234567890123";

	//status = nRF24L01_init("nRF24L01");
	if(status == RT_EOK) 
	{
		while(1) {
			RF24L01_TX_Mode();			
			status = RF24L01_Tx_Dat(&txbuf[0]);						
			switch(status)
			{
				case MAX_RT:
					rt_kprintf("max rt \r\n");
					rt_thread_delay( RT_TICK_PER_SECOND*5);
					break;
				case ERROR:
					rt_kprintf("error \r\n");
					rt_thread_delay( RT_TICK_PER_SECOND*15);
					break;
				case TX_DS:
					rt_kprintf("success \r\n");
					rt_thread_delay(RT_TICK_PER_SECOND);
					break;  								
			}			   	       
		}			
	}

}

rt_err_t app2401Init(void)
{
	rt_thread_t tid;
	tid = rt_thread_create("2401",
												 app_2401_thread_entry, RT_NULL,
												 2048, 8, 20);
	if (tid != RT_NULL)
        rt_thread_startup(tid);
	
	return RT_EOK;
}

