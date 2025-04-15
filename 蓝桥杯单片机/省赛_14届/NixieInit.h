#ifndef    _SMG_INIT_
#define    _SMG_INIT_

void Delay20us();
extern unsigned char code SMG_NoDot[10];
extern unsigned char code SMG_Dot[10];
void HC573Init(unsigned char num1,num2);
void SMG_Show(unsigned char pose,Data);
void Timer0Init(void);		//@12.000MHz

#endif