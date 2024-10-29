#include "color/rgb_to_xterm256.h"
#include "font/gly_type_render.h"
#include "zeebo.h"

union color_u {
    uint32_t raw;
    struct {
        uint8_t a;
        uint8_t b;
        uint8_t g;
        uint8_t r;
    } c;
};

uint8_t current_color_tint;
uint8_t current_color_clear;
uint8_t current_font_size = 1;

static char terminal_main_pos = 0;
static char terminal_main_buffer[40960];

static int
native_draw_start(lua_State *L) {
    terminal_main_pos = 0;
    terminal_main_pos += sprintf(&terminal_main_buffer[terminal_main_pos], "\x1b[48;5;%dm", 0);
    return 0;
}

static int
native_draw_flush(lua_State *L) {
    tui_clear();
    terminal_main_pos += sprintf(&terminal_main_buffer[terminal_main_pos], "\x1b[?25l");
    write(1, terminal_main_buffer, terminal_main_pos);
    return 0;
}

/**
 * @short @c std.draw.clear
 * @param[in] color @c int
 */
static int
native_draw_clear(lua_State *L) {
    tui_lua_check_top(L, 1);
    union color_u color = { .raw = luaL_checkinteger(L, 1) };
    current_color_clear = rgb_to_xterm256(color.c.r, color.c.g, color.c.b);
    lua_pop(L, 1);
    return 0;
}

/**
 * @short @c std.draw.color
 * @param[in] color @c int
 */
static int
native_draw_color(lua_State *L) {
    tui_lua_check_top(L, 1);
    union color_u color = { .raw = luaL_checkinteger(L, 1) };
    current_color_tint = rgb_to_xterm256(color.c.r, color.c.g, color.c.b);
    lua_pop(L, 1);
    return 0;
}

/**
 * @short @c std.draw.rect
 * @param[in] mode @c int 0 fill, 1 frame
 * @param[in] x @c double pos X
 * @param[in] y @c double pos Y
 * @param[in] w @c double width
 * @param[in] h @c double height
 */
static int
native_draw_rect(lua_State *L) {
    tui_lua_check_top(L, 5);

    int mode = luaL_checkinteger(L, 1);     
    uint16_t x = (uint16_t) luaL_checknumber(L, 2);
    uint16_t y = (uint16_t) luaL_checknumber(L, 3); 
    uint16_t w = (uint16_t) luaL_checknumber(L, 4); 
    uint16_t h = (uint16_t) luaL_checknumber(L, 5);

    for (uint16_t row = 0; row < h; row++) {
        terminal_main_pos += sprintf(&terminal_main_buffer[terminal_main_pos], "\x1b[%d;%dH\x1b[48;5;%dm", 
                             row + y, x, current_color_tint);
        memset(&terminal_main_buffer[terminal_main_pos],' ', 1);
        terminal_main_pos += 1;
    }
    terminal_main_pos += sprintf(&terminal_main_buffer[terminal_main_pos], "\x1b[48;5;%dm", current_color_clear);
    
    return 0;
}


/**
 * @short @c std.draw.line
 * @param[in] x1 @c double
 * @param[in] y1 @c double
 * @param[in] x2 @c double
 * @param[in] y2 @c double
 */
static int
native_draw_line(lua_State *L) {
    tui_lua_check_top(L, 4);
    return 0;
}

/**
 * @short @c std.draw.font
 * @param[in] font @c string
 * @param[in] size @c double
 */
static int
native_draw_font(lua_State *L) {
    uint8_t top = lua_gettop(L);
    if (top == 1) {
        current_font_size = (uint8_t)luaL_checknumber(L, 1);
    } else {
        current_font_size = (uint8_t)luaL_checknumber(L, 2);
    }
    if (current_font_size == 2 || current_font_size == 0 || current_font_size > 24) {
        current_font_size = 1;
    }
    lua_pop(L, top);
    return 0;
}

/**
 * @short @c std.draw.text
 * @param[in] x @c double
 * @param[in] y @c double
 * @param[in] text @c string
 */
static int
native_draw_text(lua_State *L) {
    const char* text = NULL;
    if (lua_gettop(L) == 3) {
        uint16_t x = (uint16_t) luaL_checknumber(L, 1);
        uint16_t y = (uint16_t) luaL_checknumber(L, 2);
        text = luaL_checkstring(L, 3);
        terminal_main_pos += sprintf(&terminal_main_buffer[terminal_main_pos], "\x1b[%d;%dH\x1b[38;5;%dm%s", 
        y, x, current_color_tint, text);

    }
    return 0;
}

/**
 * @}
 */

void
native_draw_install(lua_State *L) {
    int i = 0;
    static const luaL_Reg lib[] = { { "native_draw_start", native_draw_start }, { "native_draw_flush", native_draw_flush },
                                    { "native_draw_clear", native_draw_clear }, { "native_draw_color", native_draw_color },
                                    { "native_draw_rect", native_draw_rect },   { "native_draw_line", native_draw_line },
                                    { "native_draw_font", native_draw_font },   { "native_draw_text", native_draw_text } };

    while (i < sizeof(lib) / sizeof(luaL_Reg)) {
        lua_pushcfunction(L, lib[i].func);
        lua_setglobal(L, lib[i].name);
        i = i + 1;
    }

    lua_newtable(L);
    lua_pushstring(L, "repeats");
    lua_newtable(L);
    lua_pushboolean(L, 1);
    lua_seti(L, -2, 1);
    lua_pushboolean(L, 1);
    lua_seti(L, -2, 2);
    lua_settable(L, -3);
    lua_pushstring(L, "line");
    lua_pushcfunction(L, native_draw_line);
    lua_settable(L, -3);
    lua_setglobal(L, "native_dict_poly");
}
