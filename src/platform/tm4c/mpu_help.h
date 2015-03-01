#ifndef __MPU_HELP_H__
#define __MPU_HELP_H__

void proxima_help_reg_write_byte(unsigned char addr, unsigned char reg, unsigned char data);
int proxima_help_reg_read_byte(unsigned char addr, unsigned char reg, unsigned char* data);
int proxima_help_reg_read_word(unsigned char addr, unsigned char reg, short* data);

#endif
