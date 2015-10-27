#include <rtthread.h>
#include "drv_mpu6050.h"
#include <math.h>

#define maxFilterCount	64
#define Gyro_G					9.8
#define	samplePeriod		0.01

static rt_uint8_t	filter_cnt = 0;
static rt_uint16_t ACC_X_BUF[maxFilterCount] = {0};
static rt_uint16_t ACC_Y_BUF[maxFilterCount] = {0};
static rt_uint16_t ACC_Z_BUF[maxFilterCount] = {0};

//加速度平均
static float ACC_X_AVG = 0;
static float ACC_Y_AVG = 0;
static float ACC_Z_AVG = 0;
//角加速度
static double GYRO_X_I = 0;
static double GYRO_Y_I = 0;
static double GYRO_Z_I = 0;

//四元素 
static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
static float exInt = 0, eyInt = 0, ezInt = 0; 
//为什么要用陀螺仪的积分呢？ 加速度积分就是速度
#define Kp 2.0f                        // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.005f                // integral gain governs rate of convergence of gyroscope biases
#define halfT 0.5f                // half the sample period

static void rt_appMpu6050_thread_entry(void* parameter);

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

static void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az) {
        float norm;
        float vx, vy, vz;
        float ex, ey, ez;         

        // normalise the measurements 把加计的三维向量转成单位向量。
        norm = sqrt(ax*ax + ay*ay + az*az);      
        ax = ax / norm;
        ay = ay / norm;
        az = az / norm;      
/*
这是把四元数换算成《方向余弦矩阵》中的第三列的三个元素。
根据余弦矩阵和欧拉角的定义，地理坐标系的重力向量，转到机体坐标系，正好是这三个元素。
所以这里的vx\y\z，其实就是当前的欧拉角（即四元数）的机体坐标参照系上，换算出来的重力单位向量。 */
        // estimated direction of gravity
        vx = 2*(q1*q3 - q0*q2);
        vy = 2*(q0*q1 + q2*q3);
        vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
/*
axyz是机体坐标参照系上，加速度计测出来的重力向量，也就是实际测出来的重力向量。
axyz是测量得到的重力向量，vxyz是陀螺积分后的姿态来推算出的重力向量，它们都是机体坐标参照系上的重力向量。
那它们之间的误差向量，就是陀螺积分后的姿态和加计测出来的姿态之间的误差。
向量间的误差，可以用向量叉积（也叫向量外积、叉乘）来表示，exyz就是两个重力向量的叉积。
这个叉积向量仍旧是位于机体坐标系上的，而陀螺积分误差也是在机体坐标系，而且叉积的大小与陀螺积
分误差成正比，正好拿来纠正陀螺。（你可以自己拿东西想象一下）由于陀螺是对机体直接积分，所以对
陀螺的纠正量会直接体现在对机体坐标系的纠正。
*/
        // error is sum of cross product between reference direction of field and direction  measured by sensor
        ex = (ay*vz - az*vy);
        ey = (az*vx - ax*vz);
        ez = (ax*vy - ay*vx);
        // integral error scaled integral gain
        exInt = exInt + ex*Ki;
        eyInt = eyInt + ey*Ki;
        ezInt = ezInt + ez*Ki;
        // adjusted gyroscope measurements 用叉积误差来做PI修正陀螺零偏
        gx = gx + Kp*ex + exInt;
        gy = gy + Kp*ey + eyInt;
        gz = gz + Kp*ez + ezInt;
        // integrate quaternion rate and normalise 四元数微分方程
        q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
        q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
        q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
        q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;  
        // normalise quaternion 四元数规范化
        norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
        q0 = q0 / norm;
        q1 = q1 / norm;
        q2 = q2 / norm;
        q3 = q3 / norm;
}

static void prepare_Data(rt_device_t mpu6050) 
{
	rt_uint32_t temp1=0,temp2=0,temp3=0;
	rt_uint8_t i;
	rt_uint8_t mpu_data[14];
	
	mpu6050->read(mpu6050, MPU6050_RA_ACCEL_XOUT_H, mpu_data, 14);
	ACC_X_BUF[filter_cnt] = ((mpu_data[0] << 8) | mpu_data[1]);
	ACC_Y_BUF[filter_cnt] = ((mpu_data[2] << 8) | mpu_data[3]);
	ACC_Z_BUF[filter_cnt] = ((mpu_data[4] << 8) | mpu_data[5]);

	
	for(i=0;i<maxFilterCount;i++)
	{
		temp1 += ACC_X_BUF[i];
		temp2 += ACC_Y_BUF[i];
		temp3 += ACC_Z_BUF[i];
	}
	ACC_X_AVG = temp1 / maxFilterCount;
	ACC_Y_AVG = temp2 / maxFilterCount;
	ACC_Z_AVG = temp3 / maxFilterCount;
	filter_cnt++;
	if(filter_cnt >= maxFilterCount)
		filter_cnt = 0;
	
	temp1 = ((mpu_data[8] << 8) | mpu_data[9]);
	temp2 = ((mpu_data[10] << 8) | mpu_data[11]);
	temp3 = ((mpu_data[12] << 8) | mpu_data[13]);
	
	GYRO_X_I = temp1 * Gyro_G * samplePeriod;
	GYRO_Y_I = temp2 * Gyro_G * samplePeriod;
	GYRO_Z_I = temp3 * Gyro_G * samplePeriod;	
}

static rt_uint8_t mpu6050_test(void)
{
    rt_device_t mpu6050 = RT_NULL;
    rt_uint8_t mpu6050_id = 0;
	
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
			prepare_Data(mpu6050);	
			IMUupdate(GYRO_X_I,GYRO_Y_I,GYRO_Z_I,ACC_X_AVG,ACC_Y_AVG,ACC_Z_AVG);
      //rt_kprintf("%05d %05d %05d %05d %05d %05d\n", acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z);
      rt_thread_delay(10);
    }
    //return 0 ;
}

static void rt_appMpu6050_thread_entry(void* parameter) {

	while(1) {
		mpu6050_test();
		rt_thread_delay(600);
	}	
}
