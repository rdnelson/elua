#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "lua.h"
#include "platform.h"

#define BUTTON_LP1 0x01
#define BUTTON_LP2 0x02
#define BUTTON_PROXIMA_SW1 0x03
#define BUTTON_PROXIMA_SW2 0x04
#define BUTTON_PROXIMA_SW3 0x05
#define BUTTON_PROXIMA_SW4 0x06

u8 proxima_buttons_wait( lua_State* L );
u8 proxima_buttons_check( lua_State* L );

#endif

