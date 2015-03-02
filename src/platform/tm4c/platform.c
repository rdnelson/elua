// Platform-dependent functions

#include "platform.h"
#include "type.h"
#include "devman.h"
#include "genstd.h"
#include <reent.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "elua_adc.h"
#include "platform_conf.h"
#include "common.h"
#include "math.h"
#include "diskio.h"
#include "lua.h"
#include "lauxlib.h"
#include "lrotable.h"
#include "elua_int.h" 

// Platform specific includes

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/can.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/adc.h"
#include "driverlib/systick.h"
#include "driverlib/i2c.h"
#include "driverlib/flash.h"
#include "hw_types.h"
#include "hw_gpio.h"
#include "buf.h"
#include "utils.h"

#define TARGET_IS_BLIZZARD_RB1
#define PART_TM4C123GH6PM
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#define UNLOCK_MAGIC_NUM 0x4C4F434B

// USB CDC Stuff
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usbcdc.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcdc.h"
#include "usb_serial_structs.h"

#include "proxima/spi_help.h"

#define SYSTICKHZ       5
#define SYSTICKMS       (1000 / SYSTICKHZ)

//Initializers
static void timers_init();
static void uarts_init();
static void pios_init();

#if NUM_SPI > 0
static void spis_init();
#endif

#if NUM_I2C > 0
static void i2c_init();
#endif

#if NUM_PWM > 0
static void pwms_init();
#endif

#ifdef BUILD_ADC
static void adcs_init();
#endif

#ifdef BUILD_CAN
static void cans_init();
#endif

#ifdef BUILD_USB_CDC
static void usb_init();
#endif

int platform_init()
{
#if defined ( TM4C123GH6PM )
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
#endif

    pios_init();

#if NUM_SPI > 0
    spis_init();
#endif

#ifdef BUILD_MMCFS
    setup_select();
    select_chip(0);
#endif

#if NUM_I2C > 0
    i2c_init();
#endif

    uarts_init();
    timers_init();

#if NUM_PWM > 0
    pwms_init();
#endif

#ifdef BUILD_ADC
    adcs_init();
#endif

#ifdef BUILD_CAN
    cans_init();
#endif

#ifdef BUILD_USB_CDC
    usb_init();
#endif

    cmn_systimer_set_base_freq( MAP_SysCtlClockGet() );
    cmn_systimer_set_interrupt_freq( SYSTICKHZ );

    cmn_platform_init();

#if VTMR_NUM_TIMERS > 0
    MAP_SysTickPeriodSet( MAP_SysCtlClockGet() / SYSTICKHZ );
    MAP_SysTickEnable();
    MAP_SysTickIntEnable();
    MAP_IntMasterEnable();
#endif

    return PLATFORM_OK;
}

#if defined( TM4C123GH6PM )
    const u32 pio_base[] = { GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTC_BASE, GPIO_PORTD_BASE,
        GPIO_PORTE_BASE, GPIO_PORTF_BASE };

    const u32 pio_sysctl[] = { SYSCTL_PERIPH_GPIOA, SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOD,
        SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOF };
#endif

static void pios_init()
{
    unsigned int i;

    for ( i = 0; i < NUM_PIO; i++ )
    {
        MAP_SysCtlPeripheralEnable(pio_sysctl[ i ]);
    }

    HWREG( GPIO_PORTF_BASE + GPIO_O_LOCK ) = UNLOCK_MAGIC_NUM;
    HWREG( GPIO_PORTD_BASE + GPIO_O_LOCK ) = UNLOCK_MAGIC_NUM;

    HWREG( GPIO_PORTF_BASE + GPIO_O_CR ) |= GPIO_PIN_0;
    HWREG( GPIO_PORTD_BASE + GPIO_O_CR ) |= GPIO_PIN_7;
}

pio_type platform_pio_op( unsigned port, pio_type pinmask, int op )
{
    pio_type retval = 1, base = pio_base [ port ];

    switch( op )
    {
        case PLATFORM_IO_PORT_SET_VALUE:
            MAP_GPIOPinWrite( base, 0xFF, pinmask );
            break;

        case PLATFORM_IO_PIN_SET:
            MAP_GPIOPinWrite( base, pinmask, pinmask );
            break;

        case PLATFORM_IO_PIN_CLEAR:
            MAP_GPIOPinWrite( base, pinmask, 0 );
            break;

        case PLATFORM_IO_PORT_DIR_INPUT:
            pinmask = 0xFF;
        case PLATFORM_IO_PIN_DIR_INPUT:
            MAP_GPIOPinTypeGPIOInput( base, pinmask );
            break;

        case PLATFORM_IO_PORT_DIR_OUTPUT:
            pinmask = 0xFF;
        case PLATFORM_IO_PIN_DIR_OUTPUT:
            MAP_GPIOPinTypeGPIOOutput( base, pinmask );
            break;

        case PLATFORM_IO_PORT_GET_VALUE:
            retval = MAP_GPIOPinRead( base, 0xFF );
            break;

        case PLATFORM_IO_PIN_GET:
            retval = !!MAP_GPIOPinRead( base, pinmask );
            break;

        case PLATFORM_IO_PIN_PULLUP:
        case PLATFORM_IO_PIN_PULLDOWN:
            MAP_GPIOPadConfigSet( base, pinmask, GPIO_STRENGTH_8MA,
                    op == PLATFORM_IO_PIN_PULLUP ? GPIO_PIN_TYPE_STD_WPU : GPIO_PIN_TYPE_STD_WPD );
            break;

        case PLATFORM_IO_PIN_NOPULL:
            MAP_GPIOPadConfigSet( base, pinmask, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD );
            break;

        default:
            retval = 0;
            break;
    }

    return retval;
}

#ifdef BUILD_CAN

#ifndef CAN_INIT_SPEED
#define CAN_INIT_SPEED 500000
#endif

#define CAN_MSG_OBJ_RX 1
#define CAN_MSG_OBJ_TX 2

volatile u8 can_rx_flag = 0;
volatile u8 can_tx_flag = 0;
volatile u8 can_err_flag = 0;
char can_tx_buf[PLATFORM_CAN_MAXLEN];
tCANMsgObject can_msg_rx;

#ifdef TM4C123GH6PM
#define TM4C_CAN_CLOCK MAP_SysCtlClockGet()
#endif

void CANIntHandler(void)
{
    u32 status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    if ( status == CAN_INT_INTID_STATUS )
    {
        status = MAP_CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        can_err_flag = 1;
        can_tx_flag = 0;
    }
    else if ( status == CAN_MSG_OBJ_RX )
    {
        MAP_CANIntClear(CAN0_BASE, CAN_MSG_OBJ_RX);
        can_rx_flag = 1;
        can_err_flag = 0;
    }
    else if ( status == CAN_MSG_OBJ_TX )
    {
        MAP_CANIntClear(CAN0_BASE, CAN_MSG_OBJ_TX);
        can_tx_flag = 0;
        can_err_flag = 0;
    }
    else
    {
        MAP_CANIntClear(CAN0_BASE, status);
    }
}

void cans_init(void)
{
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    MAP_CANInit(CAN0_BASE);
    MAP_CANBitRateSet(CAN0_BASE, TM4C_CAN_CLOCK, CAN_INIT_SPEED);
    MAP_CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
    MAP_IntEnable(INT_CAN0);
    MAP_CANEnable(CAN0_BASE);

    can_msg_rx.ulMsgID = 0;
    can_msg_rx.ulMsgIDMask = 0;
    can_msg_rx.ulFlags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    can_msg_rx.ulMsgLen = PLATFORM_CAN_MAXLEN;
    MAP_CANMessageSet(CAN0_BASE, CAN_MSG_OBJ_RX, &can_msg_rx, MSG_OBJ_TYPE_RX);
}

u32 platform_can_setup( unsigned int id, u32 clock )
{
    MAP_GPIOPinConfigure(GPIO_PE4_CAN0RX);
    MAP_GPIOPinConfigure(GPIO_PE5_CAN0TX);
    MAP_GPIOPinTypeCAN(CPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    MAP_CANDisable(CAN0_BASE);
    MAP_CANBitRateSet(CAN0_BASE, TM4C_CAN_CLOCK, clock );
    MAP_CANEnable(CAN0_BASE);
    return clock;
}

int platform_can_send( unsigned int id, u32 canid, u8 idtype, u8 len, const u8 *data )
{
    tCANMsgObject msg_tx;
    const char *s = (char*)data;
    char *d;

    while( can_tx_flag == 1 );

    msg_tx.ulFlags = MSG_OBJ_TX_INT_ENABLE;

    if ( idtype == ELUA_CAN_ID_EXT )
    {
        msg_tx.ulFlags |= MSG_OBJ_EXTENDED_ID;
    }

    msg_tx.ulMsgIDMask = 0;
    msg_tx.ulMsgID = canid;
    msg_tx.ulMsgLen = len;
    msg_tx.pucMsgData = (u8*)can_tx_buf;

    d = can_tx_buf;
    DUFF_DEVICE_8( len, *d++ = *s++);

    can_tx_flag = 1;
    MAP_CANMessageSet(CAN0_BASE, CAN_MSG_OBJ_TX, &msg_tx, MSG_OBJ_TYPE_TX);

    return PLATFORM_OK;
}

int platform_can_recv( unsigned int id, u32 *canid, u8 *idtype, u8 *len, u8 *data)
{
    if( can_rx_flag != 0)
    {
        can_msg_rx.pucMsgData = data;
        MAP_CANMessageGet(CAN0_BASE, CAN_MSG_OBJ_RX, &can_msg_rx, 0);
        can_rx_flag = 0;

        *canid = (u32)can_msg_rx.ulMsgID;
        *idtype = (can_msg_rx.ulFlags & MSG_OBJ_EXTENDED_ID) ? ELUA_CAN_ID_EXT : ELUA_CAN_ID_STD;
        *len = can_msg_rx.ulMsgLen;
        return PLATFORM_OK;
    }
    else
    {
        return PLATFORM_UNDERFLOW;
    }
}

#endif // BUILD_CAN

#if NUM_SPI > 0

static const u32 spi_base[] = { SSI0_BASE, SSI1_BASE, SSI2_BASE, SSI3_BASE };
static const u32 spi_sysctl[] = { SYSCTL_PERIPH_SSI0, SYSCTL_PERIPH_SSI1, SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3 };

static const u32 spi_gpio_base[] = { GPIO_PORTA_BASE, GPIO_PORTF_BASE, GPIO_PORTB_BASE, GPIO_PORTD_BASE };
static const u8 spi_gpio_pins[] = { GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5,
                                    GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2,
                                    GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7,
                                    GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 };

static const u32 spi_gpiofunc[] = { GPIO_PA2_SSI0CLK, GPIO_PA4_SSI0RX, GPIO_PA5_SSI0TX,
                                    GPIO_PF0_SSI1RX, GPIO_PF1_SSI1TX, GPIO_PF2_SSI1CLK,
                                    GPIO_PB4_SSI2CLK, GPIO_PB6_SSI2RX, GPIO_PB7_SSI2TX,
                                    GPIO_PD0_SSI3CLK, GPIO_PD2_SSI3RX, GPIO_PD3_SSI3TX };

static void spis_init()
{
    unsigned i;

    for ( i = 0; i < NUM_SPI; i++ )
    {
        MAP_SysCtlPeripheralEnable( spi_sysctl[i] );
    }
}

u32 platform_spi_setup( unsigned id, int mode, u32 clock, unsigned cpol, unsigned cpha, unsigned databits)
{
    unsigned protocol;

    if (cpol == 0)
    {
        protocol = cpha ? SSI_FRF_MOTO_MODE_1 : SSI_FRF_MOTO_MODE_0;
    }
    else
    {
        protocol = cpha ? SSI_FRF_MOTO_MODE_3 : SSI_FRF_MOTO_MODE_2;
    }

    mode = mode == PLATFORM_SPI_MASTER ? SSI_MODE_MASTER : SSI_MODE_SLAVE;

    MAP_SSIDisable( spi_base[ id ] );

    MAP_GPIOPinTypeSSI( spi_gpio_base[ id ], spi_gpio_pins[ id ] );

    MAP_GPIOPinConfigure( spi_gpiofunc[ id * 3 ] );
    MAP_GPIOPinConfigure( spi_gpiofunc[ id * 3 + 1 ] );
    MAP_GPIOPinConfigure( spi_gpiofunc[ id * 3 + 2 ] );

    MAP_GPIOPadConfigSet( spi_gpio_base[ id ], spi_gpio_pins[ id ], GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU );

    MAP_SSIConfigSetExpClk( spi_base[ id ], MAP_SysCtlClockGet(), protocol, mode, clock, databits );
    MAP_SSIEnable( spi_base[ id ] );

    return clock;
}

spi_data_type platform_spi_send_recv( unsigned id, spi_data_type data )
{
    MAP_SSIDataPut( spi_base[ id ], data);
    MAP_SSIDataGet( spi_base[ id ], &data );
    return data;
}

void platform_spi_select( unsigned id, int is_select )
{
    (void)id;
    (void)is_select;
}

#endif // NUM_SPI > 0

#if NUM_I2C > 0

static const u32 i2c_base[] = { I2C0_BASE, I2C1_BASE };
static const u32 i2c_sysctl[] = { SYSCTL_PERIPH_I2C0, SYSCTL_PERIPH_I2C1 };
static const u32 i2c_gpio_base[] = { GPIO_PORTB_BASE, GPIO_PORTA_BASE };
static const u8  i2c_gpio_sclpins[] = { GPIO_PIN_2,
                                     GPIO_PIN_6 };
static const u8 i2c_gpio_sdapins[] = { GPIO_PIN_3,
                                       GPIO_PIN_7 };
static const u32 i2c_gpiofunc[] = { GPIO_PB2_I2C0SCL, GPIO_PB3_I2C0SDA,
                                    GPIO_PA6_I2C1SCL, GPIO_PA7_I2C1SDA };
enum I2CStates
{
    I2C_NO_TRANSFER,
    I2C_START,
    I2C_BULK_TRANSMIT_DELAY,
    I2C_BULK_TRANSMIT,
    I2C_BULK_RECEIVE,
};

static enum I2CStates i2c_flags[] = { I2C_NO_TRANSFER, I2C_NO_TRANSFER };

static void i2c_init()
{
    unsigned i;
    for( i = 0; i < NUM_I2C; i++ )
        MAP_SysCtlPeripheralEnable( i2c_sysctl[ i ] );
}

u32 platform_i2c_setup( unsigned id, u32 speed )
{

    MAP_I2CMasterEnable( i2c_base[ id ] );

    MAP_GPIOPinConfigure( i2c_gpiofunc[ id << 1 ] );
    MAP_GPIOPinConfigure( i2c_gpiofunc[ ( id << 1 ) + 1 ] );

    MAP_GPIOPinTypeI2C( i2c_gpio_base[ id ], i2c_gpio_sdapins[ id ] );

    HWREG( i2c_gpio_base[ id ] + GPIO_O_PUR ) |= i2c_gpio_sdapins[ id ];

    MAP_GPIOPinTypeI2CSCL( i2c_gpio_base[ id ], i2c_gpio_sclpins[ id ] );
    MAP_GPIOPadConfigSet( i2c_gpio_base[ id ], i2c_gpio_sclpins[ id ], GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    MAP_I2CMasterInitExpClk( i2c_base[ id ], MAP_SysCtlClockGet(), speed == PLATFORM_I2C_SPEED_FAST );

    i2c_flags[ id ] = I2C_NO_TRANSFER;

    return speed;
}

void platform_i2c_send_start( unsigned id )
{

    i2c_flags[ id ] = I2C_START;
}

void platform_i2c_send_stop( unsigned id )
{
    MAP_I2CMasterControl( i2c_base[ id ], I2C_MASTER_CMD_BURST_SEND_FINISH );

    i2c_flags[ id ] = I2C_NO_TRANSFER;
}

int platform_i2c_send_address( unsigned id, u16 address, int direction )
{
    MAP_I2CMasterSlaveAddrSet( i2c_base[ id ], address & 0x7F, direction == PLATFORM_I2C_DIRECTION_RECEIVER );

    return 1;
}

int platform_i2c_send_byte( unsigned id, u8 data )
{

    if ( i2c_flags[ id ] == I2C_START )
    {
        MAP_I2CMasterDataPut( i2c_base[ id ], data );
        MAP_I2CMasterControl( i2c_base[ id ], I2C_MASTER_CMD_BURST_SEND_START );
        i2c_flags[ id ] = I2C_BULK_TRANSMIT_DELAY;
        while( MAP_I2CMasterBusy( i2c_base[ id ] ) );
        return 1;
    }
    else if ( i2c_flags[ id ] == I2C_NO_TRANSFER )
    {
        // No start was sent, so single transfer
        MAP_I2CMasterControl( i2c_base[ id ], I2C_MASTER_CMD_SINGLE_SEND );
    }
    else if ( i2c_flags[ id ] == I2C_BULK_TRANSMIT )
    {
        // Start was sent, bulk transfer
        MAP_I2CMasterControl( i2c_base[ id ], I2C_MASTER_CMD_BURST_SEND_CONT );
    }
    else if ( i2c_flags[ id ] == I2C_BULK_TRANSMIT_DELAY )
    {
        MAP_I2CMasterDataPut( i2c_base[ id ], data );
    }

    if ( i2c_flags[ id ] != I2C_BULK_TRANSMIT_DELAY )
    {
        while( MAP_I2CMasterBusy( i2c_base[ id ] ) );

        MAP_I2CMasterDataPut( i2c_base[ id ], data );
    }
    else
    {
        i2c_flags[ id ] = I2C_BULK_TRANSMIT;
    }

    return 1;
}

int platform_i2c_recv_byte( unsigned id, int ack )
{

    if ( ( i2c_flags[ id ] != I2C_BULK_RECEIVE ) && ack ) {
        // ACK means that there's more than one byte
        i2c_flags[ id ] = I2C_BULK_RECEIVE;

        MAP_I2CMasterControl( i2c_base[ id ], I2C_MASTER_CMD_BURST_RECEIVE_START );


    }
    else if ( i2c_flags[ id ] == I2C_BULK_RECEIVE )
    {
        if( !ack )
        {
            i2c_flags[ id ] = I2C_NO_TRANSFER;

            MAP_I2CMasterControl( i2c_base[ id ], I2C_MASTER_CMD_BURST_RECEIVE_FINISH );
        }
        else
        {
            //
            MAP_I2CMasterControl( i2c_base[ id ], I2C_MASTER_CMD_BURST_RECEIVE_CONT );
        }

    }
    else
    {
        MAP_I2CMasterControl( i2c_base[ id ], I2C_MASTER_CMD_SINGLE_RECEIVE );
    }

    while( MAP_I2CMasterBusy( i2c_base[ id ] ) );

    u32 data = MAP_I2CMasterDataGet( i2c_base[ id ] );

    return data;
}

#endif

static const u32 uart_base[] = { UART0_BASE, UART1_BASE, UART2_BASE, UART3_BASE, UART4_BASE, UART5_BASE, UART7_BASE };
static const u32 uart_sysctl[] = { SYSCTL_PERIPH_UART0, SYSCTL_PERIPH_UART1, SYSCTL_PERIPH_UART2,
    SYSCTL_PERIPH_UART3, SYSCTL_PERIPH_UART4, SYSCTL_PERIPH_UART5, SYSCTL_PERIPH_UART7 };
static const u32 uart_gpio_base[] = { GPIO_PORTA_BASE, GPIO_PORTC_BASE, GPIO_PORTD_BASE, GPIO_PORTC_BASE,
    GPIO_PORTC_BASE, GPIO_PORTE_BASE, GPIO_PORTE_BASE };
static const u8 uart_gpio_pins[] = { GPIO_PIN_0 | GPIO_PIN_1,
                                     GPIO_PIN_4 | GPIO_PIN_5,
                                     GPIO_PIN_6 | GPIO_PIN_7,
                                     GPIO_PIN_6 | GPIO_PIN_7,
                                     GPIO_PIN_4 | GPIO_PIN_5,
                                     GPIO_PIN_4 | GPIO_PIN_5,
                                     GPIO_PIN_0 | GPIO_PIN_1 };
static const u32 uart_gpiofunc[] = { GPIO_PA0_U0RX, GPIO_PA1_U0TX,
                                    GPIO_PC4_U1RX, GPIO_PC5_U1TX,
                                    GPIO_PD6_U2RX, GPIO_PD7_U2TX,
                                    GPIO_PC6_U3RX, GPIO_PC7_U3TX,
                                    GPIO_PC4_U4RX, GPIO_PC5_U4TX,
                                    GPIO_PE4_U5RX, GPIO_PE5_U5TX,
                                    GPIO_PE0_U7RX, GPIO_PE1_U7TX };

static void uarts_init()
{
    unsigned i;
    for (i = 0; i < NUM_UART; i++ )
    {
        MAP_SysCtlPeripheralEnable(uart_sysctl[ i ]);
    }
}

u32 platform_uart_setup( unsigned id, u32 baud, int databits, int parity, int stopbits )
{
    u32 config;

    if (id < NUM_UART )
    {
        MAP_GPIOPinConfigure( uart_gpiofunc[ id << 1 ] );
        MAP_GPIOPinConfigure( uart_gpiofunc[ (id << 1) + 1 ] );
        MAP_GPIOPinTypeUART( uart_gpio_base[ id ], uart_gpio_pins[ id ]);

        switch( databits )
        {
            case 5:
                config = UART_CONFIG_WLEN_5;
                break;
            case 6:
                config = UART_CONFIG_WLEN_6;
                break;
            case 7:
                config = UART_CONFIG_WLEN_7;
                break;
            default:
                config = UART_CONFIG_WLEN_8;
                break;
        }

        config |= ( stopbits == PLATFORM_UART_STOPBITS_1 ) ? UART_CONFIG_STOP_ONE : UART_CONFIG_STOP_TWO;

        if ( parity == PLATFORM_UART_PARITY_EVEN )
        {
            config |= UART_CONFIG_PAR_EVEN;
        }
        else if ( parity == PLATFORM_UART_PARITY_ODD )
        {
            config |= UART_CONFIG_PAR_ODD;
        }
        else if ( parity == PLATFORM_UART_PARITY_MARK )
        {
            config |= UART_CONFIG_PAR_ONE;
        }
        else if ( parity == PLATFORM_UART_PARITY_SPACE )
        {
            config |= UART_CONFIG_PAR_ZERO;
        }

        MAP_UARTConfigSetExpClk( uart_base[ id ], MAP_SysCtlClockGet(), baud, config );
        MAP_UARTConfigGetExpClk( uart_base[ id ], MAP_SysCtlClockGet(), &baud, &config );

        MAP_UARTEnable( uart_base[ id ] );
    }

    return baud;
}

void platform_s_uart_send( unsigned id, u8 data )
{
#ifdef BUILD_USB_CDC
    if ( id == CDC_UART_ID )
    {
        USBBufferWrite( &g_sTxBuffer, &data, 1 );
    }
    else
#endif
    {
        MAP_UARTCharPut( uart_base[ id ], data );
    }
}

#ifdef BUILD_USB_CDC
static int cdc_uart_recv( timer_data_type timeout )
{
    unsigned char data;
    unsigned long read;

    do
    {
        read = USBBufferRead( &g_sRxBuffer, &data, 1 );
    }
    while( read == 0 && timeout != 0 );

    if( read == 0 )
        return -1;
    else
        return data;
}
#endif

int platform_s_uart_recv( unsigned id, timer_data_type timeout )
{
    u32 base = uart_base[ id ];

#ifdef BUILD_USB_CDC
    if( id == CDC_UART_ID )
        return cdc_uart_recv( timeout );
#endif

    if( timeout == 0 )
        return MAP_UARTCharGetNonBlocking( base );

    return MAP_UARTCharGet( base );
}

int platform_s_uart_set_flow_control( unsigned id, int type )
{
    return PLATFORM_ERR;
}

const u32 timer_base[] = { TIMER0_BASE, TIMER1_BASE, TIMER2_BASE, TIMER3_BASE, TIMER4_BASE, TIMER5_BASE };
static const u32 timer_sysctl[] = { SYSCTL_PERIPH_TIMER0, SYSCTL_PERIPH_TIMER1, SYSCTL_PERIPH_TIMER2, SYSCTL_PERIPH_TIMER3, SYSCTL_PERIPH_TIMER4, SYSCTL_PERIPH_TIMER5 };

// TODO: Might not be correct configuration
static void timers_init()
{
    unsigned i;

    for( i = 0; i < NUM_TIMER; i++ )
    {
        MAP_SysCtlPeripheralEnable( timer_sysctl[ i ] );
        MAP_TimerConfigure( timer_base[ i ], TIMER_CFG_ONE_SHOT );
        MAP_TimerEnable( timer_base[ i ], TIMER_A);
    }
}

// TODO: Why was it done this way?
void platform_s_timer_delay( unsigned id, timer_data_type delay_us )
{
    timer_data_type final;
    u32 base = timer_base[ id ];

    final = 0xFFFFFFFF - ( ( ( u64 )delay_us * MAP_SysCtlClockGet() ) / 1000000 );
    MAP_TimerLoadSet( base, TIMER_A, 0xFFFFFFFF );
    while( MAP_TimerValueGet( base, TIMER_A ) > final );
}

timer_data_type platform_s_timer_op( unsigned id, int op, timer_data_type data )
{
    (void) data;

    u32 res = 0;
    u32 base = timer_base[ id ];
    u8 prescale = 0;

    switch( op )
    {
        case PLATFORM_TIMER_OP_START:
            res = 0xFFFFFFFF;
            MAP_TimerControlTrigger( base, TIMER_A, false );
            MAP_TimerLoadSet( base, TIMER_A, 0xFFFFFFFF );
            break;

        case PLATFORM_TIMER_OP_READ:
            res = MAP_TimerValueGet( base, TIMER_A );
            break;

        case PLATFORM_TIMER_OP_SET_CLOCK:
            res = MAP_SysCtlClockGet();
            prescale = res / data > 0xFF ? 0xFF : ( res / data );
            MAP_TimerPrescaleSet( base, TIMER_A, prescale );
            res /= prescale + 1;
            break;

        case PLATFORM_TIMER_OP_GET_CLOCK:
            res = MAP_SysCtlClockGet();
            res /= MAP_TimerPrescaleGet( base, TIMER_A ) + 1;
            break;

        case PLATFORM_TIMER_OP_GET_MAX_CNT:
            res = 0xFFFFFFFF;
            break;
    }

    return res;
}

u64 platform_timer_sys_raw_read()
{
    return MAP_SysTickPeriodGet() - 1 - MAP_SysTickValueGet();
}

void platform_timer_sys_disable_int()
{
    MAP_SysTickIntDisable();
}

void platform_timer_sys_enable_int()
{
    MAP_SysTickIntEnable();
}

timer_data_type platform_timer_read_sys()
{
    return cmn_systimer_get();
}

// TODO: Does this do anything but disable the timer and clear the interrupt?
u8 tm4c_timer_int_periodic_flag[ NUM_TIMER ];
int platform_s_timer_set_match_int( unsigned id, timer_data_type period_us, int type )
{
    u32 base = timer_base[ id ];
    u64 final;

    if( period_us == 0 )
    {
        MAP_TimerDisable( base, TIMER_A );
        MAP_TimerIntDisable( base, TIMER_TIMA_TIMEOUT );
        MAP_TimerIntClear( base, TIMER_TIMA_TIMEOUT );
        MAP_TimerLoadSet( base, TIMER_A, 0xFFFFFFFF );
        MAP_TimerEnable( base, TIMER_A );
        return PLATFORM_TIMER_INT_OK;
    }

    final = ( ( u64 )period_us * MAP_SysCtlClockGet() ) / 1000000;

    if( final == 0 )
        return PLATFORM_TIMER_INT_TOO_SHORT;
    if( final > 0xFFFFFFFFULL )
        return PLATFORM_TIMER_INT_TOO_LONG;
    tm4c_timer_int_periodic_flag[ id ] = type;

    MAP_TimerDisable( base, TIMER_A );
    MAP_TimerIntClear( base, TIMER_TIMA_TIMEOUT );
    MAP_TimerLoadSet( base, TIMER_A, ( u32 )final - 1);

    return PLATFORM_TIMER_INT_OK;
}

// TODO: Skipping PWMs for now.

// TODO: Skipping ADCs fow now

void SysTickIntHandler()
{
    cmn_virtual_timer_cb();

    cmn_systimer_periodic();
}

#if defined( BUILD_USB_CDC )

static void usb_init()
{
    USBBufferInit( &g_sTxBuffer );
    USBBufferInit( &g_sRxBuffer );

    USBDCDCInit( 0, &g_sCDCDevice );
}

// TODO: Add more usb functions

#endif

#ifdef BUILD_WOFS
u32 platform_s_flash_write( const void *from, u32 toaddr, u32 size )
{
    return MAP_FlashProgram( ( unsigned long * ) from, toaddr, size );
}

int platform_flash_erase_sector( u32 sector_id )
{
    return MAP_FlashErase( sector_id * INTERNAL_FLASH_SECTOR_SIZE ) == 0 ? PLATFORM_OK : PLATFORM_ERR;
}

#endif
