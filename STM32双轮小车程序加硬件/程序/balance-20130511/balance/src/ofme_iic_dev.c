#include "ofme_iic.h"
#include "delay.h" 
#include "ofme_iic_dev.h"


//陀螺仪、加速传感器等的虚拟方向：
//vX轴：小车前进方向；
//vY轴：小车左转方向；
//vZ轴：小车上升方向；
//实际方向（根据硬件实际情况调整）：
//rX轴：rX = -vX;
//rY轴：rY =  vY;
//rZ轴：rZ = -vZ;
//****************************************************************************//
#define ADXL345_DEVID	(0x00)
#define ADXL345_OFSX	(0x1E)
#define ADXL345_OFSY	(0x1F)
#define ADXL345_OFSZ	(0x20)
#define ADXL345_BW_RATE		(0x2C)
#define ADXL345_POWER_CTL	(0x2D)
#define ADXL345_INT_ENABLE	(0x2E)
#define ADXL345_DATA_FORMAT	(0x31)
#define ADXL345_DATAX0	(0x32)
#define ADXL345_DATAX1	(0x33)
#define ADXL345_DATAY0	(0x34)
#define ADXL345_DATAY1	(0x35)
#define ADXL345_DATAZ0	(0x36)
#define ADXL345_DATAZ1	(0x37)

////////////////////////////////////////////////////////////////////////////////
int adxl345_init(accelerometer_s* p)
{
	u8 data;
	int i;

	p->flag=0;

	if(IIC_OK == iic_dev_read(IIC_ADDR_ADXL345,ADXL345_DEVID,&data) )
	{
		if (0xE5==data)
		{
			i = iic_dev_write(IIC_ADDR_ADXL345,ADXL345_DATA_FORMAT,0x0B);	//测量范围,正负16g，13位模式
			i|= iic_dev_write(IIC_ADDR_ADXL345,ADXL345_BW_RATE,0x0A);	  //100Hz
			i|= iic_dev_write(IIC_ADDR_ADXL345,ADXL345_POWER_CTL,0x08);	//选择电源模式   参考pdf24页
			i|= iic_dev_write(IIC_ADDR_ADXL345,ADXL345_INT_ENABLE,0x00);//禁止所有中断
			i|= iic_dev_write(IIC_ADDR_ADXL345,ADXL345_OFSX,0x00);   	//X 偏移量
			i|= iic_dev_write(IIC_ADDR_ADXL345,ADXL345_OFSX,0x00);   	//Y 偏移量
			i|= iic_dev_write(IIC_ADDR_ADXL345,ADXL345_OFSX,0x00);   	//Z 偏移量
			if(i==IIC_OK)
			{
				p->flag=1;
				return 0;
			}
			else
			{
				puts("adxl345 init error.");
			}
		}
		else
		{
			printf("adxl345 id not 0xe5 but 0x%x.\r\n",(int)data);
		}
	}
	else
	{
		puts("adxl345 id read error.");
	}

	return -1;    
}
////////////////////////////////////////////////////////////////////////////////
void adxl345_read(accelerometer_s* p)
{
#if 1
	if(p->flag&1== 0) return;
	p->flag = 1;
	if(iic_dev_gets(IIC_ADDR_ADXL345,ADXL345_DATAX0,(u8*)&(p->x), 6)) return;

//	实际值转换为虚拟方向的值
	p->x = -(p->x);
	p->z = -(p->z);

	p->flag |= 0x0E;
	return;
#else
	u16 data;

	if(p->flag&1== 0) return;	// adxl345 not called before.
	p->flag = 1;
    if(iic_dev_read(IIC_ADDR_ADXL345,ADXL345_DATAX0,(u8*)&data))		return;
    if(iic_dev_read(IIC_ADDR_ADXL345,ADXL345_DATAX1,((u8*)&data)+1))	return;
	p->x = data;

    if(iic_dev_read(IIC_ADDR_ADXL345,ADXL345_DATAY0,(u8*)&data))		return;
    if(iic_dev_read(IIC_ADDR_ADXL345,ADXL345_DATAY1,((u8*)&data)+1))	return;
	p->y = data;

    if(iic_dev_read(IIC_ADDR_ADXL345,ADXL345_DATAZ0,(u8*)&data))		return;
    if(iic_dev_read(IIC_ADDR_ADXL345,ADXL345_DATAZ1,((u8*)&data)+1))	return;
	p->z = data;

//	实际值转换为虚拟方向的值
	p->x = -(p->x);
	p->z = -(p->z);

	p->flag |= 0x0E;
	return ;
#endif
}

////////////////////////////////////////////////////////////////////////////////

void adxl345_puts(accelerometer_s* p)
{
	if(p->flag!=0x0F) printf("adxl345 data not valid.\r\n");
	else printf("adx1345: %d, %d, %d.\r\n",(int)p->x, (int)p->y, (int)p->z);
}



//****************************************************************************//
#define L3G4200D_WHO_AM_I	(0x0F)
#define L3G4200D_CTRL_REG1	(0x20)
#define L3G4200D_CTRL_REG2	(0x21)
#define L3G4200D_CTRL_REG3	(0x22)
#define L3G4200D_CTRL_REG4	(0x23)
#define L3G4200D_CTRL_REG5	(0x24)

#define L3G4200D_OUT_X_L	(0x28)
#define L3G4200D_OUT_X_H	(0x29)
#define L3G4200D_OUT_Y_L	(0x2A)
#define L3G4200D_OUT_Y_H	(0x2B)
#define L3G4200D_OUT_Z_L	(0x2C)
#define L3G4200D_OUT_Z_H	(0x2D)

////////////////////////////////////////////////////////////////////////////////

int l3g4200d_init(gyroscope_s* p)
{
	u8 data;

	p->flag = 0;
	if(iic_dev_read(IIC_ADDR_L3G4200D, L3G4200D_WHO_AM_I, &data))
	{
		puts("l3g4200d id read error.");
		return -1;
	}
	if( 0xD3!= data)
	{
		printf("l3g4200d id not 0xe3 but 0x%x.\r\n",(int)data);
		return -1;
	}
	if(iic_dev_write(IIC_ADDR_L3G4200D,L3G4200D_CTRL_REG1, 0x0F))
	{
		return -1;
	}
	if(iic_dev_write(IIC_ADDR_L3G4200D,L3G4200D_CTRL_REG2, 0x00))
	{
		return -1;
	}
	if(iic_dev_write(IIC_ADDR_L3G4200D,L3G4200D_CTRL_REG3, 0x00))//0x08
	{
		return -1;
	}
	if(iic_dev_write(IIC_ADDR_L3G4200D,L3G4200D_CTRL_REG4, 0x30))//2000dps
	{
		return -1;
	}
	if(iic_dev_write(IIC_ADDR_L3G4200D,L3G4200D_CTRL_REG5, 0x00))
	{
		return -1;
	}
	p->flag = 1;

	return 0;
}	
////////////////////////////////////////////////////////////////////////////////

void l3g4200d_read(gyroscope_s* p)
{
#if 1
	if(p->flag&1== 0) return;
	p->flag = 1;
	if(iic_dev_gets(IIC_ADDR_L3G4200D,0x80 | L3G4200D_OUT_X_L,(u8*)&(p->x), 6))
		return;		// 按照手册要求，连续读数据需要设地址为：寄存器地址|0x80

//	实际值转换为虚拟方向的值
	p->x = -(p->x)+15;
	p->z = -(p->z)+15;

	p->flag |= 0x0E;
	return;

#else
	u16 data;

	if(p->flag&1== 0) return;	// l3g4200d_init() not called before.
	p->flag = 1;
    if(iic_dev_read(IIC_ADDR_L3G4200D,L3G4200D_OUT_X_L,(u8*)&data))		return;
    if(iic_dev_read(IIC_ADDR_L3G4200D,L3G4200D_OUT_X_H,((u8*)&data)+1))	return;
	p->x = data;
	p->flag|=0x02;

    if(iic_dev_read(IIC_ADDR_L3G4200D,L3G4200D_OUT_Y_L,(u8*)&data))		return;
    if(iic_dev_read(IIC_ADDR_L3G4200D,L3G4200D_OUT_Y_H,((u8*)&data)+1))	return;
	p->y = data;
	p->flag|=0x04;
    if(iic_dev_read(IIC_ADDR_L3G4200D,L3G4200D_OUT_Z_L,(u8*)&data))		return;
    if(iic_dev_read(IIC_ADDR_L3G4200D,L3G4200D_OUT_Z_H,(u8*)&data+1))	return;
	p->z = data;
	p->flag|=0x08;

//	实际值转换为虚拟方向的值
	p->x = -(p->x);
	p->z = -(p->z);

	return ;
#endif
}


////////////////////////////////////////////////////////////////////////////////

void l3g4200d_puts(gyroscope_s* p)
{
	if(p->flag!=0x0F) printf("l3g4200d data not valid.\r\n");
	else printf("l3g4200d: %d, %d, %d.\r\n",(int)p->x, (int)p->y, (int)p->z);
}
