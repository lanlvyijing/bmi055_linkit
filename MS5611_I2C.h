//YWROBOT
#ifndef MS5611_I2C_h
#define MS5611_I2C_h

#include <inttypes.h>
#include "Print.h" 
#include "Wire.h"

/***************************************/
#define MS561101BA_SlaveAddress 0xee //定义器件在IIC总线中的从地址
#define MS561101BA_D1 0x40
#define MS561101BA_D2 0x50
#define MS561101BA_RST 0x1E
//#define MS561101BA_D1_OSR_256 0x40
//#define MS561101BA_D1_OSR_512 0x42
//#define MS561101BA_D1_OSR_1024 0x44
//#define MS561101BA_D1_OSR_2048 0x46
#define MS561101BA_D1_OSR_4096 0x48
//#define MS561101BA_D2_OSR_256 0x50
//#define MS561101BA_D2_OSR_512 0x52
//#define MS561101BA_D2_OSR_1024 0x54
//#define MS561101BA_D2_OSR_2048 0x56
#define MS561101BA_D2_OSR_4096 0x58
#define MS561101BA_ADC_RD 0x00
#define MS561101BA_PROM_RD 0xA0
#define MS561101BA_PROM_CRC 0xAE
////////////////////////////////////////////////////////////////////////////////////
#define uchar unsigned char
#define ulong unsigned long
/****************************************/
class MS5611_I2C{

// Constructor / Destructor

// Method
    public:
	void MS561101BA_RESET();
        void MS561101BA_PROM_READ();
        unsigned long MS561101BA_DO_CONVERSION(uchar command);
        void MS561101BA_getTemperature(uchar OSR_Temp);
        void MS561101BA_getPressure(uchar OSR_Pres);
        void MS561101BA_Init();
        void I2C_WriteByte(uint8_t slv_add, uint8_t res_add,uint8_t w_data);
        void I2C_ReadnByte(uint8_t slv_add, uint8_t res_add, uint8_t num, uint8_t *buf);
        static unsigned long ex_Pressure; //串口读数转换值
    private:
	static unsigned int Cal_C[7]; //用于存放PROM中的8组数据
	static unsigned long D1_Pres; // 存放压力和温度
	static unsigned long D2_Temp; // 存放压力和温度
	static float dT;
	static float TEMP;
	static double OFF_,SENS;
	static float Pressure; //大气压
	static float TEMP2; //温度校验值
	static float Aux; //温度校验值
	static float OFF2; //温度校验值
	static float SENS2; //温度校验值

	static unsigned char exchange_num[8];
};
#endif
