#ifndef __SPI_HELP_H__
#define __SPI_HELP_H__

#define SPI_BASE 2

#define CS_NONE 0
#define CS_DAC 2
#define CS_NET 5

#define deselect_chip(a) select_chip(0)
#define setup_sdcard(a) setup_select()

#define send_recv(d) platform_spi_send_recv(SPI_BASE, d)

void setup_select();
void select_chip(int n);

#endif
