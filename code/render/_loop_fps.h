
void window_frame_swap() {
    // glFinish();
#if !is(ems)
    void window_vsync(float);
    window_vsync(hz);
#endif
    glfwSwapBuffers(window);
    // emscripten_webgl_commit_frame();

    static int delay = 0; do_once delay = optioni("--delay", 0);
    if( delay &&
#if HAS_COOK
        !COOK_ON_DEMAND &&
#endif
        cook_progress() >= 100
    ) sleep_ms( delay );
}
