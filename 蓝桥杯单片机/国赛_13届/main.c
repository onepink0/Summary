#include <STC15F2K60S2.H>
#include "configure.h"
#include "iic.h"
//***************************************
sbit tx=P1^0;
sbit rx=P1^1;
u8 smg_down_t=0;
u8 key_down_t=0;
u8 csb_down_t=0;
u8 sid_down_t=0;
u16 freqcnt;
u8 key_old,key_val,key_down,key_up;
u8 mode=1,modepro=1,modeplus=1;
//****
bit s7=0;
bit old_dist=0;
u16 freq;
u8 sidu=35;
u16 distance;
u8 freq_para=90;
u8 sidu_para=40;
u8 dist_para=6;
//****
u8 y5c_state=0x00;
u8 pwm=1,pwm_cnt=0;
u8 led_state=0xff;
u8 jdq_cnt=0;
u16 time1s,time100ms;
bit flag1s=0;
bit state_100ms=0;
bit flag100ms=0;
//**************************************
void led_work()
{
	if(mode==1)
	{
		led_state &= 0xFE;
		state_100ms=0;
	}else if(mode==4 && modepro==1){
		state_100ms=1;
		if(flag100ms)
		{
			flag100ms=0;
			if((led_state&0x01)==0x00)
			{
				led_state |= 0x01;
			}else{
				led_state &=~0x01;
			}
		}
	}else{
		led_state |= 0x01;
	}
//***********
	if(mode==2)
	{
		led_state &= 0xFD;
		state_100ms=0;
	}else if(mode==4 && modepro==2){
		state_100ms=1;
		if(flag100ms)
		{
			flag100ms=0;
			if((led_state&0x02)==0x00)
			{
				led_state |= 0x02;
			}else{
				led_state &=~0x02;
			}
		}
	}else{
		led_state |= 0x02;
	}
//*********
	if(mode==3)
	{
		led_state &= 0xFB;
		state_100ms=0;
	}else if(mode==4 && modepro==3){
		state_100ms=1;
		if(flag100ms)
		{
			flag100ms=0;
			if((led_state&0x04)==0x00)
			{
				led_state |= 0x04;
			}else{
				led_state &=~0x04;
			}
		}
	}else{
		led_state |= 0x04;
	}
	if(pwm==4)
	{
		led_state &= 0xF7;
	}else{
		led_state |=~0xF7;
	}
	if(sidu>sidu_para)
	{
		led_state &= 0xEF;
	}else{
		led_state |=~0xEF;
	}
	if(old_dist)
	{
		led_state &= 0xDF;
	}else{
		led_state |=~0xDF;
	}
	HC138_Init(4,led_state);
}
void dist_panduan()
{
	if(distance>dist_para*10)
	{
		y5c_state |= 0x10;
		if(!old_dist)
		{
			jdq_cnt++;
			AT24C02_Write(0x00,jdq_cnt);	
			Delay5ms();
		}
		old_dist=1;
	}else{
		old_dist=0;
		y5c_state &=~0x10;
	}
	HC138_Init(5,y5c_state);
}
void freq_panduan()
{
	if(freq>freq_para*100)
	{
		pwm=4;
	}else{
		pwm=1;
	}
	
	HC138_Init(5,y5c_state);
}
void sidu_read()
{
	u8 temp=0;
	if(sid_down_t) return;
	sid_down_t=1;
	temp = PCF8591_ADchange(0x43);
	sidu = temp/51.0*20;
}
void dac_output()
{
	float Data;
	u8 temp;
	if(sidu<=sidu_para)
	{
		temp = 51;
	}else if(sidu>=80)
	{
		temp = 255;
	}else{
		Data = 4.0*sidu/(80-sidu_para)+5-320.0/(80-sidu_para);
		temp = Data*51;
	}
	PCF8591_DAchange(temp);
}
void Sendwave()
{
	u8 i;
	for(i=0; i<8; i++)
	{
		tx=0;
		Delay14us();
		tx=1;
		Delay14us();
	}
}
void dist_read()
{
	u16 time;
	if(csb_down_t)	return;
	csb_down_t=1;
	CMOD=0x00;
	CH=CL=0;
	EA=0;
	Sendwave();
	EA=1;CR=1;
	while(rx&&!CF);
	CR=0;
	if(CF==0)
	{
		time=CH;
		time=(time<<8)|CL;
		distance=time*0.017;
	}else{
		CF=0;
		distance=255;
	}
}
u8 key_read()
{
	u8 temp=0;
	if(P33==0)	temp=4;
	if(P32==0)	temp=5;
	if(P31==0)	temp=6;
	if(P30==0)	temp=7;
	return temp;
}
void key_work()
{
	if(key_down_t)	return;
	key_down_t=1;
	key_val = key_read();
	key_down = key_val&(key_old^key_val);
	key_up   = ~key_val&(key_old^key_val);
	key_old  = key_val;
	
	if(key_down==4)
	{
		if(++mode>=5)	mode=1;
		modepro=1;
	}
	if(key_down==5)
	{
		if(mode==4)
		{
			if(++modepro==4)	modepro=1;
		}
	}
	if(key_down==6)		//加
	{
		if(mode==4 && modepro==1)
		{
			freq_para += 5;
			if(freq_para>=125)	freq_para=10;
		}
		if(mode==4 && modepro==2)
		{
			sidu_para += 10;
			if(sidu_para>=70)	sidu_para=10;
		}
		if(mode==4 && modepro==3)
		{
			dist_para += 1;
			if(dist_para>=13)	dist_para=1;
		}
		if(mode==3)
		{
			if(++modepro==3)	modepro=1;
		}
	}
	if(key_down==7)
	{
		s7=1;
		if(mode==4 && modepro==1)
		{
			freq_para -= 5;
			if(freq_para<=5)	freq_para=120;
		}
		if(mode==4 && modepro==2)
		{
			sidu_para -= 10;
			if(sidu_para<=0)	sidu_para=60;
		}
		if(mode==4 && modepro==3)
		{
			dist_para -= 1;
			if(dist_para<=0)	dist_para=12;
		}
		if(mode==1)
		{
			if(++modepro==3)	modepro=1;
		}
	}
	if(key_up==7)
	{
		s7=0;
		time1s=0;
		if(flag1s)
		{
			flag1s=0;
			jdq_cnt=0;
			AT24C02_Write(0x00,jdq_cnt);
			Delay5ms();
		}
	}
}
void smg_work()
{
	if(smg_down_t) return;
	smg_down_t=1;
	//******
	dac_output();
	dist_panduan();
	//******
	if(mode==1)		//频率界面
	{
		smg_data[0]=0x8e;smg_data[1]=0xff;
		smg_data[2]=0xff;
		if(modepro==1)			//HZ
		{
			if(freq>9999) 	smg_data[3]=SMG_NoDot[freq/10000];  else 	smg_data[3]=0xff;
			if(freq>999) 	smg_data[4]=SMG_NoDot[freq/1000%10];else smg_data[4]=0xff;
			if(freq>99) 		smg_data[5]=SMG_NoDot[freq/100%10]; else smg_data[5]=0xff;
			if(freq>9) 		smg_data[6]=SMG_NoDot[freq/10%10];  else 	smg_data[6]=0xff;
			smg_data[7]=SMG_NoDot[freq%10];
		}else if(modepro==2){	//KHZ
			smg_data[3]=0xff;smg_data[4]=0xff;
			if(freq>9999) 	smg_data[5]=SMG_NoDot[freq/10000];   else smg_data[5]=0xff;
			if(freq>999) 	smg_data[6]=SMG_Dot[freq/1000%10];  	else smg_data[6]=SMG_Dot[0];
			if(freq>99) 	smg_data[7]=SMG_NoDot[freq/100%10];  else {smg_data[7]=SMG_NoDot[0];smg_data[6]=0xff;}
		}
	}
	if(mode==2)		//湿度界面
	{
		smg_data[0]=0x89;smg_data[1]=0xff;
		smg_data[2]=0xff;smg_data[3]=0xff;
		smg_data[4]=0xff;smg_data[5]=0xff;
		smg_data[6]=SMG_NoDot[sidu/10];
		smg_data[7]=SMG_NoDot[sidu%10];
	}
	if(mode==3)		//测距界面
	{
		smg_data[0]=0x88;smg_data[1]=0xff;
		smg_data[2]=0xff;smg_data[3]=0xff;
		smg_data[4]=0xff;
		if(modepro==1)
		{
			if(distance>99)smg_data[5]=SMG_NoDot[distance/100];else smg_data[5]=0xff;
			if(distance>9)smg_data[6]=SMG_NoDot[distance/10%10];else smg_data[6]=0xff;
			smg_data[7]=SMG_NoDot[distance%10];
		}else if(modepro==2){
			smg_data[5]=SMG_Dot[distance/100];
			smg_data[6]=SMG_NoDot[distance/10%10];
			smg_data[7]=SMG_NoDot[distance%10];
		}
	}
	if(mode==4)		//参数界面
	{
		smg_data[0]=0x8c;smg_data[2]=0xff;
		smg_data[3]=0xff;smg_data[4]=0xff;
		if(modepro==1)	//频率参数
		{
			smg_data[1]=SMG_NoDot[modepro];
			if(freq_para>99)smg_data[5]=SMG_NoDot[freq_para/100];else smg_data[5]=0xff;
			if(freq_para>9)smg_data[6]=SMG_Dot[freq_para/10%10];else smg_data[6]=SMG_Dot[0];
			smg_data[7]=SMG_NoDot[freq_para%10];
		}
		if(modepro==2)	//湿度参数
		{
			smg_data[1]=SMG_NoDot[modepro];
			smg_data[5]=0xff;
			smg_data[6]=SMG_NoDot[sidu_para/10];
			smg_data[7]=SMG_NoDot[sidu_para%10];
		}
		if(modepro==3)	//距离参数
		{
			smg_data[1]=SMG_NoDot[modepro];
			smg_data[5]=0xff;
			if(dist_para>9)smg_data[6]=SMG_Dot[dist_para/10]; else smg_data[6]=SMG_Dot[0];
			smg_data[7]=SMG_NoDot[dist_para%10];
		}
	}
}
void Time01init(void)//200微秒
{
	TMOD=0x15;
	TH0=0;TH0=0;
	TH1=0xFF;
	TL1=0x38;
	EA = 1;
	ET1= 1;
	EA = 1;
	TR0=1;
	TR1=1;
}
void Timer2Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xFB;		//定时器时钟12T模式
	T2L = 0x18;		//设置定时初值
	T2H = 0xFC;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0x04;
	EA  =1;
}
void system_init()
{
	HC138_Init(4,0xff);
	HC138_Init(5,0x00);
	Time01init();
	Timer2Init();
	
}
void main()
{
	system_init();
	while(1)
	{
		dist_read();
		key_work();
		smg_work();
		sidu_read();
	}
}
void Timer1_Routine() interrupt 3
{
	TH1=0xFF;
	TL1=0x38;
	pwm_cnt++;
	pwm_cnt%=6;
	if(++freqcnt==5000)
	{
		freqcnt=0;
		freq=TH0;
		freq=(freq<<8)|TL0;
		TH0=0;TL0=0;
	}
	if(pwm_cnt<=pwm)
	{
		y5c_state |= 0x20;
	}else{
		y5c_state &=~0x20;
	}
	HC138_Init(5,y5c_state);
}
void Timer2_Routine() interrupt 12
{
	if(++smg_down_t==10)		smg_down_t=0;
	if(++key_down_t==10)		key_down_t=0;
	if(++csb_down_t==200)	csb_down_t=0;
	if(++sid_down_t==5)		sid_down_t=0;
	if(s7)					time1s++;
	if(state_100ms)			time100ms++;
	if(time100ms==100)
	{
		time100ms=0;
		flag100ms=1;
	}
	if(time1s>=1000)
	{
		flag1s=1;
		time1s=0;
	}
	smg_bit();
	freq_panduan();
	led_work();
}