#include <rtthread.h>

#define		ComPort		"uart1"

//Short Commands
#define		Reset		0
#define 	Run			1
#define 	ID			2
#define 	XON			0x11
#define 	XOFF		0x13
#define 	IDLE    0xFF
#define 	PAUSE   0x04

//Long Commands
#define		setDivider								0x80
#define		setReadAndDelayCount			0x81
#define		setFlags									0x82

#define		TriggerMask								0xF3
#define		setTriggerMask						0xC0
#define		setTriggerValue						0xC1
#define		setTriggerConfiguration		0xC2


/* UART接收消息结构*/
struct rx_msg
{
	rt_device_t dev;
	rt_size_t size;
};

struct rx_msg msg;
/* 用于等待数据 */
static rt_sem_t sem = RT_NULL;
/* 接收线程的接收缓冲区*/
static unsigned char buffer[8];
static unsigned char deviceCmd[5];
static unsigned char bufferCounts = 0;

rt_bool_t isLongCMD = RT_FALSE;

void rt_appDeviceAnalyzer_thread_entry(void* parameter);

/* 数据到达回调函数*/
rt_err_t uartInput(rt_device_t dev, rt_size_t size)
{
	msg.dev = dev;
	msg.size = size;	
	
		/* 释放一次信号量 */
	rt_sem_release(sem);
	return RT_EOK;
}

rt_err_t initDevice(void) 
{
	rt_thread_t init_thread;
		
	init_thread = rt_thread_create("analyzer",
														 rt_appDeviceAnalyzer_thread_entry, RT_NULL,
														 2048, 7, 20);
	
	if (init_thread != RT_NULL) 
	{
		rt_thread_startup(init_thread);
		return RT_EOK;
	}	
	else
		return RT_ERROR;
	
}

rt_bool_t isShortCMD(rt_uint8_t cmd)
{
	if(!isLongCMD){
		if((cmd != 0) && ((cmd & 0x80) != 0x80))
		{
			return RT_TRUE;
		}
		else
		{
			isLongCMD = RT_TRUE;
			return RT_FALSE;
		}
	}
	else
		return RT_FALSE;
}

unsigned char parseCMD(unsigned char *cmd,rt_device_t uartDevice) 
{
	if(isShortCMD(cmd[0])) {// short cmd
		
		bufferCounts = 0;
		
		switch(cmd[0]) 
		{
			case ID:
				buffer[0] = '1';
				buffer[1] = 'A';
				buffer[2] = 'L';
				buffer[3] = 'S';
				buffer[4] = 0;
				rt_device_write(uartDevice,0,buffer,4);
				rt_kprintf("return Device ID:%X%X%X%X..\n\r",buffer[3],buffer[2],buffer[1],buffer[0]);
				break;
			case Run:
				rt_kprintf("Device run..\n\r");
				break;
			case XON:
				rt_kprintf("Device XON..\n\r");
				break;
			case XOFF:
				rt_kprintf("Device XOFF..\n\r");
				break;
			case PAUSE:
				rt_kprintf("Device PAUSE..\n\r");
				break;
			case IDLE:
				rt_kprintf("Device IDLE..\n\r");
				break;
		}
	}
	else// long cmd include Reset, until there are 5 cmd
	{
		if(bufferCounts == 5) {
			isLongCMD = RT_FALSE;
			bufferCounts = 0;
			
			if((cmd[0] & setTriggerMask) != setTriggerMask)
			{
				rt_uint16_t readCount = 0,delayCount = 0;
				rt_uint8_t * sendDataBuffer;
				switch(cmd[0]) 
				{
					case Reset:
						rt_kprintf("Device reset..\n\r");
						break;
					case setDivider:
						rt_kprintf("Device setDivider:%X%X%X%X..\n\r",cmd[4],cmd[3],cmd[2],cmd[1]);
						break;		
					case setReadAndDelayCount:
						readCount |= cmd[1];
						readCount |= cmd[2]<< 8;
						delayCount |= cmd[3];
						delayCount |= cmd[4]<< 8;
					
						sendDataBuffer = rt_calloc(readCount,sizeof(rt_uint8_t));
					
						for(;readCount <= 0; readCount--)
								sendDataBuffer[readCount] = readCount;
						
						rt_device_write(uartDevice,0,sendDataBuffer,readCount);
						
						rt_kprintf("cmd:%X%X%X%X\n\r",cmd[1],cmd[2],cmd[3],cmd[4]);
						rt_kprintf("set Device ReadCount:0x%04X,DelayCount:0x%04X\n\r",readCount,delayCount);
						break;	
					case setFlags:
						rt_kprintf("Device setFlags:%X%X%X%X..\n\r",cmd[4],cmd[3],cmd[2],cmd[1]);
						break;	
				}		
			}
			else
			{
				switch(cmd[0]&TriggerMask)
				{
					case setTriggerMask:
						rt_kprintf("Device setTriggerMask:%X%X%X%X..\n\r",cmd[4],cmd[3],cmd[2],cmd[1]);
						break;
					case setTriggerValue:
						rt_kprintf("Device setTriggerValue:%X%X%X%X..\n\r",cmd[4],cmd[3],cmd[2],cmd[1]);
						break;
					case setTriggerConfiguration:
						rt_kprintf("Device setTriggerConfiguration:%X%X%X%X..\n\r",cmd[4],cmd[3],cmd[2],cmd[1]);
						break;
				}		
			}
		}
		else if(bufferCounts > 5) 
		{
			isLongCMD = RT_FALSE;
			bufferCounts = 0;
		}	
	}
}

void rt_appDeviceAnalyzer_thread_entry(void* parameter) 
{
	rt_device_t device;
	rt_err_t result = RT_EOK;
	
		/* 创建一个信号量，初始值是0 */
	sem = rt_sem_create("analyzerSem", 0, RT_IPC_FLAG_FIFO);
	
	/* 查找系统中的串口1设备*/
	device = rt_device_find(ComPort);
	
	if (device!= RT_NULL)
	{
		/* 设置回调函数及打开设备*/
		rt_device_set_rx_indicate(device, uartInput);
		rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
	}
	else
	{
		rt_kprintf("can't find the %s\n\r",ComPort);
		return;
	}
		
	while (1)
	{
		result = rt_sem_take(sem,RT_WAITING_FOREVER);
		if (result == -RT_ETIMEOUT)
		{
			/* 接收超时*/
			rt_kprintf("take analyzer sem timeout !\n\r");
		}
				/* 成功收到消息*/
		if (result == RT_EOK)
		{
			rt_uint32_t rx_length;
			rt_uint8_t i;
			rx_length = (sizeof(buffer) - 1) > msg.size ? msg.size : sizeof(buffer) - 1;
			/* 读取消息*/
			rx_length = rt_device_read(msg.dev, 0, &buffer[0],rx_length);
						
			if(rx_length != 0) 
			{
//				rt_kprintf("recv cmd length is %d\n\r",rx_length);
//				rt_kprintf("recv cmd Data is : ");
//				for(i = 0; i < rx_length; i++)
//				{
//					rt_kprintf(" %X ",buffer[i]);
//				}
//				rt_kprintf("\n\r");		
				
				for(i = bufferCounts; i < rx_length; i++)
				{
					deviceCmd[i] = buffer[i - bufferCounts];
				}
				bufferCounts += rx_length;
//				if(bufferCounts == 5)
//					__nop();
				
				parseCMD(&deviceCmd[0],device);
			}
		}
	}
}
