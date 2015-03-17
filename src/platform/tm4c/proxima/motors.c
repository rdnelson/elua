#include "motors.h"

#include "platform.h"

#define MC1_ADDR 0x60
#define MC2_ADDR 0x61

#define FWD 0x11
#define REV 0x22
#define RIGHT 0x12
#define LEFT 0x21
#define STOP 0x00

#define NOT_INIT    0
#define INIT        1

static unsigned char accel_state = NOT_INIT;

int motor_impl(int dir)
{
    u8 addr = MC1_ADDR;
    u8 i;

    if ( accel_state == NOT_INIT ) {
        platform_i2c_setup( 1, PLATFORM_I2C_SPEED_FAST );

        accel_state = INIT;
    }

    // This must only run twice
    for(i = 0; i < 1; i++) {
        u8 cmd;

        switch(dir & 0xF) {
            case 0x01:
                cmd = 0xaa;
                break;
            case 0x02:
                cmd = 0xa9;
                break;
            default:
                cmd = 0xab;
                break;
        }

        platform_i2c_send_address(1, addr, PLATFORM_I2C_DIRECTION_TRANSMITTER);
        platform_i2c_send_start(1);
        platform_i2c_send_byte(1, cmd);
        platform_i2c_send_stop(1);

        dir >>= 4;
        addr = MC2_ADDR;
    }

    return 0;
}

int proxima_motors_forward( lua_State* L )
{
    return motor_impl(FWD);
}

int proxima_motors_reverse( lua_State* L )
{
    return motor_impl(REV);
}

int proxima_motors_right( lua_State* L )
{
    return motor_impl(RIGHT);
}

int proxima_motors_left( lua_State* L )
{
    return motor_impl(LEFT);
}

int proxima_motors_stop( lua_State* L )
{
    return motor_impl(STOP);
}

