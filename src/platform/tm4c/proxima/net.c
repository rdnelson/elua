#include "net.h"

#include "spi_help.h"
#include "platform.h"
#include "lauxlib.h"

#define NOT_INIT 0
#define INIT 1

static u8 net_init = NOT_INIT;
static u8 status = NET_STATUS_UNKNOWN;

void proxima_net_setup()
{
    platform_spi_setup( SPI_BASE, PLATFORM_SPI_MASTER, 20000000, 0, 0, 8);

    setup_select();
    select_chip(0);

    net_init = INIT;
}

void proxima_net_write_reg(u8 reg, const char* msg)
{
    select_chip(CS_NET);

    status = send_recv(REG_WRITE(reg));

    for(; *msg != '\0'; msg++)
    {
        send_recv(*msg);
    }

    select_chip(CS_NONE);
}

void proxima_net_read_reg(u8 reg, luaL_Buffer* b, u32 len )
{
    select_chip(CS_NET);

    status = send_recv(REG_READ(reg));

    for(; len > 0; len--)
    {
         luaL_putchar( b, (char)send_recv(0) );
    }

    select_chip(CS_NONE);
}


void proxima_net_send_cmd(u8 cmd)
{
    select_chip(CS_NET);

    status = send_recv(cmd);

    select_chip(CS_NONE);
}

int proxima_net_open( lua_State* L )
{
    if( net_init == NOT_INIT )
    {
        proxima_net_setup();
    }

    proxima_net_send_cmd(NET_CMD_OPEN);

    return 0;
}

int proxima_net_close( lua_State* L )
{
    if( net_init == NOT_INIT )
    {
        proxima_net_setup();
    }

    proxima_net_send_cmd(NET_CMD_CLOSE);

    return 0;
}

int proxima_net_commit( lua_State* L )
{
    if( net_init == NOT_INIT )
    {
        proxima_net_setup();
    }

    proxima_net_send_cmd(NET_CMD_COMMIT);

    return 0;
}

int proxima_net_flush( lua_State* L )
{
    if( net_init == NOT_INIT )
    {
        proxima_net_setup();
    }

    proxima_net_send_cmd(NET_CMD_FLUSH);

    return 0;
}

int proxima_net_set_host( lua_State* L )
{
    const char* buf;

    if( net_init == NOT_INIT )
    {
        proxima_net_setup();
    }

    buf = luaL_checkstring( L, 1 );

    proxima_net_write_reg( NET_REG_HOST, buf);

    return 0;
}

int proxima_net_write( lua_State* L )
{
    const char* buf;

    if( net_init == NOT_INIT )
    {
        proxima_net_setup();
    }

    buf = luaL_checkstring( L, 1 );

    proxima_net_write_reg( NET_REG_DATA, buf );

    return 0;
}

int proxima_net_read( lua_State* L )
{
    luaL_Buffer b;
    u32 len = luaL_checkinteger( L, 1 );

    if( net_init == NOT_INIT )
    {
        proxima_net_setup();
    }

    luaL_buffinit( L, &b );

    proxima_net_read_reg( NET_REG_DATA, &b, len );

    luaL_pushresult( &b );

    return 1;
}

int proxima_net_set_key( lua_State* L )
{
    const char* buf;

    if( net_init == NOT_INIT )
    {
        proxima_net_setup();
    }

    buf = luaL_checkstring( L, 1 );

    proxima_net_write_reg( NET_REG_KEY, buf );

    return 0;
}

int proxima_net_set_value( lua_State* L )
{
    const char* buf;

    if( net_init == NOT_INIT )
    {
        proxima_net_setup();
    }

    buf = luaL_checkstring( L, 1 );

    proxima_net_write_reg( NET_REG_VALUE, buf );

    return 0;
}

int proxima_net_get_status( lua_State* L )
{
    lua_pushinteger( L, status );

    return 1;
}

