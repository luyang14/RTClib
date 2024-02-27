#include "RTClib.h"

#define HT1381_SET_ADDRESS 0x80		//ht1381写地址起始位  每个数据间地址差2 共7个数据
#define HT1381_GET_ADDRESS 0x81		//ht1381读地址起始位  每个数据间地址差2 共7个数据
void RTC_HT1381::Delay(uint16_t ns)
{
    while(--ns);
}

void RTC_HT1381::WriteByte(uint8_t ucDat)
{
    unsigned char i;
    pinMode(io_pin,OUTPUT);
    Delay(10);
    for(i=8; i>0; i--)
    {
        digitalWrite(io_pin,ucDat & 0x01);

        digitalWrite(scl_pin,HIGH);
        Delay(10);
        digitalWrite(scl_pin,LOW);

        ucDat >>= 1;
    }
}
uint8_t RTC_HT1381::ReadByte(void)
{
    uint8_t i;
    uint8_t ucDat=0;

    pinMode(io_pin,INPUT);
    Delay(10);

    for(i=8; i>0; i--)
    {
        digitalWrite(io_pin,HIGH);
        Delay(10);
        ucDat >>= 1;
        Delay(10);
        if(digitalRead(io_pin))  ucDat|=0x80;

        digitalWrite(scl_pin,HIGH);
        Delay(10);
        digitalWrite(scl_pin,LOW);
        Delay(10);
    }
    return(ucDat);
}

void RTC_HT1381::WriteData(uint8_t ucAddress, uint8_t ucData)
{
	  digitalWrite(rst_pin,LOW);
    Delay(10);
    digitalWrite(scl_pin,LOW);
    Delay(10);
    digitalWrite(rst_pin,HIGH);
    Delay(10);
    WriteByte(ucAddress);
    WriteByte(ucData);
    digitalWrite(scl_pin,HIGH);
    Delay(10);
    digitalWrite(rst_pin,LOW);
    return;
}

uint8_t RTC_HT1381::ReadData(uint8_t ucAddress)
{
    uint8_t ucDat = 0;

    digitalWrite(rst_pin,LOW);
    digitalWrite(scl_pin,LOW);
    digitalWrite(rst_pin,HIGH);
    WriteByte(ucAddress);  
    ucDat = ReadByte(); 		
    digitalWrite(scl_pin,HIGH);
    digitalWrite(rst_pin,LOW);

    return(ucDat);
}
void RTC_HT1381::setSCL_pin(uint8_t scl)
{
  scl_pin = scl;
}
void RTC_HT1381::setIO_pin(uint8_t io)
{
  io_pin = io;
}
void RTC_HT1381::setRST_pin(uint8_t rst)
{
  rst_pin = rst;
}
/**************************************************************************/
/*!
    @brief  Startup for the HT1381
    @return Always true
*/
/**************************************************************************/
boolean RTC_HT1381::begin(void) {
  pinMode(rst_pin,OUTPUT);
  pinMode(io_pin,OUTPUT);
  pinMode(scl_pin,OUTPUT);

  digitalWrite(scl_pin,LOW);
  Delay(10);
  digitalWrite(rst_pin,LOW);
  Delay(10);
  WriteData(0x8e,0x00);		//取消写保护
  WriteData(0x90,0x00);		//设置充电限流电阻阻值  0 不使能没有限流电阻
  return true;
}

boolean RTC_HT1381::begin(uint8_t rst,uint8_t io,uint8_t scl) {
  rst_pin = rst;
  io_pin = io;	
  scl_pin = scl;
  
  pinMode(rst_pin,OUTPUT);
  pinMode(io_pin,OUTPUT);
  pinMode(scl_pin,OUTPUT);

  digitalWrite(scl_pin,LOW);
  Delay(10);
  digitalWrite(rst_pin,LOW);
  Delay(10);
  WriteData(0x8e,0x00);		//取消写保护
  WriteData(0x90,0x00);		//设置充电限流电阻阻值  0 不使能没有限流电阻
  return true;
}

/**************************************************************************/
/*!
    @brief  Set the date and time in the HT1381
    @param dt DateTime object containing the desired date/time
*/
/**************************************************************************/
void RTC_HT1381::adjust(const DateTime& dt) {
  uint8_t ucAddress = HT1381_SET_ADDRESS;
  WriteData(0x8e,0x00);  /* 控制命令,WP=0,写操作?*/
  
  WriteData(ucAddress,bin2bcd(dt.second()));ucAddress+=2;
  WriteData(ucAddress,bin2bcd(dt.minute()));ucAddress+=2;
  WriteData(ucAddress,bin2bcd(dt.hour()));ucAddress+=2;

  WriteData(ucAddress,bin2bcd(dt.day()));ucAddress+=2;
  WriteData(ucAddress,bin2bcd(dt.month()));ucAddress+=2;
  WriteData(ucAddress,bin2bcd(0));ucAddress+=2;// skip weekdays
  WriteData(ucAddress,bin2bcd(dt.year() - 2000));

  WriteData(0x8e,0x80);  /* 控制命令,WP=1,写保护?*/
}

/**************************************************************************/
/*!
    @brief  Get the current date and time from the HT1381
    @return DateTime object containing the current date and time
*/
/**************************************************************************/
DateTime RTC_HT1381::now() {
  uint8_t ucAddress = HT1381_GET_ADDRESS;

  uint8_t ss = bcd2bin(ReadData(ucAddress));ucAddress+= 2;
  uint8_t mm = bcd2bin(ReadData(ucAddress));ucAddress+= 2;
  uint8_t hh = bcd2bin(ReadData(ucAddress));ucAddress+= 2;

  uint8_t d = bcd2bin(ReadData(ucAddress));ucAddress+= 2;
  uint8_t m = bcd2bin(ReadData(ucAddress));ucAddress+= 2;
  ucAddress+= 2;// skip weekdays
  uint16_t y = bcd2bin(ReadData(ucAddress)) + 2000;
   
  return DateTime (y, m, d, hh, mm, ss);
}
