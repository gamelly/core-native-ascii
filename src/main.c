#include "engine/bytecode.h"
#include "game/bytecode.h"
#include "zeebo.h"

int
main() {
    uint16_t width, height;
    tui_init();

    if (tui_get_width(&width, &height) == -1) {
        tui_error("terminal error:\n", 16, "screen size");
    }

    lua_State *L = luaL_newstate();

    luaL_openlibs(L);
    native_draw_install(L);

    luaL_loadbuffer(L, (const char *)engine_bytecode_lua, engine_bytecode_lua_len, "engine");
    lua_pcall(L, 0, 0, 0);

    lua_getglobal(L, "native_callback_init");
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);

    luaL_loadbuffer(L, (const char *)game_bytecode_lua, game_bytecode_lua_len, "game");
    lua_pcall(L, 0, 1, 0);
    lua_pcall(L, 3, 0, 0);
    tui_lua_check_error(L);

    while (1) {
        native_keys_update(L);
        lua_getglobal(L, "native_callback_loop");
        lua_pushnumber(L, 16);
        lua_pcall(L, 1, 0, 0);
        tui_lua_check_error(L);

        lua_getglobal(L, "native_callback_draw");
        lua_pcall(L, 0, 0, 0);
        tui_lua_check_error(L);

        delay(16);
    }
    return 0;
}
