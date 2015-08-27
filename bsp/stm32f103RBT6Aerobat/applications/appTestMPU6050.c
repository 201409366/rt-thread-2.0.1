#include <rtthread.h>
#include "drv_mpu6050.h"

void rt_appMpu6050_thread_entry(void* parameter);

rt_err_t appMpu6050Init(void){
		rt_thread_t init_thread;

	init_thread = rt_thread_create("appMpu6050",
														 rt_appMpu6050_thread_entry, RT_NULL,
														 2048, 7, 20);
	if (init_thread != RT_NULL)
	{
		rt_thread_startup(init_thread);
		return RT_EOK;;
	}		
	else
		return RT_ERROR;
	
	
}
static rt_uint8_t mpu6050_test(void)
{
    rt_device_t mpu6050 = RT_NULL;
    rt_uint8_t mpu6050_id = 0;
    rt_uint8_t mpu_data[14];
    rt_uint16_t acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z;
    mpu6050 = rt_device_find("mpu6050");
    while (mpu6050 == RT_NULL)
    {
        rt_kprintf("can not find mpu6050!\n");
        rt_hw_mpu6050_init("i2c1", MPU6050_DEFAULT_ADDRESS);
        rt_thread_delay(5);
        mpu6050 = rt_device_find("mpu6050");
    }
		
    rt_device_open(mpu6050, RT_DEVICE_FLAG_RDONLY);
    mpu6050->read(mpu6050, MPU6050_RA_WHO_AM_I, &mpu6050_id, 1);		
		
    rt_kprintf("mpu6050 id = %x\n", mpu6050_id);
    if (mpu6050_id != 0x68)
        return 0;
    while (1)
    {
        mpu6050->read(mpu6050, MPU6050_RA_ACCEL_XOUT_H, mpu_data, 14);
        acc_x = ((mpu_data[0] << 8) | mpu_data[1]);
        acc_y = ((mpu_data[2] << 8) | mpu_data[3]);
        acc_z = ((mpu_data[4] << 8) | mpu_data[5]);
        gyro_x = ((mpu_data[8] << 8) | mpu_data[9]);
        gyro_y = ((mpu_data[10] << 8) | mpu_data[11]);
        gyro_z = ((mpu_data[12] << 8) | mpu_data[13]);
        rt_kprintf("acc_xyz:%d,%d,%d\n", acc_x, acc_y, acc_z);
        rt_kprintf("gyro_xyz:%d,%d,%d\n", gyro_x, gyro_y, gyro_z);
        rt_thread_delay(200);
    }
    //return 0 ;
}

void rt_appMpu6050_thread_entry(void* parameter) {

	while(1) {
		mpu6050_test();
		rt_thread_delay(600);
	}	
}
