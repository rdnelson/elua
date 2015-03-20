#include "clock.h"

#include "lauxlib.h"
#include "platform.h"

#define NOT_INIT    0
#define INIT        1
#define RTC_TIME_ADDR    0x33

static unsigned char rtc_state = NOT_INIT;

u8 conv_val(u8 enc)
{
    u8 retval = 0;

    retval += enc & 0x80 ? 1 : 0;
    retval += enc & 0x40 ? 2 : 0;
    retval += enc & 0x20 ? 4 : 0;
    retval += enc & 0x10 ? 8 : 0;
    retval += enc & 0x08 ? 10 : 0;
    retval += enc & 0x04 ? 20 : 0;
    retval += enc & 0x02 ? 40 : 0;
    retval += enc & 0x01 ? 80 : 0;

    return retval;
}

int proxima_clock_gettime( lua_State* L )
{
    u8 data;

    if ( rtc_state == NOT_INIT )
    {
        platform_i2c_setup( 1, PLATFORM_I2C_SPEED_FAST );
        rtc_state = INIT;
    }

    lua_createtable( L, 3, 0 );

    platform_i2c_send_address(1, RTC_TIME_ADDR, PLATFORM_I2C_DIRECTION_RECEIVER);

    // Add hours to return table
    data = conv_val(platform_i2c_recv_byte(1, 1));
    lua_pushinteger( L, data );
    lua_rawseti( L, -2, 1 );

    // Add minutes to return table
    data = conv_val(platform_i2c_recv_byte(1, 1));
    lua_pushinteger( L, data );
    lua_rawseti( L, -2, 2 );

    // Add seconds to return table
    data = conv_val(platform_i2c_recv_byte(1, 0));
    lua_pushinteger( L, data );
    lua_rawseti( L, -2, 3 );

    platform_i2c_send_stop(1);

    return 1;
}
