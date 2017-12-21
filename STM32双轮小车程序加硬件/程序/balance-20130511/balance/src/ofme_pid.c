#include "ofme_pid.h"

// 位置式PID

void pid_init(pid_t pid, float Kp, float Ki, float Kd)
{
	pid->target = 0;
	pid->integral = 0;	// 累计积分值
	pid->Kp = Kp;
	pid->Ki = Ki;
	pid->Kd = Kd;

}

int pid_proc(pid_s *p, float current, float differential)
{
	float offset;

	offset = p->target - current;
	p->integral += offset;

	return (int)(p->Kp*offset + p->Ki*p->integral + p->Kd*differential);
}
