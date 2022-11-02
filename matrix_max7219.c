#include <stdio.h>
#include<wiringPi.h>
#include<wiringPiSPI.h>
#include <time.h>
#include <wiringPiI2C.h>
#include <math.h>
#include<stdint.h> //for uint8t format
#define channel 0
#define Sample_rate 25 //0x19
#define Config 26
#define Gyro_config 27
#define Acc_config 28
#define Interrupt 56
#define PWR_Managment 107
#define Acc_X 59
#define Acc_Y 61
#define Acc_Z 63
int mpu;
int16_t read_sensor(unsigned char sensor)
{
    uint16_t high, low, data;
    high = wiringPiI2CReadReg8(mpu,sensor);
    low = wiringPiI2CReadReg8(mpu,sensor+1);
    data = (high<<8) | low;

    return data;
}
void Init_6050()
{
    //Sample_rate 1kHz
    wiringPiI2CWriteReg8(mpu,Sample_rate,15);
    // Khong su dung nguon xung ngoai, tat DPFL
    wiringPiI2CWriteReg8(mpu,Config,0);
    //gyro FS: +- 500 o/s
    wiringPiI2CWriteReg8(mpu,Gyro_config,0x08);
    //acc FS: +- 4g
    wiringPiI2CWriteReg8(mpu,Acc_config,0x10);
    //mo interrupt cua data ready
    wiringPiI2CWriteReg8(mpu,Interrupt,1);
    //Chon nguon xung Gyro X
    wiringPiI2CWriteReg8(mpu,PWR_Managment,0x01);
}
void send_data(uint8_t address, uint8_t value)
{
	unsigned char data[2];
	data[0] = address;
	data[1] = value;
	wiringPiSPIDataRW(channel, data, 2);
}

void Init_max7219(void)
{
	// set decode mode
    send_data(0x09,0x00);
	// set intensity
	send_data(0x0A,0x0B);
	//set scan limit
	send_data(0x0B,0X07);
	// no shutdown, no display test
	send_data(0x0C,1); // no shutdown, 1=0xFF
	send_data(0x0F,0); // no display off, 0=0x00
}
void up()
{
    send_data(0x01,0X00);
    send_data(0x02,0x08);
    send_data(0x03,0X04);
    send_data(0x04,0X7E);
    send_data(0x05,0X04);
    send_data(0x06,0X08);
    send_data(0x07,0X00);
    send_data(0x08,0X00);
}
void down()
{
    send_data(0x01,0X00);
    send_data(0x02,0x10);
    send_data(0x03,0X20);
    send_data(0x04,0X7E);
    send_data(0x05,0X20);
    send_data(0x06,0X10);
    send_data(0x07,0X00);
    send_data(0x08,0X00);
}
void right()
{
    send_data(0x01,0X00);
    send_data(0x02,0x10);
    send_data(0x03,0X38);
    send_data(0x04,0X54);
    send_data(0x05,0X10);
    send_data(0x06,0X10);
    send_data(0x07,0X10);
    send_data(0x08,0X00);
}
void left()
{
    send_data(0x01,0X00);
    send_data(0x02,0x10);
    send_data(0x03,0X10);
    send_data(0x04,0X10);
    send_data(0x05,0X54);
    send_data(0x06,0X38);
    send_data(0x07,0X10);
    send_data(0x08,0X00);
}
int main(void)
{
	// Initialize SPI interface
	wiringPiSPISetup(channel, 8000000); //SPIO, 8MHZ
	// Setup operation mode for max 7219
	Init_max7219();
    mpu = wiringPiI2CSetup(0x68);
    Init_6050();
    /*send_data(0x01,0X00);
    send_data(0x02,0x08);
    send_data(0x03,0X04);
    send_data(0x04,0X7E);
    send_data(0x05,0X04);
    send_data(0x06,0X08);
    send_data(0x07,0X00);
    send_data(0x08,0X00);*/
    //send_data(0x80,0x0F);

    
	
	while(1)
	{
        float Ax = (float)read_sensor(Acc_X)/4096.0;
        float Ay = (float)read_sensor(Acc_Y)/4096.0;
        float Az = (float)read_sensor(Acc_X)/4096.0;

        float pich = atan2(Ax,sqrt((Ay*Ay)+(Az*Az)))*180/M_PI;
        float roll = atan2(Ay,sqrt((Ax*Ax)+(Az*Az)))*180/M_PI;
        printf("y la: %f\n",pich);
        printf("x la: %f\n",roll);

        delay(1000);

        if((pich<50 && pich>10) && (roll>-50 && roll<-20))
        {
            up();
            //right();
        }
        else if((pich>-40 && pich<0) && (roll>-55 && roll<-25))
        {
            down();
            //left();
        }
        else if((roll>-90 && roll<-80)&&(pich>0 && pich<8))
        {
            right();
        }
        else if((pich>10 && pich<20) && (roll>20 && roll<60))
        {
            left();
        }
	}
	
	
	return 0;	
}
