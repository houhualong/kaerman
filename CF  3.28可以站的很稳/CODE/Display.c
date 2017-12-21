/** ###################################################################
**     Filename  : Display.C
**     Project   : CF2_0
**     Processor : MCF52255CAF80
**     Compiler  : CodeWarrior MCF C Compiler
**     Date/Time : 2011-12-16, 6:05
**     Contents  :
**         User source code
**
** ###################################################################*/

/* MODULE Display */
#include "Display.H"
#include "math.h"
#include "stdio.h"
#include "string.h"

#define DOT 10
#define MINUS 20
#define OFF 21
#define LEDs dis_buf[6] 

#define LEDALL 0x3f
#define LEDNONE 0x00
#define LED0 0x01 
#define LED1 0x02
#define LED2 0x04
#define LED3 0x08
#define LED4 0x10
#define LED5 0x20

uint8 dis_buf[7] =
{
	OFF,OFF,OFF,OFF,OFF,OFF,OFF
};
uint8 wei[7] = 
{
	0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf
};
uint8 Num[22] = 
{
  0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,//0 --- 9
  0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef,//0.----9.
  0x40,0x00//-
};

void SetCharBuf(char *p)
{
	if(p == NULL)
	{
		dis_buf[0] = OFF;
		dis_buf[1] = OFF;
		dis_buf[2] = OFF;
		dis_buf[3] = OFF;
		dis_buf[4] = OFF;
		dis_buf[5] = OFF;
		return;
	}
	
	dis_buf[0] = (uint8)(p[0] - '0');
	dis_buf[1] = (uint8)(p[1] - '0');
	dis_buf[2] = (uint8)(p[2] - '0');
	dis_buf[3] = (uint8)(p[3] - '0');
	dis_buf[4] = (uint8)(p[4] - '0');
	dis_buf[5] = (uint8)(p[5] - '0');
}

void Set1FloatBuf(float f)
{
    char p[7] =
    {
    	0
    };
    sprintf(p, "%07.02f", f);
    if(p[0] == '-')
    {
    	dis_buf[0] = MINUS;
    }	
	else
	{
		dis_buf[0] = OFF;
		
	}
	dis_buf[1] = (uint8)(p[1] - '0');
	dis_buf[2] = (uint8)(p[2] - '0');
	dis_buf[3] = (uint8)(p[3] - '0' + DOT);
	dis_buf[4] = (uint8)(p[5] - '0');
	dis_buf[5] = (uint8)(p[6] - '0');   

}

void Set1IntBuf(int16 i)
{
	char p[7] =
    {
    	0
    };
    sprintf(p, "%06d", i);
    if(p[0] == '-')
    {
    	dis_buf[0] = MINUS;
    }	
	else
	{
		dis_buf[0] = OFF;	
	}
	dis_buf[1] = (uint8)(p[1] - '0');
	dis_buf[2] = (uint8)(p[2] - '0');
	dis_buf[3] = (uint8)(p[3] - '0');
	dis_buf[4] = (uint8)(p[4] - '0');
	dis_buf[5] = (uint8)(p[5] - '0'); 
}

void Set2IntBuf(int16 i, int16 j)
{
    char p[7] =
    {
    	0
    };
    sprintf(p, "%03d%03d", i, j);
    if(p[0] == '-')
    {
    	dis_buf[0] = MINUS;
    }	
	else
	{
		dis_buf[0] = OFF;	
	}
	dis_buf[1] = (uint8)(p[1] - '0');
	dis_buf[2] = (uint8)(p[2] - '0');
	
	if(p[3] == '-')
    {
    	dis_buf[3] = MINUS;
    }	
	else
	{
		dis_buf[3] = OFF;	
	}
	dis_buf[4] = (uint8)(p[4] - '0');
	dis_buf[5] = (uint8)(p[5] - '0');   

}
void Set2uIntBuf(uint16 i, uint16 j)
{
    char p[7] =
    {
    	0
    };
    sprintf(p, "%03d%03d", i, j);
	dis_buf[0] = (uint8)(p[0] - '0');
	dis_buf[1] = (uint8)(p[1] - '0');
	dis_buf[2] = (uint8)(p[2] - '0');
    dis_buf[3] = (uint8)(p[3] - '0');;
	dis_buf[4] = (uint8)(p[4] - '0');
	dis_buf[5] = (uint8)(p[5] - '0');   

}
void Dis(void)
{
	static uint8 a = 0, ms2 = 0, ms200 = 0;
	
	Wei_PutVal(wei[a]);
	if(a < 6)
	{	
		Seg_PutVal(Num[dis_buf[a++]]);
	}
	else
	{
		Seg_PutVal(1 << ms200);
		a = 0;
	}
	ms2++;if(ms2 > 50){ ms2 = 0; ms200++; if(ms200 > 5) ms200 =0;}
}
void Dis_Init(void)
{
	SetCharBuf(0);
	Cpu_Delay100US(3000);
	SetCharBuf("888888");
	Cpu_Delay100US(3000);
	SetCharBuf(0);
}

/* END Display */
