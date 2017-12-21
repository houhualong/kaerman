#include "ofme_iic.h"

////////////////////////////////////////////////////////////////////////////////
// ofme_iic.c
// v2012.12.20
// Copyright(C) ofourme@163.com
// All rights reserved
////////////////////////////////////////////////////////////////////////////////
// iic ����ģ����� for F/S mode
// ��������
// ���еĺ�����������ռ��scl���ߵģ��ʲ����б��������֮����������������...
// ��������ȫͬ���������������Ҳ�����ڳ��������ڼ���뾺����
////////////////////////////////////////////////////////////////////////////////
// �����I2C���߹淶������������棩��
// �����ʽ��P8~P10
// ͬ�����ٲã�P10~P11
// ʱ��Ҫ��P27~P28
// tag: ͬ��������������Ծ������ʱ��
//      �ٲ÷����������������ݵ����������ACKλ
////////////////////////////////////////////////////////////////////////////////
// ȫ�ֱ��������ڱ���IIC_TIME_OUT�ȴ�����Ϣ��ͨ��iic_error_check()��ȡ��
static int _IIC_ERROR_CODE = IIC_OK;
////////////////////////////////////////////////////////////////////////////////
#ifndef F_SCL
    #err "F_SCL not defined."
#elif (F_SCL==100)
////////////////////////////////////////////////////////////////////////////////
// ���ظ�����ʼ�����ı���ʱ�䡣��������ں󣬲�����һ��ʱ������...
//  4.0us<t_hd_sta or 0.6us<t_hd_sta
#define T_HD_STA (4)
// SCLʱ�ӵĵ͵�ƽ���� 4.7us<t or 1.3us<t
// SDA should hold at least 300ns while SCL is falling
#define T_LOW1  (1)
#define T_LOW2  (4)
#define T_LOW   (T_LOW1+T_LOW2)
// SCLʱ�ӵĸߵ�ƽ���� 4.0us<t or 0.6us<t
#define T_HIGH  (4)
// �ظ���ʼ�����Ľ���ʱ�� 4.7us<t or 0.6us<t
#define T_SU_STA (5)
// ���ݱ���ʱ�䣺
// IIC����������0<t<3.45us or 0<t<0.9us
// ����CUBS��������5.0us<t<NULL or NULL<t<NULL
// SDA should hold at least 300ns while SCL is falling
#define T_HD_DAT (1)
// ���ݽ���ʱ�䣺250ns<t or 100ns<t
#define T_SU_DAT (1)
// ֹͣ�����Ľ���ʱ�䣺4.0us<t or 0.6us<t
#define T_SU_STO (4)
// ֹͣ����������֮������߿���ʱ�� 4.7us<t_buf or 1.3us<t_buf
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
// �ṩ��iic����ʱ��������λΪ1΢�롣
#ifndef iic_delay
    #err "iic_delay() not defined."
#endif
////////////////////////////////////////////////////////////////////////////////
// �����ͷ�SCL���ߣ����ҵȴ��ⲿ�������豸�ͷ�SCL���ߡ�����SCLͬ����
/* "IIC SCL SYNCHRONIZE." �������쳣���󣬹ʴ�д��ʾ��*/
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
// ����ʱ��T�ĸߵ�ƽ������������ߵ�ƽ���ⲿ���ͣ�����ǰ�˳�������SCLͬ����
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
	hw_iic_init(); // �ⲿ���������ö˿�Ϊ��©�����
//	_IIC_ERROR_CODE = IIC_OK;	// �Ӻ�iic_start()�������á�
	SCL_H();		// �ͷŶ˿�
	SDA_H();
}
////////////////////////////////////////////////////////////////////////////////
// iic_start()����������æµ������·���ʧ��ֵ
int iic_start(void)
{
// �����������ܴ���<1>start��<2>restart��<3>stop��<4>��дSDA��SDAΪ�ߵ�ƽ��
// �п��ܶ�ռ���߻���������ͬ��ռ�����ߣ���������ϣ������ý����
// ��iicЭ���Ƿ��п��ܵ��²�ͬ����ռ�����ߣ�
//
// ����ʵ����Ӧ�ü��������T_BUF�ڼ��Ƿ�ռ�ã���֤��ʼ��־ʱ�򲻱���ϣ�...
// ��ʹ�������ѯ��ʽ�޷�ȷ���϶�����ʱ�ڼ����ߵ�ƽû�б��ⲿ�������ͣ�...
// �������ȱ���п��ܵ��²�ͬ����ռ�����ߣ�����
// ֻ�ܼ�ϣ���ڳ����ں���Ķ�����������ʧ���˳�����������ˡ�


	_IIC_ERROR_CODE = IIC_OK;

    SCL_H();
    SDA_H();
    iic_delay(T_BUF+T_BUF_ALT);	// ��֤SCL��SDA�ߵĸߵ�ƽά��ʱ��

    if( SCL==0 )    	// SCL����æ
	{
        return IIC_SCL_BUSY;
	}
    if( SDA==0 )    	// SDA����æ
	{
		return IIC_SDA_BUSY;
	}
	
    SDA_L();
    iic_delay(T_HD_STA);
    SCL_L();    // get the SCL & SDA bus

    return IIC_OK;
}

////////////////////////////////////////////////////////////////////////////////
// �ڴ��������ݺ����������iic_restart()����iic_start()���ơ�
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
// �����������ݷ���ack, �Ƚ���Ӧλ���ж������ٲã�����ackΪ�͵�ƽ����ʵ�ʲ��ٲ�
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
// �����������ݷ���nack, �Ƚ���Ӧλ���ж������ٲ�
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
		// Ӧ�ò����ٷ���ʱ��ֱ��nack���ڽ�����
        return IIC_AARB_FAIL;
    }
    IIC_SCL_HOLD(T_HIGH, t);         // SCL SYNCHRONIZE
    SCL_L();    // get the SCL bus

    return IIC_OK;
}
////////////////////////////////////////////////////////////////////////////////
// ��������������ȴ��ӻ���Ӧack or nack�������ж������ٲ�
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
// ������ȡ���ݣ����Ƚ�����λ���ж������ٲ�
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
// ������read������write�����������շ��ֽڵĵ�1λʱ��Ӧ���ϼ��SCL�ߵ�ƽ�ڼ䣬...
// SDA�ĵ�ƽ���ޱ仯��ʶ��restart()��stop()��־����ͬʱ��Ҫ���SCL���ޱ��ⲿ����...
// �ڲ�ʹ���ж϶����ô����ѯ�ֶε�����£�ʵ�����������ѣ��ʲ����жϡ�
        IIC_SCL_HOLD(T_HIGH, t);    // SCL SYNCHRONIZE
        SCL_L();    // get the SCL bus
    }

    return d;
}

////////////////////////////////////////////////////////////////////////////////
// �����������ݣ��Ƚ�����λ���ж������ٲ�
// �����ڷ������ݵĵ�һλ�ҵ�һλΪ1ʱ����������������SCL�ߵ�ƽ�ڼ䷢��...
// restart()����stop()��־��Ҳ����ƽ0->1����1->0�������ϳ���Ӧ�ü������...
// ����ķ�������ֹͣ�������ݶ�����һ����restart����stop��־����P11����
// Ϊ�򻯳���һ���������������ת��ΪIIC_ARB_FAIL����
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
       	if( data&0x80 && (SDA==0) )//�ٲ�ʧ��
       	{   // scl & sda had been released before.
			// �������ٲ�ʧ�ܾ������������ӹܿ��������������ֹͣ����SCL...
			// ����������Ӧ�ÿ���ֱ�ӷ��� IIC_DARB_FAIL
            // return IIC_DARB_FAIL;
			// ����ѡ����0xffֱ���ֽڽ���
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
// ע�⽫IIC_DEBUG()��iic_stop()���棬����Ӱ������ʱ��

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
//  �����ٲ�ʧ�ܿ���������Ӳ��������Ƕ����������������Ӳ������Ӧ��������...
//  ʱ�ӵ��ֽڴ��������Ȼ���ͷ����ߣ���������������Ӧ���ٵ���iic_stop();
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
//  �����ٲ�ʧ�ܿ���������Ӳ��������Ƕ����������������Ӳ������Ӧ��������...
//  ʱ�ӵ��ֽڴ��������Ȼ���ͷ����ߣ���������������Ӧ���ٵ���iic_stop();
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
// ע�⽫IIC_DEBUG()��iic_stop()���棬����Ӱ������ʱ��

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
// �������IIC_NACK�������ټ������ӻ�д���ݡ�������ֻдһ�ֽڵ����ݣ��ʺ��ԡ�
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
//  �����ٲ�ʧ�ܿ���������Ӳ��������Ƕ����������������Ӳ������Ӧ��������...
//  ʱ�ӵ��ֽڴ��������Ȼ���ͷ����ߣ���������������Ӧ���ٵ���iic_stop();
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
// ע�⽫IIC_DEBUG()��iic_stop()���棬����Ӱ������ʱ��

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
   	iic_wait_ack();	// ���һ���ֽڣ�����ack��


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
//  �����ٲ�ʧ�ܿ���������Ӳ��������Ƕ����������������Ӳ������Ӧ��������...
//  ʱ�ӵ��ֽڴ��������Ȼ���ͷ����ߣ���������������Ӧ���ٵ���iic_stop();
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
