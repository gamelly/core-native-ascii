#include "zeebo.h"

void tui_draw_rect(app_t *const self, int16_t x, int16_t y, int16_t w, int16_t h)
{
    concat(self, out, "\x1b[%d;%dH#%d %d %d %d", y, x, x, y, w, h);

    //for (uint16_t row = 0; row < h; row++) {
    //    concat(self, out,  "\x1b[%d;%dH", row + y, x);
    //    memset(&self->out.msg[self->out.len], '#', w);
    //}
}
