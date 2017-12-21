								  
/***********************************************************************
// ������ƽ�⳵���հ���Ƴ���6��MPU6050+�����˲�+PWM����� 
// ��Ƭ��STC12C5A60S2 
// ����20M
// ���ڣ�2012.11.26 - ��
***********************************************************************/

#include <REG52.H>	
#include <math.h>     
#include <stdio.h>   
#include <INTRINS.H>

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

//******����ģ��ͷ�ļ�*******

#include "DELAY.H"		    //��ʱͷ�ļ�
#include "STC_ISP.H"	    //������¼ͷ�ļ�
#include "SET_SERIAL.H"		//����ͷ�ļ�
#include "SET_PWM.H"		//PWMͷ�ļ�
#include "MOTOR.H"			//�������ͷ�ļ�
#include "MPU6050.H"		//MPU6050ͷ�ļ�



//******�ǶȲ���************

float Gyro_y;        //Y�������������ݴ�
float Angle_gy;      //�ɽ��ٶȼ������б�Ƕ�
float Accel_x;	     //X����ٶ�ֵ�ݴ�
float Angle_ax;      //�ɼ��ٶȼ������б�Ƕ�
float Angle;         //С��������б�Ƕ�
uchar value;		 //�Ƕ��������Ա��	

//******PWM����*************

int   speed_mr;		 //�ҵ��ת��
int   speed_ml;		 //����ת��
int   PWM_R;         //����PWMֵ����
int   PWM_L;         //����PWMֵ����
float PWM;           //�ۺ�PWM����
float PWMI;			 //PWM����ֵ

//******�������*************

float speed_r_l;	//���ת��
float speed;        //���ת���˲�
float position;	    //λ��

//******����ң�ز���*************
uchar remote_char;
char  turn_need;
char  speed_need;

//*********************************************************
//��ʱ��100Hz���ݸ����ж�
//*********************************************************

void Init_Timer1(void)	//10����@20MHz,100Hzˢ��Ƶ��
{
	AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TMOD |= 0x10;		//���ö�ʱ��ģʽ
	TL1 = 0xE5;		    //���ö�ʱ��ֵ
	TH1 = 0xBE;		    //���ö�ʱ��ֵ
	TF1 = 0;		    //���TF1��־
	TR1 = 1;		    //��ʱ��1��ʼ��ʱ
}



//*********************************************************
//�жϿ��Ƴ�ʼ��
//*********************************************************

void Init_Interr(void)	 
{
	EA = 1;     //�����ж�
    EX0 = 1;    //���ⲿ�ж�INT0
    EX1 = 1;    //���ⲿ�ж�INT1
    IT0 = 1;    //�½��ش���
    IT1 = 1;    //�½��ش���
	ET1 = 1;    //����ʱ��1�ж�
}



//******����������************
		
float code Q_angle=0.001;  
float code Q_gyro=0.003;
float code R_angle=0.5;
float code dt=0.01;	                  //dtΪkalman�˲�������ʱ��;
char  code C_0 = 1;
float xdata Q_bias, Angle_err;
float xdata PCt_0, PCt_1, E;
float xdata K_0, K_1, t_0, t_1;
float xdata Pdot[4] ={0,0,0,0};
float xdata PP[2][2] = { { 1, 0 },{ 0, 1 } };

//*********************************************************
// �������˲�
//*********************************************************

//Kalman�˲���20MHz�Ĵ���ʱ��Լ0.77ms��

void Kalman_Filter(float Accel,float Gyro)		
{
	Angle+=(Gyro - Q_bias) * dt; //�������

	
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-����������Э�����΢��

	Pdot[1]=- PP[1][1];
	Pdot[2]=- PP[1][1];
	Pdot[3]=Q_gyro;
	
	PP[0][0] += Pdot[0] * dt;   // Pk-����������Э����΢�ֵĻ���
	PP[0][1] += Pdot[1] * dt;   // =����������Э����
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - Angle;	//zk-�������
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //����������Э����
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	Angle	+= K_0 * Angle_err;	 //�������
	Q_bias	+= K_1 * Angle_err;	 //�������
	Gyro_y   = Gyro - Q_bias;	 //���ֵ(�������)��΢��=���ٶ�

}



//*********************************************************
// ��Ǽ��㣨�������ںϣ�
//*********************************************************

void Angle_Calcu(void)	 
{
	//------���ٶ�--------------------------

	//��ΧΪ2gʱ�������ϵ��16384 LSB/g
	//�ǶȽ�Сʱ��x=sinx�õ��Ƕȣ����ȣ�, deg = rad*180/3.14
	//��Ϊx>=sinx,�ʳ���1.3�ʵ��Ŵ�

	Accel_x  = GetData(ACCEL_XOUT_H);	  //��ȡX����ٶ�
	Angle_ax = (Accel_x - 1100) /16384;   //ȥ�����ƫ��,����õ��Ƕȣ����ȣ�
	Angle_ax = Angle_ax*1.2*180/3.14;     //����ת��Ϊ��,


    //-------���ٶ�-------------------------

	//��ΧΪ2000deg/sʱ�������ϵ��16.4 LSB/(deg/s)

	Gyro_y = GetData(GYRO_YOUT_H);	      //��ֹʱ���ٶ�Y�����Ϊ-30����
	Gyro_y = -(Gyro_y + 30)/16.4;         //ȥ�����ƫ�ƣ�������ٶ�ֵ,����Ϊ������ 
	//Angle_gy = Angle_gy + Gyro_y*0.01;  //���ٶȻ��ֵõ���б�Ƕ�.	

	
	//-------�������˲��ں�-----------------------

	Kalman_Filter(Angle_ax,Gyro_y);       //�������˲��������


	/*//-------�����˲�-----------------------

	//����ԭ����ȡ��ǰ��Ǻͼ��ٶȻ����ǲ�ֵ���зŴ�Ȼ����
    //�����ǽ��ٶȵ��Ӻ��ٻ��֣��Ӷ�ʹ��������Ϊ���ٶȻ�õĽǶ�
	//0.5Ϊ�Ŵ������ɵ��ڲ����ȣ�0.01Ϊϵͳ����10ms	
		
	Angle = Angle + (((Angle_ax-Angle)*0.5 + Gyro_y)*0.01);*/
															  
}  



//*********************************************************
//���ת�ٺ�λ��ֵ����
//*********************************************************

void Psn_Calcu(void)	 
{
	
	speed_r_l =(speed_mr + speed_ml)*0.5;
	speed *= 0.7;		                  //�����ٶ��˲�
	speed += speed_r_l*0.3;	
	position += speed;	                  //���ֵõ�λ��
	position += speed_need;
	if(position<-6000) position = -6000;
	if(position> 6000) position =  6000;

	 
}


static float code Kp  = 9.0;       //PID����
static float code Kd  = 2.6;	    //PID����
static float code Kpn = 0.01;      //PID����
static float code Ksp = 2.0;	    //PID����

//*********************************************************
//���PWMֵ����
//*********************************************************

void PWM_Calcu(void)	 
{
    
	if(Angle<-40||Angle>40)               //�Ƕȹ��󣬹رյ�� 
	{  
	  CCAP0H = 0;
      CCAP1H = 0;
	  return;
	}
	PWM  = Kp*Angle + Kd*Gyro_y;          //PID�����ٶȺͽǶ�
	PWM += Kpn*position + Ksp*speed;      //PID���ٶȺ�λ��
	PWM_R = PWM + turn_need;
	PWM_L = PWM - turn_need;
	PWM_Motor(PWM_L,PWM_R); 
	 
}




//*********************************************************
//�ֻ�����ң��
//*********************************************************

void Bluetooth_Remote(void)	 
{

	remote_char = receive_char();				   //����������������

	if(remote_char ==0x02) speed_need = -80;	   //ǰ��
	else if(remote_char ==0x01) speed_need = 80;   //����
	     else speed_need = 0;					   //����

    if(remote_char ==0x03) turn_need = 15;		   //��ת
	else if(remote_char ==0x04) turn_need = -15;   //��ת
	     else turn_need = 0;					   //��ת
	 
}


/*=================================================================================*/

//*********************************************************
//main
//*********************************************************
void main()
{ 

	delaynms(500);	   //�ϵ���ʱ
	Init_PWM();	       //PWM��ʼ��
    Init_Timer0();     //��ʼ����ʱ��0����ΪPWMʱ��Դ
	Init_Timer1();     //��ʼ����ʱ��1
	Init_Interr();     //�жϳ�ʼ��
	Init_Motor();	   //������Ƴ�ʼ��
	Init_BRT();		   //���ڳ�ʼ�������������ʣ�
	InitMPU6050();     //��ʼ��MPU6050
	delaynms(500); 	   

	while(1)
	{
	   
	 Bluetooth_Remote();

	}
}


/*=================================================================================*/

//********timer1�ж�***********************

void Timer1_Update(void) interrupt 3
{
  
   TL1 = 0xE5;		    //���ö�ʱ��ֵ10MS
   TH1 = 0xBE;
   
   //STC_ISP();                    //��������
   Angle_Calcu();                  //��Ǽ���
   Psn_Calcu();                    //���λ�Ƽ���
   PWM_Calcu();                    //����PWMֵ
   
   speed_mr = speed_ml = 0;	 
 
} 


//********�ҵ���ж�***********************

void INT_L(void) interrupt 0
{

   if(SPDL == 1)  { speed_ml++; }		 //����ǰ��
   else		      { speed_ml--; }		 //��������
   LED = ~LED;

 } 


//********�����ж�***********************

void INT_R(void) interrupt 2
{

   if(SPDR == 1)  { speed_mr++; }		 //�ҵ��ǰ��
   else		      { speed_mr--; }		 //�ҵ������
   LED = ~LED;

 } 
