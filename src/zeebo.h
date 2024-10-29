#include <signal.h>
#include <stdlib.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#define delay(ms) Sleep(ms)
#else
#include <termios.h>
#include <unistd.h>
#define delay(ms) usleep((ms) * 1000)
#endif

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

void tui_init();
void tui_clear();
