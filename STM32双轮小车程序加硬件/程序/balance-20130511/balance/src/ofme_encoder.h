#ifndef __OFME_ENCODER_H__
#define __OFME_ENCODER_H__

#include "sys.h"

void encoder_init(void);
int  encoder_read(void);
void encoder_write(int data);

#endif
