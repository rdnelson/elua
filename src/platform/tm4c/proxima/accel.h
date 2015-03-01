#ifndef __ACCEL_H__
#define __ACCEL_H__

#include "lua.h"

int proxima_accel_init( lua_State* L );
int proxima_accel_get_id( lua_State* L );
int proxima_accel_get_x( lua_State* L );
int proxima_accel_get_y( lua_State* L );
int proxima_accel_get_z( lua_State* L );
int proxima_accel_get_temp( lua_State* L );

#endif
