/** ###################################################################
**     Filename  : Kalman.C
**     Project   : CF2_0
**     Processor : MCF52255CAF80
**     Compiler  : CodeWarrior MCF C Compiler
**     Date/Time : 2012-3-17, ���� 07:24
**     Contents  :
**         User source code
**
** ###################################################################*/

/* MODULE Kalman */

#include  "Kalman.h"


/*********Kalman�˲���8MHz�Ĵ���ʱ��Լ1.8ms***********/
float Angle = 0, Angle_dot = 0; 		//�ⲿ��Ҫ���õı���
////////////////////////////////////////////

float Q_angle=0.001, Q_gyro=0.003, R_angle=0.67, dt=0.005;   	//dt��ȡֵΪkalman�˲�������ʱ��;
		
float P[2][2] =
                           {
						  	       { 1, 0 },
							       { 0, 1 }
						   };
	
char  C_0 = 1;
float E;  
float q_bias;
float Angle_err;
float PCt_0, PCt_1;
float K_0, K_1;
float t_0, t_1;
float Pdot[4] ={0,0,0,0};

//-------------------------------------------------------

void Kalman(float angle_m,float gyro_m)			//gyro_m:gyro_measure
{
	Angle+=(gyro_m-q_bias) * dt;    //�������
	
	Pdot[0]=Q_angle - P[0][1] - P[1][0];// Pk-' ����������Э�����΢��
	Pdot[1]=- P[1][1];
	Pdot[2]=- P[1][1];
	Pdot[3]=Q_gyro;
	
	P[0][0] += Pdot[0] * dt;        // Pk- ����������Э����΢�ֵĻ��� = ����������Э����
	P[0][1] += Pdot[1] * dt;
	P[1][0] += Pdot[2] * dt;
	P[1][1] += Pdot[3] * dt;
		
	Angle_err = angle_m - Angle;    //zk-�������
		
	PCt_0 = C_0 * P[0][0];
	PCt_1 = C_0 * P[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;                //Kk
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * P[0][1];

	P[0][0] -= K_0 * t_0;           //����������Э����
	P[0][1] -= K_0 * t_1;
	P[1][0] -= K_1 * t_0;
	P[1][1] -= K_1 * t_1;
		
	Angle	+= K_0 * Angle_err;      //�������	
	q_bias+= K_1 * Angle_err;       //�������
	Angle_dot = gyro_m-q_bias;      //���ֵ��������ƣ���΢�� = ���ٶ�
}
/* END Kalman */
