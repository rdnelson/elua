#define NOT_INIT    0
#define INIT        1

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


static unsigned char accel_state = NOT_INIT;

static int proxima_accel_init( lua_State* L )
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

static int proxima_accel_get_id( lua_State* L )
{
    unsigned char data;

    if ( accel_state != INIT )
        proxima_accel_init( L );

    proxima_help_reg_read_byte(MPU_ADDR, MPU_WHO_AM_I, &data);
    lua_pushinteger( L, data );

    return 1;
}

static int proxima_accel_get_x( lua_State* L )
{
    short data;

    if ( accel_state != INIT )
        proxima_accel_init( L );

    proxima_help_reg_read_word(MPU_ADDR, MPU_ACCEL_X, &data);
    lua_pushinteger( L, data );

    return 1;
}

static int proxima_accel_get_y( lua_State* L )
{
    short data;

    if ( accel_state != INIT )
        proxima_accel_init( L );

    proxima_help_reg_read_word(MPU_ADDR, MPU_ACCEL_Y, &data);
    lua_pushinteger( L, data );

    return 1;
}

static int proxima_accel_get_z( lua_State* L )
{
    short data;

    if ( accel_state != INIT )
        proxima_accel_init( L );

    proxima_help_reg_read_word(MPU_ADDR, MPU_ACCEL_Z, &data);
    lua_pushinteger( L, data );

    return 1;
}

static int proxima_accel_get_temp( lua_State* L )
{
    short data;

    if ( accel_state != INIT )
        proxima_accel_init( L );

    proxima_help_reg_read_word(MPU_ADDR, MPU_TEMP, &data);
    lua_pushinteger( L, data );

    return 1;
}

#undef INIT
#undef NOT_INIT
