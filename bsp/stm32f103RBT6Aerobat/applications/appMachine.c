#include <rtthread.h>
#include "machine.h"

rt_mq_t machineData_mq;
static rt_uint8_t buff[4];

void rt_appMachine_thread_entry(void* parameter);

rt_err_t appMachineInit(void){
	rt_err_t status = RT_EOK;
	rt_thread_t init_thread;
	
	rt_hw_machine_init();
	
	init_thread = rt_thread_create("appMachine",
													 rt_appMachine_thread_entry, RT_NULL,
													 2048, 8, 20);
	
	if (init_thread != RT_NULL)
		rt_thread_startup(init_thread);
	
	return status;
}

void rt_appMachine_thread_entry(void* parameter) {
	
	machineData_mq = rt_mq_create("machineMQ",4,2,RT_IPC_FLAG_FIFO);
	
	while(1) {
		if(rt_mq_recv(machineData_mq,&buff[0],4,RT_WAITING_FOREVER) == RT_EOK) 
		{
			if(buff[0] <= 100)
			{
				rt_hw_setMathine1PWM(buff[0]);
			}
			if(buff[1] <= 100)
			{
				rt_hw_setMathine2PWM(buff[1]);
			}
			if(buff[2] <= 100)			
			{
				rt_hw_setMathine3PWM(buff[2]);
			}
			if(buff[3] <= 100)	
			{
				rt_hw_setMathine4PWM(buff[3]);
			}
		}
		//rt_thread_delay(RT_TICK_PER_SECOND * 100);
	}
}
