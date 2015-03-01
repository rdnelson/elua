#ifndef __DAC_H__
#define __DAC_H__

#include "lua.h"

int proxima_dac_init( lua_State* L );
int proxima_dac_seta( lua_State* L );
int proxima_dac_setb( lua_State* L );
int proxima_dac_setboth( lua_State* L );

#endif
