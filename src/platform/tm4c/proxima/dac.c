#include "dac.h"

#include "lauxlib.h"
#include "platform.h"

#include "spi_help.h"

#define NOT_INIT    0
#define INIT        1

static unsigned char dac_state = NOT_INIT;

int proxima_dac_init( lua_State* L )
{
    platform_spi_setup( SPI_BASE, PLATFORM_SPI_MASTER, 10000000, 0, 0, 16);

    setup_select();
    select_chip( CS_NONE );

    dac_state = INIT;

    return 0;
}

void set_a( u16 val )
{
    select_chip( CS_DAC );
    platform_spi_send_recv( SPI_BASE, 0x3000 | ((val & 0x3ff) << 2) );
    select_chip( CS_NONE );
}

void set_b( u16 val )
{
    select_chip( CS_DAC );
    platform_spi_send_recv( SPI_BASE, 0xB000 | ((val & 0x3ff) << 2) );
    select_chip( CS_NONE );
}

int proxima_dac_seta( lua_State* L )
{
    u16 val = luaL_checkinteger( L, 1 );

    if (dac_state == NOT_INIT )
        proxima_dac_init( L );

    set_a( val );

    return 0;
}

int proxima_dac_setb( lua_State* L )
{
    u16 val = luaL_checkinteger( L, 1 );

    if (dac_state == NOT_INIT )
        proxima_dac_init( L );

    set_b( val );

    return 0;
}

int proxima_dac_setboth( lua_State* L )
{
    u16 a, b;

    if (dac_state == NOT_INIT )
        proxima_dac_init( L );

    a = luaL_checkinteger( L, 1 );
    b = luaL_checkinteger( L, 2 );

    set_a( a );
    set_b( b );

    return 0;
}
