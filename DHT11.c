#include <wiringPi.h>
#include <stdio.h>
#define DATA_IO 8
unsigned char read_dht11(unsigned char *dat);
unsigned char main()
{
	unsigned char dat[4],i=0;//存放需要的数据，湿度8bit+8bit + 温度8bit+8bit
	wiringPiSetup();
	while (!read_dht11(dat))//如果一次测量失败，则再次测量
	{
		i++;
		if (i==5)
		{
			printf("Data validation error 数据校验错误\n");
			return 0;
		}
		delay(1000);
	}
	printf("humidity: %d.%d%%\ntemp: %d.%d'C\n",dat[0],dat[1],dat[2],dat[3]);
	return 1;
}
unsigned char read_dht11(unsigned char *dat)
{
unsigned int i=0,time,bit_time[41],max,min,mid=0;
unsigned char check=0;
pinMode(DATA_IO,OUTPUT);
digitalWrite(DATA_IO,0);
delay(30);	//delay 30 ms
digitalWrite(DATA_IO,1);
pinMode(DATA_IO,INPUT);
time = micros();
while(digitalRead(DATA_IO) == 1)//等待从机拉低总线
{
	if ((micros()-time) > 50) return 2;//等了50us，返回2表示设备故障，或没有设备
}
for (i=0;i<41;i++)
	{
		time = micros();
		while(digitalRead(DATA_IO) == 0);
		while(digitalRead(DATA_IO) == 1)
		{
			if((micros()-time)>300) break;//大于300us,通讯结束
		}
		bit_time[i] = micros()-time;
	}
max = min = bit_time[1];
for (i=1;i<41;i++)
	{
		if (bit_time[i] > max) max = bit_time[i];
		if (bit_time[i] < min) min = bit_time[i];
	}
for (i=1;i<41;i++)
	mid+=bit_time[i];
mid-=max;
mid-=min;
mid/=38;
//for (i=1;i<41;i++)	//查看原始40bit数据
//        printf("%d ",bit_time[i]);
//printf("mid = %d \n",mid);	//查看阈值
for (i=1;i<41;i++)//转换成0和1
	{
		if(bit_time[i]>(mid)) bit_time[i] = 1;
		else bit_time[i] = 0;
	}
//for (i=1;i<41;i++)
//	printf(" %d",bit_time[i]);
for (i=0;i<8;i++)
	{
		dat[0]<<=1;
		bit_time[i+1]?(dat[0] |= 0x01):(dat[0] &= ~0x01);
		dat[1]<<=1;
                bit_time[i+9]?(dat[1] |= 0x01):(dat[1] &= ~0x01);
		dat[2]<<=1;
                bit_time[i+17]?(dat[2] |= 0x01):(dat[2] &= ~0x01);
		dat[3]<<=1;
                bit_time[i+25]?(dat[3] |= 0x01):(dat[3] &= ~0x01);
		check<<=1;
                bit_time[i+33]?(check |= 0x01):(check &= ~0x01);
	}
if (dat[0]+dat[2] == check)
	{
//		printf(" check = %d ",check);	//查看校验位
		return 1;
	}
else
	return 0;
}
