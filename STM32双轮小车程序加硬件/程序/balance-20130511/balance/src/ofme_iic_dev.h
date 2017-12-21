#ifndef __OFME_IIC_DEV_H__
#define __OFME_IIC_DEV_H__

////////////////////////////////////////////////////////////////////////////////
//IIC�豸�������ϵĵ�ַ
#define	IIC_ADDR_ADXL345	0xA6	//���ٶȴ�����������ַ
#define	IIC_ADDR_BMP085		0xee	//��ѹ������������ַ
#define	IIC_ADDR_HMC5883L	0x3C	//�ų�������������ַ
#define	IIC_ADDR_L3G4200D	0xD2	//�����Ǵ�����������ַ
////////////////////////////////////////////////////////////////////////////////
// ���ٶȼƽṹ��
typedef struct
{
	signed short x;
	signed short y;
	signed short z;
	unsigned short flag;
}   accelerometer_s, *accelerometer_t;
// �����ǽṹ��
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
