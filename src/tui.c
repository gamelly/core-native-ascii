#include "zeebo.h"

static void tui_update_screen(app_t *const self)
{
    static struct winsize ws;
    if (self->old.width == 0) {
        self->old.width = 80;
        self->old.height = 24;
    } else {
        self->old.width = self->ctx.width;
        self->old.height = self->ctx.height;
    }
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1) {
        self->ctx.width  = ws.ws_col;
        self->ctx.height = ws.ws_row;
    }
    else {
        self->ctx.width  = 80;
        self->ctx.height = 24;
    }
    if (self->ctx.width < 80 || self->ctx.height < 24) {
        concat(self, err, "error: minimum terminal size 80x24, actual is %dx%d\n", self->ctx.width, self->ctx.height);
    }
}

bool tui_update(app_t *const self)
{
    if (self->err.len) {
        return false;
    }

    tui_update_screen(self);
    if (self->err.len) {
        return false;
    }

    if (self->L && !self->started.tty) {
        self->started.tty = true;
        tui_termios_start();
    }

    return true;
}

void tui_delete(app_t *const self)
{
    if (self->started.tty) {
        tui_termios_exit();
    }
}
