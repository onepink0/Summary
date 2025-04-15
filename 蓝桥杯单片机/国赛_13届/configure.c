#include <STC15F2K60S2.H>
#include "intrins.h"
typedef unsigned char u8;
typedef unsigned int u16;

code u8 SMG_NoDot[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
//1   2   3   4   5   6   7   8   9
code u8 SMG_Dot[10]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
// 0. 1. 2. 3. 4. 5. 6. 7. 8. 9.
u8 smg_data[8]={255,255,255,255,255,255,255,255};

void Delay14us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 38;
	while (--i);
}
void Delay5ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 59;
	j = 90;
	do
	{
		while (--j);
	} while (--i);
}
void HC138_Init(u8 num1,num2)
{
	P2 = P2&0x1f;
	P0 = num2;
	switch(num1)
	{
		case 4: P2=(P2&0x1f)|0x80; break;
		case 5: P2=(P2&0x1f)|0xa0; break;
		case 6: P2=(P2&0x1f)|0xc0; break;
		case 7: P2=(P2&0x1f)|0xe0; break;
	}
	P2 = P2&0x1f;
}
void smg_bit()
{
	static u8 pos=0;
	HC138_Init(7,0xff);
	HC138_Init(6,0x01<<pos);
	HC138_Init(7,smg_data[pos]);
	if(++pos==8)	pos=0;
}