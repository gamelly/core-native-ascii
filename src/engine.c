#include "zeebo.h"
#include "engine/bytecode.h"
#include "game/bytecode.h"

#ifndef FPS_DELTA
#define FPS_DELTA (16u)
#endif

static int native_callback_loop;
static int native_callback_draw;
static int native_callback_keyboard;

void engine_init(app_t *const self)
{
    do {
        self->L = luaL_newstate();

        luaL_openlibs(self->L);
        native_draw_install(self->L);
        native_text_install(self->L);

        luaL_loadbuffer(self->L, (const char *)engine_bytecode_lua, engine_bytecode_lua_len, "engine");
        lua_pcall(self->L, 0, 0, 0);

        lua_getglobal(self->L, "native_callback_init");
        lua_pushnumber(self->L, self->ctx.width);
        lua_pushnumber(self->L, self->ctx.height);

        luaL_loadbuffer(self->L, (const char *)game_bytecode_lua, game_bytecode_lua_len, "game");
        lua_pcall(self->L, 0, 1, 0);
        lua_pcall(self->L, 3, 0, 0);

        lua_getglobal(self->L, "native_callback_loop");
        native_callback_loop = luaL_ref(self->L, LUA_REGISTRYINDEX);

        lua_getglobal(self->L, "native_callback_draw");
        native_callback_draw = luaL_ref(self->L, LUA_REGISTRYINDEX);
        
        lua_getglobal(self->L, "native_callback_keyboard");
        native_callback_keyboard = luaL_ref(self->L, LUA_REGISTRYINDEX);
    }
    while(0);
}

void engine_update(app_t *const self)
{
    self->out.len = 0;
    concat(self, out, "\x1B[3J\x1B[H\x1B[2J");
    native_keys_update(self->L, native_callback_keyboard);
    native_loop_update(self->L, native_callback_loop, FPS_DELTA);
    native_draw_update(self->L, native_callback_draw);
    tui_queue_burn(self);
    concat(self, out, "\x1b[0;0H");
}
