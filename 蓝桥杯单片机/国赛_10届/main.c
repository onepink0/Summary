#include <STC15F2K60S2.H>
#include "configure.h"
#include "onewire.h"
#include "iic.h"
#include "stdio.h"
//****************************
sbit tx = P1^0;
sbit rx = P1^1;
u8 temp_down_t = 0;
u8 smg_down_t = 0;
u8 csb_down_t = 0;
u8 key_down_t = 0;
u8 stoc_down_t = 0;
u8 key_val,key_old,key_down,key_up;
u8 mode=1,modepro=1;
//-----
u16 temp;
float Temp_t;
u16 distance;
u16 bian_cnt = 0;
u8 temp_para_t = 30,temp_para = 30;
u8 dist_para_t = 35,dist_para = 35;
u8 led_state=0xff;
//----
u8 rx_cnt=0;
xdata u8 rx_buff[12];
xdata u8 tx_buff[12];
//-----
bit dac_output=1;//1为启动
bit long_an=0;
bit flag1s=0;
u16 time1s;
//-----
u8 sys_cli=0;//如果为1，10ms计时
u8 uar_10ms=0;//接收中断、串口处理清0，超过10ms没接收数据 即处理指令
//***********Init*****************
void delay(u16 a)
{
	while(a--);
}
void sendbyte(u8 Data)
{
	SBUF = Data;
	while(TI==0);
	TI = 0;
}
void send_string(char* str)
{
	while(*str != '\0')
	{
		sendbyte(*str++);
	}
}
u8 key_read()
{
	u8 temp=0;
	ET0 = 0;
	P35=0;P34=1;
	if(P32 == 0)	temp = 13;
	if(P33 == 0)	temp = 12;
	P35=1;P34=0;
	if(P32 == 0)	temp = 17;
	if(P33 == 0)	temp = 16;
	ET0 = 1;
	return temp;
}
void sendwave()
{
	u8 i;
	for ( i=0; i<8; i++)
	{
		tx = 0;
		Delay14us();
		tx = 1;
		Delay14us();
	}
}
void ds18b20_init()
{
	u8 i;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	for ( i=0; i<8; i++ )
	{
		delay(60000);
	}
}
//+++++++++++Work+++++++++++
//**************************
void uart_work()
{
	u8 i=0;
	if(!rx_cnt)		return;
	if(uar_10ms>=10)
	{
		sys_cli = 0;
		uar_10ms = 0;
		if(rx_buff[0]=='S' && rx_buff[1]=='T')
		{
			sprintf(tx_buff,"$%2d,%4.2f\r\n",distance,Temp_t);
		}else if(rx_buff[0]=='P' && rx_buff[1]=='A' && rx_buff[2]=='R' && rx_buff[3]=='A'){
			sprintf(tx_buff,"#%d,%d\r\n",(int)dist_para_t,(int)temp_para_t);
		}else{
			sprintf(tx_buff,"ERROR\r\n");
		}
		send_string(tx_buff);
		for(i=0; i<rx_cnt; i++)
		{
			rx_buff[i] = 0;
			tx_buff[i] = 0;
		}
		rx_cnt = 0;
	}

	
}
void led_work()
{
	if(temp>temp_para_t*100)
	{
		led_state &= 0xfe;
	}else{
		led_state |=~0xfe;
	}
	if(distance<dist_para_t)
	{
		led_state &= 0xfd;
	}else{
		led_state |=~0xfd;
	}
	if(dac_output)
	{
		led_state &= 0xfb;
	}else{
		led_state |=~0xfb;
	}
	HC138_Init(4,led_state);
}
void dac_ouput()
{
	u8 Data;
	if(dac_output)
	{
		if(distance<=dist_para_t)
		{
			Data = 102;
		}else{
			Data = 204;
		}
		DA_Output(Data);
	}else{
		DA_Output(20.4);
	}
}
void write_stock()
{
	u8 high,low;
	if(stoc_down_t)	 return;
	stoc_down_t = 1;
	if(mode==1)
	{
		if((temp_para_t!=temp_para)||(dist_para_t!=dist_para)) 
		{
			temp_para_t = temp_para;
			dist_para_t = dist_para;
			bian_cnt++;
			high = (u8)(bian_cnt>>8);
			low  = (u8)bian_cnt;
			at24c02_write(0x01,high);
			at24c02_write(0x00,low);
			Delay5ms();	
		}
	}
}
void dist_read()
{
	u16 time;
	CMOD = 0x00;
	CH = CL = 0;
	EA = 0;
	sendwave();
	EA = 1; CR=1;
	while (rx && !CF);
	CR = 0;
	if (CF == 0)
	{
		time = CH;
		time = (time<<8)|CL;
		distance = time*0.017;
		if(distance>99)		distance=99;
	}else{
		CF = 0;
		distance = 99;
	}
}
void ds18b20_read()
{
	u8 MSB,LSB;
	u16 Data;
	if(temp_down_t)		return;
	temp_down_t = 1;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	Data = MSB;
	Data = (Data<<8)|LSB;
	if((Data&0xf800) == 0x00)
	{
		Temp_t = Data*0.0625;
		temp = Temp_t*100;
	}
}
void key_work()
{
	u8 high,low;
	if(key_down_t)	return;
	key_down_t = 1;
	key_val = key_read();
	key_down = key_val&(key_val^key_old);
	key_up  = ~key_val&(key_val^key_old);
	key_old = key_val;

	if(key_down == 13)
	{
		long_an = 1;
	}
	if(key_up == 13)
	{
		long_an = 0;
		time1s = 0;
		if(!flag1s)
		{
			if(++mode==3)	mode=1;
			modepro = 1;
		}
		if(flag1s)
		{
			flag1s = 0;
			if(dac_output)	dac_output=0;else dac_output=1;
		}
	}
	if(key_down == 12)
	{
		long_an = 1;
	}
	if(key_up == 12)
	{
		long_an = 0;
		time1s = 0;
		if(!flag1s)
		{
			if(mode==1)
			{
				if(++modepro==4)	modepro=1;
			}
			if(mode==2)
			{
				if(++modepro==3)	modepro=1;
			}
		}
		if(flag1s)
		{
			flag1s = 0;
			bian_cnt = 0;
			
			high = (u8)(bian_cnt>>8);
			low  = (u8)bian_cnt;
			at24c02_write(0x01,high);
			at24c02_write(0x00,low);
			Delay5ms();	
		}
	}
	
	if(key_down == 16)	//减
	{
		if(mode==2)
		{
			if(modepro==1)
			{
				if(temp_para>=2)	temp_para -= 2;else temp_para = 0;
			}
			if(modepro==2)
			{
				if(dist_para>=5)	dist_para -= 5;else dist_para = 0;
			}
		}
	}
	if(key_down == 17)
	{
		if(mode==2)
		{
			if(modepro==1)
			{
				if(temp_para<=97)	temp_para += 2;else temp_para = 99;
			}
			if(modepro==2)
			{
				if(dist_para<=94)  dist_para += 5;else dist_para = 99;
			}
		}
	}
}
void smg_work()
{
	static u8 dac=0;
	if(smg_down_t)	return;
	smg_down_t = 1;
	//******
	
	if(++csb_down_t == 25)
	{
		csb_down_t = 0;
		dist_read();
	}
	if(++dac==5)
	{
		dac = 0;
		dac_ouput();
	}
	//******
	if(mode == 1)		//数据界面
	{
		if(modepro == 1)	//温度
		{
			smg_data[0] = 0xc6; smg_data[1] = 0xff;
			smg_data[2] = 0xff; smg_data[3] = 0xff;
			smg_data[4] = SMG_NoDot[temp/1000];
			smg_data[5] = SMG_Dot[temp/100%10];
			smg_data[6] = SMG_NoDot[temp/10%10];
			smg_data[7] = SMG_NoDot[temp%10];
		}
		if(modepro == 2)	//距离
		{
			smg_data[0] = 0xc7; smg_data[1] = 0xff;
			smg_data[2] = 0xff; smg_data[3] = 0xff;
			smg_data[4] = 0Xff; smg_data[5] = 0xff;
			smg_data[6] = SMG_NoDot[distance/10];
			smg_data[7] = SMG_NoDot[distance%10];
		}
		if( modepro == 3)	//变更次数
		{
			smg_data[0] = 0xc8; smg_data[1] = 0xff;
			smg_data[2] = 0xff;
			if(bian_cnt>9999)	smg_data[3] = SMG_NoDot[bian_cnt/10000];else smg_data[3] = 0xff;
			if(bian_cnt>999)	smg_data[4] = SMG_NoDot[bian_cnt/1000%10];else smg_data[4] = 0xff;
			if(bian_cnt>99)		smg_data[5] = SMG_NoDot[bian_cnt/100%10];else smg_data[5] = 0xff;
			if(bian_cnt>9)		smg_data[6] = SMG_NoDot[bian_cnt/10%10];else smg_data[6] = 0xff;
			smg_data[7] = SMG_NoDot[bian_cnt%10];
		}
	}
	if(mode == 2)		//参数界面
	{
		if(modepro == 1)	//温度参数
		{
			smg_data[0] = 0x8c; smg_data[1] = 0xff;
			smg_data[2] = 0xff; smg_data[4] = 0xff;
			smg_data[5] = 0xff;
			smg_data[3] = SMG_NoDot[modepro];
			smg_data[6] = SMG_NoDot[temp_para/10];
			smg_data[7] = SMG_NoDot[temp_para%10];
		}
		if(modepro == 2)	//距离参数
		{
			smg_data[0] = 0x8c; smg_data[1] = 0xff;
			smg_data[2] = 0xff; smg_data[4] = 0xff;
			smg_data[5] = 0xff;
			smg_data[3] = SMG_NoDot[modepro];
			smg_data[6] = SMG_NoDot[dist_para/10];
			smg_data[7] = SMG_NoDot[dist_para%10];
		}
	}
}
void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA  = 1;
}
void UartInit(void)		//4800bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xBF;		//定时器1时钟为Fosc/12,即12T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xCC;		//设定定时初值
	TH1 = 0xFF;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	EA = 1;
	ES = 1;
}
void system_init()
{
	HC138_Init(4,0xff);
	HC138_Init(5,0x00);
	Timer0Init();
	UartInit();
	ds18b20_init();
}
void main()
{
	system_init();
	while(1)
	{
		ds18b20_read();
		write_stock();
		uart_work();
		key_work();
		smg_work();
	}
}
void Timer_Routine() interrupt 1
{
	if(++smg_down_t == 10)		smg_down_t = 0;
	if(++temp_down_t == 200)		temp_down_t = 0;
	if(++key_down_t == 10)		key_down_t = 0;
	if(++stoc_down_t==25)		stoc_down_t = 0;
	if(long_an)		time1s++;
	
	if(sys_cli)		uar_10ms++;
	
	if(time1s>=1000)
	{
		time1s=0;
		flag1s=1;
	}
	led_work();
	smg_bit();
}
void Uart_Routine() interrupt 4
{
	if(RI==1)
	{
		sys_cli = 1;
		uar_10ms = 0;
		rx_buff[rx_cnt++] = SBUF;
		RI = 0;		
	}
}