#include "zeebo.h"

void tui_draw_rect(app_t *const self, int16_t x, int16_t y, int16_t w, int16_t h)
{
    x = MAX(1, x);
    y = MAX(1, y);
    w = MAX(1, w);
    h = MAX(1, h);

    for (uint16_t row = 0; row < h; row++) {
        concat(self, out,  "\x1b[%d;%dH", row + y, x);
        memset(&self->out.msg[self->out.len], '#', w);
        self->out.len += w;
    }
}
