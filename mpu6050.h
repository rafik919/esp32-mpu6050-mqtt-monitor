#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MPU6050_ADDR_LOW   0x68
#define MPU6050_ADDR_HIGH  0x69


#define MPU6050_OK          0
#define MPU6050_ERR_PARAM  -1
#define MPU6050_ERR_BUS    -2
#define MPU6050_ERR_ID     -3


typedef int (*mpu6050_read_fn)(uint8_t dev_addr, uint8_t reg,
                               uint8_t *buf, uint8_t len);
typedef int (*mpu6050_write_fn)(uint8_t dev_addr, uint8_t reg,
                                const uint8_t *buf, uint8_t len);

typedef struct {
    uint8_t          addr;
    mpu6050_read_fn  read;
    mpu6050_write_fn write;
} mpu6050_t;

typedef struct {
    float x;
    float y;
    float z;
} mpu6050_vec3_t;


int mpu6050_init(mpu6050_t *dev, uint8_t addr,
                 mpu6050_read_fn read, mpu6050_write_fn write);


int mpu6050_read_accel(mpu6050_t *dev, mpu6050_vec3_t *accel_g);


int mpu6050_read_temp(mpu6050_t *dev, float *temp_c);

#ifdef __cplusplus
}
#endif

#endif
