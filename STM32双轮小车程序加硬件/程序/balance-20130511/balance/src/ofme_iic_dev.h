#ifndef __OFME_IIC_DEV_H__
#define __OFME_IIC_DEV_H__

////////////////////////////////////////////////////////////////////////////////
//IIC设备在总线上的地址
#define	IIC_ADDR_ADXL345	0xA6	//加速度传感器器件地址
#define	IIC_ADDR_BMP085		0xee	//气压传感器器件地址
#define	IIC_ADDR_HMC5883L	0x3C	//磁场传感器器件地址
#define	IIC_ADDR_L3G4200D	0xD2	//陀螺仪传感器器件地址
////////////////////////////////////////////////////////////////////////////////
// 加速度计结构体
typedef struct
{
	signed short x;
	signed short y;
	signed short z;
	unsigned short flag;
}   accelerometer_s, *accelerometer_t;
// 陀螺仪结构体
typedef struct
{
	signed short x;
	signed short y;
	signed short z;
	unsigned short flag;
}   gyroscope_s, *gyroscope_t;

////////////////////////////////////////////////////////////////////////////////


//********************************************************************

void adxl345_puts(accelerometer_s* p);
void l3g4200d_puts(gyroscope_s* p);
void adxl345_read(accelerometer_s* p);
void l3g4200d_read(gyroscope_s* p);
int  adxl345_init(accelerometer_s* p);
int  l3g4200d_init(gyroscope_s* p);
void hw_iic_init(void);

#endif
