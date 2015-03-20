#include "motors.h"

#include "lauxlib.h"
#include "platform.h"

#define MC1_ADDR 0x60
#define MC2_ADDR 0x61

#define FWD 0x02
#define REV 0x01
#define BRAKE 0x03
#define COAST 0x00

#define DEADZONE 10

#define NOT_INIT    0
#define INIT        1

#define abs(a) ((a) < 0 ? -(a) : (a))

static unsigned char accel_state = NOT_INIT;

int motor_impl(int left, int right)
{
    u8 addr = MC1_ADDR;
    u8 i;
    s16 power = left;

    if ( accel_state == NOT_INIT ) {
        platform_i2c_setup( 1, PLATFORM_I2C_SPEED_FAST );

        accel_state = INIT;
    }

    // This must only run twice
    for(i = 0; i < 2; i++) {
        u8 cmd;
        power = power - 127;

        if (power > DEADZONE) {
            cmd = FWD;
        } else if (power < -DEADZONE) {
            cmd = REV;
        } else {
            cmd = BRAKE;
        }

        power = abs(power) / 3;

        platform_i2c_send_address(1, addr, PLATFORM_I2C_DIRECTION_TRANSMITTER);
        platform_i2c_send_start(1);
        platform_i2c_send_byte(1, 0x00);
        platform_i2c_send_byte(1, (power << 2) | cmd);
        platform_i2c_send_stop(1);

        addr = MC2_ADDR;
        power = (-right) + 256;
    }

    return 0;
}

int proxima_motors_set( lua_State* L )
{
    u8 left, right;

    left = luaL_checkinteger( L, 1 );
    right = luaL_checkinteger( L, 2 );

    return motor_impl(left, right);
}

int proxima_motors_forward( lua_State* L )
{
    return motor_impl(256, 256);
}

int proxima_motors_reverse( lua_State* L )
{
    return motor_impl(0, 0);
}

int proxima_motors_right( lua_State* L )
{
    return motor_impl(256, 0);
}

int proxima_motors_left( lua_State* L )
{
    return motor_impl(0, 256);
}

int proxima_motors_stop( lua_State* L )
{
    return motor_impl(127, 127);
}

