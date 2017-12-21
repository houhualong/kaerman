#ifndef __OFME_IIC_H__
#define __OFME_IIC_H__

////////////////////////////////////////////////////////////////////////////////
// 与移植有关的配置

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
// 调试与优化相关配置
#define IIC_DEBUG(A)  printf(A)
//#define IIC_DEBUG(A)
// SCL时钟频率 fscl: 100kHz or 400kHz
#define F_SCL   (400)
// 释放SCL并查询的次数，如果SCL保持被外部拉低，则退出，并设置_IIC_ERROR_COUDE为：IIC_TIME_OUT
#define IIC_TIME_OUT_COUNT	(255)
// 释放SCL并查询的次数，如果SCL保持被外部拉低，则DEBUG提示。
/*delay for voltage raising time*/
#define IIC_RAISING_COUNT	(25)
// 虽然T_BUF的值是按照规范选取的，硬件可能需要更长的延时时间。...
// 如果提示bus busy，可尝试设置一定的数值N以延长初始化时间，否则请设为0。
#define T_BUF_ALT	(0)

////////////////////////////////////////////////////////////////////////////////
// IIC函数返回值
#define IIC_OK          (0x00)
// SCL总线被外部设备拉低
#define IIC_SCL_BUSY    (0x01)
// SDA总线被外部设备拉低
#define IIC_SDA_BUSY    (0x02)
#define IIC_ACK         (0x04)
#define IIC_NACK        (0x08)
//arbitrate fail/被其它主机抢占
#define IIC_AARB_FAIL   (0x10)
#define IIC_DARB_FAIL   (0x20)
// 总线设备没有响应
#define IIC_DEVICE_FAIL	(IIC_NACK|0x00)
// 设备寄存器无响应
#define IIC_TARGET_FAIL (IIC_NACK|0x40)
// iic_start()正常启动后的内部传输期间的IIC_SCL_BUSY，但不终止传输
#define IIC_TIME_OUT	(0x80)

////////////////////////////////////////////////////////////////////////////////
// 可供调用函数
void iic_init(void);
int  iic_start(void); // 返回IIC_SCL_BUSY、IIC_SDA_BUSY或IIC_OK
void iic_restart(void);
void iic_stop(void);
void iic_ack(void);
int  iic_nack(void);  // 返回IIC_AARB_FAIL或IIC_OK
int  iic_wait_ack(void);// 返回IIC_ACK或IIC_NACK
u8   iic_read(void);  // 返回读取的数值
int  iic_write(u8 data); // 返回IIC_DARB_FAIL或IIC_OK
int  iic_dev_read (u8 dev, u8 addr, u8* data);
int  iic_dev_write(u8 dev, u8 addr, u8  data);
int  iic_dev_gets(u8 dev, u8 addr, u8* data, u16 n);
int  iic_dev_puts(u8 dev, u8 addr, u8* data, u16 n);
////////////////////////////////////////////////////////////////////////////////

#endif
