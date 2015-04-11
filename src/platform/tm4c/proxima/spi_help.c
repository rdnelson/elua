#include "spi_help.h"

#include "platform.h"
#include "lauxlib.h"

void setup_select()
{ platform_pio_op( 0, 0x1C, PLATFORM_IO_PIN_DIR_OUTPUT );
}

void select_chip(int n)
{
    n &= 7;

    platform_pio_op( 0, 7 << 2, PLATFORM_IO_PIN_CLEAR );
    platform_pio_op( 0, n << 2, PLATFORM_IO_PIN_SET );
}

int proxima_chip_select( lua_State* L )
{
    u16 val = luaL_checkinteger( L, 1 );

    setup_select();
    select_chip( val & 7 );

    return 0;
}
