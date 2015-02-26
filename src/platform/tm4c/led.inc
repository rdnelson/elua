
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
