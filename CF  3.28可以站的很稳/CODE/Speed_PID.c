/************************************************************************************

* Speed_PID.c
* ����:�ٶ�PI���ƣ����ٶȷּ����

* ��֧�ֵ�оƬ:9S12XS
* ��֧�ֵı�������CodeWarrior 5.X

* ��Ȩ���У�ɽ����ѧ���ܳ�������
* ���ߣ�    �����       (��������)
            ���Ľ�       (����������ͷ)
            ��ʤ��       (��������)

* ����汾��V1.00 
* ����ʱ�䣺2012-03-06

*************************************************************************************/

#include "DataProcess.h"
#include "Speed_PID.h"
#include "Events.H"
/*****************----PID����-----******************/

/********�� ��*********/
int16 NowSpeed_L = 0;
int16 NowSpeed_R = 0;

/*****�ٶ�λ��ʽPI*****/
int16 PI_SpeedNew;
int16 PI_SpeedOld = 0;
int32 PI_SpeedKeep = 0;
int16 ObjectSpeed = 100;
int32 PI_SpeedErr;

int16 PI_Out = 0;


//====================================
//�������SSpeed_PI
//���ã��ٶ�λ��ʽPI���ƣ�100ms����һ��
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
//�������SSpeed_PI_OUT
//���ã������ٶ�PI���Ƶ������5ms����һ��
//==================================== 

void Speed_PI_OUT(void) 
{
  int32  nValue;
  
  nValue = PI_SpeedNew - PI_SpeedOld;
  nValue = nValue * (time_5ms + 1) / (CAR_MOTION_PERIOD - 1) + PI_SpeedOld;
  PI_Out = (int16)nValue;  
}
