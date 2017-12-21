/************************************************************************************

* Speed_PID.c
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

#include "DataProcess.h"
#include "Speed_PID.h"
#include "Events.H"
/*****************----PID函数-----******************/

/********测 速*********/
int16 NowSpeed_L = 0;
int16 NowSpeed_R = 0;

/*****速度位置式PI*****/
int16 PI_SpeedNew;
int16 PI_SpeedOld = 0;
int32 PI_SpeedKeep = 0;
int16 ObjectSpeed = 100;
int32 PI_SpeedErr;

int16 PI_Out = 0;


//====================================
//函数名Speed_PI
//作用：速度位置式PI控制，100ms调用一次
//====================================   
void Speed_PI(void) 
{
	int32  nP, nI;
	int32  nSpeed;
	int32  nValue1, nValue2;
	
	nSpeed = (NowSpeed_R + NowSpeed_L) / 2;
	
	PI_SpeedErr = ObjectSpeed - nSpeed;
	nValue1 = ObjectSpeed - nSpeed;
	nValue2 = ObjectSpeed - nSpeed;
		
	if(nValue1 > 160)  nValue1 = 160;
	if(nValue1 < -160) nValue1  = -160;
	if(nValue2 > 135)  nValue2 = 135;
	if(nValue2 < -135) nValue2  = -135;

	nP = nValue1 * MOTOR_SPEED_P_INT;
	nI = nValue2 * MOTOR_SPEED_I_INT; 

	PI_SpeedOld = PI_SpeedNew;	
	PI_SpeedKeep += nI;

	PI_SpeedNew = (int16)((nP + (PI_SpeedKeep / 8)) / 100L);

	if(PI_SpeedKeep > 26000)   PI_SpeedKeep = 26000;
	if(PI_SpeedKeep < -26000)  PI_SpeedKeep = -26000;

}

//====================================
//函数名Speed_PI_OUT
//作用：计算速度PI控制的输出，5ms调用一次
//==================================== 

void Speed_PI_OUT(void) 
{
  int32  nValue;
  
  nValue = PI_SpeedNew - PI_SpeedOld;
  nValue = nValue * (time_5ms + 1) / (CAR_MOTION_PERIOD - 1) + PI_SpeedOld;
  PI_Out = (int16)nValue;  
}
