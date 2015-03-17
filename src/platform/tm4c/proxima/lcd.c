#include "lcd.h"

#include "spi_help.h"
#include "platform.h"

#define LCD_SPI 2
#define LCD_A0_PORT 2
#define LCD_A0_PIN 0x80
#define LCD_CS 3

static void send_cmd(u8 cmd)
{
    platform_pio_op( LCD_A0_PORT, LCD_A0_PIN, PLATFORM_IO_PIN_CLEAR );
    select_chip(LCD_CS);

    platform_spi_send_recv( LCD_SPI, cmd );

    select_chip(0);
}

static void send_data(u8 cmd)
{
    platform_pio_op( LCD_A0_PORT, LCD_A0_PIN, PLATFORM_IO_PIN_SET );
    select_chip(LCD_CS);

    platform_spi_send_recv( LCD_SPI, cmd );

    select_chip(0);
}

int proxima_lcd_init( lua_State* L )
{
    setup_select();

    platform_pio_op( LCD_A0_PORT, LCD_A0_PIN, PLATFORM_IO_PIN_DIR_OUTPUT );

    platform_spi_setup( LCD_SPI, PLATFORM_SPI_MASTER, 10000, 0, 0, 8 );

    // Initialization sequence
    send_cmd(0xA0);
    send_cmd(0xAE);
    send_cmd(0xC0);
    send_cmd(0xA2);
    send_cmd(0x2F);
    send_cmd(0x26);
    send_cmd(0x81);
    send_cmd(0x2F);

    return 0;
}

int proxima_lcd_pic( lua_State* L )
{
    unsigned int i,j;
    unsigned char page = 0xB0;			//Page Address + 0xB0
    char* lcd_string = logo;
    send_cmd(0xAE);					//Display OFF
    send_cmd(0x40);					//Display start address + 0x40
    for(i=0;i<4;i++){				//32pixel display / 8 pixels per page = 4 pages
        send_cmd(page);				//send page address
        send_cmd(0x10);				//column address upper 4 bits + 0x10
        send_cmd(0x00);				//column address lower 4 bits + 0x00
        for(j=0;j<128;j++){			//128 columns wide
            send_data(*lcd_string);		//send picture data
            lcd_string++;			//point to next picture data
        }
        page++;					//after 128 columns, go to next page
    }
    send_cmd(0xAF);

    return 0;
}
