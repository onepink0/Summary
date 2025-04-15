#ifndef __IIC_H
#define __IIC_H


void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(unsigned char ackbit);

void DA_Output(unsigned char num);
unsigned char AD_Input(unsigned char addr);


#endif
