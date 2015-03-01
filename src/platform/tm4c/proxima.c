

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

#include "mpu_help.h"

#if LUA_OPTIMIZE_MEMORY == 0
#error "Proxima module is requires lua LTR"
#endif


static int tm4c_proxima_mt_index( lua_State* L )
{
    return 0;
}

#include "led.inc.c"
#include "accel.inc.c"
#include "gyro.inc.c"

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

const LUA_REG_TYPE accel_map[] =
{
    { LSTRKEY("init"), LFUNCVAL( proxima_accel_init ) },
    { LSTRKEY("getx"), LFUNCVAL( proxima_accel_get_x ) },
    { LSTRKEY("gety"), LFUNCVAL( proxima_accel_get_y ) },
    { LSTRKEY("getz"), LFUNCVAL( proxima_accel_get_z ) },
    { LSTRKEY("gettemp"), LFUNCVAL( proxima_accel_get_temp ) },
    { LSTRKEY("getid"), LFUNCVAL( proxima_accel_get_id ) },
    { LNILKEY, LNILVAL },
};

const LUA_REG_TYPE gyro_map[] =
{
    { LSTRKEY("init"), LFUNCVAL( proxima_gyro_init ) },
    { LSTRKEY("getx"), LFUNCVAL( proxima_gyro_get_x ) },
    { LSTRKEY("gety"), LFUNCVAL( proxima_gyro_get_y ) },
    { LSTRKEY("getz"), LFUNCVAL( proxima_gyro_get_z ) },
    { LSTRKEY("gettemp"), LFUNCVAL( proxima_accel_get_temp ) },
    { LSTRKEY("getid"), LFUNCVAL( proxima_accel_get_id ) },
    { LNILKEY, LNILVAL },
};

const LUA_REG_TYPE proxima_map[] =
{
    { LSTRKEY( "__index" ), LFUNCVAL( tm4c_proxima_mt_index ) },
    { LSTRKEY( "__metatable" ), LROVAL( proxima_map ) },
    { LSTRKEY( "led" ), LROVAL( led_map ) },
    { LSTRKEY( "accel" ), LROVAL( accel_map ) },
    { LSTRKEY( "gyro" ), LROVAL( gyro_map ) },
    { LNILKEY, LNILVAL },
};

