#include "sys.h"
#include "usart.h"		
//#include "delay.h"	

// 编码器：100线；电机减速比：150

#include "ofme_led.h"


#include "ofme_filter.h"
#include "ofme_iic.h"
#include "ofme_iic_dev.h"
#include "ofme_pwm.h"
#include "ofme_pid.h"
#include "ofme_encoder.h"
#include "ofme_io.h"
#include "ofme_time.h"
#include "ofme_ir_nec.h"

#define PI (3.14159265)
// 度数表示的角速度*1000
#define MDPS (70)
// 弧度表示的角速度
#define RADPS ((float)MDPS*PI/180000)
// 每个查询周期改变的角度
#define RADPT (RADPS/(-100))


// 平衡的角度范围；+-60度(由于角度计算采用一阶展开，实际值约为46度)
#define ANGLE_RANGE_MAX (60*PI/180)
#define ANGLE_RANGE_MIN (-60*PI/180)

// 全局变量
pid_s sPID;					// PID控制参数结构体
float radian_filted=0;		// 滤波后的弧度
accelerometer_s acc;		// 加速度结构体，包含3维变量
gyroscope_s gyr;			// 角速度结构体，包含3维变量
int speed=0, distance=0;	// 小车移动的速度，距离
int tick_flag = 0;			// 定时中断标志
int pwm_speed = 0;			// 电机pwm控制的偏置值，两个电机的大小、正负相同，使小车以一定的速度前进
int pwm_turn = 0;			// 电机pwm控制的差异值，两个电机的大小相同，正负相反，使小车左、右转向
float angle_balance = 0;	// 小车的平衡角度。由于小车重心的偏移，小车的平衡角度不一定是radian_filted为零的时候


// 定时器周期中断，10ms
void sys_tick_proc(void)
{
	static unsigned int i = 0;

	tick_flag++;

	i++;
	if(i>=100) i=0;

	if(i==0)	   	// 绿灯的闪烁周期为1秒
	{
		LED1_OFF();
	}
	else if(i==50)
	{
		LED1_ON();
	}
}

void control_proc(void)
{
	int i = ir_key_proc(); // 将红外接收到的按键值，转换为小车控制的相应按键值。

	switch(i)
	{
		case KEY_TURN_LEFT:
			if(pwm_turn<500) pwm_turn += 50;
			break;
		case KEY_TURN_RIGHT:
			if(pwm_turn>-500) pwm_turn -= 50;
			break;
		case KEY_TURN_STOP:
			pwm_turn = 0;
			distance = 0;
			pwm_speed = 0;
			break;
		case KEY_SPEED_UP:
			if(pwm_speed<500) pwm_speed+=100;
			break;
		case KEY_SPEED_DOWN:
			if(pwm_speed>-500) pwm_speed-=100;
			break;
		case KEY_SPEED_0:
			pwm_speed = 0;
			break;
		case KEY_SPEED_F1:
			pwm_speed = 150;
			break;
		case KEY_SPEED_F2:
			pwm_speed = 300;
			break;
		case KEY_SPEED_F3:
			pwm_speed = 450;
			break;
		case KEY_SPEED_F4:
			pwm_speed = 600;
			break;
		case KEY_SPEED_F5:
			pwm_speed = 750;
			break;
		case KEY_SPEED_F6:
			pwm_speed = 900;
			break;
		case KEY_SPEED_B1:
			pwm_speed = -150;
		case KEY_SPEED_B2:
			pwm_speed = -300;
		case KEY_SPEED_B3:
			pwm_speed = -450;
			break;
		default:
			break;
	}

	pwm_turn *= 0.9;	// pwm_turn的值以0.9的比例衰减，使小车在接收到一个转向信号后只转动一定的时间后停止转动。


	speed = speed*0.7 +0.3*(encoder_read());	// 定周期（10ms）读取编码器数值得到实时速度，再对速度进行平滑滤波
 	if(speed!=0)
	{
		printf("speed: %d, dst: %d, pwm: %d \r\n", speed,distance,(int)(speed*6+distance*0.1));
	}



	encoder_write(0);							// 编码器值重新设为0

	distance += speed;							// 对速度进行积分，得到移动距离

	if(distance>6000) distance = 6000;			// 减少小车悬空、空转对控制的影响
	else if(distance<-6000) distance = -6000;

}

void balance_proc(void)
{
	static unsigned int err_cnt=0;

//	float tFloat;
	int pwm_balance;
//	static float angle;
//	float angle_t;
	float radian, radian_pt;  	// 当前弧度及弧度的微分(角速度，角度值用弧度表示）

	adxl345_read(&acc);			// 读取当前加速度。由于传感器按照的位置原因，传感器的值在函数内部经过处理，变为小车的虚拟坐标系。
	l3g4200d_read(&gyr);		// 读取当前角速度。同样经过坐标系变换。


// 此段程序用于传感器出错时停止小车
	err_cnt = err_cnt*115>>7;	// err_cnt以0.9的比例系数衰减(115>>7的值约为0.9，避免浮点数，提高速度)
	if(acc.flag != 0x0F || gyr.flag != 0x0F)   // 读取的角度、角速度值有误。可能是电磁干扰、iic线太长等导致出错。
	{
		LED0_ON();		// 亮红灯
		err_cnt +=100;	// 等比数列，比例系数0.9(115>>7)，常数项100；根据公式，连续10项的和约为657
		if(err_cnt>657) goto err;	// 当连续发生约10次（约0.1秒）错误则超过657而溢出。
	}


// 此段程序用于倒立或失重时停止小车
	if(acc.z<=0)
	{
		goto err;
	}


// 小车的虚拟x轴方向为小车前进方向，虚拟y轴为小车左边，虚拟z轴为小车上升方向。
// 前倾角度为负，后倾角度为正。
	// 通过计算加速度分量，得到小车倾斜弧度（未滤波）
	radian = (float)(acc.x)/acc.z;	//  一阶展开：Q =f(x)=x-x^3/3+x^5/5+...+(-1)^k*x^(2k+1)/(2k+1)+...
	// 通过角速度传感器，得到小车的角速度（单位为 弧度/秒）
	radian_pt = gyr.y*RADPT;
	radian_filted = ofme_filter(radian_filted, radian, radian_pt);		// 互补滤波得到小车的倾斜角度

// 此段程序用于小车倾斜角度过大时，停止小车
	if(radian_filted> ANGLE_RANGE_MAX || radian_filted<ANGLE_RANGE_MIN)
	{
		goto err;
	}

// 通过PID计算，得到保持小车角度为零所需要的电机pwm输出
	pwm_balance = pid_proc(&sPID, radian_filted, radian_pt);
	//	printf("%d\r\n",speed);
// 通过小车移动速度与移动距离，调整小车平衡所需的pwm输出
	pwm_balance += (speed*6+distance*0.1);

// 在pwm_balance的基础上，加上速度分量与转动分量，调整小车两个电机的转速。
	pwm_control(pwm_balance+pwm_speed+pwm_turn, pwm_balance+pwm_speed-pwm_turn);

// 如果pwm超出有效值，红灯亮。用于调试，了解系统状态。
	if(pwm_balance>=1000||pwm_balance<=-1000) LED1_ON();
	LED0_OFF();
	return;
err:
	puts("balance error.\r\n");
	pwm_control(0, 0);	   			// 关闭电机
	return;
}


int main(void)
{
//	int i=0, t;
//	int pwm;
//	float radian, radian_pt;

  	Stm32_Clock_Init(9);//系统时钟设置
	uart_init(72,57600); //串口1初始化   
	hw_tick_start();   // 定时器周期性中断，用于提供系统脉搏
////////////////////////////////////////////////////////////////////////////////
	led_init();
	pwm_init();
	iic_init();
	adxl345_init(&acc);
	l3g4200d_init(&gyr);
	hw_ir_init();
	encoder_init();
	while(0)
	{
		if(tick_flag>100)
		{
			tick_flag = 0;
			printf("count: %d\r\n",encoder_read());

		}
	}



////////////////////////////////////////////////////////////////////////////////
//	pid_init(&sPID, 4500,0,-300);6000--350	;8000--350;11000--350;
//	pid_init(&sPID, 6000,0,-35000);
	pid_init(&sPID, 7500,0,-35000);	  // 调节PID参数，后3个形参分别为：比例系数P，积分系数I，微分系数D
	sPID.target = -3.5*PI/180;

	radian_filted = 0;
	adxl345_init(&acc);
	l3g4200d_init(&gyr);
	while(1)
	{
		if(tick_flag)
		{
			tick_flag = 0;
			balance_proc();	// 调节小车，保持平衡
			control_proc();	// 根据遥控接收到的数据，调整电机输出参数
		}
	}
}

