#include "zeebo.h"

int
main() {
    static app_t self;
    tui_update(&self);
    engine_init(&self);

    while(tui_update(&self)) {
        engine_update(&self);
        if(self.out.len) {
            print(self.out.msg, self.out.len);
        }
        delay(100);
    }

    tui_delete(&self);
    if (self.err.len) {
        print("\n", 1);
        print(self.err.msg, self.err.len);
    }

    return !!self.err.len;
}
