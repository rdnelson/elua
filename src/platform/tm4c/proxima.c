

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

#include "proxima/accel.h"
#include "proxima/adc.h"
#include "proxima/buttons.h"
#include "proxima/dac.h"
#include "proxima/gyro.h"
#include "proxima/led.h"
#include "proxima/lcd.h"

#if LUA_OPTIMIZE_MEMORY == 0
#error "Proxima module is requires lua LTR"
#endif


static int tm4c_proxima_mt_index( lua_State* L )
{
    return 0;
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

const LUA_REG_TYPE dac_map[] =
{
    { LSTRKEY("init"), LFUNCVAL( proxima_dac_init ) },
    { LSTRKEY("seta"), LFUNCVAL( proxima_dac_seta ) },
    { LSTRKEY("setb"), LFUNCVAL( proxima_dac_setb ) },
    { LSTRKEY("setboth"), LFUNCVAL( proxima_dac_setboth ) },
    { LNILKEY, LNILVAL },
};

const LUA_REG_TYPE button_map[] =
{
    { LSTRKEY("wait"), LFUNCVAL( proxima_buttons_wait ) },
    { LSTRKEY("check"), LFUNCVAL( proxima_buttons_check ) },
    { LNILKEY, LNILVAL },
};

const LUA_REG_TYPE lcd_map[] =
{
    { LSTRKEY("init"), LFUNCVAL( proxima_lcd_init ) },
    { LSTRKEY("pic"), LFUNCVAL( proxima_lcd_pic ) },
    { LNILKEY, LNILVAL },
};

const LUA_REG_TYPE proxima_adc_map[] =
{
    { LSTRKEY("geta"), LFUNCVAL( proxima_adc_geta ) },
    { LSTRKEY("getb"), LFUNCVAL( proxima_adc_getb ) },
    { LSTRKEY("getleft"), LFUNCVAL( proxima_adc_getleft ) },
    { LSTRKEY("getright"), LFUNCVAL( proxima_adc_getright ) },
    { LSTRKEY("getcenter"), LFUNCVAL( proxima_adc_getcenter ) },
    { LSTRKEY("getknob"), LFUNCVAL( proxima_adc_getknob ) },
    { LNILKEY, LNILVAL },
};

const LUA_REG_TYPE proxima_map[] =
{
    { LSTRKEY( "__index" ), LFUNCVAL( tm4c_proxima_mt_index ) },
    { LSTRKEY( "__metatable" ), LROVAL( proxima_map ) },
    { LSTRKEY( "accel" ), LROVAL( accel_map ) },
    { LSTRKEY( "adc" ), LROVAL( proxima_adc_map ) },
    { LSTRKEY( "buttons" ), LROVAL( button_map ) },
    { LSTRKEY( "dac" ), LROVAL( dac_map ) },
    { LSTRKEY( "gyro" ), LROVAL( gyro_map ) },
    { LSTRKEY( "lcd" ), LROVAL( lcd_map ) },
    { LSTRKEY( "led" ), LROVAL( led_map ) },
    { LNILKEY, LNILVAL },
};

