#include "ofme_iic.h"

////////////////////////////////////////////////////////////////////////////////
// ofme_iic.c
// v2012.12.20
// Copyright(C) ofourme@163.com
// All rights reserved
////////////////////////////////////////////////////////////////////////////////
// iic 主机模拟程序 for F/S mode
// 不可重入
// 所有的函数结束后都是占有scl总线的，故不会有别的主机与之竞争（除非与别的主...
// 机处于完全同步），但别的主机也可能在程序运行期间加入竞争。
////////////////////////////////////////////////////////////////////////////////
// 详见《I2C总线规范（周立功翻译版）》
// 传输格式：P8~P10
// 同步与仲裁：P10~P11
// 时序要求：P27~P28
// tag: 同步发生于主机活跃的所有时间
//      仲裁发生于主机发送数据的情况，包括ACK位
////////////////////////////////////////////////////////////////////////////////
// 全局变量，用于保存IIC_TIME_OUT等错误信息。通过iic_error_check()获取。
static int _IIC_ERROR_CODE = IIC_OK;
////////////////////////////////////////////////////////////////////////////////
#ifndef F_SCL
    #err "F_SCL not defined."
#elif (F_SCL==100)
////////////////////////////////////////////////////////////////////////////////
// （重复）起始条件的保持时间。在这个周期后，产生第一个时间脉冲...
//  4.0us<t_hd_sta or 0.6us<t_hd_sta
#define T_HD_STA (4)
// SCL时钟的低电平周期 4.7us<t or 1.3us<t
// SDA should hold at least 300ns while SCL is falling
#define T_LOW1  (1)
#define T_LOW2  (4)
#define T_LOW   (T_LOW1+T_LOW2)
// SCL时钟的高电平周期 4.0us<t or 0.6us<t
#define T_HIGH  (4)
// 重复起始条件的建立时间 4.7us<t or 0.6us<t
#define T_SU_STA (5)
// 数据保持时间：
// IIC总线器件：0<t<3.45us or 0<t<0.9us
// 兼容CUBS的主机：5.0us<t<NULL or NULL<t<NULL
// SDA should hold at least 300ns while SCL is falling
#define T_HD_DAT (1)
// 数据建立时间：250ns<t or 100ns<t
#define T_SU_DAT (1)
// 停止条件的建立时间：4.0us<t or 0.6us<t
#define T_SU_STO (4)
// 停止和启动条件之间的总线空闲时间 4.7us<t_buf or 1.3us<t_buf
#define T_BUF   (5)
////////////////////////////////////////////////////////////////////////////////
#elif (F_SCL==400)
////////////////////////////////////////////////////////////////////////////////
#define T_HD_STA    (1)
#define T_LOW1      (1)
#define T_LOW2      (1)
#define T_LOW       (T_LOW1+T_LOW2)
#define T_HIGH      (1)
#define T_SU_STA    (1)
#define T_HD_DAT    (0)
#define T_SU_DAT    (1)
#define T_SU_STO    (1)
#define T_BUF       (2)
////////////////////////////////////////////////////////////////////////////////
#else
#err "F_SCL value error."
#endif
////////////////////////////////////////////////////////////////////////////////
// 提供给iic的延时函数，单位为1微秒。
#ifndef iic_delay
    #err "iic_delay() not defined."
#endif
////////////////////////////////////////////////////////////////////////////////
// 主机释放SCL总线，而且等待外部主机、设备释放SCL总线。用于SCL同步。
/* "IIC SCL SYNCHRONIZE." 不属于异常错误，故大写表示。*/
#define IIC_SCL_RELEASE(t) \
{\
    SCL_H();\
	t = 0;\
	while(SCL==0)\
	{\
		t++;\
		if(t==IIC_RAISING_COUNT) IIC_DEBUG("IIC SCL SYNCHRONIZE.\r\n");\
		if(t>=IIC_TIME_OUT_COUNT)\
		{\
			_IIC_ERROR_CODE = IIC_TIME_OUT;\
			IIC_DEBUG("iic scl synchronize time out.\r\n");\
			break;\
		}\
	}\
}
////////////////////////////////////////////////////////////////////////////////
// 保持时间T的高电平。但是如果总线电平被外部拉低，则提前退出。用于SCL同步。
#define IIC_SCL_HOLD(T, t) \
{\
    for(t=0; t<T; t++)\
    {\
        iic_delay(1);\
        if(SCL==0) break;\
    }\
}
////////////////////////////////////////////////////////////////////////////////
void iic_init(void)
{
	hw_iic_init(); // 外部函数。配置端口为开漏输出。
//	_IIC_ERROR_CODE = IIC_OK;	// 延后到iic_start()里面设置。
	SCL_H();		// 释放端口
	SDA_H();
}
////////////////////////////////////////////////////////////////////////////////
// iic_start()函数在总线忙碌的情况下返回失败值
int iic_start(void)
{
// 其它主机可能处于<1>start、<2>restart、<3>stop、<4>读写SDA且SDA为高电平。
// 有可能独占总线或与别的主机同步占有总线，这是我们希望的最好结果。
// 但iic协议是否有可能导致不同步地占有总线？
//
// 程序实际上应该检查总线在T_BUF期间是否被占用，保证起始标志时序不被打断，...
// 但使用软件查询方式无法确切认定在延时期间总线电平没有被外部主机拉低，...
// 本程序的缺陷有可能导致不同步地占有总线！！！
// 只能寄希望于程序在后面的多主机竞争中失败退出而避免错误了。


	_IIC_ERROR_CODE = IIC_OK;

    SCL_H();
    SDA_H();
    iic_delay(T_BUF+T_BUF_ALT);	// 保证SCL与SDA线的高电平维持时间

    if( SCL==0 )    	// SCL总线忙
	{
        return IIC_SCL_BUSY;
	}
    if( SDA==0 )    	// SDA总线忙
	{
		return IIC_SDA_BUSY;
	}
	
    SDA_L();
    iic_delay(T_HD_STA);
    SCL_L();    // get the SCL & SDA bus

    return IIC_OK;
}

////////////////////////////////////////////////////////////////////////////////
// 在传输完数据后可立即调用iic_restart()，与iic_start()类似。
void iic_restart(void)
{
	int t;
// scl==0
    SDA_H();
    iic_delay(T_LOW);
    IIC_SCL_RELEASE(t);
    iic_delay(T_SU_STA);
    SDA_L();
    iic_delay(T_HD_STA);
    SCL_L();    // get the SCL & SDA bus
}
////////////////////////////////////////////////////////////////////////////////
void iic_stop(void)
{
// scl==0
    SDA_L();
    iic_delay(T_LOW);
	SCL_H();	// release SCL, ignore pulling down by other device.
    iic_delay(T_SU_STO);
    SDA_H();    // release SDA
}

////////////////////////////////////////////////////////////////////////////////
// 主机接收数据发送ack, 比较响应位进行多主机仲裁，由于ack为低电平，故实际不仲裁
void iic_ack(void)
{
    int t;

// scl==0
    SDA_L();    // ack
    iic_delay(T_LOW);
    IIC_SCL_RELEASE(t);         // SCL SYNCHRONIZE
    IIC_SCL_HOLD(T_HIGH, t);   // SCL SYNCHRONIZE
    SCL_L();    // get the SCL bus
}

////////////////////////////////////////////////////////////////////////////////
// 主机接收数据发送nack, 比较响应位进行多主机仲裁
int iic_nack(void)
{
    int t;

// scl==0
    SDA_H();  // nack
    iic_delay(T_LOW);
    IIC_SCL_RELEASE(t);         // SCL SYNCHRONIZE
    if(SDA==0)
    {   // scl & sda had been released before.
		IIC_DEBUG("iic_nack() arbitrate failed.\r\n");
		// 应该不用再发送时钟直到nack周期结束？
        return IIC_AARB_FAIL;
    }
    IIC_SCL_HOLD(T_HIGH, t);         // SCL SYNCHRONIZE
    SCL_L();    // get the SCL bus

    return IIC_OK;
}
////////////////////////////////////////////////////////////////////////////////
// 主机发送数据完等待从机响应ack or nack，不进行多主机仲裁
int iic_wait_ack(void)
{
    int t, data;

// scl==0
    SDA_H();            // release SDA
    iic_delay(T_LOW);   // wait for SDA to be change
    IIC_SCL_RELEASE(t);	// SCL SYNCHRONIZE
	data = SDA;
    IIC_SCL_HOLD(T_HIGH, t);  // SCL SYNCHRONIZE
    SCL_L();    // get the SCL bus

    if(data) return IIC_NACK;
    else     return IIC_ACK;
}
////////////////////////////////////////////////////////////////////////////////
// 主机读取数据，不比较数据位进行多主机仲裁
u8 iic_read(void)
{
    u8 d;
    int i, t;

// sda==0, scl==0;
    SDA_H(); // release SDA, wait for SDA to be change
    for(i=0, d=0; i<8; i++)
    {
        iic_delay(T_LOW);
        IIC_SCL_RELEASE(t);          // SCL SYNCHRONIZE
//      read_bit();
        d<<=1;
        if(SDA) d++;
// 理论上read函数和write函数在这里收发字节的第1位时，应不断检测SCL高电平期间，...
// SDA的电平有无变化以识别restart()或stop()标志，但同时还要检测SCL有无被外部拉低...
// 在不使用中断而采用纯粹查询手段的情况下，实现起来有困难，故不做判断。
        IIC_SCL_HOLD(T_HIGH, t);    // SCL SYNCHRONIZE
        SCL_L();    // get the SCL bus
    }

    return d;
}

////////////////////////////////////////////////////////////////////////////////
// 主机发送数据，比较数据位进行多主机仲裁
// 主机在发送数据的第一位且第一位为1时，其它主机可能在SCL高电平期间发送...
// restart()或是stop()标志，也即电平0->1或是1->0，理论上程序应该检测这种...
// 情况的发生，并停止发送数据而发送一样的restart或是stop标志（见P11）。
// 为简化程序，一旦遇到这种情况既转化为IIC_ARB_FAIL处理。
int iic_write(u8 data)
{
    int i, t, err = IIC_OK;

// sda==0, scl==0;
    for(i=0; i<8; i++, data<<=1)
    {
        iic_delay(T_LOW1);

//		send_bit();
       	if(data&0x80)
           	SDA_H();
       	else
           	SDA_L();
//
	    iic_delay(T_LOW2);
   	    IIC_SCL_RELEASE(t);          // SCL SYNCHRONIZE
       	if( data&0x80 && (SDA==0) )//仲裁失败
       	{   // scl & sda had been released before.
			// 理论上仲裁失败就由其它主机接管控制器，程序可以停止产生SCL...
			// 在这里我们应该可以直接返回 IIC_DARB_FAIL
            // return IIC_DARB_FAIL;
			// 但我选择发送0xff直到字节结束
			err = IIC_DARB_FAIL;
			data = 0xFF;
        }
        IIC_SCL_HOLD(T_HIGH, t);    // SCL SYNCHRONIZE
        SCL_L();    // get the SCL bus
    }

    return err;
}

////////////////////////////////////////////////////////////////////////////////
#if 0
int iic_dev_read(u8 dev, u8 addr, u8* data)
{
// 注意将IIC_DEBUG()放iic_stop()后面，以免影响总线时序。

    int i;

    i = iic_start();  // select the device and set address
    if( i != IIC_OK ) goto err_bus_busy;
    i = iic_write(dev);
    if( i != IIC_OK ) goto err_arb_fail;
    i = iic_wait_ack();
    if( i != IIC_ACK) goto err_dev_fail;
    i = iic_write(addr);
    if( i != IIC_OK ) goto err_arb_fail;
    i = iic_wait_ack();
    if( i != IIC_ACK) goto err_tar_fail;

    iic_restart();
    i = iic_write(dev|1);  // start read
    if( i != IIC_OK ) goto err_arb_fail;
    i = iic_wait_ack();
    if( i != IIC_ACK) goto err_dev_fail;
    *data = iic_read();
    i = iic_nack();// write nack to tell the slave stop transfer data.
    if( i != IIC_OK ) goto err_arb_fail;

//end:
    iic_stop();
//	IIC_DEBUG("R: IIC READ DONE.\r\n");
	if(_IIC_ERROR_CODE & IIC_TIME_OUT)
	{
		IIC_DEBUG("r: iic time out.\r\n");
		return _IIC_ERROR_CODE;
	}
    return IIC_OK;
err_bus_busy:
	if(i == IIC_SCL_BUSY)
		IIC_DEBUG("r: iic scl bus busy.\r\n");
	else
		IIC_DEBUG("r: iic sda bus busy.\r\n");
	return i | _IIC_ERROR_CODE;
err_arb_fail:
//  总线仲裁失败可能是由于硬件错误或是多主机竞争。如果是硬件错误，应继续产生...
//  时钟到字节传输结束，然后释放总线？不管怎样，都不应该再调用iic_stop();
	SDA_H();
	SCL_H();
	IIC_DEBUG("r: iic bus arbitrate failed.\r\n");
	return i | _IIC_ERROR_CODE;	// IIC_ARB_FAIL
err_dev_fail:
	iic_stop();
	IIC_DEBUG("r: iic device not respond.\r\n");
	return IIC_DEVICE_FAIL | _IIC_ERROR_CODE;
err_tar_fail:
	iic_stop();
	IIC_DEBUG("r: device target not respond.\r\n");
	return IIC_TARGET_FAIL | _IIC_ERROR_CODE;
}
#else
int iic_dev_read(u8 dev, u8 addr, u8* data)
{
	return iic_dev_gets(dev, addr, data, 1);
}

#endif
////////////////////////////////////////////////////////////////////////////////

int iic_dev_gets(u8 dev, u8 addr, u8* data, u16 n)
{
    int i;

    i = iic_start();  // select the device and set address
    if( i != IIC_OK ) goto err_bus_busy;
    i = iic_write(dev);
    if( i != IIC_OK ) goto err_arb_fail;
    i = iic_wait_ack();
    if( i != IIC_ACK) goto err_dev_fail;
    i = iic_write(addr);
    if( i != IIC_OK ) goto err_arb_fail;
    i = iic_wait_ack();
    if( i != IIC_ACK) goto err_tar_fail;

    iic_restart();
    i = iic_write(dev|1);  // start read
    if( i != IIC_OK ) goto err_arb_fail;
    i = iic_wait_ack();
    if( i != IIC_ACK) goto err_dev_fail;
	if(n<1) n=1;
	while(--n)
    {
		*data++ = iic_read();
		iic_ack();
	}
	*data = iic_read();
    i = iic_nack();// write nack to tell the slave stop transfer data.
    if( i != IIC_OK ) goto err_arb_fail;

//end:
    iic_stop();
//	IIC_DEBUG("R: IIC READ DONE.\r\n");
	if(_IIC_ERROR_CODE & IIC_TIME_OUT)
	{
		IIC_DEBUG("r: iic time out.\r\n");
		return _IIC_ERROR_CODE;
	}
    return IIC_OK;
err_bus_busy:
	if(i == IIC_SCL_BUSY)
		IIC_DEBUG("r: iic scl bus busy.\r\n");
	else
		IIC_DEBUG("r: iic sda bus busy.\r\n");
	return i | _IIC_ERROR_CODE;
err_arb_fail:
//  总线仲裁失败可能是由于硬件错误或是多主机竞争。如果是硬件错误，应继续产生...
//  时钟到字节传输结束，然后释放总线？不管怎样，都不应该再调用iic_stop();
	SDA_H();
	SCL_H();
	IIC_DEBUG("r: iic bus arbitrate failed.\r\n");
	return i | _IIC_ERROR_CODE;	// IIC_ARB_FAIL
err_dev_fail:
	iic_stop();
	IIC_DEBUG("r: iic device not respond.\r\n");
	return IIC_DEVICE_FAIL | _IIC_ERROR_CODE;
err_tar_fail:
	iic_stop();
	IIC_DEBUG("r: device target not respond.\r\n");
	return IIC_TARGET_FAIL | _IIC_ERROR_CODE;
}


////////////////////////////////////////////////////////////////////////////////
#if 0
int iic_dev_write(u8 dev, u8 addr, u8 data)
{
// 注意将IIC_DEBUG()放iic_stop()后面，以免影响总线时序。

    int i;

    i = iic_start();
    if( i != IIC_OK ) goto err_bus_busy;
    i = iic_write(dev);
    if( i != IIC_OK ) goto err_arb_fail;
    i = iic_wait_ack();
    if( i != IIC_ACK) goto err_dev_fail;
    i = iic_write(addr);
    if( i != IIC_OK ) goto err_arb_fail;
    i = iic_wait_ack();
    if( i != IIC_ACK) goto err_tar_fail;
    i = iic_write(data);
    if( i != IIC_OK ) goto err_arb_fail;
// 如果返回IIC_NACK，则不能再继续往从机写数据。本函数只写一字节的数据，故忽略。
    i = iic_wait_ack();
//end:
    iic_stop();
//	IIC_DEBUG("W: IIC WRITE DONE.\r\n");
	if( i != IIC_ACK)
		IIC_DEBUG("w: IIC DEVICE NO ACK.\r\n");
	if(_IIC_ERROR_CODE & IIC_TIME_OUT)
	{
		IIC_DEBUG("w: iic time out.\r\n");
		return _IIC_ERROR_CODE;
	}
    return IIC_OK;
err_bus_busy:
	if(i == IIC_SCL_BUSY)
		IIC_DEBUG("w: iic scl bus busy.\r\n");
	else
		IIC_DEBUG("w: iic sda bus busy.\r\n");
	return i | _IIC_ERROR_CODE;
err_arb_fail:
//  总线仲裁失败可能是由于硬件错误或是多主机竞争。如果是硬件错误，应继续产生...
//  时钟到字节传输结束，然后释放总线？不管怎样，都不应该再调用iic_stop();
	SDA_H();
	SCL_H();
	IIC_DEBUG("w: iic bus arbitrate failed.\r\n");
	return i | _IIC_ERROR_CODE;	// IIC_ARB_FAIL
err_dev_fail:
	iic_stop();
	IIC_DEBUG("w: iic device not respond.\r\n");
	return IIC_DEVICE_FAIL | _IIC_ERROR_CODE;
err_tar_fail:
	iic_stop();
	IIC_DEBUG("w: device target not respond.\r\n");
	return IIC_TARGET_FAIL | _IIC_ERROR_CODE;
}

#else

int iic_dev_write(u8 dev, u8 addr, u8 data)
{
	u8 buf = data;
	return iic_dev_puts(dev, addr, &buf, 1);
}

#endif

////////////////////////////////////////////////////////////////////////////////

int iic_dev_puts(u8 dev, u8 addr, u8* data, u16 n)
{
// 注意将IIC_DEBUG()放iic_stop()后面，以免影响总线时序。

    int i;

    i = iic_start();
    if( i != IIC_OK ) goto err_bus_busy;
    i = iic_write(dev);
    if( i != IIC_OK ) goto err_arb_fail;
    i = iic_wait_ack();
    if( i != IIC_ACK) goto err_dev_fail;
    i = iic_write(addr);
    if( i != IIC_OK ) goto err_arb_fail;
    i = iic_wait_ack();
    if( i != IIC_ACK) goto err_tar_fail;

	if(n<1) n=1;
	while(--n)
	{
		i = iic_write(*data++);
    	if( i != IIC_OK ) goto err_arb_fail;
    	i = iic_wait_ack();
		if( i != IIC_ACK) goto err_tar_fail;	//could not write data.
	}
	i = iic_write(*data);
   	if( i != IIC_OK ) goto err_arb_fail;
   	iic_wait_ack();	// 最后一个字节，忽略ack。


//end:
    iic_stop();
//	IIC_DEBUG("W: IIC WRITE DONE.\r\n");

	if(_IIC_ERROR_CODE & IIC_TIME_OUT)
	{
		IIC_DEBUG("w: iic time out.\r\n");
		return _IIC_ERROR_CODE;
	}
    return IIC_OK;
err_bus_busy:
	if(i == IIC_SCL_BUSY)
		IIC_DEBUG("w: iic scl bus busy.\r\n");
	else
		IIC_DEBUG("w: iic sda bus busy.\r\n");
	return i | _IIC_ERROR_CODE;
err_arb_fail:
//  总线仲裁失败可能是由于硬件错误或是多主机竞争。如果是硬件错误，应继续产生...
//  时钟到字节传输结束，然后释放总线？不管怎样，都不应该再调用iic_stop();
	SDA_H();
	SCL_H();
	IIC_DEBUG("w: iic bus arbitrate failed.\r\n");
	return i | _IIC_ERROR_CODE;	// IIC_ARB_FAIL
err_dev_fail:
	iic_stop();
	IIC_DEBUG("w: iic device not respond.\r\n");
	return IIC_DEVICE_FAIL | _IIC_ERROR_CODE;
err_tar_fail:
	iic_stop();
	IIC_DEBUG("w: device target not respond.\r\n");
	return IIC_TARGET_FAIL | _IIC_ERROR_CODE;
}

////////////////////////////////////////////////////////////////////////////////
