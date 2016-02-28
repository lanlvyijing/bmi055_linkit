/*
 * Displays text sent over the serial port (e.g. from the Serial Monitor) on
 * an attached LCD.
 * YWROBOT
 *Compatible with the Arduino IDE 1.0
 *Library version:1.1
 */

#include "BMI055_I2C.h"
#include "MS5611_I2C.h"
#include "LGATTSUart.h"
#include "LGATTSABC.h"
#include "Kalman.h"

#include <LGATTUUID.h>

#ifdef APP_LOG
#undef APP_LOG
#endif

#define APP_LOG(...) Serial.printf(__VA_ARGS__); \
    Serial.println();

LGATTSUart uart;
LGATTUT abc;

BMI055_I2C bmi055;
MS5611_I2C ms5611;


float fRad2Deg = 57.295779513f; //将弧度转为角度的乘数
const int MPU = 0x68; //MPU-6050的I2C地址
const int nValCnt = 6; //一次读取寄存器的数量

const int nCalibTimes = 100; //校准时读数的次数
short calibData[nValCnt]; //校准数据

unsigned long nLastTime = 0; //上一次读数的时间
float fLastRoll = 0.0f; //上一次滤波得到的Roll角
float fLastPitch = 0.0f; //上一次滤波得到的Pitch角
Kalman kalmanRoll; //Roll角滤波器
Kalman kalmanPitch; //Pitch角滤波器

void setup()
{
  delay(7000);
  Wire.begin();
  Serial.begin(9600);
  delay(100);
  bmi055.bmi055_init();
  
  Calibration();
  nLastTime = micros(); //记录当前时间
  Serial.println("[micros] nLastTime is: "+String(nLastTime));
  
  /**************BT begin***********************/
  /*
    if (!LGATTServer.begin(2, &uart, &abc))
    {
        APP_LOG("[FAILED] GATTS begin");
    }
    else
    {
        APP_LOG("GATTS begin");
    }
   // ms5611.MS561101BA_Init();*/
}

void loop()
{  
  short readouts[nValCnt];

  bmi055.ReadAccGyr(readouts); //读出测量值

  float realVals[6];
  Rectify(readouts, realVals); //根据校准的偏移量进行纠正

  //计算加速度向量的模长，均以g为单位
  float fNorm = sqrt(realVals[0] * realVals[0] + realVals[1] * realVals[1] + realVals[2] * realVals[2]);
  float fRoll = GetRoll(realVals, fNorm); //计算Roll角
  if (realVals[1] > 0) 
  {
    fRoll = -fRoll;
  }
  float fPitch = GetPitch(realVals, fNorm); //计算Pitch角
  if (realVals[0] < 0) 
  {
    fPitch = -fPitch;
  }

  //计算两次测量的时间间隔dt，以秒为单位
  unsigned long nCurTime = micros();
  float dt = (double)(nCurTime - nLastTime) / 1000000.0;
  //对Roll角和Pitch角进行卡尔曼滤波
  float fNewRoll = kalmanRoll.getAngle(fRoll, realVals[3], dt);
  float fNewPitch = kalmanPitch.getAngle(fPitch, realVals[4], dt);
  //跟据滤波值计算角度速
  float fRollRate = (fNewRoll - fLastRoll) / dt;
  float fPitchRate = (fNewPitch - fLastPitch) / dt;
 
 //更新Roll角和Pitch角
  fLastRoll = fNewRoll;
  fLastPitch = fNewPitch;
  //更新本次测的时间
  nLastTime = nCurTime;

  //向串口打印输出Roll角和Pitch角，运行时在Arduino的串口监视器中查看
  Serial.print("Roll:");
  Serial.print(fNewRoll); Serial.print('(');
  Serial.print(fRollRate); Serial.print("),\tPitch:");
  Serial.print(fNewPitch); Serial.print('(');
  Serial.print(fPitchRate); Serial.print(")\n");
  delay(10);
  
  
  /*
    ms5611.MS561101BA_getPressure(MS561101BA_D1_OSR_4096);        
    Serial.println("ex_Pressure: "+String(ms5611.ex_Pressure));
    ms5611.MS561101BA_getTemperature(MS561101BA_D2_OSR_4096);
     
    bmi055.BMI055_ReadSensor(&acc_x, &acc_y, &acc_z, &gyo_x, &gyo_y, &gyo_z);
       
     LGATTServer.handleEvents();
  //LGATTAttributeValue value = {13, "hello, world!"};
  if (uart.isConnected())                                //boolean isConnected(){ return _connected; };
  {
    APP_LOG("isConnected");
    LGATTAttributeValue value = {0};
    const char *str = "[peripheral] hello";
    value.len = strlen(str);
    memcpy(value.value, str, value.len);
    boolean ret = uart.sendIndication(value, uart.getHandleNotify(), false);
    if (!ret)
    {
        APP_LOG("[FAILED] send [%d]", ret);
    }
    APP_LOG("send [%d][%s]", ret, str);
  }
  //uart.send(value, uart.getHandle(0), false); // just a notify
  delay(2000);*/
}


void Calibration()
{
  float valSums[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0};
  //先求和
  for (int i = 0; i < nCalibTimes; ++i) 
  {
     short mpuVals[nValCnt];
     bmi055.ReadAccGyr(mpuVals);
     for (int j = 0; j < nValCnt; ++j) 
     {
        valSums[j] += mpuVals[j];
     }
     delay(10);
  }
  //再求平均
  for (int i = 0; i < nValCnt; ++i) {
    calibData[i] = short(valSums[i] / nCalibTimes);
  }
  calibData[2] -= 512; //设芯片Z轴竖直向下，设定静态工作点。
  Serial.println("[Calibration] calibData ACC_X: "+String(calibData[0]));
  Serial.println("[Calibration] calibData ACC_Y: "+String(calibData[1]));
  Serial.println("[Calibration] calibData ACC_Z: "+String(calibData[2]));
  Serial.println("[Calibration] calibData GYO_X: "+String(calibData[3]));
  Serial.println("[Calibration] calibData GYO_Y: "+String(calibData[4]));
  Serial.println("[Calibration] calibData GYO_Z: "+String(calibData[5]));
  delay(3000);
  
}


//算得Roll角。算法见文档。
float GetRoll(float *pRealVals, float fNorm) {
  float fNormXZ = sqrt(pRealVals[0] * pRealVals[0] + pRealVals[2] * pRealVals[2]);
  float fCos = fNormXZ / fNorm;
  return acos(fCos) * fRad2Deg;
}

//算得Pitch角。算法见文档。
float GetPitch(float *pRealVals, float fNorm) {
  float fNormYZ = sqrt(pRealVals[1] * pRealVals[1] + pRealVals[2] * pRealVals[2]);
  float fCos = fNormYZ / fNorm;
  return acos(fCos) * fRad2Deg;
}

//对读数进行纠正，消除偏移，并转换为物理量。公式见文档。
void Rectify(short *pReadout, float *pRealVals) {
  for (int i = 0; i < 3; ++i) {
    //pRealVals[i] = (float)(pReadout[i] - calibData[i]) / 16384.0f;
    pRealVals[i] = ((float)(pReadout[i] - calibData[i]))/512.0f;
  }
 // pRealVals[3] = pReadout[3] / 340.0f + 36.53;
  for (int i = 3; i < 6; ++i) {
    pRealVals[i] = (float)(pReadout[i] - calibData[i]) / 16.4f;
  }
}

