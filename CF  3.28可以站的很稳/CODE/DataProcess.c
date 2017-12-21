/** ###################################################################
**     Filename  : DataProcess.C
**     Project   : CF2_0
**     Processor : MCF52255CAF80
**     Compiler  : CodeWarrior MCF C Compiler
**     Date/Time : 2011-12-15, 6:56
**     Contents  :
**     User source code
**
** ###################################################################*/
/* MODULE DataProcess */
#include "DataProcess.h"
#include "LED0.h"
#include "math.h"
#include "Kalman.h"
#include "Speed_PID.h"

#define z_acc_scale 0.2309228 //5V: 0.3497057; 3.3V: 0.2308058
#define gyro_scale  2.5182904 //5V: 0.7287780; 3.3V: 0.4809935
#define MAXANGLE 40.0
#define MINANGLE -40.0
int16   MAXSPEED = 99;
int8  direction_R = 1;
int8  direction_L = 1;
int   cnt = 0;
bool OneLoop = FALSE;
bool Loop_100ms = FALSE;

uint16 ADValue[8];

int16 adval_gyro = 0;
int16 adval_acc = 0;

int16 Offset_z_acc = 593;
int16 Offset_gyro = 472;
float z_acc =0, gyro = 0;
int16 z_acc_X10 = 0, gyro_X10 = 0;

int16 dutyL = 0;
int16 dutyR = 0;
int16 Kp = 320;   //          785  835 340
int16 Kd = 4;   //PD�������� 19 23

//==========================================
//�������DDuty_Set()
//���ã����ռ�ձ����� 0 -  50% - 100% <---> -32767 - 0 - +32767
//==========================================

static void Delay_us(unsigned int t) 
{
    while(t--)
    ;   
}

static void Duty_Set(void)
{
	int32 duty;
	//L
	duty = dutyL;	
	
	if(duty > 0) duty += 14;
 	else if(duty < 0) duty -= 14;
	if(duty > 500)  duty = 500;
 	else if(duty < -500)  duty = -500;
/* 	if(duty >= 0) 
	{
		if(direction_L == -1)	
		{
		  MotorL_SetRatio16((uint16)(32767));
		  Delay_us(80);
	      direction_L=1;
		}
	}
	else
	{
		if(direction_L == 1) 
		{
			MotorL_SetRatio16((uint16)(32767));
			Delay_us(80);
			direction_L = -1;
		}	
	}*/
	duty <<= 6;		
	MotorL_SetRatio16((uint16)(32767 + duty));
	
	//R
	duty = dutyR;		
	if(duty > 0)  duty += 14;
 	else if(duty < 0) duty -= 14;
 	
 	if(duty > 500)  duty = 500;
 	else if(duty < -500)  duty = -500;
/* 	if(duty >= 0) 
	{
		if(direction_R == -1)	
		{
		  MotorR_SetRatio16((uint16)(32767));
		  Delay_us(80);
	      direction_R=1;
		}
	}
	else
	{
		if(direction_R == 1) 
		{
			MotorR_SetRatio16((uint16)(32767));
			Delay_us(80);
			direction_R = -1;
		}	
	}*/
	duty <<= 6;
	MotorR_SetRatio16((uint16)(32767 + duty));
}


//==========================================
//�������SSensorDataProcess()
//���ã����������ݴ���
//==========================================
void SensorDataProcess(void)
{
	ADCon_GetValue16(ADValue); 
	/************gyro***********/	
	adval_gyro = (int16)(ADValue[GYRO] >> 6);//������
			
	/************acc***********/	
	adval_acc = (int16)(ADValue[ACC] >> 6);//���ٶ�  �ɼ�ʮλAD  ��ƽ��ֵ
	
	/***********angle***********/	
	gyro = (float)((adval_gyro - Offset_gyro) * gyro_scale);	//���ٶ�
		
	z_acc = (float)((adval_acc - Offset_z_acc) * z_acc_scale); //���ٶ�~~���
	
	Kalman(z_acc,gyro);
	
	z_acc_X10 = Angle * 10;//����10�����������������
	gyro_X10 = Angle_dot * 10;
}

//==========================================
//�������CCarAngleCtrl()
//���ã�����Ƕȿ��ƣ��ڻ�
//==========================================
void CarAngleCtrl(void)
{
	int32 duty;
	duty  = (Kp * z_acc_X10 + Kd * gyro_X10) / 100;
	if(duty > 380)
	{
		duty = 380;
	}
	else if(duty < -380)
	{
		duty = -380;
	}
    
//    PI_Out = 0;//������ٶȱջ��轫��ע��
    
	dutyR = (int16)(duty + PI_Out);
	dutyL = (int16)(duty + PI_Out );
	Duty_Set();
}


void CarInit(void)
{

	int16 i;
    uint32 sum = 0;
    uint16 tmp = 0;
    
    LED0_ClrVal();
    
	for(i = 0; i < 1024; i++)
	{
		ADCon_MeasureChan(TRUE, GYRO);
		
		ADCon_GetChanValue16(GYRO, &tmp);
		
		sum += (tmp >> 6);               //ʮλ����
		
		Cpu_Delay100US(5);
	}
	
	
			
	Offset_gyro = (int16)(sum >> 10);   //10λ��ָAD����ֵ2��ʮ�η�
	
	sum = 0;
	
	for(i = 0; i < 1024; i++)
	{
		ADCon_MeasureChan(TRUE, ACC);
		
		ADCon_GetChanValue16(ACC, &tmp);
		
		sum += (tmp >> 6);
		
		Cpu_Delay100US(5);
	}
	
	
	Offset_z_acc = (int16)(sum >> 10);
	
	LED0_SetVal();
	
	adval_gyro = Offset_gyro;
	adval_acc =  Offset_z_acc;
	Angle = 0;
	OneLoop = FALSE;	
}


//	Angle_dot = (float)(gyro * 0.005);
//	Angle =(float) (0.98 * (Angle + Angle_dot) + 0.02 * z_acc);
//	angle_int = (int8)(Angle * 10);
/* END DataProcess */
