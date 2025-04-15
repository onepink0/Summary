#ifndef  __CONFIGURE_H
#define  __CONFIGURE_H

typedef unsigned char u8;
typedef unsigned int u16;

extern code u8 SMG_NoDot[10];
extern code u8 SMG_Dot[10];
extern u8 smg_data[8];

void Delay14us();		//@12.000MHz
void Delay5ms();		//@12.000MHz

void HC138_Init(u8 num1,num2);
void smg_bit();

#endif
