#ifndef _MPU6050_H
#define _MPU6050_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "usart.h"
#include "i2c.h"
#include "stdio.h"

#define mpu_addr 0xD0

    void mpu_write(uint8_t addr, uint8_t data);
    uint8_t mpu_read(uint8_t addr);
    void mpu_init(void);
    int16_t mpu_acc_x(void);
    int16_t mpu_acc_y(void);
    int16_t mpu_acc_z(void);
    int16_t mpu_temp(void);
    int16_t mpu_gy_x(void);
    int16_t mpu_gy_y(void);
    int16_t mpu_gy_z(void);

#ifdef __cplusplus
}
#endif
#endif // _MPU6050_H