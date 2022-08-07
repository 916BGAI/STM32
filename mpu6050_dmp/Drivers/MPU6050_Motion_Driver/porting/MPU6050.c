#include "MPU6050.h"

/* Private typedef ----------------------------------------------------------------------------------------------------------------*/
volatile uint32_t hal_timestamp = 0;

struct rx_s
{
    unsigned char header[3];
    unsigned char cmd;
};
struct hal_s
{
    unsigned char lp_accel_mode;
    unsigned char sensors;
    unsigned char dmp_on;
    unsigned char wait_for_tap;
    volatile unsigned char new_gyro;
    unsigned char motion_int_mode;
    unsigned long no_dmp_hz;
    unsigned long next_pedo_ms;
    unsigned long next_temp_ms;
    unsigned long next_compass_ms;
    unsigned int report;
    unsigned short dmp_features;
    struct rx_s rx;
};
static struct hal_s hal = {0};

/* USB RX binary semaphore. Actually, it's just a flag. Not included in struct
 * because it's declared extern elsewhere.
 */
volatile unsigned char rx_new;

unsigned char *mpl_key = (unsigned char *)"eMPL 5.1";

/* Platform-specific information. Kinda like a boardfile. */
struct platform_data_s
{
    signed char orientation[9];
};

/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from the
 * driver(s).
 * TODO: The following matrices refer to the configuration on internal test
 * boards at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static struct platform_data_s gyro_pdata = {
    .orientation = {1, 0, 0,
                    0, 1, 0,
                    0, 0, 1}};
/* Private typedef ----------------------------------------------------------------------------------------------------------------*/

/**********************************************
函数名称：MPU_Write_Byte
函数功能：IIC写一个字节
函数参数：data:写入的数据    reg:要写的寄存器地址
函数返回值：0,写入成功  其他,写入失败
**********************************************/
uint8_t MPU_Write_Byte(uint8_t reg, uint8_t data) // IIC写一个字节
{
    HAL_I2C_Mem_Write(&hi2c1, 0xD0, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    return 0;
}

/**********************************************
函数名称：MPU_Read_Byte
函数功能：IIC读一个字节
函数参数：reg:要读的寄存器地址
函数返回值：data:读取到的数据
**********************************************/
uint8_t MPU_Read_Byte(uint8_t reg) // IIC读一个字节
{
    uint8_t data[] = {
        0x00,
    };
    HAL_I2C_Mem_Read(&hi2c1, 0xD0, reg, I2C_MEMADD_SIZE_8BIT, data, 1, 100);
    return data[0];
}

/**********************************************
函数名称：MPU_Write_Len
函数功能：IIC连续写(写器件地址、寄存器地址、数据)
函数参数：addr:器件地址      reg:寄存器地址
                 len:写入数据的长度  data:数据区
函数返回值：0,写入成功  其他,写入失败
**********************************************/
uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
    HAL_I2C_Mem_Write(&hi2c1, addr, reg, I2C_MEMADD_SIZE_8BIT, data, len, 100);
    return 0;
}

/**********************************************
函数名称：MPU_Read_Len
函数功能：IIC连续读(写入器件地址后,读寄存器地址、数据)
函数参数：addr:器件地址        reg:要读的寄存器地址
                 len:要读取的数据长度  data:读取到的数据存储区
函数返回值：0,读取成功  其他,读取失败
**********************************************/
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
    HAL_I2C_Mem_Read(&hi2c1, addr, reg, I2C_MEMADD_SIZE_8BIT, data, len, 100);
    return 0;
}

/**********************************************
函数名称：MPU_Set_Gyro_Fsr
函数功能：设置MPU6050陀螺仪传感器满量程范围
函数参数：fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
函数返回值：0,设置成功  其他,设置失败
**********************************************/
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
    return MPU_Write_Byte(MPU_GYRO_CFG_REG, fsr << 3); //设置陀螺仪满量程范围
}

/**********************************************
函数名称：MPU_Set_Accel_Fsr
函数功能：设置MPU6050加速度传感器满量程范围
函数参数：fsr:0,±2g;1,±4g;2,±8g;3,±16g
函数返回值：0,设置成功  其他,设置失败
**********************************************/
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
    return MPU_Write_Byte(MPU_ACCEL_CFG_REG, fsr << 3); //设置加速度传感器满量程范围
}

/**********************************************
函数名称：MPU_Set_LPF
函数功能：设置MPU6050的数字低通滤波器
函数参数：lpf:数字低通滤波频率(Hz)
函数返回值：0,设置成功  其他,设置失败
**********************************************/
uint8_t MPU_Set_LPF(uint16_t lpf)
{
    uint8_t data = 0;

    if (lpf >= 188)
        data = 1;
    else if (lpf >= 98)
        data = 2;
    else if (lpf >= 42)
        data = 3;
    else if (lpf >= 20)
        data = 4;
    else if (lpf >= 10)
        data = 5;
    else
        data = 6;
    return MPU_Write_Byte(MPU_CFG_REG, data); //设置数字低通滤波器
}

/**********************************************
函数名称：MPU_Set_Rate
函数功能：设置MPU6050的采样率(假定Fs=1KHz)
函数参数：rate:4~1000(Hz)  初始化中rate取50
函数返回值：0,设置成功  其他,设置失败
**********************************************/
uint8_t MPU_Set_Rate(uint16_t rate)
{
    uint8_t data;
    if (rate > 1000) rate = 1000;
    if (rate < 4) rate = 4;
    data = 1000 / rate - 1;
    data = MPU_Write_Byte(MPU_SAMPLE_RATE_REG, data); //设置数字低通滤波器
    return MPU_Set_LPF(rate / 2);                     //自动设置LPF为采样率的一半
}

/**********************************************
函数名称：MPU_Init
函数功能：初始化MPU6050
函数参数：无
函数返回值：0,初始化成功  其他,初始化失败
**********************************************/
uint8_t MPU6050_Init(void)
{
    uint8_t res;

    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80); //复位MPU6050
    HAL_Delay(100);
    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00); //唤醒MPU6050
    MPU_Set_Gyro_Fsr(3);                     //陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                    //加速度传感器,±2g
    MPU_Set_Rate(50);                        //设置采样率50Hz
    MPU_Write_Byte(MPU_INT_EN_REG, 0X00);    //关闭所有中断
    MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00); // I2C主模式关闭
    MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);   //关闭FIFO
    MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80); // INT引脚低电平有效

    res = MPU_Read_Byte(MPU_DEVICE_ID_REG);
    if (res == MPU_ADDR) //器件ID正确,即res = MPU_ADDR = 0x68
    {
        MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01); //设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00); //加速度与陀螺仪都工作
        MPU_Set_Rate(50);                        //设置采样率为50Hz
    }
    else
        return 1; //地址设置错误,返回1
    return 0;     //地址设置正确,返回0
}

/**********************************************
函数名称：MPU_Get_Temperature
函数功能：得到温度传感器值
函数参数：无
函数返回值：温度值(扩大了100倍)
**********************************************/
short MPU_Get_Temperature(void)
{
    uint8_t buf[2];
    short raw;
    float temp;

    MPU_Read_Len(0xD0, MPU_TEMP_OUTH_REG, 2, buf);
    raw = ((uint16_t)buf[0] << 8) | buf[1];
    temp = 36.53 + ((double)raw) / 340;
    return temp * 100;
}

/**********************************************
函数名称：MPU_Get_Gyroscope
函数功能：得到陀螺仪值(原始值)
函数参数：gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
函数返回值：0,读取成功  其他,读取失败
**********************************************/
uint8_t MPU_Get_Gyroscope(short *gx, short *gy, short *gz)
{
    uint8_t buf[6], res;

    res = MPU_Read_Len(0xD0, MPU_GYRO_XOUTH_REG, 6, buf);
    if (res == 0)
    {
        *gx = ((uint16_t)buf[0] << 8) | buf[1];
        *gy = ((uint16_t)buf[2] << 8) | buf[3];
        *gz = ((uint16_t)buf[4] << 8) | buf[5];
    }
    return res;
}

/**********************************************
函数名称：MPU_Get_Accelerometer
函数功能：得到加速度值(原始值)
函数参数：ax,ay,az:加速度传感器x,y,z轴的原始读数(带符号)
函数返回值：0,读取成功  其他,读取失败
**********************************************/
uint8_t MPU_Get_Accelerometer(short *ax, short *ay, short *az)
{
    uint8_t buf[6], res;
    res = MPU_Read_Len(0xD0, MPU_ACCEL_XOUTH_REG, 6, buf);
    if (res == 0)
    {
        *ax = ((uint16_t)buf[0] << 8) | buf[1];
        *ay = ((uint16_t)buf[2] << 8) | buf[3];
        *az = ((uint16_t)buf[4] << 8) | buf[5];
    }
    return res;
}

/**********************************************
函数名称：get_tick_count
函数功能：
函数参数：
函数返回值：
**********************************************/
int get_tick_count(unsigned long *count)
{
    count[0] = HAL_GetTick();
    return 0;
}

/**********************************************
函数名称：read_from_mpl
函数功能：从MPL库获取数据，并将获取的数据添加到 inv_get_sensor_type_xxx APIs，进行新数据与旧数据的区分
函数参数：无
函数返回值：无
**********************************************/
static void read_from_mpl(void)
{
    long msg, data[9];
    int8_t accuracy;
    unsigned long timestamp;
    float float_data[3] = {0};

    if (inv_get_sensor_type_quat(data, &accuracy, (inv_time_t *)&timestamp))
    {
        /* 将四元数数据包发送到PC上位机，使用其来直观的表示3D四元数
           因此每次MPL有新数据时都会执行该函数 */
        eMPL_send_quat(data);

        /* 可以使用USB串口命令发送或禁止特定的数据包 */
        if (hal.report & PRINT_QUAT)
            eMPL_send_data(PACKET_DATA_QUAT, data);
    }

    if (hal.report & PRINT_ACCEL)
    {
        if (inv_get_sensor_type_accel(data, &accuracy,
                                      (inv_time_t *)&timestamp))
            eMPL_send_data(PACKET_DATA_ACCEL, data);
    }
    if (hal.report & PRINT_GYRO)
    {
        if (inv_get_sensor_type_gyro(data, &accuracy,
                                     (inv_time_t *)&timestamp))
            eMPL_send_data(PACKET_DATA_GYRO, data);
    }
#ifdef COMPASS_ENABLED
    if (hal.report & PRINT_COMPASS)
    {
        if (inv_get_sensor_type_compass(data, &accuracy,
                                        (inv_time_t *)&timestamp))
            eMPL_send_data(PACKET_DATA_COMPASS, data);
    }
#endif
    if (hal.report & PRINT_EULER)
    {
        if (inv_get_sensor_type_euler(data, &accuracy,
                                      (inv_time_t *)&timestamp))
            eMPL_send_data(PACKET_DATA_EULER, data);
    }
    if (hal.report & PRINT_ROT_MAT)
    {
        if (inv_get_sensor_type_rot_mat(data, &accuracy,
                                        (inv_time_t *)&timestamp))
            eMPL_send_data(PACKET_DATA_ROT, data);
    }
    if (hal.report & PRINT_HEADING)
    {
        if (inv_get_sensor_type_heading(data, &accuracy,
                                        (inv_time_t *)&timestamp))
            eMPL_send_data(PACKET_DATA_HEADING, data);
    }
    if (hal.report & PRINT_LINEAR_ACCEL)
    {
        if (inv_get_sensor_type_linear_acceleration(float_data, &accuracy, (inv_time_t *)&timestamp))
        {
            MPL_LOGI("Linear Accel: %7.5f %7.5f %7.5f\r\n",
                     float_data[0], float_data[1], float_data[2]);
        }
    }
    if (hal.report & PRINT_GRAVITY_VECTOR)
    {
        if (inv_get_sensor_type_gravity(float_data, &accuracy,
                                        (inv_time_t *)&timestamp))
            MPL_LOGI("Gravity Vector: %7.5f %7.5f %7.5f\r\n",
                     float_data[0], float_data[1], float_data[2]);
    }
    if (hal.report & PRINT_PEDO)
    {
        unsigned long timestamp;
        timestamp = HAL_GetTick();
        if (timestamp > hal.next_pedo_ms)
        {
            hal.next_pedo_ms = timestamp + PEDO_READ_MS;
            unsigned long step_count, walk_time;
            dmp_get_pedometer_step_count(&step_count);
            dmp_get_pedometer_walk_time(&walk_time);
            MPL_LOGI("Walked %ld steps over %ld milliseconds..\n", step_count,
                     walk_time);
        }
    }

    /* Whenever the MPL detects a change in motion state, the application can
     * be notified. For this example, we use an LED to represent the current
     * motion state.
     */
    msg = inv_get_message_level_0(INV_MSG_MOTION_EVENT |
                                  INV_MSG_NO_MOTION_EVENT);
    if (msg)
    {
        if (msg & INV_MSG_MOTION_EVENT)
        {
            MPL_LOGI("Motion!\n");
        }
        else if (msg & INV_MSG_NO_MOTION_EVENT)
        {
            MPL_LOGI("No motion!\n");
        }
    }
}

/**********************************************
函数名称：send_status_compass
函数功能：发送指南针状态
函数参数：无
函数返回值：无
**********************************************/
#ifdef COMPASS_ENABLED
void send_status_compass()
{
    long data[3] = {0};
    int8_t accuracy = {0};
    unsigned long timestamp;
    inv_get_compass_set(data, &accuracy, (inv_time_t *)&timestamp);
    MPL_LOGI("Compass: %7.4f %7.4f %7.4f ",
             data[0] / 65536.f, data[1] / 65536.f, data[2] / 65536.f);
    MPL_LOGI("Accuracy= %d\r\n", accuracy);
}
#endif

/**********************************************
函数名称：setup_gyro
函数功能：陀螺仪工作配置函数
函数参数：无
函数返回值：无
**********************************************/
static void setup_gyro(void)
{
    unsigned char mask = 0, lp_accel_was_on = 0;
    if (hal.sensors & ACCEL_ON)
        mask |= INV_XYZ_ACCEL;
    if (hal.sensors & GYRO_ON)
    {
        mask |= INV_XYZ_GYRO;
        lp_accel_was_on |= hal.lp_accel_mode;
    }
#ifdef COMPASS_ENABLED
    if (hal.sensors & COMPASS_ON)
    {
        mask |= INV_XYZ_COMPASS;
        lp_accel_was_on |= hal.lp_accel_mode;
    }
#endif
    /* 如果需要电源转换，则应在仍启用传感器掩码的情况下调用此功能
       驱动程序将关闭此掩码中未包含的所有传感器 */
    mpu_set_sensors(mask);
    mpu_configure_fifo(mask);
    if (lp_accel_was_on)
    {
        unsigned short rate;
        hal.lp_accel_mode = 0;
        /* 退出LP加速度，将新的加速度采样率通知MPL */
        mpu_get_sample_rate(&rate);
        inv_set_accel_sample_rate(1000000L / rate);
    }
}

/**********************************************
函数名称：tap_cb
函数功能：
函数参数：无
函数返回值：无
**********************************************/
static void tap_cb(unsigned char direction, unsigned char count)
{
    switch (direction)
    {
        case TAP_X_UP:
            MPL_LOGI("Tap X+ ");
            break;
        case TAP_X_DOWN:
            MPL_LOGI("Tap X- ");
            break;
        case TAP_Y_UP:
            MPL_LOGI("Tap Y+ ");
            break;
        case TAP_Y_DOWN:
            MPL_LOGI("Tap Y- ");
            break;
        case TAP_Z_UP:
            MPL_LOGI("Tap Z+ ");
            break;
        case TAP_Z_DOWN:
            MPL_LOGI("Tap Z- ");
            break;
        default:
            return;
    }
    MPL_LOGI("x%d\n", count);
    return;
}

/**********************************************
函数名称：android_orient_cb
函数功能：
函数参数：无
函数返回值：无
**********************************************/
static void android_orient_cb(unsigned char orientation)
{
    switch (orientation)
    {
        case ANDROID_ORIENT_PORTRAIT:
            MPL_LOGI("Portrait\n");
            break;
        case ANDROID_ORIENT_LANDSCAPE:
            MPL_LOGI("Landscape\n");
            break;
        case ANDROID_ORIENT_REVERSE_PORTRAIT:
            MPL_LOGI("Reverse Portrait\n");
            break;
        case ANDROID_ORIENT_REVERSE_LANDSCAPE:
            MPL_LOGI("Reverse Landscape\n");
            break;
        default:
            return;
    }
}

/**********************************************
函数名称：run_self_test
函数功能：自检函数，用于坐标原标定
函数参数：无
函数返回值：无
**********************************************/
static inline void run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result == 0x7)
    {
        MPL_LOGI("Passed!\n");
        MPL_LOGI("accel: %7.4f %7.4f %7.4f\n",
                 accel[0] / 65536.f,
                 accel[1] / 65536.f,
                 accel[2] / 65536.f);
        MPL_LOGI("gyro: %7.4f %7.4f %7.4f\n",
                 gyro[0] / 65536.f,
                 gyro[1] / 65536.f,
                 gyro[2] / 65536.f);
        /* 自检通过， 我们能够在这里获取真实的陀螺仪数据，因此我们现在需要更新校准数据 */
#ifdef USE_CAL_HW_REGISTERS
        /*
         * This portion of the code uses the HW offset registers that are in the MPUxxxx devices
         * instead of pushing the cal data to the MPL software library
         */
        unsigned char i = 0;

        for (i = 0; i < 3; i++)
        {
            gyro[i] = (long)(gyro[i] * 32.8f); // convert to +-1000dps
            accel[i] *= 2048.f;                // convert to +-16G
            accel[i] = accel[i] >> 16;
            gyro[i] = (long)(gyro[i] >> 16);
        }

        mpu_set_gyro_bias_reg(gyro);

        mpu_set_accel_bias_6050_reg(accel);

#else
        /* Push the calibrated data to the MPL library.
         *
         * MPL expects biases in hardware units << 16, but self test returns
         * biases in g's << 16.
         */
        unsigned short accel_sens;
        float gyro_sens;

        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        inv_set_accel_bias(accel, 3);
        mpu_get_gyro_sens(&gyro_sens);
        gyro[0] = (long)(gyro[0] * gyro_sens);
        gyro[1] = (long)(gyro[1] * gyro_sens);
        gyro[2] = (long)(gyro[2] * gyro_sens);
        inv_set_gyro_bias(gyro, 3);
#endif
    }
    else
    {
        if (!(result & 0x1))
            MPL_LOGE("Gyro failed.\n");
        if (!(result & 0x2))
            MPL_LOGE("Accel failed.\n");
        if (!(result & 0x4))
            MPL_LOGE("Compass failed.\n");
    }
}

/**********************************************
函数名称：handle_input
函数功能：
函数参数：无
函数返回值：无
**********************************************/
static void handle_input(void)
{

    char c;
    HAL_UART_Receive(&huart2, (uint8_t *)&c, 1, 1000);

    switch (c)
    {
        /* 这些命令用来关闭单个传感器 */
        case '8':
            hal.sensors ^= ACCEL_ON;
            setup_gyro();
            if (!(hal.sensors & ACCEL_ON))
                inv_accel_was_turned_off();
            break;
        case '9':
            hal.sensors ^= GYRO_ON;
            setup_gyro();
            if (!(hal.sensors & GYRO_ON))
                inv_gyro_was_turned_off();
            break;
#ifdef COMPASS_ENABLED
        case '0':
            hal.sensors ^= COMPASS_ON;
            setup_gyro();
            if (!(hal.sensors & COMPASS_ON))
                inv_compass_was_turned_off();
            break;
#endif
        /* 这些命令将单个传感器数据或融合数据发送到PC上位机 */
        case 'a':
            hal.report ^= PRINT_ACCEL;
            break;
        case 'g':
            hal.report ^= PRINT_GYRO;
            break;
#ifdef COMPASS_ENABLED
        case 'c':
            hal.report ^= PRINT_COMPASS;
            break;
#endif
        case 'e':
            hal.report ^= PRINT_EULER;
            break;
        case 'r':
            hal.report ^= PRINT_ROT_MAT;
            break;
        case 'q':
            hal.report ^= PRINT_QUAT;
            break;
        case 'h':
            hal.report ^= PRINT_HEADING;
            break;
        case 'i':
            hal.report ^= PRINT_LINEAR_ACCEL;
            break;
        case 'o':
            hal.report ^= PRINT_GRAVITY_VECTOR;
            break;
#ifdef COMPASS_ENABLED
        case 'w':
            send_status_compass();
            break;
#endif
        /* 此命令打印出每个陀螺仪寄存器的值以进行调试。如果禁用日志记录，则此功能无效 */
        case 'd':
            mpu_reg_dump();
            break;
        /* 测试低功耗加速模式 */
        case 'p':
            if (hal.dmp_on)
                /* LP加速模式与DMP不兼容 */
                break;
            mpu_lp_accel_mode(20);
            /* 启用LP加速度模式时，驱动程序将自动为锁存的中断配置硬件，
            但是，MCU有时会错过上升/下降沿，并且永远不会设置hal.new_gyro标志。
            为避免锁定在这种状态下，我们覆盖了驱动程序的配置并坚持使用未锁定的中断模式 */
            /* 要求：MCU支持电平触发的中断 */
            mpu_set_int_latched(0);
            hal.sensors &= ~(GYRO_ON | COMPASS_ON);
            hal.sensors |= ACCEL_ON;
            hal.lp_accel_mode = 1;
            inv_gyro_was_turned_off();
            inv_compass_was_turned_off();
            break;
        /* 硬件自检可以在不完全与MPL交互的情况下运行，应为它完全位于陀螺仪驱动程序中
           假定已启用日志记录，否则，可以在此处使用几个LED来显示测试结果 */
        case 't':
            run_self_test();
            /* 告诉MPL传感器已关闭 */
            inv_accel_was_turned_off();
            inv_gyro_was_turned_off();
            inv_compass_was_turned_off();
            break;
        /* 根据你的程序，可能需要更改或更慢速率的传感器数据，这些命令可以加快或减慢将传感器数据推送到MPL的速度， */
        /* 在本示例中，指南针速率始终不变 */
        case '1':
            if (hal.dmp_on)
            {
                dmp_set_fifo_rate(10);
                inv_set_quat_sample_rate(100000L);
            }
            else
                mpu_set_sample_rate(10);
            inv_set_gyro_sample_rate(100000L);
            inv_set_accel_sample_rate(100000L);
            break;
        case '2':
            if (hal.dmp_on)
            {
                dmp_set_fifo_rate(20);
                inv_set_quat_sample_rate(50000L);
            }
            else
                mpu_set_sample_rate(20);
            inv_set_gyro_sample_rate(50000L);
            inv_set_accel_sample_rate(50000L);
            break;
        case '3':
            if (hal.dmp_on)
            {
                dmp_set_fifo_rate(40);
                inv_set_quat_sample_rate(25000L);
            }
            else
                mpu_set_sample_rate(40);
            inv_set_gyro_sample_rate(25000L);
            inv_set_accel_sample_rate(25000L);
            break;
        case '4':
            if (hal.dmp_on)
            {
                dmp_set_fifo_rate(50);
                inv_set_quat_sample_rate(20000L);
            }
            else
                mpu_set_sample_rate(50);
            inv_set_gyro_sample_rate(20000L);
            inv_set_accel_sample_rate(20000L);
            break;
        case '5':
            if (hal.dmp_on)
            {
                dmp_set_fifo_rate(100);
                inv_set_quat_sample_rate(10000L);
            }
            else
                mpu_set_sample_rate(100);
            inv_set_gyro_sample_rate(10000L);
            inv_set_accel_sample_rate(10000L);
            break;
        case ',':
            /* 将硬件设置为仅在手势事件时中断，此功能对于保持MCU处于休眠状态，直到DMP检测到发生轻敲或定向事件为止 */
            dmp_set_interrupt_mode(DMP_INT_GESTURE);
            break;
        case '.':
            /* 将硬件设置为周期性中断 */
            dmp_set_interrupt_mode(DMP_INT_CONTINUOUS);
            break;
        case '6':
            /* 切换计步器先显示 */
            hal.report ^= PRINT_PEDO;
            break;
        case '7':
            /* 重置计步器 */
            dmp_set_pedometer_step_count(0);
            dmp_set_pedometer_walk_time(0);
            break;
        case 'f':
            if (hal.lp_accel_mode)
                /* LP加速模式与DMP不兼容 */
                return;
            /* 切换DMP */
            if (hal.dmp_on)
            {
                unsigned short dmp_rate;
                unsigned char mask = 0;
                hal.dmp_on = 0;
                mpu_set_dmp_state(0);
                /* 恢复FIFO设置 */
                if (hal.sensors & ACCEL_ON)
                    mask |= INV_XYZ_ACCEL;
                if (hal.sensors & GYRO_ON)
                    mask |= INV_XYZ_GYRO;
                if (hal.sensors & COMPASS_ON)
                    mask |= INV_XYZ_COMPASS;
                mpu_configure_fifo(mask);
                /* 使用DMP时，硬件采样率固定位200Hz，并且DMP配置为使用函数dmp_set_fifo_rate对FIFO输出进行降低采样率
                   但是，当DMP关闭时，采样率保持在200Hz，可以在inv_mpu.c文件中进行处理，但需要引用inv_mpu_dmp_motion_driver.c
                   为了避免这种情况，我们将额外的逻辑代码放在应用程序层中 */
                dmp_get_fifo_rate(&dmp_rate);
                mpu_set_sample_rate(dmp_rate);
                inv_quaternion_sensor_was_turned_off();
                MPL_LOGI("DMP disabled.\n");
            }
            else
            {
                unsigned short sample_rate;
                hal.dmp_on = 1;
                /* 保持当前FIFO速率 */
                mpu_get_sample_rate(&sample_rate);
                dmp_set_fifo_rate(sample_rate);
                inv_set_quat_sample_rate(1000000L / sample_rate);
                mpu_set_dmp_state(1);
                MPL_LOGI("DMP enabled.\n");
            }
            break;
        case 'm':
            /* 测试运动中断硬件功能 */
#ifndef MPU6050 // 不适用MPU6050产品
            hal.motion_int_mode = 1;
#endif
            break;

        case 'v':
            /* 切换LP四元数，可以在运行时启用/禁用DMP功能，对其它功能可以使用相同的方法 */
            hal.dmp_features ^= DMP_FEATURE_6X_LP_QUAT;
            dmp_enable_feature(hal.dmp_features);
            if (!(hal.dmp_features & DMP_FEATURE_6X_LP_QUAT))
            {
                inv_quaternion_sensor_was_turned_off();
                MPL_LOGI("LP quaternion disabled.\n");
            }
            else
                MPL_LOGI("LP quaternion enabled.\n");
            break;
        default:
            break;
    }
    hal.rx.cmd = 0;
}

/**********************************************
函数名称：gyro_data_ready_cb
函数功能：设置标志变量hal.new_gyro，以通知MPL库有了新的数据
        每当有新的数据产生时，本函数会被外部中断服务函数调用
        在本工程中，此函数设置的标志位用于指示及保护FIFO缓冲区
函数参数：无
函数返回值：无
**********************************************/
void gyro_data_ready_cb(void)
{
    hal.new_gyro = 1;
}

/**********************************************
函数名称：DMP_Init
函数功能：初始化DMP
函数参数：无
函数返回值：无
**********************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_1)
    {
        gyro_data_ready_cb();
    }
}

/**********************************************
函数名称：DMP_Init
函数功能：初始化DMP
函数参数：无
函数返回值：无
**********************************************/
void DMP_Init(void)
{
    inv_error_t result;
    unsigned char accel_fsr, new_temp = 0;
    unsigned short gyro_rate, gyro_fsr;
    unsigned long timestamp;
    struct int_param_s int_param;

#ifdef COMPASS_ENABLED
    unsigned char new_compass = 0;
    unsigned short compass_fsr;
#endif

    result = mpu_init(&int_param); // 1.6050初始化,成功=0，失败=1
    if (result)
    {
        MPL_LOGE("Could not initialize gyro.\n");
        HAL_Delay(5000);
    }

    /* 如果你未使用MPU9150，也没有使用DMP功能，这个函数会将所有的从机置于总线上 */
    /*  mpu_set_bypass(1); */

    result = inv_init_mpl();
    if (result)
    {
        MPL_LOGE("Could not initialize MPL.\n");
        HAL_Delay(5000);
    }

    /* 计算6轴和9轴传感器的四元数 */
    inv_enable_quaternion();
    inv_enable_9x_sensor_fusion();

    /* MPL需要指南针数据处于恒定速率（匹配速率传递给inv_set_compass_sample_rate）
       如果你的程序有需要的话，调用此函数，MPL将依据时间戳传递给inv_build_compass执行 */
    // inv_9x_fusion_use_timestamps(1);

    /* 无运动状态时更新陀螺仪 */
    /* 注：下面这三个函数的功能是各不相同的 */
    inv_enable_fast_nomot();
    /* inv_enable_motion_no_motion(); */
    /* inv_set_no_motion_time(1000); */

    /* 当温度变化时更新陀螺仪数据 */
    inv_enable_gyro_tc();

    /* 此函数在运动时更新加速度数据，当进行自检时可以获得更准确的测量数据（参见handle_input中的't'）
    但是如果自检失败，可以启用这个函数 */
    // inv_enable_in_use_auto_calibration();

#ifdef COMPASS_ENABLED
    /* 指南针校准 */
    inv_enable_vector_compass_cal();
    inv_enable_magnetic_disturbance();
#endif
    /* 如果你需要在校准指南针之前估算航向，那么启用此函数
     * 当航向估算完毕，此函数就没有用了，因此我们注释它，以节省内存 */
    // inv_enable_heading_from_gyro();

    /* 允许 read_from_mpl 使用 MPL APIs */
    inv_enable_eMPL_outputs();

    result = inv_start_mpl();
    if (result == INV_ERROR_NOT_AUTHORIZED)
    {
        while (1)
        {
            MPL_LOGE("Not authorized.\n");
            HAL_Delay(1000);
        }
    }
    if (result)
    {
        MPL_LOGE("Could not start the MPL.\n");
        HAL_Delay(1000);
    }
    /* 设置寄存器，开启陀螺仪 */
    /* 唤醒所有传感器*/
#ifdef COMPASS_ENABLED
    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
#else
    result = mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    if (result)
    {
        MPL_LOGE("Could not set sensors.\n");
        HAL_Delay(1000);
    }
#endif
    /* 将陀螺仪和加速度数据都送入FIFO */
    result = mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL); // 配置陀螺仪和加速计开启FIFO通道函数
    if (result)
    {
        MPL_LOGE("Could not configure fifo.\n");
        HAL_Delay(1000);
    }
    result = mpu_set_sample_rate(DEFAULT_MPU_HZ); // 配置默认的采样率
    if (result)
    {
        MPL_LOGE("Could not set sample rate.\n");
        HAL_Delay(1000);
    }
#ifdef COMPASS_ENABLED
    /* 罗盘采样率可以小于陀螺仪/加速度采样率，使用此函数进行合适的电源管理 */
    mpu_set_compass_sample_rate(1000 / COMPASS_READ_MS);
#endif
    /* 重新读取配置，确认前面的设置成功 */
    mpu_get_sample_rate(&gyro_rate);
    mpu_get_gyro_fsr(&gyro_fsr);
    mpu_get_accel_fsr(&accel_fsr);
#ifdef COMPASS_ENABLED
    mpu_get_compass_fsr(&compass_fsr);
#endif
    /* 与MPL同步程序驱动程序配置 */
    /* 设置每毫秒的采样率*/
    inv_set_gyro_sample_rate(1000000L / gyro_rate);
    inv_set_accel_sample_rate(1000000L / gyro_rate);
#ifdef COMPASS_ENABLED
    /* 指南针速率独立于陀螺仪和加速度计速率，
       只要使用合适的值，调用inv_set_compass_sample_rate，9轴融合算法的罗盘校正增益就可以正常工作了 */
    inv_set_compass_sample_rate(COMPASS_READ_MS * 1000L);
#endif
    /* 设置芯片到主体的方向矩阵 */
    /* 将硬件单位设置为 dps/g's/度 的比例因子 */
    inv_set_gyro_orientation_and_scale(
        inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
        (long)gyro_fsr << 15);
    inv_set_accel_orientation_and_scale(
        inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
        (long)accel_fsr << 15);
#ifdef COMPASS_ENABLED
    inv_set_compass_orientation_and_scale(
        inv_orientation_matrix_to_scalar(compass_pdata.orientation),
        (long)compass_fsr << 15);
#endif
/* 初始化硬件状态相关变量 */
#ifdef COMPASS_ENABLED
    hal.sensors = ACCEL_ON | GYRO_ON | COMPASS_ON;
#else
    hal.sensors = ACCEL_ON | GYRO_ON;
#endif
    hal.dmp_on = 0;
    hal.report = 0;
    hal.rx.cmd = 0;
    hal.next_pedo_ms = 0;
    hal.next_compass_ms = 0;
    hal.next_temp_ms = 0;

    /* 指南针读取获取时间戳 */
    get_tick_count(&timestamp);

    /* 初始化DMP步骤：
     * 1. 调用dmp_load_motion_driver_firmware()
     *    它会把inv_mpu_dmp_motion_driver.h文件中的DMP固件写入到MPU的存储空间
     * 2. 把陀螺仪和加速度计的原始数据矩阵送入DMP
     * 3. 注册姿态回调函数，除非相应的特性使能了，否则该回调函数不会被执行
     * 4. 调用 dmp_enable_feature(mask) 使能不同的特性
     * 5. 调用 dmp_set_fifo_rate(freq) 设置DMP输出频率
     * 6. 调用特定的特性控制相关的函数
     *
     * 调用 mpu_set_dmp_state(1)使能DMP，该函数可在DMP运行时被重复调用设置使能或关闭
     *
     * 以下是inv_mpu_dmp_motion_driver.c文件中DMP固件提供的特性的简介：
     *
     * DMP_FEATURE_LP_QUAT: 使用DMP以200Hz的频率产生一个只包含陀螺仪的四元数数据
     * 以高速的状态解算陀螺仪数据，减少错误（相对于使用MCu以一个低采样率的方式采样）
     * DMP_FEATURE_6X_LP_QUAT: 使用DMP以200Hz的频率产生 陀螺仪/加速度计 四元数，它不能与前面的DMP_FEATURE_LP_QUAT同时使用
     * DMP_FEATURE_TAP: 检测 X，Y，和 Z 轴
     * DMP_FEATURE_ANDROID_ORIENT: 谷歌屏幕翻转算法，挡屏幕翻转时，在四个方向产生一个事件
     * DMP_FEATURE_GYRO_CAL: 若8s内都没有运动，计算陀螺仪数据
     * DMP_FEATURE_SEND_RAW_ACCEL: 添加原始加速度计数据到FIFO
     * DMP_FEATURE_SEND_RAW_GYRO: 添加原始陀螺仪数据到FIFO
     * DMP_FEATURE_SEND_CAL_GYRO: 添加校准后的陀螺仪数据到FIFO，不能与DMP_FEATURE_SEND_RAW_GYRO同时使用
     */
    result = dmp_load_motion_driver_firmware();
    if (result)
    {
        MPL_LOGE("Could not load motion driver firmware.\n");
        HAL_Delay(1000);
    }
    result = dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_pdata.orientation));
    if (result)
    {
        MPL_LOGE("Could not set orientation.\n");
        HAL_Delay(1000);
    }
    dmp_register_tap_cb(tap_cb);
    dmp_register_android_orient_cb(android_orient_cb);
    /*
     * 已知错误 -
     * DMP启用后将以200Hz采样传感器数据，并以dmp_set_fifo_rate API中指定的速率传输到FIFO
     * 一旦将采样数据放入FIFO，DMP将发送一个中断，因此，如果dmp_set_fifo_rate为25Hz，则是来自MPU设备的25Hz中断。
     *
     * 一个已知的问题：如果不启用DMP_FEATURE_TAP，则即使将FIFO速率设置为其他速率，中断也会以200Hz的频率运行
     * 为避免此问题，请包括DMP_FEATURE_TAP
     * DMP传感器融合仅适用于gyro at +-2000dps and accel +-2G
     */

    hal.dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
                       DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
                       DMP_FEATURE_GYRO_CAL;
    result = dmp_enable_feature(hal.dmp_features);
    if (result)
    {
        MPL_LOGE("Could not enable feature.\n");
        HAL_Delay(1000);
    }

    result = dmp_set_fifo_rate(DEFAULT_MPU_HZ); //设置DMP输出速率(最大不超过200Hz)
    if (result)
    {
        MPL_LOGE("Could not set fifo rate.\n");
        HAL_Delay(1000);
    }

    result = mpu_set_dmp_state(1); //使能DMP
    if (result)
    {
        MPL_LOGE("Could not set dmp state.\n");
        HAL_Delay(1000);
    }
    hal.dmp_on = 1;

    run_self_test();
}

/**********************************************
函数名称：MPU6050_Get_Euler_Temputer
函数功能：
函数参数：
函数返回值：
**********************************************/
void MPU6050_Get_Euler_Temputer(float *Pitch, float *Roll, float *Yaw, long *temperature)
{
    unsigned long sensor_timestamp;
    int new_data = 0;
    unsigned long timestamp;
    char new_temp = 0;

    get_tick_count(&timestamp);

#ifdef COMPASS_ENABLED
    /* 我们没有为指南针使用数据就绪中断，因此我们将指南针改为基于计时器读取*/
    if ((timestamp > hal.next_compass_ms) && !hal.lp_accel_mode &&
        hal.new_gyro && (hal.sensors & COMPASS_ON))
    {
        hal.next_compass_ms = timestamp + COMPASS_READ_MS;
        new_compass = 1;
    }
#endif
    /* 温度数据不需要像陀螺仪数据那样每次都采样，这里设置隔一段时间采样 */
    if (timestamp > hal.next_temp_ms)
    {
        hal.next_temp_ms = timestamp + TEMP_READ_MS;
        new_temp = 1;
    }

    if (hal.motion_int_mode)
    {
        /* Enable motion interrupt. */
        mpu_lp_motion_interrupt(500, 1, 5);
        /* Notify the MPL that contiguity was broken. */
        inv_accel_was_turned_off();
        inv_gyro_was_turned_off();
        inv_compass_was_turned_off();
        inv_quaternion_sensor_was_turned_off();
        /* Wait for the MPU interrupt. */
        while (!hal.new_gyro) {}
        /* Restore the previous sensor configuration. */
        mpu_lp_motion_interrupt(0, 0, 0);
        hal.motion_int_mode = 0;
    }

    if (hal.new_gyro && hal.dmp_on)
    {
        short gyro[3], accel_short[3], sensors;
        unsigned char more;
        long accel[3], quat[4], temperature;
        /* 当使用DMP时，本函数从FIFO读取新数据，FIFO中存储了陀螺仪、加速度、四元数及手势数据，
         * 传感器参数可告知调用者哪个有了新数据
         * 例如：if sensors == (INV_XYZ_GYRO | INV_WXYZ_QUAT)，那么FIFO中就不包含加速度数据
         * 手势数据的解算由是否产生手势运动事件来触发，若产生了事件，应用函数会使用回调函数来通知
         * 如果FIFO中有剩余数据包，则more参数为非零
         */
        dmp_read_fifo(gyro, accel_short, quat, &sensor_timestamp, &sensors, &more);
        if (!more)
            hal.new_gyro = 0;
        if (sensors & INV_XYZ_GYRO)
        {
            /* Push the new data to the MPL. */
            inv_build_gyro(gyro, sensor_timestamp);
            new_data = 1;
            if (new_temp)
            {
                new_temp = 0;
                /* Temperature only used for gyro temp comp. */
                mpu_get_temperature(&temperature, &sensor_timestamp);
                inv_build_temp(temperature, sensor_timestamp);
            }
        }
        if (sensors & INV_XYZ_ACCEL)
        {
            accel[0] = (long)accel_short[0];
            accel[1] = (long)accel_short[1];
            accel[2] = (long)accel_short[2];
            inv_build_accel(accel, 0, sensor_timestamp);
            new_data = 1;
        }
        if (sensors & INV_WXYZ_QUAT)
        {
            inv_build_quat(quat, 0, sensor_timestamp);
            new_data = 1;
        }
    }

    if (new_data)
    {
        inv_execute_on_data();
        /* 本函数读取补偿后的传感器数据和经过MPL传感器融合后输出的数据
         * 输出的格式见 eMPL_outputs.c 文件，这个函数在主机需要数据的时候调用即可，对频率无要求
         */
        // read_from_mpl();
        long data[9];
        int8_t accuracy;
        unsigned long timestamp;
        if (inv_get_sensor_type_euler(data, &accuracy, (inv_time_t *)&timestamp))
        {
            *Pitch = data[0] * 1.0 / (1 << 16);
            *Roll = data[1] * 1.0 / (1 << 16);
            *Yaw = data[2] * 1.0 / (1 << 16);
        }
    }
}