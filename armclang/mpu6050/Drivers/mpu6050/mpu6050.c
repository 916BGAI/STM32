#include "mpu6050.h"

void mpu_write(uint8_t addr, uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, mpu_addr, addr, 1, &data, 1, 100);
}

uint8_t mpu_read(uint8_t addr)
{
    uint8_t data[] = {
        0x00,
    };
    HAL_I2C_Mem_Read(&hi2c1, mpu_addr, addr, 1, data, 1, 100);
    return data[0];
}

void mpu_init(void)
{
    if (mpu_read(0x75) != 0x68)
    {
        while (1)
        {
            printf("Error !");
            HAL_Delay(1000);
        }
    }
    else
    {
        mpu_write(0x1C, 0x18); //修改加速度速率
        mpu_write(0x1B, 0x18); //修改角速度速率
        mpu_write(0x6B, 0x01); //关闭休眠，修改时钟至GX
    }
}

int16_t mpu_acc_x(void)
{
    int16_t data = 0;
    data = mpu_read(0x3B) << 8;
    data = data | mpu_read(0x3C);
    return data;
}

int16_t mpu_acc_y(void)
{
    int16_t data = 0;
    data = mpu_read(0x3D) << 8;
    data = data | mpu_read(0x3E);
    return data;
}

int16_t mpu_acc_z(void)
{
    int16_t data = 0;
    data = mpu_read(0x3F) << 8;
    data = data | mpu_read(0x40);
    return data;
}

int16_t mpu_temp(void)
{
    int16_t data = 0;
    data = mpu_read(0x41) << 8;
    data = data | mpu_read(0x42);
    return data;
}

int16_t mpu_gy_x(void)
{
    int16_t data = 0;
    data = mpu_read(0x43) << 8;
    data = data | mpu_read(0x44);
    return data;
}

int16_t mpu_gy_y(void)
{
    int16_t data = 0;
    data = mpu_read(0x45) << 8;
    data = data | mpu_read(0x46);
    return data;
}

int16_t mpu_gy_z(void)
{
    int16_t data = 0;
    data = mpu_read(0x47) << 8;
    data = data | mpu_read(0x48);
    return data;
}