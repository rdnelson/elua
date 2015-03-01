#ifndef __LED_H__
#define __LED_H__

#include "lua.h"

int proxima_led_setrgb( lua_State* L );
int proxima_led_setred( lua_State* L );
int proxima_led_setgreen( lua_State* L );
int proxima_led_setblue( lua_State* L );

#endif
