#if !CODE

typedef void (*event_fn)();

API int  event(const char *name);      // fire registered events for a given category
API void events(const char *fmt, ...); // register comma-separated event functions

#else

array(event_fn) inits = 0;
array(event_fn) ticks = 0;
array(event_fn) draws = 0;
array(event_fn) swaps = 0;
array(event_fn) quits = 0;

int event(const char *name) {
    /**/ if(name[0] == 't') for(int i = 0, e = array_count(ticks); i < e; ++i) ticks[i]();
    else if(name[0] == 'd') for(int i = 0, e = array_count(draws); i < e; ++i) draws[i]();
    else if(name[0] == 's') for(int i = 0, e = array_count(swaps); i < e; ++i) swaps[i]();
    else if(name[0] == 'i') for(int i = 0, e = array_count(inits); i < e; ++i) inits[i]();
    else if(name[0] == 'q') for(int i = 0, e = array_count(quits); i < e; ++i) quits[i]();
    else return 0;
    return 1;
}

void events(const char *fmt, ...) {
    const char *names[32] = {fmt}; // @fixme: 32
    int count = 1; while(*fmt++) if( fmt[-1] == ',' ) names[count++] = fmt;

    va_list list;
    va_start(list, fmt);

    for( int i = 0; i < count; ++i ) {
        event_fn fun = va_arg(list, event_fn);
        /**/ if(names[i][0] == 'i') array_push(inits, fun);
        else if(names[i][0] == 't') array_push(ticks, fun);
        else if(names[i][0] == 'd') array_push(draws, fun);
        else if(names[i][0] == 's') array_push(swaps, fun);
        else if(names[i][0] == 'q') array_push(quits, fun);
        else printf("warning: event not found '%s'\n", names[i]);
    }

    va_end(list);
}

#endif

#if 0

int window_frame_begin() {
    glfwPollEvents();

    // we cannot simply terminate threads on some OSes. also, aborted cook jobs could leave temporary files on disc.
    // so let's try to be polite: we will be disabling any window closing briefly until all cook is either done or canceled.
    static bool has_cook; do_once has_cook = !COOK_ON_DEMAND && have_tools() && cook_jobs();
    if( has_cook ) {
        has_cook = cook_progress() < 100;
        if( glfwWindowShouldClose(g->window) ) cook_cancel();
        glfwSetWindowShouldClose(g->window, GLFW_FALSE);
    }

    if( glfwWindowShouldClose(g->window) ) {
        return 0;
    }

    glNewFrame();

    ui_create();

#if !ENABLE_RETAIL
    bool has_menu = ui_has_menubar();
    bool may_render_debug_panel = 1;

    if( have_tools() ) {
        static int cook_has_progressbar; do_once cook_has_progressbar = !COOK_ON_DEMAND;
        if( cook_has_progressbar) {
            // render profiler, unless we are in the cook progress screen
            static unsigned frames = 0; if(frames <= 0) frames += cook_progress() >= 100;
            may_render_debug_panel = (frames > 0);
        }
    }

    if (!win_debug_visible)
        may_render_debug_panel = 0;

    // generate Debug panel contents
    if( may_render_debug_panel ) {
        if( has_menu ? ui_window("Debug " ICON_MD_SETTINGS, 0) : ui_panel("Debug " ICON_MD_SETTINGS, 0) ) {
            ui_engine();

            (has_menu ? ui_window_end : ui_panel_end)();
        }

        API int engine_tick();
        engine_tick();
    }
#endif // ENABLE_RETAIL

    double now = paused ? t : glfwGetTime();
    dt = now - t;
    t = now;

#if !ENABLE_RETAIL
    char *st = window_stats();
    static double timer = 0;
    timer += window_delta();
    if( timer >= 0.25 ) {
        glfwSetWindowTitle(window, st);
        timer = 0;
    }
#else
    glfwSetWindowTitle(window, title);
#endif

    void input_update();
    input_update();

    return 1;
}

void window_frame_end() {
}

#endif
