#include <STC15F2K60S2.H>
unsigned char code SMG_NoDot[10] =	{0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
//Ϊ0��1��2��3��4��5��6��7��8��9
unsigned char code SMG_Dot[10]   =	{0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
//Ϊ0. 1. 2. 3. 4. 5. 6. 7. 8. 9.
void Delay20us()		//@12.000MHz
{
	unsigned char i;
	i = 57;
	while (--i);
}
void Delay_SMG(unsigned int a)		//@12.000MHz
{
	while(a--);
}
void HC573Init(unsigned char num1,num2)
{
	P2 = (P2&0x1f)|0x00;
	P0 = num2;
	switch(num1)
	{
		case 0:	P2 = (P2&0x1f)|0x00;	break;//��ʼ��
		case 4:	P2 = (P2&0x1f)|0x80;	break;//LED
		case 5:	P2 = (P2&0x1f)|0xA0;	break;//���������̵���
		case 6:	P2 = (P2&0x1f)|0xC0;	break;//λѡ
		case 7:	P2 = (P2&0x1f)|0xE0;	break;//��ѡ
	}	
	P2 = (P2&0x1f)|0x00;
}
void SMG_Show(unsigned char pose,Data)
{
	HC573Init(6,(0x01<<(pose-1)));
	HC573Init(7,Data);
	Delay_SMG(500);
	HC573Init(6,(0x01<<(pose-1)));
	HC573Init(7,0xFF);
}
void Timer0Init(void)		//@12.000MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD = 0x16;		//���ö�ʱ��ģʽ
	TL0 = 0xFF;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ
	TL1 = (65535-50000)%256;	//���ö�ʱ��ֵ
	TH1 = (65535-50000)/256;		//���ö�ʱ��ֵ
	EA  = 1;
	ET0 = 1;
	ET1 = 1;
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	TR1 = 1;
}
