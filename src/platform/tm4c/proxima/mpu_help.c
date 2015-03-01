
#include "platform.h"
#include "mpu_help.h"

void proxima_help_reg_write_byte(u8 addr, u8 reg, u8 data)
{
    platform_i2c_send_address(1, addr, PLATFORM_I2C_DIRECTION_TRANSMITTER);
    platform_i2c_send_start(1);
    platform_i2c_send_byte(1, reg);
    platform_i2c_send_byte(1, data);
    platform_i2c_send_stop(1);
}

int proxima_help_reg_read_byte(u8 addr, u8 reg, u8* data)
{
    platform_i2c_send_address(1, addr, PLATFORM_I2C_DIRECTION_TRANSMITTER);
    platform_i2c_send_start(1);
    platform_i2c_send_byte(1, reg);
    platform_i2c_send_address(1, addr, PLATFORM_I2C_DIRECTION_RECEIVER);
    *data = platform_i2c_recv_byte(1, 1);
    platform_i2c_send_stop(1);

    return 0;
}

int proxima_help_reg_read_word(u8 addr, u8 reg, s16* data)
{
    *data = 0;

    platform_i2c_send_address(1, addr, PLATFORM_I2C_DIRECTION_TRANSMITTER);
    platform_i2c_send_start(1);
    platform_i2c_send_byte(1, reg);
    platform_i2c_send_address(1, addr, PLATFORM_I2C_DIRECTION_RECEIVER);

    *data |= platform_i2c_recv_byte(1, 0) << 8;
    platform_i2c_send_stop(1);


    platform_i2c_send_address(1, addr, PLATFORM_I2C_DIRECTION_TRANSMITTER);
    platform_i2c_send_start(1);
    platform_i2c_send_byte(1, reg + 1);
    platform_i2c_send_address(1, addr, PLATFORM_I2C_DIRECTION_RECEIVER);

    *data |= platform_i2c_recv_byte(1, 1);
    platform_i2c_send_stop(1);

    return 0;
}

