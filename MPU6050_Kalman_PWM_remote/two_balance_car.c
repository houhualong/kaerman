								  
/***********************************************************************
// 两轮自平衡车最终版控制程序（6轴MPU6050+互补滤波+PWM电机） 
// 单片机STC12C5A60S2 
// 晶振：20M
// 日期：2012.11.26 - ？
***********************************************************************/

#include <REG52.H>	
#include <math.h>     
#include <stdio.h>   
#include <INTRINS.H>

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

//******功能模块头文件*******

#include "DELAY.H"		    //延时头文件
#include "STC_ISP.H"	    //程序烧录头文件
#include "SET_SERIAL.H"		//串口头文件
#include "SET_PWM.H"		//PWM头文件
#include "MOTOR.H"			//电机控制头文件
#include "MPU6050.H"		//MPU6050头文件



//******角度参数************

float Gyro_y;        //Y轴陀螺仪数据暂存
float Angle_gy;      //由角速度计算的倾斜角度
float Accel_x;	     //X轴加速度值暂存
float Angle_ax;      //由加速度计算的倾斜角度
float Angle;         //小车最终倾斜角度
uchar value;		 //角度正负极性标记	

//******PWM参数*************

int   speed_mr;		 //右电机转速
int   speed_ml;		 //左电机转速
int   PWM_R;         //右轮PWM值计算
int   PWM_L;         //左轮PWM值计算
float PWM;           //综合PWM计算
float PWMI;			 //PWM积分值

//******电机参数*************

float speed_r_l;	//电机转速
float speed;        //电机转速滤波
float position;	    //位移

//******蓝牙遥控参数*************
uchar remote_char;
char  turn_need;
char  speed_need;

//*********************************************************
//定时器100Hz数据更新中断
//*********************************************************

void Init_Timer1(void)	//10毫秒@20MHz,100Hz刷新频率
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x10;		//设置定时器模式
	TL1 = 0xE5;		    //设置定时初值
	TH1 = 0xBE;		    //设置定时初值
	TF1 = 0;		    //清除TF1标志
	TR1 = 1;		    //定时器1开始计时
}



//*********************************************************
//中断控制初始化
//*********************************************************

void Init_Interr(void)	 
{
	EA = 1;     //开总中断
    EX0 = 1;    //开外部中断INT0
    EX1 = 1;    //开外部中断INT1
    IT0 = 1;    //下降沿触发
    IT1 = 1;    //下降沿触发
	ET1 = 1;    //开定时器1中断
}



//******卡尔曼参数************
		
float code Q_angle=0.001;  
float code Q_gyro=0.003;
float code R_angle=0.5;
float code dt=0.01;	                  //dt为kalman滤波器采样时间;
char  code C_0 = 1;
float xdata Q_bias, Angle_err;
float xdata PCt_0, PCt_1, E;
float xdata K_0, K_1, t_0, t_1;
float xdata Pdot[4] ={0,0,0,0};
float xdata PP[2][2] = { { 1, 0 },{ 0, 1 } };

//*********************************************************
// 卡尔曼滤波
//*********************************************************

//Kalman滤波，20MHz的处理时间约0.77ms；

void Kalman_Filter(float Accel,float Gyro)		
{
	Angle+=(Gyro - Q_bias) * dt; //先验估计

	
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-先验估计误差协方差的微分

	Pdot[1]=- PP[1][1];
	Pdot[2]=- PP[1][1];
	Pdot[3]=Q_gyro;
	
	PP[0][0] += Pdot[0] * dt;   // Pk-先验估计误差协方差微分的积分
	PP[0][1] += Pdot[1] * dt;   // =先验估计误差协方差
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - Angle;	//zk-先验估计
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //后验估计误差协方差
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	Angle	+= K_0 * Angle_err;	 //后验估计
	Q_bias	+= K_1 * Angle_err;	 //后验估计
	Gyro_y   = Gyro - Q_bias;	 //输出值(后验估计)的微分=角速度

}



//*********************************************************
// 倾角计算（卡尔曼融合）
//*********************************************************

void Angle_Calcu(void)	 
{
	//------加速度--------------------------

	//范围为2g时，换算关系：16384 LSB/g
	//角度较小时，x=sinx得到角度（弧度）, deg = rad*180/3.14
	//因为x>=sinx,故乘以1.3适当放大

	Accel_x  = GetData(ACCEL_XOUT_H);	  //读取X轴加速度
	Angle_ax = (Accel_x - 1100) /16384;   //去除零点偏移,计算得到角度（弧度）
	Angle_ax = Angle_ax*1.2*180/3.14;     //弧度转换为度,


    //-------角速度-------------------------

	//范围为2000deg/s时，换算关系：16.4 LSB/(deg/s)

	Gyro_y = GetData(GYRO_YOUT_H);	      //静止时角速度Y轴输出为-30左右
	Gyro_y = -(Gyro_y + 30)/16.4;         //去除零点偏移，计算角速度值,负号为方向处理 
	//Angle_gy = Angle_gy + Gyro_y*0.01;  //角速度积分得到倾斜角度.	

	
	//-------卡尔曼滤波融合-----------------------

	Kalman_Filter(Angle_ax,Gyro_y);       //卡尔曼滤波计算倾角


	/*//-------互补滤波-----------------------

	//补偿原理是取当前倾角和加速度获得倾角差值进行放大，然后与
    //陀螺仪角速度叠加后再积分，从而使倾角最跟踪为加速度获得的角度
	//0.5为放大倍数，可调节补偿度；0.01为系统周期10ms	
		
	Angle = Angle + (((Angle_ax-Angle)*0.5 + Gyro_y)*0.01);*/
															  
}  



//*********************************************************
//电机转速和位移值计算
//*********************************************************

void Psn_Calcu(void)	 
{
	
	speed_r_l =(speed_mr + speed_ml)*0.5;
	speed *= 0.7;		                  //车轮速度滤波
	speed += speed_r_l*0.3;	
	position += speed;	                  //积分得到位移
	position += speed_need;
	if(position<-6000) position = -6000;
	if(position> 6000) position =  6000;

	 
}


static float code Kp  = 9.0;       //PID参数
static float code Kd  = 2.6;	    //PID参数
static float code Kpn = 0.01;      //PID参数
static float code Ksp = 2.0;	    //PID参数

//*********************************************************
//电机PWM值计算
//*********************************************************

void PWM_Calcu(void)	 
{
    
	if(Angle<-40||Angle>40)               //角度过大，关闭电机 
	{  
	  CCAP0H = 0;
      CCAP1H = 0;
	  return;
	}
	PWM  = Kp*Angle + Kd*Gyro_y;          //PID：角速度和角度
	PWM += Kpn*position + Ksp*speed;      //PID：速度和位置
	PWM_R = PWM + turn_need;
	PWM_L = PWM - turn_need;
	PWM_Motor(PWM_L,PWM_R); 
	 
}




//*********************************************************
//手机蓝牙遥控
//*********************************************************

void Bluetooth_Remote(void)	 
{

	remote_char = receive_char();				   //接收蓝牙串口数据

	if(remote_char ==0x02) speed_need = -80;	   //前进
	else if(remote_char ==0x01) speed_need = 80;   //后退
	     else speed_need = 0;					   //不动

    if(remote_char ==0x03) turn_need = 15;		   //左转
	else if(remote_char ==0x04) turn_need = -15;   //右转
	     else turn_need = 0;					   //不转
	 
}


/*=================================================================================*/

//*********************************************************
//main
//*********************************************************
void main()
{ 

	delaynms(500);	   //上电延时
	Init_PWM();	       //PWM初始化
    Init_Timer0();     //初始化定时器0，作为PWM时钟源
	Init_Timer1();     //初始化定时器1
	Init_Interr();     //中断初始化
	Init_Motor();	   //电机控制初始化
	Init_BRT();		   //串口初始化（独立波特率）
	InitMPU6050();     //初始化MPU6050
	delaynms(500); 	   

	while(1)
	{
	   
	 Bluetooth_Remote();

	}
}


/*=================================================================================*/

//********timer1中断***********************

void Timer1_Update(void) interrupt 3
{
  
   TL1 = 0xE5;		    //设置定时初值10MS
   TH1 = 0xBE;
   
   //STC_ISP();                    //程序下载
   Angle_Calcu();                  //倾角计算
   Psn_Calcu();                    //电机位移计算
   PWM_Calcu();                    //计算PWM值
   
   speed_mr = speed_ml = 0;	 
 
} 


//********右电机中断***********************

void INT_L(void) interrupt 0
{

   if(SPDL == 1)  { speed_ml++; }		 //左电机前进
   else		      { speed_ml--; }		 //左电机后退
   LED = ~LED;

 } 


//********左电机中断***********************

void INT_R(void) interrupt 2
{

   if(SPDR == 1)  { speed_mr++; }		 //右电机前进
   else		      { speed_mr--; }		 //右电机后退
   LED = ~LED;

 } 
