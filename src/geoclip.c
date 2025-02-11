#include "zeebo.h"

bool geoclip_text(app_t *const self, int16_t *const x, int16_t *const y, int16_t *const pos)
{
    if (0 > *x || *x > self->ctx.width) {
        return false;
    }

    if (0 > *y || *y > self->ctx.height) {
        return false;
    }

    return true;
}
