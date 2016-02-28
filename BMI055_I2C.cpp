//YWROBOT
//last updated on 21/12/2011
//Tim Starling Fix the reset bug (Thanks Tim)
//wiki doc http://www.dfrobot.com/wiki/index.php?title=I2C/TWI_LCD1602_Module_(SKU:_DFR0063)
//Support Forum: http://www.dfrobot.com/forum/
//Compatible with the Arduino IDE 1.0
//Library version:1.1


#include "BMI055_I2C.h"
#include <inttypes.h>
//#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"


int BMI055_I2C::ii=0;

void BMI055_I2C::bmi055_init(void)
{
	I2C_WriteByte(BMI055_ACC_ADDR, ACC_SFRSET, 0xb6); //acc soft reset, check
	delay(100);
	I2C_WriteByte(BMI055_ACC_ADDR, ACC_RANGE, 0x05); //4g  0x03:2g 0x05:4g 0x08:8g 0x0a:16g
	delay(10);
	I2C_WriteByte(BMI055_ACC_ADDR, ACC_BW, 0x0c); //1000HZ 0x0c:250HZ 0x0d:500HZ 0x0f:2000HZ
	delay(10);
//	I2C_WriteByte(BMI055_ACC_ADDR, ACC_OFC_CTRL, 0x07); //enable acc slow offset compensation
//	delay(10);
	
	I2C_WriteByte(BMI055_GYRO_ADDR, GYRO_RANGE, 0x00); //2000 deg/s  0x00:2000 0x01:1000 0x02:500 0x03:250 0x04:125
	delay(10);
	I2C_WriteByte(BMI055_GYRO_ADDR, GYRO_BW, 0x03); //1000HZ  0x01:2000 0x02:1000 0x03:400 0x04:200 0x05:100
	delay(10);
	I2C_WriteByte(BMI055_GYRO_ADDR, GYRO_INT_EN0, 0xa0); //new data int enable and auto-offset compensation
	delay(10);
//	I2C_WriteByte(BMI055_GYRO_ADDR, GYRO_SOC, 0x67); //enables gyro slow offset compensation for x/y/z
//	delay();
//	I2C_WriteByte(BMI055_GYRO_ADDR, GYRO_FOC, 0xc7); //enables fast offset compensation for x/y/z
//	delay();
        //I2C_WriteByte(BMI055_ACC_ADDR, ACC_SFRSET, 0xb6); //acc soft reset, check
	DisplayBMI055Log();
}

void BMI055_I2C::DisplayBMI055Log()
{
        Serial.println("start to display");
	uint8_t tmpreg[16];
	I2C_ReadnByte(BMI055_ACC_ADDR, ACC_CHIPID, 1, &tmpreg[0]);
	Serial.print("acc chip id: 0x");
        Serial.println(tmpreg[0],HEX);
	delay(10);
	I2C_ReadnByte(BMI055_ACC_ADDR, ACC_RANGE, 1, &tmpreg[1]);
	Serial.print("acc range: 0x");
        Serial.println(tmpreg[1],HEX);
	delay(10);
	I2C_ReadnByte(BMI055_ACC_ADDR, ACC_BW, 1, &tmpreg[2]);
	Serial.print("acc bw: 0x");
        Serial.println(tmpreg[2],HEX);
	delay(10);
	I2C_ReadnByte(BMI055_ACC_ADDR, ACC_OFC_CTRL, 1, &tmpreg[3]);
	Serial.print("acc ofc: 0x");
        Serial.println(tmpreg[3],HEX);
	delay(10);
	
	I2C_ReadnByte(BMI055_GYRO_ADDR, GYRO_RANGE, 1, &tmpreg[4]);
	Serial.print("gyro range: 0x");
        Serial.println(tmpreg[4],HEX);
	I2C_ReadnByte(BMI055_GYRO_ADDR, GYRO_BW, 1, &tmpreg[5]);
	Serial.print("gyro bw: 0x");
        Serial.println(tmpreg[5],HEX);
	I2C_ReadnByte(BMI055_GYRO_ADDR, GYRO_INT_EN0, 1, &tmpreg[6]); 
	Serial.print("gyro int en0: 0x");
        Serial.println(tmpreg[6],HEX);
	I2C_ReadnByte(BMI055_GYRO_ADDR, GYRO_SOC, 1, &tmpreg[7]);
	Serial.print("gyro soc: 0x");
        Serial.println(tmpreg[7],HEX);
	I2C_ReadnByte(BMI055_GYRO_ADDR, GYRO_FOC, 1, &tmpreg[8]);
	Serial.print("gyro foc: 0x");
        Serial.println(tmpreg[8],HEX);
        Serial.println("end display");
        
}

void BMI055_I2C::BMI055_ReadSensor(float *ax, float *ay, float *az,float *gx, float *gy, float *gz)
{
	uint8_t buf[6];
	float a_x = 0, a_y = 0, a_z = 0;
        float g_x = 0, g_y = 0, g_z = 0;
        int gg_x = 0, gg_y = 0, gg_z = 0;
	/****************陀螺仪****************************/
	I2C_ReadnByte(BMI055_GYRO_ADDR, GYRO_INT_STATUS1, 1, &buf[0]);

	while(!(buf[0] & 0x80))
	{
		//Serial.println("buf[0] & 0x80 != 1");
                I2C_ReadnByte(BMI055_GYRO_ADDR, GYRO_INT_STATUS1, 1, &buf[0]);
		delay(10);
	}
       
	I2C_ReadnByte(BMI055_GYRO_ADDR, GYRO_X_LSB, 6, &buf[0]);
        
          gg_x =  ((buf[1]<<8) |buf[0]) ;
          gg_y =  ((buf[3]<<8) |buf[2]) ;
          gg_z =  ((buf[5]<<8) |buf[4]) ;
          Serial.println(gg_x);
         Serial.println(gg_y);
         Serial.println(gg_z);
          if(gg_x>32768)
          {
            gg_x = -(gg_x-32768);           
          }
          g_x = gg_x/32768.0*2000;
          if(gg_y>32768)
          {
            gg_y = -(gg_y-32768);
          }
          g_y = gg_y/32768.0*2000;
          if(gg_z>32768)
          {
            gg_z = -(gg_z-32768);
          }
          g_z = gg_z/32768.0*2000;
         
/*
	
        gyo_x = g_x;
        gyo_y = g_y;
        gyo_z = g_z;*/
	Serial.println("gx="+ String(g_x)+" dec/sec");
        Serial.println("gy="+ String(g_y)+" dec/sec");
        Serial.println("gz="+ String(g_z)+" dec/sec");
	/****************加速度计****************************/
	I2C_ReadnByte(BMI055_ACC_ADDR, ACC_X_LSB, 6, &buf[0]);
	while(!((buf[0] & 0x01) && (buf[2] & 0x01) && (buf[4] & 0x01)))
	{
		I2C_ReadnByte(BMI055_ACC_ADDR, ACC_X_LSB, 6, &buf[0]);
		delay(10);
	}

        a_x = (((short)((buf[1] << 8) | buf[0])) >> 4)/512.0f;
	a_y = (((short)((buf[3] << 8) | buf[2])) >> 4)/512.0f;
	a_z = (((short)((buf[5] << 8) | buf[4])) >> 4)/512.0f;
/*
        acc_x = a_x;
        acc_y = a_y;
        acc_z = a_z;
        */
	Serial.println("ax="+ String(a_x));
        Serial.println("ay="+ String(a_y));
        Serial.println("az="+ String(a_z));
}


void BMI055_I2C::ReadAccGyr(short *pVals) 
{
        uint8_t buf[6];
       
        /****************加速度计****************************/
	I2C_ReadnByte(BMI055_ACC_ADDR, ACC_X_LSB, 6, &buf[0]);
	while(!((buf[0] & 0x01) && (buf[2] & 0x01) && (buf[4] & 0x01)))
	{
		I2C_ReadnByte(BMI055_ACC_ADDR, ACC_X_LSB, 6, &buf[0]);
		delay(10);
	}

        pVals[0] =  ((buf[1]<<8) |buf[0]);
        pVals[0] = pVals[0]>>4;
        pVals[1] =  ((buf[3]<<8) |buf[2]);
        pVals[1] = pVals[1]>>4;
        pVals[2] =  ((buf[5]<<8) |buf[4]);
        pVals[2] = pVals[2]>>4;
        
	/****************陀螺仪****************************/
	I2C_ReadnByte(BMI055_GYRO_ADDR, GYRO_INT_STATUS1, 1, &buf[0]);

	while(!(buf[0] & 0x80))
	{
		//Serial.println("buf[0] & 0x80 != 1");
                I2C_ReadnByte(BMI055_GYRO_ADDR, GYRO_INT_STATUS1, 1, &buf[0]);
		delay(10);
	}
       
	I2C_ReadnByte(BMI055_GYRO_ADDR, GYRO_X_LSB, 6, &buf[0]);
        
          pVals[3] =  ((buf[1]<<8) |buf[0]);
          pVals[4] =  ((buf[3]<<8) |buf[2]);
          pVals[5] =  ((buf[5]<<8) |buf[4]);
          
}

void BMI055_I2C::I2C_WriteByte(uint8_t slv_add, uint8_t res_add,uint8_t w_data)
{
  Wire.beginTransmission(slv_add>>1); //开启MPU6050的传输
  Wire.write(res_add); //指定寄存器地址
  Wire.write(w_data); //写入一个字节的数据
  Wire.endTransmission(true); //结束传输，true表示释放总线
}

void BMI055_I2C::I2C_ReadnByte(uint8_t slv_add, uint8_t res_add, uint8_t num, uint8_t *buf)
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
