#ifndef __OFME_IR_NEC_H__
#define __OFME_IR_NEC_H__

#include "sys.h"

#define RDATA PCin(1)	 //红外数据输入脚
//红外遥控识别码(ID),每款遥控器的该值基本都不一样,但也有一样的.
//我们选用的遥控器识别码为0

void hw_ir_init(void);

#define KEY_IR_VDONW (7)
#define KEY_IR_7	(8)
#define KEY_IR_VUP	(9)
#define KEY_IR_4	(12)
#define KEY_IR_3	(13)
#define KEY_IR_DOWN	(21)
#define KEY_IR_1	(22)
#define KEY_IR_5	(24)
#define KEY_IR_2	(25)
#define KEY_IR_8	(28)
#define KEY_IR_CTRL	(64)
#define KEY_IR_0	(66)
#define KEY_IR_RIGHT (67)
#define KEY_IR_LEFT	(68)
#define KEY_IR_PWR	(69)
#define KEY_IR_UP	(70)
#define KEY_IR_FUNC (71)
#define KEY_IR_BDEL	(74)
#define KEY_IR_9	(90)
#define KEY_IR_6	(94)


extern u32 ir_data;
extern int ir_repeat;



#define REMOTE_ID 0

#endif
