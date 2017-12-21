/************************************************************************************

* Speed_PID.h
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

/*--------------- I N C L U D E S ------------------------------------*/

#ifndef _SPEED_PID_H
#define _SPEED_PID_H

/***********�ٶȱջ���������**********/
#define CAR_MOTION_PERIOD		20			 	// ���ٿ�������
#define MOTOR_SPEED_P			  1.35      //����   1.78   1.96    1.33
#define MOTOR_SPEED_P_INT		(int32)(MOTOR_SPEED_P * 100)
#define MOTOR_SPEED_I		    1.45 			//����   1.72   2.23    1.36
#define MOTOR_SPEED_I_INT		(int32)(MOTOR_SPEED_I * 100) 

/////////////////////////////////////////////////
extern  int16  NowSpeed_L;         //�����
extern  int16  NowSpeed_R;         //�Ҳ���
extern  int16  NowSpeed_L_temp;
extern  int16  NowSpeed_R_temp;
extern  int16  NowSpeed_L_history1;
extern  int16  NowSpeed_R_history1;

extern  int16  PI_Out;


/////////////////////////////////////////////////
extern void Speed_PI(void);
extern void Speed_PI_OUT(void);

#endif /*PORT_H END */