/**************************** nRF24L01.c *************************************
暂时不知道把这个东西做成SPI设备吧

不知道把他归到哪一类设备中。

**********************************************************************************/
#include "nRF24L01.h"
#include <drivers/spi.h>

#define FLASH_DEBUG

#ifdef FLASH_DEBUG
#define FLASH_TRACE         rt_kprintf
#else
#define FLASH_TRACE(...)
#endif /* #ifdef FLASH_DEBUG */

static struct rt_spi_device * rt_spi_device;
static struct rt_mutex lock;

u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01}; // 定义一个静态发送地址
u8 RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};

/*
 * 函数名：SPI_RF24L01_RW
 * 描述  ：用于向RF24L01读/写一字节数据
 * 输入  ：写入的数据
 * 输出  ：读取得的数据
 * 调用  ：内部调用
 */
u8 SPI_RF24L01_RW(u8 dat)
{  	
//   /* 当 SPI发送缓冲器非空时等待 */
//  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//  
//   /* 通过 SPI2发送一字节数据 */
//  SPI_I2S_SendData(SPI1, dat);		
// 
//   /* 当SPI接收缓冲器为空时等待 */
//  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
	u8 val;
	
	if(rt_spi_device != RT_NULL) 
		val = rt_spi_sendrecv8(rt_spi_device,dat);
	else
		val = 0;
	
  return val;
}

/*
 * 函数名：SPI_RF24L01_WriteReg
 * 描述  ：用于向RF24L01特定的寄存器写入数据
 * 输入  ：reg:RF24L01的命令+寄存器地址。
 		   dat:将要向寄存器写入的数据
 * 输出  ：RF24L01的status寄存器的状态
 * 调用  ：内部调用
 */
u8 SPI_RF24L01_WriteReg(u8 reg,u8 dat)
{
 	u8 status;
	 RF24L01_CE_LOW();
	/*置低CSN，使能SPI传输*/
//    RF24L01_CSN_LOW();
				
	/*发送命令及寄存器号 */
	status = SPI_RF24L01_RW(reg);
		 
	 /*向寄存器写入数据*/
    SPI_RF24L01_RW(dat); 
	          
	/*CSN拉高，完成*/	   
//  	RF24L01_CSN_HIGH();	
		
	/*返回状态寄存器的值*/
   	return(status);
}


/*
 * 函数名：SPI_RF24L01_ReadReg
 * 描述  ：用于从RF24L01特定的寄存器读出数据
 * 输入  ：reg:RF24L01的命令+寄存器地址。
 * 输出  ：寄存器中的数据
 * 调用  ：内部调用
 */
u8 SPI_RF24L01_ReadReg(u8 reg)
{
 	u8 reg_val;

	RF24L01_CE_LOW();
	/*置低CSN，使能SPI传输*/
// 	RF24L01_CSN_LOW();
				
  	 /*发送寄存器号*/
	SPI_RF24L01_RW(reg); 

	 /*读取寄存器的值 */
	reg_val = SPI_RF24L01_RW(NOP);
	            
   	/*CSN拉高，完成*/
//	RF24L01_CSN_HIGH();		
   	
	return reg_val;
}	


/*
 * 函数名：SPI_RF24L01_ReadBuf
 * 描述  ：用于从RF24L01的寄存器中读出一串数据
 * 输入  ：reg:RF24L01的命令+寄存器地址。
 		   pBuf：用于存储将被读出的寄存器数据的数组，外部定义
		   bytes: pBuf的数据长度	
 * 输出  ：RF24L01的status寄存器的状态
 * 调用  ：外部调用
 */
u8 SPI_RF24L01_ReadBuf(u8 reg,u8 *pBuf,u8 bytes)
{
 	u8 status, byte_cnt;

	  RF24L01_CE_LOW();
	/*置低CSN，使能SPI传输*/
//	RF24L01_CSN_LOW();
		
	/*发送寄存器号*/		
	status = SPI_RF24L01_RW(reg); 

 	/*读取缓冲区数据*/
	 for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)		  
	   pBuf[byte_cnt] = SPI_RF24L01_RW(NOP); //从RF24L01读取数据  

	 /*CSN拉高，完成*/
//	RF24L01_CSN_HIGH();	
		
 	return status;		//返回寄存器状态值
}



/*
 * 函数名：SPI_RF24L01_WriteBuf
 * 描述  ：用于向RF24L01的寄存器中写入一串数据
 * 输入  ：reg:RF24L01的命令+寄存器地址。
 		   pBuf：存储了将要写入写寄存器数据的数组，外部定义
		   bytes: pBuf的数据长度	
 * 输出  ：RF24L01的status寄存器的状态
 * 调用  ：外部调用
 */
u8 SPI_RF24L01_WriteBuf(u8 reg ,u8 *pBuf,u8 bytes)
{
	 u8 status,byte_cnt;
	 RF24L01_CE_LOW();
   	 /*置低CSN，使能SPI传输*/
//	 RF24L01_CSN_LOW();			

	 /*发送寄存器号*/	
  	 status = SPI_RF24L01_RW(reg); 
 	
  	  /*向缓冲区写入数据*/
	 for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
		SPI_RF24L01_RW(*pBuf++);	//写数据到缓冲区 	 
	  	   
	/*CSN拉高，完成*/
//	RF24L01_CSN_HIGH();			
  
  	return (status);	//返回RF24L01的状态 		
}

/*
 * 函数名：void RF24L01_Init(void)_Check
 * 描述  ：主要用于void RF24L01_Init(void)与MCU是否正常连接
 * 输入  ：无	
 * 输出  ：SUCCESS/ERROR 连接正常/连接失败
 * 调用  ：外部调用
 */
u8 RF24L01_Check(void)
{
	u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};
	u8 buf1[5];
	u8 i; 
	 
	/*写入5个字节的地址.  */  
	SPI_RF24L01_WriteBuf(RF24L01_WRITE_REG+TX_ADDR,buf,5);

	/*读出写入的地址 */
	SPI_RF24L01_ReadBuf(TX_ADDR,buf1,5); 
	 
	/*比较*/               
	for(i=0;i<5;i++)
	{
		if(buf1[i]!=0xC2)
		break;
	} 
	       
	if(i==5)
		return SUCCESS ;        //MCU与void RF24L01_Init(void)成功连接 
	else
		return ERROR ;        //MCU与void RF24L01_Init(void)不正常连接
}

/*
 * 函数名：void RF24L01_Init(void)_TX_Mode
 * 描述  ：配置发送模式
 * 输入  ：无	
 * 输出  ：无
 * 调用  ：外部调用
 */
void RF24L01_TX_Mode(void)
{  
	RF24L01_CE_LOW();		

   SPI_RF24L01_WriteBuf(RF24L01_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);    //写TX节点地址 

   SPI_RF24L01_WriteBuf(RF24L01_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK   

   SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    

   SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  

   SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次

   SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+RF_CH,CHANAL);       //设置RF通道为CHANAL

   SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+RF_SETUP,0x07);  //设置TX发射参数,0db增益,1Mbps,低噪声增益开启   
	
   SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,发射模式,开启所有中断

/*CE拉高，进入发送模式*/	
  RF24L01_CE_HIGH();
}

 /*
 * 函数名：void RF24L01_Init(void)_Tx_Dat
 * 描述  ：用于向void RF24L01_Init(void)的发送缓冲区中写入数据
 * 输入  ：txBuf：存储了将要发送的数据的数组，外部定义	
 * 输出  ：发送结果，成功返回TXDS,失败返回MAXRT或ERROR
 * 调用  ：外部调用
 */ 
u8 RF24L01_Tx_Dat(u8 *txbuf)
{
	u8 state;  

	 /*ce为低，进入待机模式1*/
	RF24L01_CE_LOW();

	/*写数据到TX BUF 最大 32个字节*/						
   SPI_RF24L01_WriteBuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);

      /*CE为高，txbuf非空，发送数据包 */   
 	 RF24L01_CE_HIGH();
	  	
	  /*等待发送完成中断 */                            
	while(RF24L01_Read_IRQ()!=0); 	
	
	/*读取状态寄存器的值 */                              
	state = SPI_RF24L01_ReadReg(STATUS);

	 /*清除TX_DS或MAX_RT中断标志*/                  
	SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+STATUS,state); 	

	SPI_RF24L01_WriteReg(FLUSH_TX,NOP);    //清除TX FIFO寄存器 

	 /*判断中断类型*/    
	if(state&MAX_RT)                     //达到最大重发次数
			 return MAX_RT; 

	else if(state&TX_DS)                  //发送完成
		 	return TX_DS;
	 else						  
			return ERROR;                 //其他原因发送失败
} 


/*
 * 函数名：void RF24L01_Init(void)_RX_Mode
 * 描述  ：配置并进入接收模式
 * 输入  ：无	
 * 输出  ：无
 * 调用  ：外部调用
 */
void RF24L01_RX_Mode(void)

{
	RF24L01_CE_LOW();	

	SPI_RF24L01_WriteBuf(RF24L01_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址

	SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+EN_AA,0x00);    //使能通道0的自动应答    

	SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址    

	SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+RF_CH,CHANAL);      //设置RF通信频率    

	SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度      

	SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+RF_SETUP,0x07); //设置TX发射参数,0db增益,1Mbps,低噪声增益开启   

	SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+CONFIG, 0x0f);  //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 

	
/*CE拉高，进入接收模式*/	
  RF24L01_CE_HIGH();
}

 /*
 * 函数名：void RF24L01_Init(void)_Rx_Dat
 * 描述  ：用于从void RF24L01_Init(void)的接收缓冲区中读出数据
 * 输入  ：rxBuf：用于接收该数据的数组，外部定义	
 * 输出  ：接收结果，
 * 调用  ：外部调用
 */ 
u8 RF24l01_Rx_Dat(u8 *rxbuf)
{
	u8 state; 
	RF24L01_CE_HIGH();	 //进入接收状态
	 /*等待接收中断*/
	while(RF24L01_Read_IRQ()!=0){
		rt_thread_delay(1);
	} 
	
	RF24L01_CE_LOW();  	 //进入待机状态
	/*读取status寄存器的值  */               
	state=SPI_RF24L01_ReadReg(STATUS);
	 
	/* 清除中断标志*/      
	SPI_RF24L01_WriteReg(RF24L01_WRITE_REG+STATUS,state);

	/*判断是否接收到数据*/
	if(state&RX_DR)                                 //接收到数据
	{
	  SPI_RF24L01_ReadBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
	     SPI_RF24L01_WriteReg(FLUSH_RX,NOP);          //清除RX FIFO寄存器
	  return RX_DR; 
	}
	else    
		return ERROR;                    //没收到任何数据
}

//-------------------------------------硬件部分--------------------------------------------------------------
static void RF24L01_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
 /*使能GPIOA,GPIOC,复用功能时钟*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  /*配置SPI_RF24L01_SPI的CE引脚，GPIOB^1*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

   /*配置SPI_RF24L01_SPI的IRQ引脚，GPIOB^0*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  //上拉输入
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
}


/*
	主要在SPI BUS上面附加一个SPI设备，建立通信。接下来需要实现发送和接收。
*/
rt_err_t nRF24L01_init(const char * spi_device_name){
	RF24L01_IO_Init();
	
		/* initialize mutex */
	if (rt_mutex_init(&lock, spi_device_name, RT_IPC_FLAG_FIFO) != RT_EOK)
	{
			rt_kprintf("init nRF24L01 lock mutex failed\n");
			return -RT_ENOSYS;
	}

	rt_spi_device = (struct rt_spi_device *)rt_device_find(spi_device_name);
	if(rt_spi_device == RT_NULL)
	{
			FLASH_TRACE("spi device %s not found!\r\n", spi_device_name);
			return -RT_ENOSYS;
	}

	/* config spi */
	{
			struct rt_spi_configuration cfg;
			cfg.data_width = 8;
			cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
			cfg.max_hz = 9 * 1000 * 1000; /* 10M */
			rt_spi_configure(rt_spi_device, &cfg);
	}
	return	RT_EOK;
}


#ifdef RT_USING_FINSH
#include <finsh.h>



u8 spiTest(u8 value)
{
	return SPI_RF24L01_RW(value);
}
FINSH_FUNCTION_EXPORT(spiTest, send then recv a byte.)
//FINSH_FUNCTION_EXPORT_ALIAS(led, ld, set led[0 - 1] on[1] or off[0].)
//FINSH_VAR_EXPORT(led_inited, finsh_type_uchar, just a led_inited for test)
#endif

