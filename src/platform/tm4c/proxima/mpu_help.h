#ifndef __MPU_HELP_H__
#define __MPU_HELP_H__

#define MPU_ADDR            0x68

#define MPU_PWR_MGMT_1      0x6B
#define MPU_USER_CTRL       0x6A
#define MPU_ACCEL_CONFIG    0x1C
#define MPU_GYRO_CONFIG     0x1B
#define MPU_CONFIG          0x1A
#define MPU_SMPLRT_DIV      0x19

#define MPU_WHO_AM_I        0x75
#define MPU_ACCEL_X         0x3B
#define MPU_ACCEL_Y         0x3D
#define MPU_ACCEL_Z         0x3F
#define MPU_TEMP            0x41

void proxima_help_reg_write_byte(u8 addr, u8 reg, u8 data);
int proxima_help_reg_read_byte(u8 addr, u8 reg, u8* data);
int proxima_help_reg_read_word(u8 addr, u8 reg, s16* data);

#endif
