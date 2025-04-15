#include <STC15F2K60S2.H>
#include "configure.h"
#include "ds1302.h"
#include "onewire.h"
#include "iic.h"
//===================================
u8 code write_addr[7]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
u8 code read_addr[7]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};
u8 time[7]={0x50,0x59,0x16,0x07,0x05,0x02,0x24};
// 秒 分 时 日 月 周 年
u8 sec,min,hour;
u8 key_old,key_val,key_up,key_down;
u8 smg_down_t=0;
u8 key_down_t=0;
u8 temp_down_t=0;
u8 dac_down_t=0;
u8 led_down_t=0;
//----
u8 mode=1,mode1=1;
bit ray_state=0;   //0为亮1为暗
bit led3_flag=0;
bit one=0,flag=0;
u16 temp;//三位数1小数
u16 voltage;
u16 time3s;
u8 adc_old;
u8 time_para=17,time_para_s=17;
u8 temp_para=25,temp_para_s=25;
u8 led_para=4,led_para_s=4;
u8 led_state=0xff;
u8 led_state_t=0xff;
//===================================
void Delay(u16 a)
{
	while(a--);
}
u8 key_read()
{
	u8 temp=0;
	ET0=0;
	P44=0;P42=1;
	if(P32==0)	temp=5;
	if(P33==0)	temp=4;
	P44=1;P42=0;
	if(P32==0)	temp=9;
	if(P33==0)	temp=8;
	ET0=1;
	return temp;
}
void da_read()
{
	u8 Data;
	if(dac_down_t)	return;
	dac_down_t=1;
	Data = AD_Input(0x41);
	if(Data>180)
	{
		ray_state=0;
	}else{
		ray_state=1;
	}
	if(one)
	{
		if((adc_old<120 && Data<120)|(adc_old>180 && Data>180))
		{
			flag=1;
		}else{
			flag=0;
			time3s=0;
		}
	}
	adc_old = Data;
	one=1;
	voltage = Data*5.0/255*100;
}
void ds18b20_init()
{
	u8 i;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	for(i=0; i<8; i++)
	{
		Delay(60000);
	}
}
void ds18b20_read()
{
	float temp0;
	u8 LSB,MSB;
	if(temp_down_t)  return;
	temp_down_t=1;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	temp = MSB;
	temp = (temp<<8)|LSB;
	if((temp&0xf800) ==0x0000)
	{
		temp0 = temp*0.0625;
		temp  = temp0*10;
	}
}
void ds1302_init()
{
	u8 i;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0; i<7; i++)
	{
		Write_Ds1302_Byte(write_addr[i],time[i]);
	}
	Write_Ds1302_Byte(0x8e,0x80);
}
void ds1302_read()
{
	time[0]  = Read_Ds1302_Byte(0x81);
	time[1]  = Read_Ds1302_Byte(0x83);
	time[2]  = Read_Ds1302_Byte(0x85);
	sec  = time[0]/16*10+time[0]%16;
	min  = time[1]/16*10+time[1]%16;
	hour = time[2]/16*10+time[2]%16;
}
void led_work()
{
	if(led_down_t) return;
	led_down_t=1;
	if(((hour>=time_para) && (hour<8)) | (time_para>=8)&&(hour>=time_para))
	{
		led_state &= ~0x01;
	}else{
		led_state |=  0x01;
	}
	//------
	if(temp<(temp_para*10))
	{
		led_state &= ~0x02;
	}else{
		led_state |=  0x02;
	}
	//------
	if(led3_flag)
	{
		led_state &= ~0x04;
	}else{
		led_state |=  0x04;
	}
	//---LED4-8
	if(ray_state)
	{
		led_state_t &= ~(0x01<<(led_para-1));
		led_state = (led_state&0x07)|(led_state_t&0xf8);
		led_state_t=0xff;
	}else{
		led_state_t=0xff;
		led_state = (led_state&0x07)|(led_state_t&0xf8);
	}
	HC138_Init(4,led_state);
}
void update_key()
{
	if(key_down_t)	return;
	key_down_t=1;
	key_val=key_read();
	key_down=key_val&(key_old^key_val);
	key_up=~key_old&(key_old^key_val);
	key_old=key_val;
	
	if(!(mode==2 && mode1==1))	time_para = time_para_s;
	if(!(mode==2 && mode1==2))	temp_para = temp_para_s;
	if(!(mode==2 && mode1==3))	led_para = led_para_s;
	
	if(key_down==4)
	{
		if(++mode==3)	mode=1;
		mode1=1;
	}
	if(key_down==5)
	{
		if(++mode1==4)	mode1=1;
	}
	if(key_down==8)
	{
		if(mode==2)
		{
			if(mode1==1)
			{
				if(time_para_s==0)
				{
					time_para_s=0;
				}else{
					time_para_s--;
				}
			}
			if(mode1==2)
			{
				if(temp_para_s==0)
				{
					temp_para_s=0;
				}else{
					temp_para_s--;
				}
			}
			if(mode1==3)
			{
				if(led_para_s==4)
				{
					led_para_s=4;
				}else{
					led_para_s--;
				}
			}
		}
	}
	if(key_down==9)
	{
		if(mode==2)
		{
			if(mode1==1)
			{
				if(time_para_s==23)
				{
					time_para_s=23;
				}else{
					time_para_s++;
				}
			}
			if(mode1==2)
			{
				if(temp_para_s==99)
				{
					temp_para_s=99;
				}else{
					temp_para_s++;
				}
			}
			if(mode1==3)
			{
				if(led_para_s==8)
				{
					led_para_s=8;
				}else{
					led_para_s++;
				}
			}
		}
	}
}
void update_smg()
{
	if(smg_down_t)	return;
	smg_down_t=1;
//--------
	ds1302_read();
//--------
	if(mode==1)	//	数据界面
	{
		if(mode1==1)		//时间数据显示
		{
			smg_data[0]=SMG_NoDot[hour/10];smg_data[1]=SMG_NoDot[hour%10];
			smg_data[2]=0xbf;
			smg_data[3]=SMG_NoDot[min/10];smg_data[4]=SMG_NoDot[min%10];
			smg_data[5]=0xbf;
			smg_data[6]=SMG_NoDot[sec/10];smg_data[7]=SMG_NoDot[sec%10];
		}
		if(mode1==2)		//温度数据显示
		{
			smg_data[0]=0xc6;smg_data[1]=0xff;smg_data[2]=0xff;
			smg_data[3]=0xff;smg_data[4]=0xff;
			smg_data[5]=SMG_NoDot[temp/100];
			smg_data[6]=SMG_Dot[temp/10%10];
			smg_data[7]=SMG_NoDot[temp%10];
		}
		if(mode1==3)		//亮暗状态
		{
			smg_data[0]=0x86;smg_data[1]=0xff;
			smg_data[5]=0xff;smg_data[6]=0xff;
			smg_data[2]=SMG_Dot[voltage/100];
			smg_data[3]=SMG_NoDot[voltage/10%10];
			smg_data[4]=SMG_NoDot[voltage%10];
			smg_data[7]=SMG_NoDot[ray_state];
		}
	}
	if(mode==2)	//参数界面
	{
		if(mode1==1)		//时间参数
		{
			smg_data[0]=0x8c;smg_data[2]=0xff;
			smg_data[3]=0xff;smg_data[4]=0xff;
			smg_data[5]=0xff;
			smg_data[1]=SMG_NoDot[1];
			smg_data[6]=SMG_NoDot[time_para_s/10];
			smg_data[7]=SMG_NoDot[time_para_s%10];
		}
		if(mode1==2)		//温度参数
		{
			smg_data[0]=0x8c;smg_data[2]=0xff;
			smg_data[3]=0xff;smg_data[4]=0xff;
			smg_data[5]=0xff;
			smg_data[1]=SMG_NoDot[2];
			smg_data[6]=SMG_NoDot[temp_para_s/10];
			smg_data[7]=SMG_NoDot[temp_para_s%10];
		}
		if(mode1==3)		//指示灯参数
		{
			smg_data[0]=0x8c;smg_data[2]=0xff;
			smg_data[3]=0xff;smg_data[4]=0xff;
			smg_data[5]=0xff;smg_data[6]=0xff;
			smg_data[1]=SMG_NoDot[3];
			smg_data[7]=SMG_NoDot[led_para_s];
		}
	}
}
void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA  = 1;
}
void system_init()
{
	HC138_Init(4,0xff);
	HC138_Init(5,0x00);
	Timer0Init();
	ds18b20_init();
	ds1302_init();
	temp_down_t=0;
	ds18b20_read();
}
void main()
{
	system_init();
	while(1)
	{
		ds18b20_read();
		da_read();
		led_work();
		update_smg();
		update_key();
	}
}
void Timer0_Routine() interrupt 1
{
	if(++smg_down_t==10)		smg_down_t=0;
	if(++key_down_t==10)		key_down_t=0;
	if(++temp_down_t==200)	temp_down_t=0;
	if(++dac_down_t==30)		dac_down_t=0;
	if(++led_down_t==5)		led_down_t=0;
	if(flag)				time3s++;
	if(time3s==3000)
	{
		time3s=0;
		if(ray_state)
		{
			led3_flag=1;
		}else{
			led3_flag=0;
		}
	}
	smg_bit();
}
