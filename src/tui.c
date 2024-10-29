#include "zeebo.h"

void tui_clear()
{
    static char cmd[] = "\033[3J\033[H\033[2J";
#if _WIN32
#error implement this!
#else
    write(1, cmd, sizeof(cmd) - 1);
#endif
}

void tui_signal(int sig)
{
    tui_clear();
    exit(sig == SIGTERM);
}

void tui_init()
{
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
}
