#include "buttons.h"

#include "platform.h"

#define LP_PORT 5
#define LP_PINS 0x11

#define ROW_PORT 3
#define ROW_PINS 0xC0
#define ROW1 0x40
#define ROW2 0x80

#define COL_PORT 2
#define COL_PINS 0x60
#define COL1 0x20
#define COL2 0x40

#define PROX_PORT

static void proxima_buttons_init()
{
    platform_pio_op(LP_PORT, LP_PINS, PLATFORM_IO_PIN_DIR_INPUT);
    platform_pio_op(LP_PORT, LP_PINS, PLATFORM_IO_PIN_PULLUP);

    platform_pio_op(ROW_PORT, ROW_PINS, PLATFORM_IO_PIN_DIR_OUTPUT);
    platform_pio_op(ROW_PORT, ROW_PINS, PLATFORM_IO_PIN_SET);

    platform_pio_op(COL_PORT, COL_PINS, PLATFORM_IO_PIN_DIR_INPUT);
}

u8 proxima_buttons_wait( lua_State* L )
{
    u8 val;
    proxima_buttons_init();

    while ( 1 )
    {
        val = platform_pio_op(LP_PORT, LP_PINS, PLATFORM_IO_PORT_GET_VALUE) & LP_PINS;

        if (val != LP_PINS)
        {
            if ( val & BUTTON_LP1 )
            {
                lua_pushinteger( L, BUTTON_LP2 );
            }
            else
            {
                lua_pushinteger( L, BUTTON_LP1 );
            }

            return 1;
        }

        platform_pio_op(ROW_PORT, ROW_PINS, PLATFORM_IO_PIN_SET);
        val = platform_pio_op(COL_PORT, COL_PINS, PLATFORM_IO_PORT_GET_VALUE) & COL_PINS;

        if (val) {
            // Proxima button is pushed
            if ( val & COL1 )
            {
                platform_pio_op(ROW_PORT, ROW1, PLATFORM_IO_PIN_CLEAR);

                if ( platform_pio_op(COL_PORT, COL_PINS, PLATFORM_IO_PORT_GET_VALUE) & COL1 )
                {
                    // Row2, Col1
                    lua_pushinteger( L, BUTTON_PROXIMA_SW2 );
                    return 1;
                }

                platform_pio_op(ROW_PORT, ROW_PINS, PLATFORM_IO_PIN_SET);
                platform_pio_op(ROW_PORT, ROW2, PLATFORM_IO_PIN_CLEAR);

                if ( platform_pio_op(COL_PORT, COL_PINS, PLATFORM_IO_PORT_GET_VALUE) & COL1 )
                {
                    // Row1, Col1
                    lua_pushinteger( L, BUTTON_PROXIMA_SW1 );
                    return 1;
                }
            }
            else if ( val & COL2 )
            {
                platform_pio_op(ROW_PORT, ROW1, PLATFORM_IO_PIN_CLEAR);

                if ( platform_pio_op(COL_PORT, COL_PINS, PLATFORM_IO_PORT_GET_VALUE) & COL2 )
                {
                    // Row2, Col2
                    lua_pushinteger( L, BUTTON_PROXIMA_SW4 );
                    return 1;
                }

                platform_pio_op(ROW_PORT, ROW_PINS, PLATFORM_IO_PIN_SET);
                platform_pio_op(ROW_PORT, ROW2, PLATFORM_IO_PIN_CLEAR);

                if ( platform_pio_op(COL_PORT, COL_PINS, PLATFORM_IO_PORT_GET_VALUE) & COL2 )
                {
                    // Row1, Col2
                    lua_pushinteger( L, BUTTON_PROXIMA_SW3 );
                    return 1;
                }
            }
        }
    }

    return 0;
}

u8 proxima_buttons_check( lua_State* L )
{

    return 0;
}
