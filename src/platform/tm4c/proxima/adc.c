#include "adc.h"

#include "elua_adc.h"
#include "platform.h"
#include "lauxlib.h"

#define ADC_CH1     8
#define ADC_CH2     11
#define ADC_LEFT    5
#define ADC_RIGHT   4
#define ADC_CENTER  2
#define ADC_POT     1

int proxima_adc_getch( u8 id, lua_State* L )
{
    int res = adc_setup_channel( id, 1 );

    if ( res != PLATFORM_OK )
    {
        return luaL_error( L, "Failed to setup sampler" );
    }

    while ( adc_samples_available( id ) > 0 )
    {
        adc_get_processed_sample( id );
    }

    platform_adc_start_sequence();

    if ( adc_wait_samples( id, 1 ) >= 1 )
    {
        lua_pushinteger( L, adc_get_processed_sample( id ) );
        return 1;
    }

    return 0;
}

int proxima_adc_geta( lua_State* L )
{
    return proxima_adc_getch( ADC_CH1, L );
}

int proxima_adc_getb( lua_State* L )
{
    return proxima_adc_getch( ADC_CH2, L );
}

int proxima_adc_getright( lua_State* L )
{
    return proxima_adc_getch( ADC_RIGHT, L );
}

int proxima_adc_getleft( lua_State* L )
{
    return proxima_adc_getch( ADC_LEFT, L );
}

int proxima_adc_getcenter( lua_State* L )
{
    return proxima_adc_getch( ADC_CENTER, L );
}

int proxima_adc_getknob( lua_State* L )
{
    return proxima_adc_getch( ADC_POT, L );
}

