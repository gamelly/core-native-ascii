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

void tui_draw_text(app_t *const self, int16_t x, int16_t y, int16_t text_id, int16_t text_size)
{
    const char* text = tui_queue_get_text(text_id);
    int16_t pos = 0;

    if (geoclip_text(self, &x, &y, &pos)) {
        concat(self, out,  "\x1b[%d;%dH\x1B[%dm%s\x1B[0m", y, x, text_size == 2, &text[pos]);
    }
}
