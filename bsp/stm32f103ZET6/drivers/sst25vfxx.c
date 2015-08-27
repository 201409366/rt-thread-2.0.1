/*
 * File      : sst25vfxx.c
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-11-10     aozima       first implementation
 * 2010-11-15     aozima       port for RT-thread
*/

#include <stdint.h>
#include "sst25vfxx.h"

/* 0:don'ot use DMA 1:use DMA */
#define SPI_FLASH_USE_DMA         0
/* secotr_size = 4096byte,secotr_size % DMA_BUFFER_SIZE == 0 */
#define DMA_BUFFER_SIZE           512

static uint32_t device_id = 0;

// bsp support:
// port_init()                         : init hardware GPIO.
// CS_HIGH()                           :
// CS_LOW()                            :
// spi_lock()                          : spi lock
// spi_unlock()                        : spi unlock
// spi_config()                        : config spi for sst25vfxx
// uint8_t spi_readwrite(uint8_t data) : spi rw

/********* bsp **********/
#include "rtthread.h"
#include "board.h"
#include <drivers/spi.h>

/* SPI_FLASH_CS   PA4 */
// SPI <--> SPI1

#include "stm32f10x.h"
//#include "rtthread.h"

//#define FLASH_TRACE(...)
#define FLASH_TRACE  rt_kprintf

//#define CS_LOW()      GPIO_ResetBits(GPIOB, GPIO_Pin_12)
//#define CS_HIGH()     GPIO_SetBits(GPIOB, GPIO_Pin_12)

#define spi_lock()    rt_sem_take(&spi2_lock, RT_WAITING_FOREVER);
#define spi_unlock()  rt_sem_release(&spi2_lock);

#define spi_config()  rt_hw_spi2_baud_rate(SPI_BaudRatePrescaler_8);/* 72M/8=9M */

static struct rt_spi_device * rt_spi_device;
struct rt_semaphore spi2_lock;

#if SPI_FLASH_USE_DMA
static uint8_t dummy = 0xFF;
static uint8_t _spi_flash_buffer[ DMA_BUFFER_SIZE ];
#endif

#if SPI_FLASH_USE_DMA
static void DMA_RxConfiguration(rt_uint32_t addr, rt_size_t size)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_ClearFlag(DMA1_FLAG_TC2 | DMA1_FLAG_TE2 | DMA1_FLAG_TC3 | DMA1_FLAG_TE3);

    /* DMA Channel configuration ----------------------------------------------*/
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(SPI1->DR));
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32) addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel2, ENABLE);

    /* Dummy TX channel configuration */
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(SPI1->DR));
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(&dummy);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel3, ENABLE);
}
#endif

static void port_init(void)
{
#if SPI_FLASH_USE_DMA
    /* Enable the DMA1 Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif
}

void rt_hw_spi2_baud_rate(uint16_t SPI_BaudRatePrescaler)
{
    SPI2->CR1 &= ~SPI_BaudRatePrescaler_256;
    SPI2->CR1 |= SPI_BaudRatePrescaler;
}

static uint8_t spi_readwrite(uint8_t data)
{
//    //Wait until the transmit buffer is empty
//    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
//    // Send the byte
//    SPI_I2S_SendData(SPI2, data);

//    //Wait until a data is received
//    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//    // Get the received data
//    data = SPI_I2S_ReceiveData(SPI2);

//    // Return the shifted data
//    return data;
	u8 val;

	if(rt_spi_device != RT_NULL) 
		val = rt_spi_sendrecv8(rt_spi_device,data);
	else
		val = 0;

	return val;
}
/********* bsp end **********/

static uint8_t sst25vfxx_read_status(void)
{
    uint8_t tmp;

    //CS_LOW();
    spi_readwrite( CMD_RDSR );
    tmp=spi_readwrite(0XFF);
    //CS_HIGH();
    return tmp;
}

static void sst25vfxx_wait_busy(void)
{
    while( sst25vfxx_read_status() & (0x01));
}

/** \brief read [size] byte from [offset] to [buffer]
 *
 * \param offset uint32_t unit : byte
 * \param buffer uint8_t*
 * \param size uint32_t   unit : byte
 * \return uint32_t byte for read
 *
 */
uint32_t sst25vfxx_read(uint32_t offset,uint8_t * buffer,uint32_t size)
{
    uint32_t index;

    spi_lock();
    spi_config();

    //CS_LOW();
    spi_readwrite( CMD_WRDI );
    //CS_HIGH();

    //CS_LOW();
    spi_readwrite( CMD_READ);
    spi_readwrite(  offset>>16 );
    spi_readwrite(  offset>>8 );
    spi_readwrite(  offset );
#if SPI_FLASH_USE_DMA
    for(index=0; index<size/DMA_BUFFER_SIZE; index++)
    {
        DMA_RxConfiguration((rt_uint32_t)_spi_flash_buffer, DMA_BUFFER_SIZE);
        SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
        SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
        while (DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET);
        SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, DISABLE);
        rt_memcpy(buffer,_spi_flash_buffer,DMA_BUFFER_SIZE);
        buffer += DMA_BUFFER_SIZE;
    }
#else
    for(index=0; index<size; index++)
    {
        *buffer++ = spi_readwrite(0xFF);
    }
#endif
    //CS_HIGH();

    spi_unlock();

    return size;
}

/** \brief write N page on [page]
 *
 * \param page uint32_t unit : byte (4096 * N,1 page = 4096byte)
 * \param buffer const uint8_t*
 * \param size uint32_t unit : byte ( 4096*N )
 * \return uint32_t
 *
 */
uint32_t sst25vfxx_page_write(uint32_t page,const uint8_t * buffer,uint32_t size)
{
    uint32_t index;

    page &= ~0xFFF; // page size = 4096byte

    spi_lock();
    spi_config();

    //CS_LOW();
    spi_readwrite( CMD_WREN );//write en
    //CS_HIGH();

    //CS_LOW();
    spi_readwrite( CMD_ERASE_4K );
    spi_readwrite( page >> 16 );
    spi_readwrite( page >> 8 );
    spi_readwrite( page  );
    //CS_HIGH();

    sst25vfxx_wait_busy(); // wait erase done.

    //CS_LOW();
    spi_readwrite( CMD_WREN );//write en
    //CS_HIGH();

    //CS_LOW();
    spi_readwrite( CMD_AAIP );
    spi_readwrite(  page>>16 );
    spi_readwrite(  page>>8 );
    spi_readwrite(  page );

    spi_readwrite( *buffer++ );
    spi_readwrite( *buffer++ );
    size -= 2;
    //CS_HIGH();

    sst25vfxx_wait_busy();

    for(index=0; index < size/2; index++)
    {
        //CS_LOW();
        spi_readwrite( CMD_AAIP );
        spi_readwrite( *buffer++ );
        spi_readwrite( *buffer++ );
        //CS_HIGH();
        sst25vfxx_wait_busy();
    }
    //CS_HIGH();

    //CS_LOW();
    spi_readwrite( CMD_WRDI );
    //CS_HIGH();

    spi_unlock();
    return size;
}

/* SPI DEVICE */
static struct rt_device spi_flash_device;

/* RT-Thread Device Driver Interface */
/** \brief
 *
 * \param rt_device_t dev
 * \return rt_err_t
 *
 */
static rt_err_t sst25vfxx_flash_init(rt_device_t dev)
{
    return RT_EOK;
}

/** \brief
 *
 * \param rt_device_t dev
 * \param rt_uint16_t oflag
 * \return rt_err_t
 *
 */
static rt_err_t sst25vfxx_flash_open(rt_device_t dev, rt_uint16_t oflag)
{

    return RT_EOK;
}

/** \brief
 *
 * \param rt_device_t dev
 * \return rt_err_t
 *
 */
static rt_err_t sst25vfxx_flash_close(rt_device_t dev)
{
    return RT_EOK;
}

/** \brief
 *
 * \param rt_device_t dev
 * \param rt_uint8_t cmd
 * \param void* args
 * \return rt_err_t
 *
 */
static rt_err_t sst25vfxx_flash_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);

    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL) return -RT_ERROR;

        geometry->bytes_per_sector = 4096;
        geometry->sector_count = 512;
        geometry->block_size = 4096; /* block erase: 4k */
    }

    return RT_EOK;
}

/** \brief
 *
 * \param rt_device_t dev
 * \param rt_off_t pos
 * \param void* buffer
 * \param rt_size_t size
 * \return rt_size_t
 *
 */
static rt_size_t sst25vfxx_flash_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    sst25vfxx_read(pos*4096,buffer,size*4096);
    return size;
}

/** \brief
 *
 * \param rt_device_t dev
 * \param rt_off_t pos
 * \param const void* buffer
 * \param rt_size_t size
 * \return rt_size_t
 *
 */
static rt_size_t sst25vfxx_flash_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    sst25vfxx_page_write(pos*4096,buffer,size*4096);
    return size;
}

/** \brief sst25vfxx SPI flash init
 *
 * \param void
 * \return void
 *
 */
rt_err_t rt_hw_sst25vfxx_init(const char * spi_device_name)
{
    port_init();
	
		if (rt_sem_init(&spi2_lock, "spi2lock", 1, RT_IPC_FLAG_FIFO) != RT_EOK)
		{
				rt_kprintf("init spi2 lock semaphore failed\n");
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
		
    spi_lock();
    spi_config();
    //CS_LOW();
    spi_readwrite( CMD_WRDI );
    //CS_HIGH();

    //CS_LOW();
    spi_readwrite( CMD_JEDEC_ID );
    device_id  = spi_readwrite(0xFF);
    device_id |= spi_readwrite(0xFF)<<8;
    device_id |= spi_readwrite(0xFF)<<16;
    //CS_HIGH();

		
    if(device_id == SST25VF016)
    {
        FLASH_TRACE("FLASH TYPE : SST25VF016\r\n");
				rt_kprintf("SPI Flash device_id is : 0x%X !\n",device_id);
        //CS_LOW();
        spi_readwrite( CMD_DBSY );
        //CS_HIGH();

        //CS_LOW();
        spi_readwrite( CMD_EWSR );
        //CS_HIGH();

        //CS_LOW();
        spi_readwrite( CMD_WRSR );
        spi_readwrite( 0 );
        //CS_HIGH();
    }
rt_kprintf("SPI Flash device_id is : 0x%X !\n",device_id);
    spi_unlock();

    spi_flash_device.type    = RT_Device_Class_Block;
    spi_flash_device.init    = sst25vfxx_flash_init;
    spi_flash_device.open    = sst25vfxx_flash_open;
    spi_flash_device.close   = sst25vfxx_flash_close;
    spi_flash_device.read 	 = sst25vfxx_flash_read;
    spi_flash_device.write   = sst25vfxx_flash_write;
    spi_flash_device.control = sst25vfxx_flash_control;
    /* no private */
    spi_flash_device.user_data = RT_NULL;

    rt_device_register(&spi_flash_device, "spi0",
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);			
		return	RT_EOK;
}
