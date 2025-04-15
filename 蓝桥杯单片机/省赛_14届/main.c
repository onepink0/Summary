#include <STC15F2K60S2.H>
#include "iic.h"
#include "ds1302.h"
#include "onewire.h"
#include "NixieInit.h"
//=========================================
sbit C1=P3^2;
sbit C2=P3^3;
sbit R1=P4^2;
sbit R2=P4^4;
void SMG_Dynamic();
void Key_Run();
unsigned char Write_Addr[7] = {0x80,0x82,0x84,0x86,0x88,0x8A,0x8C};
unsigned char Read_Addr[7]  = {0x81,0x83,0x85,0x87,0x89,0x8B,0x8D};
unsigned char Time[7] = {0x40,0x15,0x05,0x22,0x03,0x05,0x24};
	//秒分时 日月周 年
//----------------------------------
unsigned char Mode=1,ModePro=1,ModeStock;
unsigned char ADdata0,ADdata1;//光敏的值
unsigned int Freq;//温度，频率
unsigned int Temp0,Temp,TempMax,TempSum,Tempcnt,TempAe;//温度、温度最大值、 、 、温度平均值
unsigned char Tempara=30;//温度参数
unsigned char Check_flag,sidu_flag,s2_flag,led4_flag;		//测试、湿度、2秒标志位
unsigned int sidu0,sidu,siduMax,sidusum,siducnt,siduAe;
unsigned char hour,min,led_state=0xFF;
unsigned char time_800ms,time_2s,time_3s,time_100ms;		//亮暗状态变化
unsigned char flag_800ms,flag_2s,flag_3s,flag_100ms;
unsigned char youxiao;
unsigned int cnt0,cnt1;			//定时器0和1
unsigned char one,two,three;
//========================================
void DS1302_Init()
{
	unsigned char i;
	Write_Ds1302_Byte(0x8E,0x00);
	for(i=0; i<7; i++)
	{
		Write_Ds1302_Byte(Write_Addr[i],Time[i]);
	}
	Write_Ds1302_Byte(0x8E,0x80);
}
void DS1302_Read()
{
	Time[0] = Read_Ds1302_Byte(0x81);//秒 分 时
	Time[1] = Read_Ds1302_Byte(0x83);
	Time[2] = Read_Ds1302_Byte(0x85);
	if(Check_flag)
	{
		min  = Read_Ds1302_Byte(0x83);
		hour = Read_Ds1302_Byte(0x85);
		Check_flag=0;
	}
}
void DS18B20_Read()
{
	if(Check_flag)
	{
		unsigned char LSB,MSB;
		Init_DS18B20();
		Write_DS18B20(0xCC);
		Write_DS18B20(0x44);
		SMG_Dynamic();
		Init_DS18B20();
		SMG_Dynamic();
		Write_DS18B20(0xCC);
		Write_DS18B20(0xBE);
		LSB = Read_DS18B20();
		MSB = Read_DS18B20();
		Temp = (MSB<<8)|LSB;
		if(two)		Temp0 = Temp;
		Temp = (Temp>>4)*100+(LSB&0x0F)*6.25;//两位小数，共四位
		two=1;
		if(TempMax<Temp)	TempMax = Temp;//温度最大值
		if(youxiao) 
		{
			TempSum+=Temp;
			Tempcnt++;
			youxiao=0;
		}
		TempAe = TempSum/siducnt;//温度平均值
	}
}
void Get_Sidu()
{
	if(Check_flag)
	{
		if(Freq>=200 && Freq<=2000)
		{
			youxiao=1;
			if(three)	sidu0=sidu;
			sidu=(Freq*2+50)*10/45;
			three=1;
			siducnt++;
			sidusum+=sidu;
			if(siduMax<sidu)	siduMax=sidu;
			siduAe = sidusum/siducnt;
			sidu_flag=0;
		}else{
		sidu_flag=1;
		}
	}
}
void Light_Anchange()
{
	if(flag_3s==0)
	{
		if(one)		ADdata0 = ADdata1;
		ADdata1 = PCF8591_ADchange(0x01);
		one=1;
		flag_800ms=0;
		if(ADdata1<140 && ADdata0 >190)
		{
			Check_flag=1;
			flag_3s=1;
			ModeStock = Mode;
			Mode=4;
		}
	}
}
void Clean()
{
	TempMax=0;
	TempSum=0;
	Temp=0;
	TempAe=0;
	siduMax=0;
	sidu=0;
	sidusum=0;
	siducnt=0;
	siduAe=0;
	min=0;
	hour=0;
}
void LED_Run()
{
	if(Mode==1)	{led_state&=0xFE;}else{led_state|=~0xFE;}
	if(Mode==2)	{led_state&=0xFD;}else{led_state|=~0xFD;}
	if(Mode==4)	{led_state&=0xFB;}else{led_state|=~0xFB;}
	if(Temp>Tempara*100)
	{
		flag_100ms=1;
	}else{
		flag_100ms=0;
		time_100ms=0;
	}
	if(led4_flag)
	{
		if((led_state&0x08)==0x00)
		{
			led_state|=0x08;
		}else if(led_state&0x08==0x08)
		{
			led_state&=~0x08;
		}
		led4_flag=0;
	}
	if(sidu_flag)	{led_state&=0xEF;}else{led_state|=~0xEF;}
	if(Tempcnt>2 && Temp0<Temp && sidu0<sidu)
	{
		led_state&=0xDF;
	}else{
		led_state|=~0xDF;
	}
	HC573Init(4,led_state);
}
void System_Init()
{
	HC573Init(4,led_state);
	HC573Init(5,0x00);
	DS1302_Init();
	DS18B20_Init();
	Timer0Init();
	
}
void main()
{
	System_Init();
	while(1)
	{
		if(flag_800ms)	Light_Anchange();
		Get_Sidu();
		DS18B20_Read();
		DS1302_Read();
		LED_Run();
		if(Mode!=4)		Key_Run();
		SMG_Dynamic();
	}
}
void SMG_Dynamic()
{
	if(Mode==1)//时钟界面
	{
		SMG_Show(1,SMG_NoDot[Time[2]/16]);
		SMG_Show(2,SMG_NoDot[Time[2]%16]);
		SMG_Show(3,0xBF);
		SMG_Show(4,SMG_NoDot[Time[1]/16]);
		SMG_Show(5,SMG_NoDot[Time[1]%16]);
		SMG_Show(6,0xBF);
		SMG_Show(7,SMG_NoDot[Time[0]/16]);
		SMG_Show(8,SMG_NoDot[Time[0]%16]);
	}
	if(Mode==2)//回显界面
	{
		if(ModePro==1)//温度回显
		{
			SMG_Show(1,0xC6);	
			if(Tempcnt!=0)
			{
				SMG_Show(3,SMG_NoDot[TempMax/1000]);
				SMG_Show(4,SMG_NoDot[TempMax/100%10]);
				SMG_Show(5,0xBF);
				SMG_Show(6,SMG_NoDot[TempAe/1000]);
				SMG_Show(7,SMG_Dot[TempAe/100%10]);
				SMG_Show(8,SMG_NoDot[TempAe/10%10]);
			}else{
				SMG_Show(3,0xFF);
				SMG_Show(4,0xFF);
				SMG_Show(5,0xFF);
				SMG_Show(6,0xFF);
				SMG_Show(7,0xFF);
				SMG_Show(8,0xFF);
			}
		}
		if(ModePro==2)//湿度回显
		{
			SMG_Show(1,0x89);
			if(Tempcnt!=0)
			{
				SMG_Show(3,SMG_NoDot[siduMax/100]);
				SMG_Show(4,SMG_NoDot[siduMax/10%10]);
				SMG_Show(5,0xBF);
				SMG_Show(6,SMG_NoDot[siduAe/100]);
				SMG_Show(7,SMG_Dot[siduAe/10%10]);
				SMG_Show(8,SMG_NoDot[siduAe%10]);
			}else{
				SMG_Show(3,0xFF);
				SMG_Show(4,0xFF);
				SMG_Show(5,0xFF);
				SMG_Show(6,0xFF);
				SMG_Show(7,0xFF);
				SMG_Show(8,0xFF);
			}
		}
		if(ModePro==3)//时间回显
		{
			SMG_Show(1,0x8E);		
			SMG_Show(2,SMG_NoDot[Tempcnt/10]);
			SMG_Show(3,SMG_NoDot[Tempcnt%10]);
			if(Tempcnt!=0)
			{
				SMG_Show(4,SMG_NoDot[hour/16]);
				SMG_Show(5,SMG_NoDot[hour%16]);
				SMG_Show(6,0xBF);
				SMG_Show(7,SMG_NoDot[min/16]);
				SMG_Show(8,SMG_NoDot[min%16]);
			}else{
				SMG_Show(4,0xFF);
				SMG_Show(5,0xFF);
				SMG_Show(6,0xFF);
				SMG_Show(7,0xFF);
				SMG_Show(8,0xFF);
			}
		}
	}
	if(Mode==3)//参数界面
	{
		SMG_Show(1,0x8C);
		SMG_Show(7,SMG_NoDot[Tempara/10]);
		SMG_Show(8,SMG_NoDot[Tempara%10]);
		SMG_Show(2,0xFF);
		SMG_Show(3,0xFF);
		SMG_Show(4,0xFF);
		SMG_Show(5,0xFF);
		SMG_Show(6,0xFF);
	}
	if(Mode==4)//温湿度界面
	{
		SMG_Show(1,0x86);
		SMG_Show(2,0xFF);
		SMG_Show(3,0xFF);
		SMG_Show(4,SMG_NoDot[Temp/1000]);
		SMG_Show(5,SMG_NoDot[Temp/100%10]);
		SMG_Show(6,0xBF);
		if(sidu_flag)
		{
			SMG_Show(7,0x88);
			SMG_Show(8,0x88);
		}else{
			SMG_Show(7,SMG_NoDot[sidu/100]);
			SMG_Show(8,SMG_NoDot[sidu/10%10]);
		}
	}
}
void Key_Run()
{
	C1=C2=R1=R2=1;
	C1=0;
	if(R1==0)    		//S9
	{
		Delay20us();
		if(R1==0)
		{
			if(Mode==2 && ModePro==3)
			{
				flag_2s=1;
				time_2s=0;
			}
			while(R1==0)		SMG_Dynamic();
			if(flag_2s==1 && s2_flag==1)
			{
				flag_2s=0;
				Clean();
			}
			if(Mode==3)
			{
				if(Tempara>0)	Tempara--;
			}
		}
	}
	if(R2==0)    		//S5
	{
		Delay20us();
		if(R2==0)
		{
			while(R2==0)		SMG_Dynamic();
			if(Mode==2)
			{
				ModePro++;
				if(ModePro==4)	ModePro=1;
			}
		}
	}
	//====================
	C1=1;C2=0;
	if(R1==0)    		//S8
	{
		Delay20us();
		if(R1==0)
		{
			while(R1==0)		SMG_Dynamic();
			if(Mode==3)
			{
				Tempara++;
				if(Tempara==100)		Tempara=99;
			}
		}
	}
	if(R2==0)    		//S4
	{
		Delay20us();
		if(R2==0)
		{
			while(R2==0)		SMG_Dynamic();
			Mode++;
			if(Mode==4)		Mode=1;
			if(Mode==2)		ModePro=1;
		}
	}
}
void Timer0_Routine() interrupt 1
{
	cnt0++;
}
void Timer1_Routine() interrupt 3
{
	cnt1++;
	time_800ms++;
	if(flag_2s)		time_2s++;
	if(flag_3s)		time_3s++;
	if(flag_100ms)	time_100ms++;
	if(cnt1 == 20)
	{
		Freq = cnt0;
		cnt0 = 0;
		cnt1 = 0;
	}
	if(time_800ms==4)
	{
		time_800ms=0;
		flag_800ms=1;
	}
	if(time_2s==40)
	{
		time_2s=0;
		s2_flag=1;
	}
	if(time_3s==60)
	{
		time_3s=0;
		flag_3s=0;
		Mode = ModeStock;
	}
	if(time_100ms==2)
	{
		time_100ms=0;
		led4_flag=1;
	}
}
