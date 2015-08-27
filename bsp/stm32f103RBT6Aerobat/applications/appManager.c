#include <board.h>
#include <rtthread.h>
#include "nRF24L01.h"
#include "CommunicationProtocol.h"

/* 用于发送数据的消息队列*/
rt_mq_t protocalData_mq;
extern rt_mq_t machineData_mq;
extern rt_mq_t ledData_mq;

static uint8_t buff[TX_PLOAD_WIDTH * 2];

void rt_appManager_thread_entry(void* parameter);

rt_err_t appManagerInit(void){
	rt_thread_t init_thread;
	
	init_thread = rt_thread_create("appManager",
														 rt_appManager_thread_entry, RT_NULL,
														 2048, 6, 20);
	if (init_thread != RT_NULL) {
		rt_thread_startup(init_thread);
		return RT_EOK;
	}	else
		return RT_ERROR;
}

void rt_appManager_thread_entry(void* parameter) {
	
	rt_err_t result = RT_EOK;
	CommunicationProtocol *cp;
		/* 创建一个信号量，初始值是0 */	
	protocalData_mq = rt_mq_create("protocalMQ",TX_PLOAD_WIDTH,2,RT_IPC_FLAG_FIFO);
	
	while (1)
	{
		result = rt_mq_recv(protocalData_mq, &buff[0], TX_PLOAD_WIDTH, RT_WAITING_FOREVER);
		if(result != RT_EOK) {
			rt_kprintf("recv mq error !");
			return;
		}
		
		//deal msg from nRF2401
		cp = (CommunicationProtocol *)buff;
		if(cp->authentication	== 0x7a79 && cp->type == 1) //yz
		{
			//这里应该检测CRC16
				
			//电机,采用写信的方式吧。
			rt_mq_send(machineData_mq,&cp->m1pwm,4);
			//LED,采用写信的方式吧。
			rt_mq_send(ledData_mq,&cp->LEDS,1);
		}
	}
}
