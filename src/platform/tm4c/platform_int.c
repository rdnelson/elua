// tm4c interrupt support

#define PART_TM4C123GH6PM
#define TARGET_IS_BLIZZARD_RB1

#include "platform_conf.h"
#if defined( BUILD_C_INT_HANDLERS ) || defined( BUILD_LUA_INT_HANDLERS )

#include "platform.h"
#include "elua_int.h"
#include "common.h"

//#include "tm4c123gh6pm.h"

#include "rom.h"
#include "rom_map.h"
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_gpio.h"
#include "hw_timer.h"
#include "gpio.h"
#include "uart.h"
#include "interrupt.h"
#include "driverlib/timer.h"

#define GPIO_INT_POSEDGE_ENABLED 1
#define GPIO_INT_NEGEDGE_ENABLED 2
#define GPIO_INT_BOTHEDGE_ENABLED ( GPIO_INT_POSEDGE_ENABLED | GPIO_INT_NEGEDGE_ENABLED )

extern const u32 uart_base[];
extern const u32 pio_base[];

static const int uart_int_mask = UART_INT_RX | UART_INT_RT;
static const u8 gpio_int_ids[] = { INT_GPIOA, INT_GPIOB, INT_GPIOC, INT_GPIOD, INT_GPIOE, INT_GPIOF };

extern const u32 timer_base[];
extern u8 tm4c_timer_int_periodic_flag[ NUM_TIMER ];
static const u8 timer_int_ids[] = { INT_TIMER0A, INT_TIMER1A, INT_TIMER2A, INT_TIMER3A, INT_TIMER4A, INT_TIMER5A };

static void uart_common_rx_handler( int resnum )
{
    MAP_UARTIntClear( uart_base[ resnum ], uart_int_mask );
    while( MAP_UARTCharsAvail( uart_base[ resnum ] ) )
        cmn_int_handler( INT_UART_RX, resnum );
}

void uart0_handler()
{
    uart_common_rx_handler( 0 );
}

void uart1_handler()
{
    uart_common_rx_handler( 1 );
}

void uart2_handler()
{
    uart_common_rx_handler( 2 );
}

void uart3_handler()
{
    uart_common_rx_handler( 3 );
}

void uart4_handler()
{
    uart_common_rx_handler( 4 );
}

void uart5_handler()
{
    uart_common_rx_handler( 5 );
}

void uart7_handler()
{
    uart_common_rx_handler( 7 );
}


static void gpio_common_handler( int port )
{
    u32 base = pio_base[ port ];
    u8 pin, pinmask;
    u32 ibe = HWREG( base + GPIO_O_IBE );
    u32 iev = HWREG( base + GPIO_O_IEV );

    for( pin = 0, pinmask = 1; pin < platform_pio_get_num_pins( port ); pin++, pinmask <<= 1 )
        if( HWREG( base + GPIO_O_MIS ) & pinmask )
        {
            if( MAP_GPIOPinRead( base, pinmask ) && ( ( ibe & pinmask ) || ( iev & pinmask ) ) )
                cmn_int_handler( INT_GPIO_POSEDGE, PLATFORM_IO_ENCODE( port, pin, 0 ) );
            else if( ( ibe & pinmask ) || !( iev & pinmask ) )
                cmn_int_handler( INT_GPIO_NEGEDGE, PLATFORM_IO_ENCODE( port, pin, 0 ) );

            HWREG( base + GPIO_O_ICR ) = pinmask;
        }
}

void gpioa_handler()
{
    gpio_common_handler( 0 );
}

void gpiob_handler()
{
    gpio_common_handler( 1 );
}

void gpioc_handler()
{
    gpio_common_handler( 2 );
}

void gpiod_handler()
{
    gpio_common_handler( 3 );
}

void gpioe_handler()
{
    gpio_common_handler( 4 );
}

void gpiof_handler()
{
    gpio_common_handler( 5 );
}

static void tmr_common_handler( elua_int_resnum id )
{
    u32 base = timer_base[ id ];

    MAP_TimerIntClear( base, TIMER_TIMA_TIMEOUT );
    if( tm4c_timer_int_periodic_flag[ id ] != PLATFORM_TIMER_INT_CYCLIC )
    {
        MAP_TimerIntDisable( base, TIMER_TIMA_TIMEOUT );
        MAP_TimerLoadSet( base, TIMER_A, 0xFFFFFFFF );
    }

    cmn_int_handler( INT_TMR_MATCH, id );
}

void tmr0_handler()
{
    tmr_common_handler( 0 );
}

void tmr1_handler()
{
    tmr_common_handler( 1 );
}

void tmr2_handler()
{
    tmr_common_handler( 2 );
}

void tmr3_handler()
{
    tmr_common_handler( 3 );
}

void tmr4_handler()
{
    tmr_common_handler( 4 );
}

void tmr5_handler()
{
    tmr_common_handler( 5 );
}

static int inth_gpio_get_int_status( elua_int_resnum resnum )
{
    const u32 portbase = pio_base[ PLATFORM_IO_GET_PORT( resnum ) ];
    const u8 pinmask = 1 << PLATFORM_IO_GET_PIN( resnum );

    if( MAP_GPIOIntStatus( portbase, pinmask ) == 0 )
        return 0;

    switch( MAP_GPIOIntTypeGet( portbase, pinmask ) )
    {
        case GPIO_BOTH_EDGES:
            return GPIO_INT_BOTHEDGE_ENABLED;

        case GPIO_RISING_EDGE:
            return GPIO_INT_POSEDGE_ENABLED;

        default:
        case GPIO_FALLING_EDGE:
            return GPIO_INT_NEGEDGE_ENABLED;
    }
}

static int int_uart_rx_get_status( elua_int_resnum resnum )
{
    return ( MAP_UARTIntStatus( uart_base[ resnum ], false ) & uart_int_mask ) == uart_int_mask ? 1 : 0;
}

static int int_uart_rx_set_status( elua_int_resnum resnum, int status )
{
    int prev = int_uart_rx_get_status( resnum );

    if ( status == PLATFORM_CPU_ENABLE )
        MAP_UARTIntEnable( uart_base[ resnum ], uart_int_mask );
    else
        MAP_UARTIntDisable( uart_base[ resnum ], uart_int_mask );

    return prev;
}

static int int_uart_rx_get_flag( elua_int_resnum resnum, int clear )
{
    int flag = ( MAP_UARTIntStatus( uart_base[ resnum ], false ) & uart_int_mask ) == uart_int_mask ? 1 : 0;

    if ( clear )
        MAP_UARTIntClear( uart_base[ resnum ], uart_int_mask );

    return flag;
}

static int int_gpio_posedge_get_status( elua_int_resnum resnum )
{
    u32 port = pio_base[ PLATFORM_IO_GET_PORT( resnum )], pin = PLATFORM_IO_GET_PIN( resnum );
    unsigned long type;

    if( MAP_GPIOIntStatus( port, pin ) == 0 )
        return 0;

    type = MAP_GPIOIntTypeGet( port, pin );
    return ( type == GPIO_RISING_EDGE || type == GPIO_BOTH_EDGES ) ? 1 : 0;
}

static int int_gpio_posedge_set_status( elua_int_resnum resnum, int status )
{
    u32 portbase = pio_base[ PLATFORM_IO_GET_PORT( resnum ) ];
    u8 pinmask = 1 << PLATFORM_IO_GET_PIN( resnum );
    int prev = int_gpio_posedge_get_status( resnum );
    int crtstat = inth_gpio_get_int_status( resnum );

    if( status == PLATFORM_CPU_ENABLE )
    {
        if( crtstat & GPIO_INT_NEGEDGE_ENABLED )
            HWREG( portbase + GPIO_O_IBE ) |= pinmask;
        else
            HWREG( portbase + GPIO_O_IEV ) |= pinmask;

        MAP_GPIOIntEnable( portbase, pinmask );
    }
    else
    {
        // TODO: Is this right?
        if( crtstat == GPIO_INT_BOTHEDGE_ENABLED )
        {
            HWREG( portbase + GPIO_O_IBE ) &= ( u8 ) ~pinmask;
            HWREG( portbase + GPIO_O_IEV ) &= ( u8 ) ~pinmask;
        }
        else if( crtstat == GPIO_INT_POSEDGE_ENABLED )
            MAP_GPIOIntDisable( portbase, pinmask );
    }
    return prev;
}

static int int_gpio_posedge_get_flag( elua_int_resnum resnum, int clear )
{
    u32 portbase = pio_base[ PLATFORM_IO_GET_PORT( resnum ) ];
    u8 pinmask = 1 << PLATFORM_IO_GET_PIN( resnum );

    if( MAP_GPIOPinRead( portbase, pinmask ) == 0 )
        return 0;
    if( MAP_GPIOIntStatus( portbase, true ) & pinmask )
    {
        if( clear )
            MAP_GPIOIntClear( portbase, pinmask );

        return 1;
    }

    return 0;
}

static int int_gpio_negedge_get_status( elua_int_resnum resnum )
{
    u32 port = PLATFORM_IO_GET_PORT( resnum ), pin = PLATFORM_IO_GET_PIN( resnum );
    unsigned long type;

    if( ( HWREG( pio_base[ port ] + GPIO_O_IM ) & ( 1 << pin ) ) == 0 )
        return 0;

    type = MAP_GPIOIntTypeGet( pio_base[ port ], pin );
    return ( type == GPIO_FALLING_EDGE || type == GPIO_BOTH_EDGES ) ? 1 : 0;
}

static int int_gpio_negedge_set_status( elua_int_resnum resnum, int status )
{
    u32 portbase = pio_base[ PLATFORM_IO_GET_PORT( resnum ) ];
    u8 pinmask = 1 << PLATFORM_IO_GET_PIN( resnum );
    int prev = int_gpio_negedge_get_status( resnum );
    int crtstat = inth_gpio_get_int_status( resnum );

    if( status == PLATFORM_CPU_ENABLE )
    {
        if( crtstat * GPIO_INT_NEGEDGE_ENABLED )
            HWREG( portbase + GPIO_O_IBE ) |= pinmask;
        else
            HWREG( portbase + GPIO_O_IEV ) &= ( u8 ) ~pinmask;

        MAP_GPIOIntEnable( portbase, pinmask );
    }
    else
    {
        // TODO: Is this right?
        if( crtstat == GPIO_INT_BOTHEDGE_ENABLED )
        {
            HWREG( portbase + GPIO_O_IBE ) &= ( u8 ) ~pinmask;
            HWREG( portbase + GPIO_O_IEV ) |= pinmask;
        }
        else if( crtstat == GPIO_INT_NEGEDGE_ENABLED )
            MAP_GPIOIntDisable( portbase, pinmask );
    }
    return prev;
}

static int int_gpio_negedge_get_flag( elua_int_resnum resnum, int clear )
{
    u32 portbase = pio_base[ PLATFORM_IO_GET_PORT( resnum ) ];
    u8 pinmask = 1 << PLATFORM_IO_GET_PIN( resnum );

    if( MAP_GPIOPinRead( portbase, pinmask ) == 0 )
        return 0;
    if( MAP_GPIOIntStatus( portbase, true ) & pinmask )
    {
        if( clear )
            MAP_GPIOIntClear( portbase, pinmask );

        return 1;
    }

    return 0;
}

#define tmr_is_enabled( base ) ( ( HWREG( base + TIMER_O_CTL ) & TIMER_CTL_TAEN ) != 0 )

static int int_tmr_match_get_status( elua_int_resnum resnum )
{
    u32 base = timer_base [ resnum ];

    return ( tmr_is_enabled( base ) && ( HWREG( base + TIMER_O_IMR ) & TIMER_TIMA_TIMEOUT ) ) ? 1 : 0;
}

static int int_tmr_match_set_status( elua_int_resnum resnum, int status )
{
    int prev = int_tmr_match_get_status( resnum );
    u32 base = timer_base[ resnum ];

    if( status == PLATFORM_CPU_ENABLE )
    {
        MAP_TimerEnable( base, TIMER_A );
        MAP_TimerIntEnable( base, TIMER_TIMA_TIMEOUT );
    }
    else
    {
        MAP_TimerIntDisable( base, TIMER_TIMA_TIMEOUT );
        MAP_TimerDisable( base, TIMER_A );
    }

    return prev;
}

static int int_tmr_match_get_flag( elua_int_resnum resnum, int clear )
{
    u32 base = timer_base[ resnum ];
    int status = MAP_TimerIntStatus( base, true ) & TIMER_TIMA_TIMEOUT;

    if( clear )
        MAP_TimerIntClear( base, TIMER_TIMA_TIMEOUT );
    return status && tmr_is_enabled( base ) ? 1 : 0;
}

void platform_int_init()
{
    unsigned i;

    MAP_IntEnable( INT_UART0 );
    MAP_IntEnable( INT_UART1 );
    MAP_IntEnable( INT_UART2 );
    MAP_IntEnable( INT_UART3 );
    MAP_IntEnable( INT_UART4 );
    MAP_IntEnable( INT_UART5 );
    MAP_IntEnable( INT_UART7 );

    for( i = 0; i < sizeof( gpio_int_ids ) / sizeof( u8 ); i++ )
        MAP_IntEnable( gpio_int_ids[ i ] );
    for( i = 0; i < sizeof( timer_int_ids ) / sizeof( u8 ); i++ )
        MAP_IntEnable( timer_int_ids[ i ] );
}

const elua_int_descriptor elua_int_table[ INT_ELUA_LAST ] =
{
    { int_uart_rx_set_status, int_uart_rx_get_status, int_uart_rx_get_flag },
    { int_gpio_posedge_set_status, int_gpio_posedge_get_status, int_gpio_posedge_get_flag },
    { int_gpio_negedge_set_status, int_gpio_negedge_get_status, int_gpio_negedge_get_flag },
    { int_tmr_match_set_status, int_tmr_match_get_status, int_tmr_match_get_flag }
};

#endif
