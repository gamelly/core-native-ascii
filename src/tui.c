#include "zeebo.h"

void
tui_exit() {
#ifndef _WIN32
    static struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
#endif
}

/**
 * @par[out] width number of cols
 * @par[out] height number of rows
 * @return screen size status
 * @retval 0 ok
 * @retval 1 changed
 * @retval -1 invalid size
 */
int
tui_get_width(uint16_t *width, uint16_t *height) {
    static struct winsize ws;
    static uint16_t old_width = 80;
    static uint16_t old_height = 24;

    *width = old_width;
    *height = old_height;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1) {
        *width = ws.ws_col;
        *height = ws.ws_row;
    }

    if (*width < 80 || *height < 24) {
        return -1;
    }

    if (old_width != *width || old_height != *height) {
        return 1;
    }

    return 0;
}

void
tui_clear() {
    static char txt[] = "\033[3J\033[H\033[2J";
#if _WIN32
#error implement this!
#else
    write(1, txt, sizeof(txt) - 1);
#endif
}

void
tui_signal(int sig) {
    tui_clear();
    tui_exit();
    exit(sig == SIGTERM);
}

void
tui_error(const char *txt1, const size_t len1, const char *txt2) {
    tui_clear();
    tui_exit();
    write(2, txt1, len1);
    if (txt2) {
        write(2, txt2, strlen(txt2));
    }
    write(2, "\n", 1);
    exit(1);
}

void
tui_lua_check_top_fname(lua_State *L, uint8_t size, const char *function_name) {
    static const char txt[] = "lua stack error:\n";
    if (lua_gettop(L) != size) {
        tui_error(txt, sizeof(txt), function_name);
    }
}

void
tui_lua_check_error(lua_State *L) {
    static const char txt[] = "lua runtime error:\n";
    if (lua_gettop(L) > 0) {
        tui_error(txt, sizeof(txt), luaL_checkstring(L, 1));
    }
}

void
tui_init() {
    static char txt[] = "\x1b[0m\x1b[H";
#ifdef SIGINT
    signal(SIGINT, tui_signal);
#endif
#ifdef SIGTERM
    signal(SIGTERM, tui_signal);
#endif
#ifdef SIGKILL
    signal(SIGKILL, tui_signal);
#endif
#ifdef SIGQUIT
    signal(SIGQUIT, tui_signal);
#endif
#ifdef SIGHUP
    signal(SIGHUP, tui_signal);
#endif

#ifndef _WIN32
    static struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    write(STDOUT_FILENO, txt, sizeof(txt));
    write(STDERR_FILENO, txt, sizeof(txt));
#endif
}
