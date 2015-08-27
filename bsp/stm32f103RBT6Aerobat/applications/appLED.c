#include "led.h"

rt_mq_t ledData_mq;
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 512 ];
static struct rt_thread led_thread;
static void led_thread_entry(void* parameter)
{
		rt_uint8_t ledStatus;
	
    rt_hw_led_init();		
		ledData_mq = rt_mq_create("LEDMQ",1,2,RT_IPC_FLAG_FIFO);
		
    while (1)
    {
				if(rt_mq_recv(ledData_mq,&ledStatus,1,RT_WAITING_FOREVER) == RT_EOK)
				{
					if(ledStatus &0x10)
					{
						rt_hw_led_on(4);											
					}
					else
					{
						rt_hw_led_off(4);			
					}
					
					if(ledStatus &0x08)
					{
						rt_hw_led_on(3);											
					}
					else
					{
						rt_hw_led_off(3);			
					}
					
					if(ledStatus &0x04)
					{
						rt_hw_led_on(2);											
					}
					else
					{
						rt_hw_led_off(2);			
					}
					if(ledStatus &0x2)
					{
						rt_hw_led_on(1);											
					}
					else
					{
						rt_hw_led_off(1);			
					}
					if(ledStatus &0x01)
					{
						rt_hw_led_on(0);											
					}
					else
					{
						rt_hw_led_off(0);			
					}					
				}        			
    }
}

rt_err_t appLEDInit(void){	
  
    rt_err_t result;
	
   /* init led thread */
    result = rt_thread_init(&led_thread,
                            "appLED",
                            led_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&led_stack[0],
                            sizeof(led_stack),
                            20,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&led_thread);
				return RT_EOK;
    }
		
		return RT_ERROR;
}
