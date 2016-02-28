//YWROBOT
#ifndef BMI055_I2C_h
#define BMI055_I2C_h

#include <inttypes.h>
#include "Print.h" 
#include "Wire.h"
#include "commondata.h"
/***************************************/

#define ACC_CHIPID  0x00
#define ACC_X_LSB   0x02
#define ACC_X_MSB   0x03
#define ACC_Y_LSB   0x04
#define ACC_Y_MSB   0x05
#define ACC_Z_LSB   0x06
#define ACC_Z_MSB   0x07
#define ACC_RANGE   0x0f
#define ACC_BW      0x10
#define ACC_SFRSET  0x14
#define ACC_OFC_CTRL     0x36

#define GYRO_CHIPID       0x00
#define GYRO_X_LSB        0x02
#define GYRO_X_MSB        0x03
#define GYRO_Y_LSB        0x04
#define GYRO_Y_MSB        0x05
#define GYRO_Z_LSB        0x06
#define GYRO_Z_MSB        0x07
#define GYRO_INT_STATUS1  0x0a
#define GYRO_RANGE        0x0f
#define GYRO_BW           0x10
#define GYRO_SFRSET       0x14
#define GYRO_INT_EN0      0x15
#define GYRO_SOC          0x31
#define GYRO_FOC          0x32

#define BMI055_ACC_ADDR  0x30
#define BMI055_GYRO_ADDR 0xd0
/************************************/
#define ACC_0G_X      2048
#define ACC_1G_X      (2048+1024)
#define ACC_MINUS1G_X (2048-1024)
#define ACC_0G_Y      2048   
#define ACC_1G_Y      (2048+1024)
#define ACC_MINUS1G_Y (2048-1024)
#define ACC_0G_Z      2048       
#define ACC_1G_Z      (2048+1024)
#define ACC_MINUS1G_Z (2048-1024)
/****************************************/
class BMI055_I2C{

// Constructor / Destructor

// Method
public:
	void I2C_WriteByte(uint8_t slv_add, uint8_t res_add,uint8_t w_data);
        void I2C_ReadnByte(uint8_t slv_add, uint8_t res_add, uint8_t num, uint8_t *buf);
        void BMI055_ReadSensor(float *ax, float *ay, float *az,float *gx, float *gy, float *gz);
        void DisplayBMI055Log(void);
        void bmi055_init(void);
        void ReadAccGyr(short *pVals) ;
private:
	static int ii;
};


#endif
