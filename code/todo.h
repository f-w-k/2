// @fixme: ui_image() return value of xy coords

/* 

@todo: find a way to fix SDL click passthrough on transparent windows + sdl3 backend. idea:

{
SDL_Window* window = SDL_GL_GetCurrentWindow(); // SDL_GetCurrentWindow();
SDL_Surface *surface = SDL_GetWindowSurface(window);
SDL_SetWindowShape(window, surface);
}

*/

#if !CODE

char *file_preprocess(const char *src, const char *path, char* (*fs_read)(const char *fname), const char *parent_function); // must FREE() after use

#define HAS_OBJ 1
#define HAS_LOCK 0
// HAS_AUDIO

#define countof COUNTOF
#define ENABLE_PROFILER 1

#define gizmo_hovered() 0
#define gizmo_active() 0

#define input_filter_deadzone(...) vec2(0,0)
#define input_string(x) ""
#define input_use(x)
#define input_enum(x) 0
#define stack(n) va("%*.s",(n)-1,"")
#define PANIC(...) die(va(__VA_ARGS__))
#define EVAL atof
#define ui_collapse(l,x) ui_label(l)
#define ui_collapse_end() 1
#define ui_collapse_clicked() 0
#define image_from_mem image
#define C_CAST C_CAST_RENAMED_TO_CAST
#define input_blocked() 0
#define input_block(x) 0

#define app_width() window_width()
#define app_height() window_height()
#define app_time() window_time()
#define app_path() app_recv("APPDIR")
#define window_msaa() 0
#define window_delta(...) (igGetIO()->DeltaTime)
#define window_width() ((int)igGetIO()->DisplaySize.x)
#define window_height() ((int)igGetIO()->DisplaySize.y)
#define window_fps() (igGetIO()->Framerate)
#define window_frame() render_frame()
#define window_cursor(m) do { char x[] = "0"; x[0] += !!m; mouse_send("cursor", x); } while(0)
#define window_reload() 0
#define window_fullscreen(x)
#define window_has_fullscreen()
#define window_time() time_ss()
#define window_resize(w,h) SDL_SetWindowSize(window,w,h)
// @todo render_resize(w,h)
// @todo useful? before SDL_init(): SDL_SetAppMetadata("Example App", "1.0", "com.example.app");

//#define file_normalize(...) file_norm(__VA_ARGS__)

API array(char*) file_list(const char *pathmasks); // semi-colon separated list of files and masks. recursive with `**` mask

#define vfs_handle(fname) file_handle(fname)

#else

array(char*) file_list(const char *pathmasks) {
    static __thread array(char*) list = 0; // @fixme: add 16 slots

    for( int i = 0; i < array_count(list); ++i ) {
        FREE(list[i]);
    }
    array_resize(list, 0);

    for each_substring(pathmasks,";",pathmask) {
        const char *cwd = 0, *masks = 0;
#if 0
            char *slash = strrchr(pathmask, '/');
            if( !slash ) cwd = "./", masks = pathmask;
            else {
                masks = va("%s", slash+1);
                cwd = pathmask, slash[1] = '\0';
            }
            if( !masks[0] ) masks = "*";

        ASSERT(strend(cwd, "/"), "Error: dirs like '%s' must end with slash", cwd);

        int recurse = strstr(cwd, "**") || strstr(masks, "**");
        strswap(cwd, "**", "./");
#else
        cwd = "./";

        if(strchr("*.", pathmask[0])) masks = pathmask;
        else if(pathmask[0] == '/') masks = va("*%s", pathmask);
        else masks = va("*/%s", pathmask);

        int recurse = !!strstr(pathmask, "**");
#endif

        dir *d = dir_open(cwd, recurse ? "r" : "");
        if( d ) {
            for( int i = 0; i < dir_count(d); ++i ) {
                if( dir_file(d,i) ) {
                    // dir_name() should return full normalized paths "C:/project/demos/art/fx/fxBloom.fs". should exclude system dirs as well
                    const char *entry = dir_name(d,i);
                    //const char *fname = file_name(entry);

                    int allowed = 0;
                    for each_substring(masks,";",mask) {
                        allowed |= strmatch(entry/*fname*/, mask);
                    }
                    if( !allowed ) continue;

                    // if( strstr(fname, "/.") ) continue; // @fixme: still needed? useful?

                    // insert copy
                    char *copy = STRDUP(entry);
                    array_push(list, copy);
                }
            }
            dir_close(d);
        }
    }

    array_sort(list, strcmp);
    return list;
}
char* vfs_load(const char *fname, int *sz) {
    return file_read(fname, sz);
}
char* vfs_read(const char *fname) {
    return file_read(fname, NULL);
}
int vfs_size(const char *fname) {
    int sz;
    return file_read(fname, &sz), sz; // @fixme: suboptimal
}
char *file_preprocess(const char *src, const char *path, char* (*fs_read)(const char *fname), const char *parent_function) { // must FREE() after use
    if (!src) return NULL;

    char *includes = NULL;
    for each_substring(src, "\n", line) {
        if (strbeg(line,"#include")) { // } || strmatch(line, "*#*include")) {
            const char *start = strstri(line, "\"");
            const char *end = strstri(start+1, "\"");
            if (start && end) {
                char *filename = va("%s%.*s", path ? path : "", (int)(end-start-1), start+1);
                char *included = fs_read(filename);
                if (included) {
                    char *nested_includes = file_preprocess(included, path, fs_read, parent_function);
                    includes = strcatf(&includes, "// begin: %s\n%s\n// end: %s\n", line, nested_includes ? nested_includes : "", line); //@leak
                } else {
                    PANIC("!ERROR: %s: Include file not found: %s\n", parent_function, filename);
                }
            } else {
                PANIC("!ERROR: %s: Invalid #include directive: %s\n", parent_function, line);
            }
        } else 
        {
            includes = strcatf(&includes, "\n%s", line); //@leak
        }
    }

    return includes;
}

#endif
