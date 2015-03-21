

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
#include "proxima/clock.h"
#include "proxima/dac.h"
#include "proxima/gyro.h"
#include "proxima/led.h"
#include "proxima/lcd.h"
#include "proxima/motors.h"
#include "proxima/net.h"

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

const LUA_REG_TYPE proxima_mc_map[] =
{
    { LSTRKEY("forward"), LFUNCVAL( proxima_motors_forward ) },
    { LSTRKEY("reverse"), LFUNCVAL( proxima_motors_reverse ) },
    { LSTRKEY("right"), LFUNCVAL( proxima_motors_right ) },
    { LSTRKEY("left"), LFUNCVAL( proxima_motors_left ) },
    { LSTRKEY("stop"), LFUNCVAL( proxima_motors_stop ) },
    { LSTRKEY("getstatus"), LFUNCVAL( proxima_motors_get_status ) },
    { LSTRKEY("set"), LFUNCVAL( proxima_motors_set ) },
    { LNILKEY, LNILVAL },
};

const LUA_REG_TYPE proxima_clock_map[] =
{
    { LSTRKEY("gettime"), LFUNCVAL( proxima_clock_gettime ) },
    { LNILKEY, LNILVAL },
};

const LUA_REG_TYPE proxima_net_map[] =
{
    { LSTRKEY("open"), LFUNCVAL( proxima_net_open ) },
    { LSTRKEY("close"), LFUNCVAL( proxima_net_close ) },
    { LSTRKEY("commit"), LFUNCVAL( proxima_net_commit ) },
    { LSTRKEY("flush"), LFUNCVAL( proxima_net_flush ) },
    { LSTRKEY("set_host"), LFUNCVAL( proxima_net_set_host ) },
    { LSTRKEY("write"), LFUNCVAL( proxima_net_write ) },
    { LSTRKEY("read"), LFUNCVAL( proxima_net_read ) },
    { LSTRKEY("set_key"), LFUNCVAL( proxima_net_set_key ) },
    { LSTRKEY("set_value"), LFUNCVAL( proxima_net_set_value ) },
    { LSTRKEY("get_status"), LFUNCVAL( proxima_net_get_status ) },
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
    { LSTRKEY( "motors" ), LROVAL( proxima_mc_map ) },
    { LSTRKEY( "clock" ), LROVAL( proxima_clock_map ) },
    { LSTRKEY( "net" ), LROVAL( proxima_net_map ) },
    { LNILKEY, LNILVAL },
};

