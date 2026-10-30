#include "mpu6050.h"


#define REG_ACCEL_CONFIG   0x1C
#define REG_ACCEL_XOUT_H   0x3B
#define REG_TEMP_OUT_H     0x41
#define REG_PWR_MGMT_1     0x6B
#define REG_WHO_AM_I       0x75

#define WHO_AM_I_EXPECTED  0x68


#define PWR_MGMT_1_SLEEP   (1u << 6)
#define ACCEL_FS_SEL_MASK  (3u << 3)


#define ACCEL_LSB_PER_G    16384.0f


static int16_t to_i16(uint8_t hi, uint8_t lo)
{
    return (int16_t)(((uint16_t)hi << 8) | lo);
}


static int update_bits(mpu6050_t *dev, uint8_t reg, uint8_t mask, uint8_t value)
{
    uint8_t v;

    if (dev->read(dev->addr, reg, &v, 1) != 0) {
        return MPU6050_ERR_BUS;
    }
    v = (uint8_t)((v & ~mask) | (value & mask));
    if (dev->write(dev->addr, reg, &v, 1) != 0) {
        return MPU6050_ERR_BUS;
    }
    return MPU6050_OK;
}

int mpu6050_init(mpu6050_t *dev, uint8_t addr,
                 mpu6050_read_fn read, mpu6050_write_fn write)
{
    uint8_t id;
    int err;

    if (dev == 0 || read == 0 || write == 0) {
        return MPU6050_ERR_PARAM;
    }
    dev->addr  = addr;
    dev->read  = read;
    dev->write = write;


    if (dev->read(dev->addr, REG_WHO_AM_I, &id, 1) != 0) {
        return MPU6050_ERR_BUS;
    }
    if (id != WHO_AM_I_EXPECTED) {
        return MPU6050_ERR_ID;
    }


    err = update_bits(dev, REG_PWR_MGMT_1, PWR_MGMT_1_SLEEP, 0);
    if (err != MPU6050_OK) {
        return err;
    }


    return update_bits(dev, REG_ACCEL_CONFIG, ACCEL_FS_SEL_MASK, 0);
}

int mpu6050_read_accel(mpu6050_t *dev, mpu6050_vec3_t *accel_g)
{
    uint8_t raw[6];

    if (dev == 0 || accel_g == 0) {
        return MPU6050_ERR_PARAM;
    }

    if (dev->read(dev->addr, REG_ACCEL_XOUT_H, raw, sizeof raw) != 0) {
        return MPU6050_ERR_BUS;
    }
    accel_g->x = to_i16(raw[0], raw[1]) / ACCEL_LSB_PER_G;
    accel_g->y = to_i16(raw[2], raw[3]) / ACCEL_LSB_PER_G;
    accel_g->z = to_i16(raw[4], raw[5]) / ACCEL_LSB_PER_G;
    return MPU6050_OK;
}

int mpu6050_read_temp(mpu6050_t *dev, float *temp_c)
{
    uint8_t raw[2];

    if (dev == 0 || temp_c == 0) {
        return MPU6050_ERR_PARAM;
    }
    if (dev->read(dev->addr, REG_TEMP_OUT_H, raw, sizeof raw) != 0) {
        return MPU6050_ERR_BUS;
    }

    *temp_c = to_i16(raw[0], raw[1]) / 340.0f + 36.53f;
    return MPU6050_OK;
}
