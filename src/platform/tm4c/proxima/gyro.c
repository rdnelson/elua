#include "gyro.h"

#include "platform.h"

#include "mpu_help.h"

#define NOT_INIT    0
#define INIT        1

#define MPU_GYRO_X         0x43
#define MPU_GYRO_Y         0x45
#define MPU_GYRO_Z         0x47

unsigned char gyro_state = NOT_INIT;

int proxima_gyro_init( lua_State* L )
{
    platform_i2c_setup( 1, PLATFORM_I2C_SPEED_FAST );
    gyro_state = INIT;

    proxima_help_reg_write_byte(MPU_ADDR, MPU_PWR_MGMT_1, 0x01);
    proxima_help_reg_write_byte(MPU_ADDR, MPU_USER_CTRL, 0x01);
    proxima_help_reg_write_byte(MPU_ADDR, MPU_GYRO_CONFIG, 0x00);
    proxima_help_reg_write_byte(MPU_ADDR, MPU_CONFIG, 0x01);
    proxima_help_reg_write_byte(MPU_ADDR, MPU_SMPLRT_DIV, 0x09);

    return 0;
}


int proxima_gyro_get_x( lua_State* L )
{
    short data;

    if ( gyro_state != INIT )
        proxima_gyro_init( L );

    proxima_help_reg_read_word(MPU_ADDR, MPU_GYRO_X, &data);
    lua_pushinteger( L, data );

    return 1;
}

int proxima_gyro_get_y( lua_State* L )
{
    short data;

    if ( gyro_state != INIT )
        proxima_gyro_init( L );

    proxima_help_reg_read_word(MPU_ADDR, MPU_GYRO_Y, &data);
    lua_pushinteger( L, data );

    return 1;
}

int proxima_gyro_get_z( lua_State* L )
{
    short data;

    if ( gyro_state != INIT )
        proxima_gyro_init( L );

    proxima_help_reg_read_word(MPU_ADDR, MPU_GYRO_Z, &data);
    lua_pushinteger( L, data );

    return 1;
}

