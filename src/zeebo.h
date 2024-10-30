#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#define delay(ms) Sleep(ms)
#else
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define delay(ms) usleep((ms) * 1000)
#endif

#ifdef __GNUC__
#define tui_lua_check_top(a,b) tui_lua_check_top_fname(a,b,__func__)
#else
#define tui_lua_check_top(a,b) tui_lua_check_top_fname(a,b,"function")
#endif

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

void native_keys_update(lua_State* L);

void native_draw_install(lua_State* L);

void tui_init();
void tui_clear();
int tui_get_width(uint16_t* width, uint16_t* height);
void tui_error(const char* txt1, const size_t len1, const char* txt2);
void tui_lua_check_top_fname(lua_State *L, uint8_t size, const char* function_name);
void tui_lua_check_error(lua_State *L);
