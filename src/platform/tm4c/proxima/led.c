#include "led.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "platform.h"
#include "auxmods.h"
#include "lrotable.h"
#include "platform_conf.h"

#include "gpio.h"
#include "uart.h"
#include "i2c.h"

#include "rom.h"
#include "rom_map.h"

#define RED 0x02
#define GREEN 0x08
#define BLUE 0x04

#define LED_MASK ( RED | GREEN | BLUE )

static int proxima_help_led_state( u8 pinmask, u8 led )
{
    MAP_GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE, pinmask );
    MAP_GPIOPinWrite( GPIO_PORTF_BASE, pinmask , led );

    return 0;
}

int proxima_led_setrgb( lua_State* L )
{
    u32 red = luaL_checkinteger( L, 1 );
    u32 green = luaL_checkinteger( L, 2 );
    u32 blue = luaL_checkinteger( L, 3 );

    u8 led = ( ( red != 0 ) ? RED : 0 ) | ( ( blue != 0 ) ? BLUE : 0 ) | ( ( green != 0 ) ? GREEN : 0 );

    return proxima_help_led_state( LED_MASK, led );
}

int proxima_led_setred( lua_State* L )
{
    return proxima_help_led_state( RED, luaL_checkinteger( L, 1 ) ? RED : 0 );
}

int proxima_led_setgreen( lua_State* L )
{
    return proxima_help_led_state( GREEN, luaL_checkinteger( L, 1 ) ? GREEN : 0 );
}

int proxima_led_setblue( lua_State* L )
{
    return proxima_help_led_state( BLUE, luaL_checkinteger( L, 1 ) ? BLUE : 0 );
}
