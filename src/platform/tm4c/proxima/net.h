#ifndef __NET_H__
#define __NET_H__

#include "lua.h"

#define NET_CMD_OPEN     0x40
#define NET_CMD_CLOSE    0x41
#define NET_CMD_COMMIT   0x42
#define NET_CMD_CLEARKV  0x43
#define NET_CMD_DROP     0x44
#define NET_CMD_FLUSH    0x45

#define NET_REG_HOST   0x80
#define NET_REG_DATA   0x81
#define NET_REG_KEY    0x82
#define NET_REG_VALUE  0x83
#define NET_REG_STATUS 0x84

#define NET_STATUS_OK      0x00
#define NET_STATUS_UNKNOWN 0x80

#define REG_READ(a) (0x40 | (a))
#define REG_WRITE(a) (a)

int proxima_net_open( lua_State* L );
int proxima_net_close( lua_State* L );
int proxima_net_commit( lua_State* L );
int proxima_net_flush( lua_State* L );
int proxima_net_clearkv( lua_State* L );
int proxima_net_set_host( lua_State* L );
int proxima_net_write( lua_State* L );
int proxima_net_read( lua_State* L );
int proxima_net_set_key( lua_State* L );
int proxima_net_set_value( lua_State* L );
int proxima_net_get_status( lua_State* L );

#endif
