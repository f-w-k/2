#if !CODE
typedef enum OBJTYPE_game {
    /// --
    OBJTYPE_camera = 11,
    OBJTYPE_node   = 12,
    OBJTYPE_scene  = 13,
} OBJTYPE_game;
#endif

#include "game_types.h" // 1st
#include "game_math.h" // 2nd
#include "game_app.h" // 3rd
#include "game_camera.h"
#include "game_collide.h"
#include "game_ease.h"
#include "game_gamepad.h"
#include "game_image.h"
#include "game_input.h"
#include "game_keyboard.h"
#include "game_level.h"
#include "game_mouse.h"
#include "game_profiler.h"
#include "game_script.h"
#include "game_text.h"
#include "game_ui.h"
#include "game_wget.h"

#if CODE

void initG() {
    profiler_init();
    //scene_init();
    //network_init();
    //audio_init();
    //midi_init();
}
void drawG() {
//ifdef(DEV,
    if( debug ) {
        float fps = igGetIO()->Framerate;
        igText("FPS: %.2f (%.2gms)", fps, fps ? 1000.0f / fps : 0.0f);

        ui_profiler();
    }
//)
}
void tickG() {
    ui_tick();
    mouse_tick();
    input_tick();
}

AUTORUN {
    events("init,draw,tick",initG,drawG,tickG);
}

#endif
