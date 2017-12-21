
/*********************************************************************************************

    两路PWM输出控制设置

/*********************************************************************************************/

#ifndef __SET_PWM_H__
#define __SET_PWM_H__


//Declare SFR associated with the PCA 

 sfr   CCON    = 0xD8;      //PCA control register
 sbit  CCF0    = CCON^0;    //PCA module-0 interrupt flag
 sbit  CCF1    = CCON^1;    //PCA module-1 interrupt flag
 sbit  CR      = CCON^6;    //PCA timer run control bit
 sbit  CF      = CCON^7;    //PCA timer overflow flag
 sfr   CMOD    = 0xD9;      //PCA mode register
 sfr   CL      = 0xE9;      //PCA base timer LOW
 sfr   CH      = 0xF9;      //PCA base timer HIGH
 sfr   CCAPM0  = 0xDA;      //PCA module-0 mode register
 sfr   CCAP0L  = 0xEA;      //PCA module-0 capture register LOW
 sfr   CCAP0H  = 0xFA;      //PCA module-0 capture register HIGH
 sfr   CCAPM1  = 0xDB;      //PCA module-1 mode register
 sfr   CCAP1L  = 0xEB;      //PCA module-1 capture register LOW
 sfr   CCAP1H  = 0xFB;      //PCA module-1 capture register HIGH
 sfr   PCAPWM0 = 0xf2;
 sfr   PCAPWM1 = 0xf3;



//***********************************************************************************/
 //PWM模式设置

 void Init_PWM()
 {
   // set PWM mode
  CCON = 0;                  //Initial PCA control register(PCA timer stop,Clear CF flag,Clear all module interrupt flag)
  CL = 0;                    //Reset PCA base timer
  CH = 0;
  CMOD = 0x04;               //Set PCA timer clock source as timer0 overflow,Disable PCA timer overflow interrupt
  CCAP0H = CCAP0L = 255;     //PWM0 port output X% duty cycle square wave
  CCAPM0 = 0x42;             //PCA module-0 work in 8-bit PWM mode and no PCA interrupt
  CCAP1H = CCAP1L = 255;     //PWM1 port output X% duty cycle square wave
  CCAPM1 = 0x42;             //PCA module-1 work in 8-bit PWM mode and no PCA interrupt
  CR = 1;                    //PCA timer start run
 }



//***********************************************************************************/
 //设置Timer0为8位自动重载模式，作为PWM时钟源

 void Init_Timer0()
 {
    // set timer0 mode
  AUXR = 0x00;               //timer0 work in 12T mode
  TMOD|= 0x02;   			 //set timer0 counter mode2 (8-bit auto-reload)
  TH0=TL0=130;				 //PWM 50Hz
  TR0 = 1;					 //timer0 start running(as PWM clk)

  }



#endif