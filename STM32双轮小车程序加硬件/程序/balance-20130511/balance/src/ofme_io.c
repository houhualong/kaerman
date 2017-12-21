#include "ofme_io.h"

int ir_key_proc(void)
{
    u8 t1,t2;   

	if(ir_repeat>0)
	{
		ir_repeat--;

	    t1=ir_data;		//得到地址码
    	t2=ir_data>>8;	//得到地址反码 
    	if(t1^t2==0xFF && (t1==REMOTE_ID))
    	{ 
	        t1=ir_data>>16; 	
    	    t2=ir_data>>24;
        	if(t1^t2==0xFF)
			{
				printf("key: %d\r\n",(int)(t1));
				switch(t1)
				{
					case KEY_IR_LEFT:
						return KEY_TURN_LEFT;
					case KEY_IR_RIGHT:
						return KEY_TURN_RIGHT;
					case KEY_IR_UP:
						return KEY_SPEED_UP;
					case KEY_IR_DOWN:
						return KEY_SPEED_DOWN;
					case KEY_IR_0:
						return KEY_SPEED_0;
					case KEY_IR_1:
						return KEY_SPEED_F1;
					case KEY_IR_2:
						return KEY_SPEED_F2;
					case KEY_IR_3:
						return KEY_SPEED_F3;
					case KEY_IR_4:
						return KEY_SPEED_F4;
					case KEY_IR_5:
						return KEY_SPEED_F5;
					case KEY_IR_6:
						return KEY_SPEED_F6;
					case KEY_IR_9:
						return KEY_SPEED_B1;
					case KEY_IR_8:
						return KEY_SPEED_B2;
					case KEY_IR_7:
						return KEY_SPEED_B3;
					case KEY_IR_CTRL:
						return KEY_TURN_STOP;
					default:
						return KEY_NULL;
				}
			}
		}
    }
	return KEY_NULL;     
}
