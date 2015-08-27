#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>

#ifdef RT_USING_I2C_BITOPS

#define RCC_I2C_SCL         RCC_APB2Periph_GPIOB
#define GPIO_PORT_I2C_SCL   GPIOB
#define PIN_I2C_SCL		      GPIO_Pin_6

#define RCC_I2C_SDA         RCC_APB2Periph_GPIOB
#define GPIO_PORT_I2C_SDA   GPIOB
#define PIN_I2C_SDA		      GPIO_Pin_7

//#define RCC_I2C_SCL         RCC_APB2Periph_GPIOC
//#define GPIO_PORT_I2C_SCL   GPIOC
//#define PIN_I2C_SCL		    GPIO_Pin_12

//#define RCC_I2C_SDA         RCC_APB2Periph_GPIOC
//#define GPIO_PORT_I2C_SDA   GPIOC
//#define PIN_I2C_SDA		    GPIO_Pin_11

////IO方向设置
//#define SDA_IN()  {GPIO_PORT_I2C_SDA->CRH&=0XFFFF0FFF;GPIO_PORT_I2C_SDA->CRH|=8<<12;}
////上拉/下拉 输入

//#define SDA_OUT() {GPIO_PORT_I2C_SDA->CRH&=0XFFFF0FFF;GPIO_PORT_I2C_SDA->CRH|=3<<12;}
////

//#define SCL_IN()  {GPIO_PORT_I2C_SCL->CRH&=0XFFF0FFFF;GPIO_PORT_I2C_SCL->CRH|=8<<16;}
//#define SCL_OUT() {GPIO_PORT_I2C_SCL->CRH&=0XFFF0FFFF;GPIO_PORT_I2C_SCL->CRH|=3<<16;}

static struct rt_i2c_bus_device i2c_device;

static uint8_t isSDAOut = 1;//1 out 0 in
static uint8_t isSCLOut = 1;//1 out 0 in

static void SDA_IN(void)
{
	
		GPIO_InitTypeDef GPIO_InitStructure;
	
		GPIO_InitStructure.GPIO_Pin = PIN_I2C_SDA ;			
		GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;  	// 浮空输入
		GPIO_Init(GPIO_PORT_I2C_SCL, &GPIO_InitStructure);				     	// 选择C端口
		
}

static void SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = PIN_I2C_SDA ;			
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_OD;		  	// 开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	// 最高输出速率50MHz
	GPIO_Init(GPIO_PORT_I2C_SCL, &GPIO_InitStructure);				 	    // 选择C端口
}

static void SCL_IN(void)
{
	
}

static void SCL_OUT(void)
{
	
}

static void gpio_set_sda(void *data, rt_int32_t state)
{
		if(!isSDAOut) 
		{
			SDA_OUT();
			isSDAOut = 1;
		}
    if (state)
    {
        GPIO_SetBits(GPIO_PORT_I2C_SDA, PIN_I2C_SDA);
    }
    else
    {
        GPIO_ResetBits(GPIO_PORT_I2C_SDA, PIN_I2C_SDA);
    }
}

static rt_int32_t gpio_get_sda(void *data)
{
		if(isSDAOut) 
		{
			SDA_IN();
			isSDAOut = 0;
		}		
    return GPIO_ReadInputDataBit(GPIO_PORT_I2C_SDA, PIN_I2C_SDA);
}

static void gpio_set_scl(void *data, rt_int32_t state)
{
		if(!isSCLOut) 
		{
			SCL_OUT();
			isSCLOut = 1;
		}
    if (state)
    {
        GPIO_SetBits(GPIO_PORT_I2C_SCL, PIN_I2C_SCL);
    }
    else
    {
        GPIO_ResetBits(GPIO_PORT_I2C_SCL, PIN_I2C_SCL);
    }
}

static rt_int32_t gpio_get_scl(void *data)
{
		if(isSCLOut) 
		{
			SCL_IN();
			isSCLOut = 0;
		}
		return GPIO_ReadInputDataBit(GPIO_PORT_I2C_SCL, PIN_I2C_SCL);
}

static void gpio_udelay(rt_uint32_t us)
{
    volatile rt_int32_t i;
    for (; us > 0; us--)
    {
        i = 50;
        while(i--);
    }
}

static const struct rt_i2c_bit_ops bit_ops =
{
    RT_NULL,
    gpio_set_sda,
    gpio_set_scl,
    gpio_get_sda,
    gpio_get_scl,

    gpio_udelay,

    1,
    100
};

#else

#if 0
static rt_err_t stm32_i2c_configure(struct rt_i2c_bus_device * device,
                                    struct rt_i2c_configuration* configuration)
{
    return RT_EOK;
}

static rt_err_t stm32_i2c_start(struct rt_i2c_bus_device * device)
{
//    /* Send START condition */
//    I2C_GenerateSTART(I2C1, ENABLE);
//
//    /* Test on EV5 and clear it */
//    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    return RT_EOK;
}

static rt_err_t stm32_i2c_stop(struct rt_i2c_bus_device * device)
{
//    /* Send STOP Condition */
//    I2C_GenerateSTOP(I2C1, ENABLE);
//
    return RT_EOK;
}

static rt_size_t stm32_i2c_read(struct rt_i2c_bus_device * device,
                                struct rt_i2c_message * message,
                                rt_uint8_t * read_buffer,
                                rt_size_t size)
{
    uint16_t temp;

    /* Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C1, message->device_addr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, *message->device_offset);

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send STRAT condition a second time */
    I2C_GenerateSTART(I2C1, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2C1, message->device_addr, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));


    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));

    I2C_AcknowledgeConfig(I2C1,DISABLE);

    *read_buffer++ = I2C_ReceiveData(I2C1);

    /* Test on EV7 and clear it */
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));

    *read_buffer++ |= I2C_ReceiveData(I2C1);

    /* Send STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);

    I2C_AcknowledgeConfig(I2C1,ENABLE);

    return size;
}

static rt_size_t stm32_i2c_write(struct rt_i2c_bus_device * device,
                                 struct rt_i2c_message * message,
                                 const rt_uint8_t * write_buffer,
                                 rt_size_t size)
{
    /* Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C1, message->device_addr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* Send the EEPROM's internal address to write to */
    I2C_SendData(I2C1, *message->device_offset);

    /* Test on EV8 and clear it */
    while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send the current byte */
    I2C_SendData(I2C1, (uint8_t)*write_buffer++);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send the current byte */
    I2C_SendData(I2C1, (uint8_t)*write_buffer++);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send STOP condition */
    I2C_GenerateSTOP(I2C1, ENABLE);

    return 0;
}
#endif

static rt_size_t stm32_i2c_recv_bytes(I2C_TypeDef* I2Cx, struct rt_i2c_msg *msg)
{
    rt_size_t bytes = 0;
    rt_size_t len = msg->len;

    while (len--)
    {
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

        msg->buf[bytes++] = (rt_uint8_t)I2Cx->DR;
    }

    return bytes;
}


static rt_size_t stm32_i2c_send_bytes(I2C_TypeDef* I2Cx, struct rt_i2c_msg *msg)
{
    rt_size_t bytes = 0;
    rt_size_t len = msg->len;

    while (len--)
    {
        I2Cx->DR = msg->buf[bytes++];
        while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    return bytes;
}


static void I2C_SendAddress(I2C_TypeDef* I2Cx, struct rt_i2c_msg *msg)
{
    rt_uint16_t addr;
    rt_uint16_t flags = msg->flags;
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    /* Test on the direction to set/reset the read/write bit */
    addr = msg->addr << 1;
    if (flags & RT_I2C_RD)
    {
        /* Set the address bit0 for read */
        addr |= 1;
    }
    /* Send the address */
    I2Cx->DR = addr;
}


static rt_size_t stm32_i2c_xfer(struct rt_i2c_bus_device *bus,
                                struct rt_i2c_msg msgs[], rt_uint32_t num)
{
    struct rt_i2c_msg *msg;
    rt_int32_t i, ret;
    rt_uint16_t ignore_nack;

    I2C_GenerateSTART(I2C1, ENABLE);
    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    for (i = 0; i < num; i++)
    {
        msg = &msgs[i];
        ignore_nack = msg->flags & RT_I2C_IGNORE_NACK;
        if (!(msg->flags & RT_I2C_NO_START))
        {
            if (i)
            {
                I2C_GenerateSTART(I2C1, ENABLE);
            }
            I2C_SendAddress(I2C1, msg);
            if (msg->flags & RT_I2C_RD)
                while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
            else
                while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

        }
        if (msg->flags & RT_I2C_RD)
        {
            if (!(msg->flags & RT_I2C_NO_READ_ACK))
                I2C_AcknowledgeConfig(I2C1,ENABLE);
            else
                I2C_AcknowledgeConfig(I2C1,DISABLE);
            ret = stm32_i2c_recv_bytes(I2C1, msg);
            if (ret >= 1)
                i2c_dbg("read %d byte%s\n",
                        ret, ret == 1 ? "" : "s");
            if (ret < msg->len)
            {
                if (ret >= 0)
                    ret = -RT_EIO;
                goto out;
            }
        }
        else
        {
            ret = stm32_i2c_send_bytes(I2C1, msg);
            if (ret >= 1)
                i2c_dbg("write %d byte%s\n",
                        ret, ret == 1 ? "" : "s");
            if (ret < msg->len)
            {
                if (ret >= 0)
                    ret = -RT_ERROR;
                goto out;
            }
        }
    }
    ret = i;

out:
    i2c_dbg("send stop condition\n");
    I2C_GenerateSTOP(I2C1, ENABLE);

    return ret;
}


static const struct rt_i2c_bus_device_ops i2c1_ops =
{
    stm32_i2c_xfer,
    RT_NULL,
    RT_NULL
};

static struct rt_i2c_bus_device stm32_i2c1;

#endif

rt_err_t rt_hw_i2c_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

#ifdef RT_USING_I2C_BITOPS
	
		RCC_APB2PeriphClockCmd( RCC_I2C_SCL , ENABLE  );
		GPIO_InitStructure.GPIO_Pin =  PIN_I2C_SCL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
		GPIO_Init(GPIO_PORT_I2C_SCL, &GPIO_InitStructure);
	
		RCC_APB2PeriphClockCmd( RCC_I2C_SDA , ENABLE  );	
		GPIO_InitStructure.GPIO_Pin =  PIN_I2C_SDA;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
		GPIO_Init(GPIO_PORT_I2C_SDA, &GPIO_InitStructure);

    GPIO_SetBits(GPIO_PORT_I2C_SDA, PIN_I2C_SDA);
		GPIO_SetBits(GPIO_PORT_I2C_SCL, PIN_I2C_SCL);
		isSDAOut = 1;
		isSCLOut = 1;
    rt_memset((void *)&i2c_device, 0, sizeof(struct rt_i2c_bus_device));
    i2c_device.priv = (void *)&bit_ops;
    rt_i2c_bit_add_bus(&i2c_device, "i2c1");

#else
		I2C_InitTypeDef  I2C_InitStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);

//		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE  );
//		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12 | GPIO_Pin_11;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
//		GPIO_Init(GPIOC, &GPIO_InitStructure);
//		
//		GPIO_SetBits(GPIOC, GPIO_Pin_11);
		
		/* Configure I2C1 pins: PB6->SCL and PB7->SDA */
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		I2C_DeInit(I2C1);
		I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
		I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
		I2C_InitStructure.I2C_OwnAddress1 = 0x30;
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_InitStructure.I2C_ClockSpeed = 100000;  /* 100K速度 */
		
		I2C_Init(I2C1, &I2C_InitStructure);
		
		I2C_Cmd(I2C1, ENABLE);		
		/*允许1字节1应答模式*/
		I2C_AcknowledgeConfig(I2C1, ENABLE);

		rt_memset((void *)&stm32_i2c1, 0, sizeof(struct rt_i2c_bus_device));
		stm32_i2c1.ops = &i2c1_ops;

		rt_i2c_bus_device_register(&stm32_i2c1, "i2c1");

#endif
}

INIT_BOARD_EXPORT(rt_hw_i2c_init);
