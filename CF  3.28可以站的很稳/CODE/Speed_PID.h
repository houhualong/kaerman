/************************************************************************************

* Speed_PID.h
* 描述:速度PI控制，及速度分级输出

* 所支持的芯片:9S12XS
* 所支持的编译器：CodeWarrior 5.X

* 版权所有：山东大学智能车工作室
* 作者：    万冰冰       (第六届电磁)
            孙文健       (第六届摄像头)
            段胜才       (第六届电磁)

* 程序版本：V1.00 
* 更新时间：2012-03-06

*************************************************************************************/

/*--------------- I N C L U D E S ------------------------------------*/

#ifndef _SPEED_PID_H
#define _SPEED_PID_H

/***********速度闭环参数配置**********/
#define CAR_MOTION_PERIOD		20			 	// 车速控制周期
#define MOTOR_SPEED_P			  1.35      //比例   1.78   1.96    1.33
#define MOTOR_SPEED_P_INT		(int32)(MOTOR_SPEED_P * 100)
#define MOTOR_SPEED_I		    1.45 			//积分   1.72   2.23    1.36
#define MOTOR_SPEED_I_INT		(int32)(MOTOR_SPEED_I * 100) 

/////////////////////////////////////////////////
extern  int16  NowSpeed_L;         //左测速
extern  int16  NowSpeed_R;         //右测速
extern  int16  NowSpeed_L_temp;
extern  int16  NowSpeed_R_temp;
extern  int16  NowSpeed_L_history1;
extern  int16  NowSpeed_R_history1;

extern  int16  PI_Out;


/////////////////////////////////////////////////
extern void Speed_PI(void);
extern void Speed_PI_OUT(void);

#endif /*PORT_H END */