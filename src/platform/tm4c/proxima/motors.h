#ifndef __MOTORS_H__
#define __MOTORS_H__

#include "lua.h"

int proxima_motors_forward( lua_State* L );
int proxima_motors_reverse( lua_State* L );
int proxima_motors_right( lua_State* L );
int proxima_motors_left( lua_State* L );
int proxima_motors_stop( lua_State* L );
int proxima_motors_get_status( lua_State* L );

#endif
