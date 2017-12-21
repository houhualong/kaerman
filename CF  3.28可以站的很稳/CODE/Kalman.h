/** ###################################################################
**     Filename  : Kalman.H
**     Project   : CF2_0
**     Processor : MCF52255CAF80
**     Compiler  : CodeWarrior MCF C Compiler
**     Date/Time : 2012-3-17, ���� 07:24
**     Contents  :
**         User source code
**
** ###################################################################*/

#ifndef __Kalman_H
#define __Kalman_H

/* MODULE Kalman */

/////////////////////////////////////////
extern float Angle, Angle_dot; 		//�ⲿ��Ҫ���õı���

//////////////////////////////////////////
void Kalman(float angle_m,float gyro_m);//���ٶȼ�  ������

/* END Kalman */

#endif

