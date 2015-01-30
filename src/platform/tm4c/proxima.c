

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "platform.h"
#include "auxmods.h"
#include "lrotable.h"
#include "platform_conf.h"

#include "gpio.h"
#include "uart.h"

#include "rom.h"
#include "rom_map.h"

#define RED 0x02
#define GREEN 0x08
#define BLUE 0x04

#define LED_MASK ( RED | GREEN | BLUE )

#if LUA_OPTIMIZE_MEMORY == 0
#error "Proxima module is requires lua LTR"
#endif

static int tm4c_proxima_mt_index( lua_State* L )
{
    return 0;
}

static int proxima_help_led_state( u8 pinmask, u8 led )
{
    MAP_GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE, pinmask );
    MAP_GPIOPinWrite( GPIO_PORTF_BASE, pinmask , led );

    return 0;
}

static int proxima_led_setrgb( lua_State* L )
{
    u32 red = luaL_checkinteger( L, 1 );
    u32 green = luaL_checkinteger( L, 2 );
    u32 blue = luaL_checkinteger( L, 3 );

    u8 led = ( ( red != 0 ) ? RED : 0 ) | ( ( blue != 0 ) ? BLUE : 0 ) | ( ( green != 0 ) ? GREEN : 0 );

    return proxima_help_led_state( LED_MASK, led );
}

static int proxima_led_setred( lua_State* L )
{
    return proxima_help_led_state( RED, luaL_checkinteger( L, 1 ) ? RED : 0 );
}

static int proxima_led_setgreen( lua_State* L )
{
    return proxima_help_led_state( GREEN, luaL_checkinteger( L, 1 ) ? GREEN : 0 );
}

static int proxima_led_setblue( lua_State* L )
{
    return proxima_help_led_state( BLUE, luaL_checkinteger( L, 1 ) ? BLUE : 0 );
}

#define MIN_OPT_LEVEL 2
#include "lrodefs.h"

const LUA_REG_TYPE led_map[] =
{
    { LSTRKEY("setrgb"), LFUNCVAL( proxima_led_setrgb ) },
    { LSTRKEY( "setred" ), LFUNCVAL( proxima_led_setred ) },
    { LSTRKEY( "setgreen" ), LFUNCVAL( proxima_led_setgreen ) },
    { LSTRKEY( "setblue" ), LFUNCVAL( proxima_led_setblue ) },
    { LNILKEY, LNILVAL },
};

const LUA_REG_TYPE proxima_map[] =
{
    { LSTRKEY( "__index" ), LFUNCVAL( tm4c_proxima_mt_index ) },
    { LSTRKEY( "__metatable" ), LROVAL( proxima_map ) },
    { LSTRKEY( "led" ), LROVAL( led_map ) },
    { LNILKEY, LNILVAL },
};

