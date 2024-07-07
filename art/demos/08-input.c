#include "engine.h"

int main() {
    app_create(0.75, 0);

    while (app_swap()) {
        if( ui_panel("Mouse", 0) ) {
            ui_mouse();
            ui_panel_end();
        }
        if( ui_panel("Keyboard", 0) ) {
            ui_keyboard();
            ui_panel_end();
        }
        if( ui_panel("Gamepads", 0) ) {
            ui_gamepads();
            ui_panel_end();
        }
    }
}
