#ifndef __GYRO_H__
#define __GYRO_H__

#include "lua.h"

int proxima_gyro_init( lua_State* L );
int proxima_gyro_get_x( lua_State* L );
int proxima_gyro_get_y( lua_State* L );
int proxima_gyro_get_z( lua_State* L );

#endif
