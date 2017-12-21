#ifndef __OFME_IIC_H__
#define __OFME_IIC_H__

////////////////////////////////////////////////////////////////////////////////
// ����ֲ�йص�����

#include "sys.h"
#include "stdio.h"

//extern void hw_delay_us(u32 nus);
#include "ofme_time.h"
extern void hw_iic_init(void);
#define iic_delay(t_us) hw_delay_us(t_us)
////////////////////////////////////////
// IIC_PORT(1): SCL(PC12), SDA(PC11).
// IIC_PORT(2): SCL(PC5/NRF_IRQ/JF24_BKT), SDA(PA7/SPI1_MOSI), GND(PC4/NRF_CS/JF24_CS)
#define IIC_PORT (1)
////////////////////////////////////////
#if (IIC_PORT==1)
	#define SCL 	PCin(12)
	#define SCL_L()	PCout(12)=0
	#define SCL_H()	PCout(12)=1
	#define SDA		PCin(11)
	#define SDA_L()	PCout(11)=0
	#define SDA_H()	PCout(11)=1
#elif (IIC_PORT==2)
	#define SCL 	PCin(5)
	#define SCL_L()	PCout(5)=0
	#define SCL_H()	PCout(5)=1
	#define SDA		PAin(7)
	#define SDA_L()	PAout(7)=0
	#define SDA_H()	PAout(7)=1
#else
	#error "IIC_PORT not defined."
#endif

////////////////////////////////////////////////////////////////////////////////
// �������Ż��������
#define IIC_DEBUG(A)  printf(A)
//#define IIC_DEBUG(A)
// SCLʱ��Ƶ�� fscl: 100kHz or 400kHz
#define F_SCL   (400)
// �ͷ�SCL����ѯ�Ĵ��������SCL���ֱ��ⲿ���ͣ����˳���������_IIC_ERROR_COUDEΪ��IIC_TIME_OUT
#define IIC_TIME_OUT_COUNT	(255)
// �ͷ�SCL����ѯ�Ĵ��������SCL���ֱ��ⲿ���ͣ���DEBUG��ʾ��
/*delay for voltage raising time*/
#define IIC_RAISING_COUNT	(25)
// ��ȻT_BUF��ֵ�ǰ��չ淶ѡȡ�ģ�Ӳ��������Ҫ��������ʱʱ�䡣...
// �����ʾbus busy���ɳ�������һ������ֵN���ӳ���ʼ��ʱ�䣬��������Ϊ0��
#define T_BUF_ALT	(0)

////////////////////////////////////////////////////////////////////////////////
// IIC��������ֵ
#define IIC_OK          (0x00)
// SCL���߱��ⲿ�豸����
#define IIC_SCL_BUSY    (0x01)
// SDA���߱��ⲿ�豸����
#define IIC_SDA_BUSY    (0x02)
#define IIC_ACK         (0x04)
#define IIC_NACK        (0x08)
//arbitrate fail/������������ռ
#define IIC_AARB_FAIL   (0x10)
#define IIC_DARB_FAIL   (0x20)
// �����豸û����Ӧ
#define IIC_DEVICE_FAIL	(IIC_NACK|0x00)
// �豸�Ĵ�������Ӧ
#define IIC_TARGET_FAIL (IIC_NACK|0x40)
// iic_start()������������ڲ������ڼ��IIC_SCL_BUSY��������ֹ����
#define IIC_TIME_OUT	(0x80)

////////////////////////////////////////////////////////////////////////////////
// �ɹ����ú���
void iic_init(void);
int  iic_start(void); // ����IIC_SCL_BUSY��IIC_SDA_BUSY��IIC_OK
void iic_restart(void);
void iic_stop(void);
void iic_ack(void);
int  iic_nack(void);  // ����IIC_AARB_FAIL��IIC_OK
int  iic_wait_ack(void);// ����IIC_ACK��IIC_NACK
u8   iic_read(void);  // ���ض�ȡ����ֵ
int  iic_write(u8 data); // ����IIC_DARB_FAIL��IIC_OK
int  iic_dev_read (u8 dev, u8 addr, u8* data);
int  iic_dev_write(u8 dev, u8 addr, u8  data);
int  iic_dev_gets(u8 dev, u8 addr, u8* data, u16 n);
int  iic_dev_puts(u8 dev, u8 addr, u8* data, u16 n);
////////////////////////////////////////////////////////////////////////////////

#endif
