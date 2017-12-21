#include "sys.h"
#include "usart.h"		
//#include "delay.h"	

// ��������100�ߣ�������ٱȣ�150

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
// ������ʾ�Ľ��ٶ�*1000
#define MDPS (70)
// ���ȱ�ʾ�Ľ��ٶ�
#define RADPS ((float)MDPS*PI/180000)
// ÿ����ѯ���ڸı�ĽǶ�
#define RADPT (RADPS/(-100))


// ƽ��ĽǶȷ�Χ��+-60��(���ڽǶȼ������һ��չ����ʵ��ֵԼΪ46��)
#define ANGLE_RANGE_MAX (60*PI/180)
#define ANGLE_RANGE_MIN (-60*PI/180)

// ȫ�ֱ���
pid_s sPID;					// PID���Ʋ����ṹ��
float radian_filted=0;		// �˲���Ļ���
accelerometer_s acc;		// ���ٶȽṹ�壬����3ά����
gyroscope_s gyr;			// ���ٶȽṹ�壬����3ά����
int speed=0, distance=0;	// С���ƶ����ٶȣ�����
int tick_flag = 0;			// ��ʱ�жϱ�־
int pwm_speed = 0;			// ���pwm���Ƶ�ƫ��ֵ����������Ĵ�С��������ͬ��ʹС����һ�����ٶ�ǰ��
int pwm_turn = 0;			// ���pwm���ƵĲ���ֵ����������Ĵ�С��ͬ�������෴��ʹС������ת��
float angle_balance = 0;	// С����ƽ��Ƕȡ�����С�����ĵ�ƫ�ƣ�С����ƽ��ǶȲ�һ����radian_filtedΪ���ʱ��


// ��ʱ�������жϣ�10ms
void sys_tick_proc(void)
{
	static unsigned int i = 0;

	tick_flag++;

	i++;
	if(i>=100) i=0;

	if(i==0)	   	// �̵Ƶ���˸����Ϊ1��
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
	int i = ir_key_proc(); // ��������յ��İ���ֵ��ת��ΪС�����Ƶ���Ӧ����ֵ��

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

	pwm_turn *= 0.9;	// pwm_turn��ֵ��0.9�ı���˥����ʹС���ڽ��յ�һ��ת���źź�ֻת��һ����ʱ���ֹͣת����


	speed = speed*0.7 +0.3*(encoder_read());	// �����ڣ�10ms����ȡ��������ֵ�õ�ʵʱ�ٶȣ��ٶ��ٶȽ���ƽ���˲�
 	if(speed!=0)
	{
		printf("speed: %d, dst: %d, pwm: %d \r\n", speed,distance,(int)(speed*6+distance*0.1));
	}



	encoder_write(0);							// ������ֵ������Ϊ0

	distance += speed;							// ���ٶȽ��л��֣��õ��ƶ�����

	if(distance>6000) distance = 6000;			// ����С�����ա���ת�Կ��Ƶ�Ӱ��
	else if(distance<-6000) distance = -6000;

}

void balance_proc(void)
{
	static unsigned int err_cnt=0;

//	float tFloat;
	int pwm_balance;
//	static float angle;
//	float angle_t;
	float radian, radian_pt;  	// ��ǰ���ȼ����ȵ�΢��(���ٶȣ��Ƕ�ֵ�û��ȱ�ʾ��

	adxl345_read(&acc);			// ��ȡ��ǰ���ٶȡ����ڴ��������յ�λ��ԭ�򣬴�������ֵ�ں����ڲ�����������ΪС������������ϵ��
	l3g4200d_read(&gyr);		// ��ȡ��ǰ���ٶȡ�ͬ����������ϵ�任��


// �˶γ������ڴ���������ʱֹͣС��
	err_cnt = err_cnt*115>>7;	// err_cnt��0.9�ı���ϵ��˥��(115>>7��ֵԼΪ0.9�����⸡����������ٶ�)
	if(acc.flag != 0x0F || gyr.flag != 0x0F)   // ��ȡ�ĽǶȡ����ٶ�ֵ���󡣿����ǵ�Ÿ��š�iic��̫���ȵ��³���
	{
		LED0_ON();		// �����
		err_cnt +=100;	// �ȱ����У�����ϵ��0.9(115>>7)��������100�����ݹ�ʽ������10��ĺ�ԼΪ657
		if(err_cnt>657) goto err;	// ����������Լ10�Σ�Լ0.1�룩�����򳬹�657�������
	}


// �˶γ������ڵ�����ʧ��ʱֹͣС��
	if(acc.z<=0)
	{
		goto err;
	}


// С��������x�᷽��ΪС��ǰ����������y��ΪС����ߣ�����z��ΪС����������
// ǰ��Ƕ�Ϊ��������Ƕ�Ϊ����
	// ͨ��������ٶȷ������õ�С����б���ȣ�δ�˲���
	radian = (float)(acc.x)/acc.z;	//  һ��չ����Q =f(x)=x-x^3/3+x^5/5+...+(-1)^k*x^(2k+1)/(2k+1)+...
	// ͨ�����ٶȴ��������õ�С���Ľ��ٶȣ���λΪ ����/�룩
	radian_pt = gyr.y*RADPT;
	radian_filted = ofme_filter(radian_filted, radian, radian_pt);		// �����˲��õ�С������б�Ƕ�

// �˶γ�������С����б�Ƕȹ���ʱ��ֹͣС��
	if(radian_filted> ANGLE_RANGE_MAX || radian_filted<ANGLE_RANGE_MIN)
	{
		goto err;
	}

// ͨ��PID���㣬�õ�����С���Ƕ�Ϊ������Ҫ�ĵ��pwm���
	pwm_balance = pid_proc(&sPID, radian_filted, radian_pt);
	//	printf("%d\r\n",speed);
// ͨ��С���ƶ��ٶ����ƶ����룬����С��ƽ�������pwm���
	pwm_balance += (speed*6+distance*0.1);

// ��pwm_balance�Ļ����ϣ������ٶȷ�����ת������������С�����������ת�١�
	pwm_control(pwm_balance+pwm_speed+pwm_turn, pwm_balance+pwm_speed-pwm_turn);

// ���pwm������Чֵ������������ڵ��ԣ��˽�ϵͳ״̬��
	if(pwm_balance>=1000||pwm_balance<=-1000) LED1_ON();
	LED0_OFF();
	return;
err:
	puts("balance error.\r\n");
	pwm_control(0, 0);	   			// �رյ��
	return;
}


int main(void)
{
//	int i=0, t;
//	int pwm;
//	float radian, radian_pt;

  	Stm32_Clock_Init(9);//ϵͳʱ������
	uart_init(72,57600); //����1��ʼ��   
	hw_tick_start();   // ��ʱ���������жϣ������ṩϵͳ����
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
	pid_init(&sPID, 7500,0,-35000);	  // ����PID��������3���βηֱ�Ϊ������ϵ��P������ϵ��I��΢��ϵ��D
	sPID.target = -3.5*PI/180;

	radian_filted = 0;
	adxl345_init(&acc);
	l3g4200d_init(&gyr);
	while(1)
	{
		if(tick_flag)
		{
			tick_flag = 0;
			balance_proc();	// ����С��������ƽ��
			control_proc();	// ����ң�ؽ��յ������ݣ���������������
		}
	}
}

