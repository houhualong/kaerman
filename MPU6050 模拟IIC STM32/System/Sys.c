#include "stm32f10x.h"
#include "Sys.h"
#include "iic_analog.h"
#include "MPU6050.h"

void Sys_Configuration(void)
{
	IIC_GPIO_Configuration( IIC_GOIO_SDA , IIC_SDA , IIC_GPIO_SCL , IIC_SCL );
	MPU6050_Inital();
}

