#include "zeebo.h"

static uint8_t queue_command[2048];
static int16_t queue_param[8192];

static uint16_t index_cmd = 0;
static uint16_t index_param = 0;

void tui_queue_push(uint8_t cmd, int16_t a, int16_t b, int16_t c, int16_t d)
{
    if (index_cmd < sizeof(queue_command)) {
        queue_command[index_cmd++] = cmd;
        queue_param[index_param++] = a;
        queue_param[index_param++] = b;
        queue_param[index_param++] = c;
        queue_param[index_param++] = d;
    }
}


void tui_queue_burn(app_t *const self)
{
    uint16_t index = 0;
    uint16_t param = 0;

    static const cmd_t commands[] = {
        NULL,
        NULL,
        tui_draw_rect,
        NULL
    };

    while (index < index_cmd) {
        uint8_t cmd = queue_command[index] - 48;
        if (cmd < sizeof(commands) / sizeof(commands[0]) && commands[cmd] != NULL) {
            uint8_t param_1 = queue_param[param++];
            uint8_t param_2 = queue_param[param++];
            uint8_t param_3 = queue_param[param++];
            uint8_t param_4 = queue_param[param++];
            commands[cmd](self, param_1, param_2, param_3, param_4);
        }
        index++;
    }

    index_cmd = 0;
    index_param = 0;
}
