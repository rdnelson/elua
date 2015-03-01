#include "accel.h"

#include "platform.h"

#include "mpu_help.h"


#define NOT_INIT    0
#define INIT        1


static unsigned char accel_state = NOT_INIT;

int proxima_accel_init( lua_State* L )
{
    platform_i2c_setup( 1, PLATFORM_I2C_SPEED_FAST );

    proxima_help_reg_write_byte(MPU_ADDR, MPU_PWR_MGMT_1, 0x01);
    proxima_help_reg_write_byte(MPU_ADDR, MPU_USER_CTRL, 0x01);
    proxima_help_reg_write_byte(MPU_ADDR, MPU_ACCEL_CONFIG, 0x00);
    proxima_help_reg_write_byte(MPU_ADDR, MPU_CONFIG, 0x01);
    proxima_help_reg_write_byte(MPU_ADDR, MPU_SMPLRT_DIV, 0x09);

    accel_state = INIT;

    return 0;
}

int proxima_accel_get_id( lua_State* L )
{
    unsigned char data;

    if ( accel_state != INIT )
        proxima_accel_init( L );

    proxima_help_reg_read_byte(MPU_ADDR, MPU_WHO_AM_I, &data);
    lua_pushinteger( L, data );

    return 1;
}

int proxima_accel_get_x( lua_State* L )
{
    short data;

    if ( accel_state != INIT )
        proxima_accel_init( L );

    proxima_help_reg_read_word(MPU_ADDR, MPU_ACCEL_X, &data);
    lua_pushinteger( L, data );

    return 1;
}

int proxima_accel_get_y( lua_State* L )
{
    short data;

    if ( accel_state != INIT )
        proxima_accel_init( L );

    proxima_help_reg_read_word(MPU_ADDR, MPU_ACCEL_Y, &data);
    lua_pushinteger( L, data );

    return 1;
}

int proxima_accel_get_z( lua_State* L )
{
    short data;

    if ( accel_state != INIT )
        proxima_accel_init( L );

    proxima_help_reg_read_word(MPU_ADDR, MPU_ACCEL_Z, &data);
    lua_pushinteger( L, data );

    return 1;
}

int proxima_accel_get_temp( lua_State* L )
{
    short data;

    if ( accel_state != INIT )
        proxima_accel_init( L );

    proxima_help_reg_read_word(MPU_ADDR, MPU_TEMP, &data);
    lua_pushinteger( L, data );

    return 1;
}

