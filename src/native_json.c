#include "zeebo.h"

extern int luaopen_cjson(lua_State *L);

void native_json_install(lua_State* L)
{
    luaopen_cjson(L);
    int cjson = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, cjson);
    lua_pushstring(L, "encode_empty_table_as_object");
    lua_gettable(L, -2);
    lua_pushboolean(L, false);
    lua_pcall(L, 1, 0, 0);

    lua_rawgeti(L, LUA_REGISTRYINDEX, cjson);
    lua_pushstring(L, "encode_skip_unsupported_value_types");
    lua_gettable(L, -2);
    lua_pushboolean(L, true);
    lua_pcall(L, 1, 0, 0);

    lua_rawgeti(L, LUA_REGISTRYINDEX, cjson);
    lua_pushstring(L, "encode");
    lua_gettable(L, -2);
    lua_setglobal(L, "native_json_encode");

    lua_rawgeti(L, LUA_REGISTRYINDEX, cjson);
    lua_pushstring(L, "decode");
    lua_gettable(L, -2);
    lua_setglobal(L, "native_json_decode");

    lua_settop(L, 0);
}
