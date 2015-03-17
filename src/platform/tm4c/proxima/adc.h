#ifndef __ADC_H__
#define __ADC_H__

#include "lua.h"

int proxima_adc_geta( lua_State* L );
int proxima_adc_getb( lua_State* L );
int proxima_adc_getright( lua_State* L );
int proxima_adc_getleft( lua_State* L );
int proxima_adc_getcenter( lua_State* L );
int proxima_adc_getknob( lua_State* L );

#endif
