//YWROBOT
//last updated on 21/12/2011
//Tim Starling Fix the reset bug (Thanks Tim)
//wiki doc http://www.dfrobot.com/wiki/index.php?title=I2C/TWI_LCD1602_Module_(SKU:_DFR0063)
//Support Forum: http://www.dfrobot.com/forum/
//Compatible with the Arduino IDE 1.0
//Library version:1.1


#include "MS5611_I2C.h"
#include <inttypes.h>
//#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

unsigned int MS5611_I2C::Cal_C[7]; //用于存放PROM中的8组数据
unsigned long MS5611_I2C::D1_Pres; // 存放压力和温度
unsigned long MS5611_I2C::D2_Temp; // 存放压力和温度
float MS5611_I2C::dT;
float MS5611_I2C::TEMP;
double MS5611_I2C::OFF_;
double MS5611_I2C::SENS;
float MS5611_I2C::Pressure; //大气压
float MS5611_I2C::TEMP2; //温度校验值
float MS5611_I2C::Aux; //温度校验值
float MS5611_I2C::OFF2; //温度校验值
float MS5611_I2C::SENS2; //温度校验值
unsigned long MS5611_I2C::ex_Pressure; //串口读数转换值
unsigned char MS5611_I2C::exchange_num[8];

void MS5611_I2C::MS561101BA_RESET()
{
   //Serial.println("start MS561101BA_RESET");
   Wire.beginTransmission(MS561101BA_SlaveAddress>>1); //开启MPU6050的传输
   Wire.write(MS561101BA_RST); //指定寄存器地址
   Wire.endTransmission(true); //结束传输，true表示释放总线
   //Serial.println("end MS561101BA_RESET");
}

void MS5611_I2C::MS561101BA_PROM_READ()
{
    uchar d1,d2,i;
    Serial.println("start MS561101BA_PROM_READ");
    for(i=0;i<=6;i++)
    {
        Wire.beginTransmission(MS561101BA_SlaveAddress>>1);
        Wire.write((MS561101BA_PROM_RD+i*2));

        Wire.requestFrom(MS561101BA_SlaveAddress>>1,2,true);
        while((Wire.available()))
        {
           d1 = char(Wire.read());
           d2 = char(Wire.read());
        }
        
        Wire.endTransmission(true);
        
        delay(5);
        Cal_C[i]=((unsigned int)d1<<8)|d2;
        Serial.println("Cal_C["+String(i)+"]"+ String(Cal_C[i]));
    }
    Serial.println("end MS561101BA_PROM_READ");
}

ulong MS5611_I2C::MS561101BA_DO_CONVERSION(uchar command)
{
      uchar con = 0;
      ulong conversion=0;
      ulong conv1,conv2,conv3;
      Wire.beginTransmission(MS561101BA_SlaveAddress);
      Wire.write(command);
      Wire.endTransmission(true);
      delay(100);
      Wire.beginTransmission(MS561101BA_SlaveAddress);
      Wire.write(0);
      Wire.requestFrom(MS561101BA_SlaveAddress>>1,3,true);
      while((Wire.available()))
      {
 
           conv1 = char(Wire.read());

           conv2 = char(Wire.read());

           conv3 = char(Wire.read());

      }
      Wire.endTransmission(true);
      Serial.println(" conv1 = " + String(conv1) +" conv2 = " + String(conv2) +" conv3 = " + String(conv3));
      conversion=conv1*65535+conv2*256+conv3;
      Serial.println("conversion = " + String(conversion));
      return conversion;
}

void MS5611_I2C::MS561101BA_getTemperature(uchar OSR_Temp)
{
    D2_Temp= MS561101BA_DO_CONVERSION(OSR_Temp);
    delay(10);
    dT=D2_Temp - (((ulong)Cal_C[5])<<8);
    TEMP=2000+dT*((ulong)Cal_C[6])/8388608;
}

void MS5611_I2C::MS561101BA_getPressure(uchar OSR_Pres)
{
      Serial.println("start MS561101BA_getPressure");
      D1_Pres= MS561101BA_DO_CONVERSION(OSR_Pres);
      Serial.println("D1_Pres = " + String(OSR_Pres));
      delay(10);
      OFF_=(ulong)Cal_C[2]*65536+((ulong)Cal_C[4]*dT)/128;
      SENS=(ulong)Cal_C[1]*32768+((ulong)Cal_C[3]*dT)/256;
      Serial.println("OFF_ = " + String(OFF_));
      Serial.println("SENS = " + String(SENS));
      if(TEMP < 2000)
      {
            // second order temperature compensation when under 20 degrees C
            TEMP2 = (dT*dT) / 0x80000000;
            Aux = TEMP*TEMP;
            OFF2 = 2.5*Aux;
            SENS2 = 1.25*Aux;
            TEMP = TEMP - TEMP2;
            OFF_ = OFF_ - OFF2;
            SENS = SENS - SENS2;
      }
      Pressure=(D1_Pres*SENS/2097152-OFF_)/32768;
      Serial.println("end MS561101BA_getPressure , Pressure = " + String(Pressure));
}

void MS5611_I2C::MS561101BA_Init()
{
    Serial.println("start MS561101BA_Init");
    while(1)
    MS561101BA_RESET();
    delay(1000);
    MS561101BA_PROM_READ();
    delay(1000);
    Serial.println("MS561101BA_Init ok");
}

void MS5611_I2C::I2C_WriteByte(uint8_t slv_add, uint8_t res_add, uint8_t w_data)
{
  Wire.beginTransmission(slv_add>>1); //开启MPU6050的传输
  Wire.write(res_add); //指定寄存器地址
  Wire.write(w_data); //写入一个字节的数据
  Wire.endTransmission(true); //结束传输，true表示释放总线
}

void MS5611_I2C::I2C_ReadnByte(uint8_t slv_add, uint8_t res_add, uint8_t num, uint8_t *buf)
{
  int ii=0;
  Wire.beginTransmission(slv_add>>1);
  Wire.write(res_add);
  Wire.requestFrom(slv_add>>1,num,true);
  while((Wire.available()))
  {
     buf[ii++] = char(Wire.read());
  }
  Wire.endTransmission(true);
}
