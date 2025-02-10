#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

static struct termios old_tio;
static int old_flags;

static void restore_terminal()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    fcntl(STDIN_FILENO, F_SETFL, old_flags);
}

static void handle_signal(int sig)
{
    restore_terminal();
    _exit(sig);
}

void tui_termios_start()
{
    struct termios new_tio;
    
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;

    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    
    old_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    
    fcntl(STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);
    
    atexit(restore_terminal);

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
}

void tui_termios_exit()
{
    restore_terminal();
}
