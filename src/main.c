#include "zeebo.h"

int main() {
    tui_init();

    lua_State *L = luaL_newstate();

    while(1) {
        tui_clear();
        delay(16);
    }
    return 0;
}
