#include "engine.h"

int main() {
    // create app (80% sized, MSAA x4 flag)
    app_create(80, APP_MSAA4);

    // animated models loading
    model_t girl = model("kgirls01.fbx", MODEL_NO_PBR);
    compose44( girl.pivot, vec3(0,0,0), eulerq(vec3(0,90,-90)), vec3(2,2,2)); // position, rotation, scale

    // camera
    camera_t cam = camera();

    // demo loop
    while (app_swap())
    {
        // fps camera
        bool active = ui_active() || ui_hovered() || gizmo_active() ? false : input(MOUSE_L) || input(MOUSE_M) || input(MOUSE_R);
        if( active ) cam.speed = clampf(cam.speed + input_diff(MOUSE_W) / 10, 0.05f, 5.0f);
        vec2 mouse = scale2(vec2(input_diff(MOUSE_X), -input_diff(MOUSE_Y)), 0.2f * active);
        vec3 wasdecq = scale3(vec3(input(KEY_D)-input(KEY_A),input(KEY_E)-(input(KEY_C)||input(KEY_Q)),input(KEY_W)-input(KEY_S)), cam.speed);
        camera_moveby(&cam, scale3(wasdecq, window_delta() * 60));
        camera_fps(&cam, mouse.x,mouse.y);
        window_cursor( !active );

        // debug draw
        ddraw_ground(0);

        // animate girl
        float delta = window_delta() * 30; // 30fps anim
        girl.curframe = model_animate(girl, girl.curframe + delta);

        // draw girl
        model_render(girl, cam.proj, cam.view, girl.pivot);
    }
}
