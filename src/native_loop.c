#include "zeebo.h"

int native_loop_update(lua_State* L, int callback, uint8_t dt)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, callback);
    lua_pushnumber(L, dt);
    return lua_pcall(L, 1, 0, 0);
}
