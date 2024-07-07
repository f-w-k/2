// -----------------------------------------------------------------------------
// immediate ui framework
// - rlyeh, public domain
//
// @fixme: docking colors
// @fixme: auto-relocate panels
// @fixme: PANEL_OPEN
// @fixme: panel inside window
// @fixme: test&bugfix ui_dialog()
// @fixme: notifies do not work when viewports is OFF
//
// @todo: ui_hue()
// @todo: ui_alert()
// @todo: ui_filter()
// @todo: ui_browse()
// @todo: bold font
// @todo: logger/console
// @todo: ui_gallery()

#if !CODE

enum UI_FLAGS { // for windows and panels
    UI_OPEN = 1,
    UI_MENUS = 2,
    UI_CENTER = 4,
};

API int ui_notify(const char *label, const char *body);
API int ui_window(const char *label, int *flags);
API int   ui_panel(const char *label, int flags); // can be nested within a window, or freestanding
API int     ui_section(const char *label);
API int     ui_separator();
API int     ui_int(const char *label, int value[1]);
API int     ui_int2(const char *label, int value[2]);
API int     ui_int3(const char *label, int value[3]);
API int     ui_unsigned(const char *label, unsigned value[1]);
API int     ui_unsigned2(const char *label, unsigned value[2]);
API int     ui_unsigned3(const char *label, unsigned value[3]);
API int     ui_bool(const char *label, bool *value);
API int     ui_float(const char *label, float value[1]);
API int     ui_float2(const char *label, float value[2]);
API int     ui_float3(const char *label, float value[3]);
API int     ui_float4(const char *label, float value[4]);
API int     ui_floatabs(const char *label, float value[1]);
API int     ui_mat33(const char *label, float M[9]);
API int     ui_mat34(const char *label, float M[12]);
API int     ui_mat44(const char *label, float M[16]);
API int     ui_string(const char *label, char **string);
API int     ui_buffer(const char *label, char *buffer, int buflen);
API int     ui_color3(const char *label, unsigned *color); //[0..255]
API int     ui_color3f(const char *label, float color[3]); //[0..1]
API int     ui_color4(const char *label, unsigned *color); //[0..255]
API int     ui_color4f(const char *label, float color[4]); //[0..1]
API int     ui_button(const char *label);
API int     ui_buttons(int buttons, /*labels*/...);
API int     ui_toolbar(const char *label, int icons, /*icon labels*/...); // returns 0 (no opt), or [1..N] option
API int     ui_browse(const char **outfile, bool *inlined); // can be nested within a window or within a panel
API int     ui_toggle(const char *label, bool *value);
API int     ui_alert(const char *message);
API int     ui_dialog(const char *label, const char *text, int choices, bool *show); // @fixme: return
API int     ui_list(const char *label, const char **items, int num_items, int *selector);
API int     ui_bitmask(const char *label, int bits, void *ptr);
API int     ui_label(const char *label);
API int     ui_label2(const char *label, const char *caption);
API int     ui_slider(const char *label, float *value);
API int     ui_slider2(const char *label, float *value, const char *caption);
API int     ui_const_bool(const char *label, bool);
API int     ui_const_float(const char *label, float);
API int     ui_const_float2(const char *label, float[2]);
API int     ui_const_string(const char *label, const char *);
API int     ui_image(const char *label, unsigned id, unsigned w, unsigned h); // returns XXYY percentages when clicked
API int     ui_subimage(const char *label, unsigned id, unsigned iw, unsigned ih, unsigned u0, unsigned v0, unsigned u1, unsigned v1); // returns XXYY percentages when clicked
API int     ui_clampi(const char *label, int *value, int mini, int maxi);
API int     ui_clampf(const char *label, float *value, float minf, float maxf);
API int     ui_percent(const char *label, float value); // use [0..1], [0..100], [0..1000] or NaN
API int   ui_panel_end();
API int   ui_tray(); // can be nested within a window (local tray), or freestanding (desktop tray)
API int     ui_menu(const char *label); // must be nested within tray
API int       ui_case(const char *label, bool disabled); // must be nested within menu
API int     ui_menu_end();
API int   ui_tray_end();
API int ui_window_end();

API int ui_enable(bool);
API int ui_enabled();

API int ui_hide(bool);
API int ui_hidden();

API int ui_active();
API int ui_hovered();

API int ui_demo(int do_windows);
//API void *ui_handle();

#else
#include <float.h> // FLT_MIN,FLT_MAX


int ui_is_hidden = 0;

const
int ui_slider_flags = 0;
// ImGuiSliderFlags_Logarithmic = 1 << 5,
// ImGuiSliderFlags_NoRoundToFormat = 1 << 6,
// ImGuiSliderFlags_NoInput = 1 << 7,
// ImGuiSliderFlags_WrapAround = 1 << 8,
// ImGuiSliderFlags_ClampOnInput = 1 << 9,
// ImGuiSliderFlags_ClampZeroRange = 1 << 10,
// ImGuiSliderFlags_AlwaysClamp = ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_ClampZeroRange,

int ui_global_id;
void ui_tick() {
    // new frame: restart the global ID counter
    ui_global_id = 0;
}

int ui_window(const char *label, int *flags) {
    bool wopen = !!(*flags & UI_OPEN);
    if( wopen ) {
        igPushID_Int(++ui_global_id);

        if(*flags & UI_CENTER) {
        ImVec2 center = {0};
        ImGuiViewport_GetCenter(&center, igGetMainViewport());
        igSetNextWindowPos(center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        }

        igSetNextWindowSize(ImVec2(200,1.5*200), ImGuiCond_FirstUseEver);
        igBegin(label, &wopen, ImGuiWindowFlags_NoCollapse|(*flags & UI_MENUS ? ImGuiWindowFlags_MenuBar:0)/*|ImGuiWindowFlags_AlwaysAutoResize*/);
        //igPushItemWidth(-100); // fill the space and leave 100 pixels for the label
        igPushItemWidth(-FLT_MIN); // remove the blank space typically used for the labels
        *flags = (*flags & ~UI_OPEN) | (wopen & UI_OPEN);
        return 1;
    }
    return 0;
}
int ui_window_end() {
    igEnd();
    igPopID();
    return 1;
}

int ui_panel(const char *label, int flags) {// may be embedded inside a window, or standalone
    igPushID_Int(++ui_global_id);

    if(flags & UI_CENTER) {
    ImVec2 center = {0};
    ImGuiViewport_GetCenter(&center, igGetMainViewport());
    igSetNextWindowPos(center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    }

    igSetNextWindowSize(ImVec2(200,1.5*200), ImGuiCond_FirstUseEver);
    igBegin(label, NULL, 0|ImGuiWindowFlags_NoDocking|(flags & UI_MENUS ? ImGuiWindowFlags_MenuBar:0));
    //igPushItemWidth(-100); // fill the space and leave 100 pixels for the label
    igPushItemWidth(-FLT_MIN); // remove the blank space typically used for the labels
    return 1;
}
int ui_panel_end() {
    igEnd();
    igPopID();
    return 1;
}

int ui_separator() {
    igSeparator();
    return 1;
}


int ui_notify(const char *label, const char *body) {
    int type = 'i'; // (i)nfo, o(k), (e)rror, (w)arning types
    /**/ if( strstri(label, "warn") ) type = 'w';
    else if( strstri(label, "err") ) type = 'e';
    else if( strstri(label, "succ") ) type = 'k';
    else if( strstri(label, "random") ) type = "weik"[rand()%4];
    return igInsertNotification(type, label, body, 3.0f), 1;
}


int ui_tray_global = 1;
int ui_tray() {
    ui_tray_global = igCurrentWindowStackSize() <= 1;
    return ui_tray_global ? igBeginMainMenuBar() : igBeginMenuBar();
}
int ui_tray_end() {
    ui_tray_global ? igEndMainMenuBar() : igEndMenuBar();

    return 1;
}
int ui_menu(const char *label) {
    bool enabled = 1;
    return igBeginMenu(label, enabled);
}
int ui_menu_end() {
    return igEndMenu(), 1;
}
int ui_case(const char *label, bool disabled) {
    const char *icon = NULL;
    const char *shortcut = NULL;
    bool enabled = !disabled;
    bool selected = false;
    return igMenuItemEx(label,icon,shortcut,selected,enabled);
}


/// [src] https://github.com/ocornut/imgui/issues/3565#issuecomment-2120416586 by azmr
/// Make contents in a table cover the entire row rather than just a single column.
///
/// Always covers the entire row (not just the remaining columns);
/// can sort of coexist with per-column data, but may not be as intended.
/// Accounts for:
/// - scrollbar
/// - header row
/// - column reordering
/// - hidden columns
float igTableFullRowBegin() {
    ImGuiTable *table = igGetCurrentTable();

    // Set to the first visible column, so that all contents starts from the leftmost point
    for (ImGuiTableColumnIdx *clmn_idx = table->DisplayOrderToIndex.Data,
         *end = table->DisplayOrderToIndex.DataEnd;
         clmn_idx < end; ++clmn_idx)
    {   if (igTableSetColumnIndex(*clmn_idx)) break;   }

    ImRect *work_rect    = &igGetCurrentWindow()->WorkRect;
    float   restore_x    = work_rect->Max.x;
    ImRect  bg_clip_rect = table->BgClipRect; // NOTE: this accounts for header column & scrollbar

    igPushClipRect(bg_clip_rect.Min, bg_clip_rect.Max, 0); // ensure that both our own drawing...
    work_rect->Max.x = bg_clip_rect.Max.x;                 // ...and Dear ImGui drawing will be visible across the entire row

    return restore_x;
}
void igTableFullRowEnd(float restore_x) {
    igGetCurrentWindow()->WorkRect.Max.x = restore_x;
    igPopClipRect();
}

static float UI_LABEL_RESTORE;
static int UI_LABEL_1OF2(const char *label) {
    enum { BOLD_INDENT = 10, BOLD_ALPHA = 150, BOLD_COLOR = ImGuiCol_Text };
    enum { BAR_MIN_WIDTH = 1, BAR_MAX_WIDTH = 4, BAR_HEIGHT = 8, BAR_COLOR = ImGuiCol_TitleBgActive };

    const char *tooltip = 0;
    int has_label = label && label[0], hovered = 0;
    if(!igBeginTable("##", 2, ImGuiTableFlags_Resizable|ImGuiTableFlags_SizingStretchProp|ImGuiTableFlags_ContextMenuInBody|ImGuiTableFlags_NoHostExtendX, ImVec2(0,0), 0.f))
        return 0;

    igTableNextColumn();

    if(!has_label) label = "";

        static char copy[128];
        tooltip = strrchr(label, '@');
        if( tooltip ) snprintf(copy, 128, "%.*s %s", (int)(tooltip - label), label, "(?)" );

        igTextWithHoverColor(igGetColorU32_Col(BOLD_COLOR, BOLD_ALPHA / 100.f ), ImVec2(BOLD_INDENT/2,BOLD_INDENT), tooltip ? copy : label);
        hovered = igIsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled);

        ImVec2 rmin; igGetItemRectMin(&rmin); rmin.y -= BAR_HEIGHT;
        ImVec2 rmax; igGetItemRectMax(&rmax); rmax.y += BAR_HEIGHT;
        igRenderFrame(rmin,ImVec2(rmin.x+(hovered ? BAR_MAX_WIDTH : BAR_MIN_WIDTH),rmax.y),igGetColorU32_Col(BAR_COLOR, 1.0f),false/*borders*/,0/*rounding*/);

        if( tooltip && hovered ) igSetTooltip("%s", tooltip + 1);

    igTableNextColumn();
    igPushItemWidth(-1);

    if(!has_label) UI_LABEL_RESTORE = igTableFullRowBegin();
    return 1;
}
static int UI_LABEL_2OF2( int rc ) {
    if( UI_LABEL_RESTORE ) igTableFullRowEnd(UI_LABEL_RESTORE), UI_LABEL_RESTORE = 0;
    return igPopItemWidth(), igEndTable(), rc;
}
#define UI_LABEL(label, ...) (UI_LABEL_1OF2(label) ? UI_LABEL_2OF2(__VA_ARGS__) : 0)


int ui_label(const char *label) {
    return UI_LABEL(label, (igText(""), 1));
}
int ui_label2(const char *label, const char *text) {
    return UI_LABEL(label, (igText("%s", text), 1));
}
int ui_string(const char *label, char **str) {
    if(!UI_LABEL_1OF2(label)) return 0;

    char *bak = va("%s%c", *str ? *str : "", '\0');
    int rc = ui_buffer(label, bak, strlen(bak)+2);
    if( *str ) 0[*str] = '\0';
    strcatf(str, "%s", bak);
    return UI_LABEL_2OF2(rc);
}
int ui_buffer(const char *label, char *buffer, int buflen) {
    return UI_LABEL(label, igInputText("##", buffer, buflen, 0/*ImGuiInputTextFlags*/, NULL, NULL));
}
int ui_const_string(const char *label, const char *text) {
    return ui_label2(label, text);
}
int ui_const_bool(const char *label, bool value) {
    return ui_bool(label, &value);
}
int ui_const_float(const char *label, float value) {
    return ui_float(label, &value);
}
int ui_const_float2(const char *label, float value[2]) {
    float copy2[] = { value[0], value[1] };
    return ui_float2(label, copy2);
}

int ui_clampi(const char *label, int *v, int mini, int maxi) {
    int range[] = { mini, maxi };
    return UI_LABEL(label, igDragScalarN("##", ImGuiDataType_S32,v,1,1.f,range+0,range+1,NULL,0));
}
int ui_clampf(const char *label, float *v, float minf, float maxf) {
    float range[] = { minf, maxf };
    return UI_LABEL(label, igDragScalarN("##", ImGuiDataType_Float,v,1,1.f,range+0,range+1,NULL,0));
}
int ui_int(const char *label, int v[1]) {
    // return UI_LABEL(label, igInputInt("##", v, 1, 10, 0));
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_S32,v,1,1.f,NULL,NULL,NULL,0));
}
int ui_int2(const char *label, int v[2]) {
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_S32,v,2,1.f,NULL,NULL,NULL,0));
}
int ui_int3(const char *label, int v[3]) {
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_S32,v,3,1.f,NULL,NULL,NULL,0));
}
int ui_unsigned(const char *label, unsigned v[1]) {
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_U32,v,1,1.f,NULL,NULL,NULL,0));
}
int ui_unsigned2(const char *label, unsigned v[2]) {
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_U32,v,2,1.f,NULL,NULL,NULL,0));
}
int ui_unsigned3(const char *label, unsigned v[3]) {
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_U32,v,3,1.f,NULL,NULL,NULL,0));
}

int ui_bool(const char *label, bool *value) {
    return UI_LABEL(label, igCheckbox("##", value));
}
int ui_toggle(const char *label, bool *value) {
    return UI_LABEL(label, igCheckbox("##", value));
}
int ui_float(const char *label, float *v) {
//    return UI_LABEL(label, igSliderFloat("##", v, 0.0f, 1.0f, "%.3f", ui_slider_flags));
//    return UI_LABEL(label, igDragFloat("##", v, 0.005f, 0.0f, 1.0f, "%.2f", 0/*ImGuiSliderFlags_AlwaysClamp*/));
//    float zero = 0, one = 1; return UI_LABEL(label, igSliderScalar("##", ImGuiDataType_Float, v, &zero, &one));
    return UI_LABEL(label, igDragFloat("##", v, 0.005f, 0, 0, "%.3f", 0 ));
}
int ui_floatabs(const char *label, float *v) {
    int rc = ui_float(label, v); if( *v < 0 ) *v = 0;
    return rc;
}
int ui_percent(const char *label, float pct) {
    if( pct != pct ) return UI_LABEL(label, (igProgressBar(-1.0f * igGetTime(),ImVec2(-1,0),NULL),0));
    if( pct > 100 ) return ui_percent(label, pct / 1000);
    if( pct > 1 ) return ui_percent(label, pct / 100);
    return UI_LABEL(label, (igProgressBar(pct,ImVec2(-1,0),NULL), 0));
}
int ui_float2(const char *label, float v[2]) {
    return UI_LABEL(label, igInputFloat2("##", v, "%f", ui_slider_flags));
}
int ui_float3(const char *label, float v[3]) {
    return UI_LABEL(label, igInputFloat3("##", v, "%f", ui_slider_flags));
}
int ui_float4(const char *label, float v[4]) {
    return UI_LABEL(label, igInputFloat4("##", v, "%f", ui_slider_flags));
}
int ui_slider(const char *label, float *value) {
    return UI_LABEL(label, igSliderFloat("##", value, 0, 1, "%f", ui_slider_flags));
}
int ui_slider2(const char *label, float *value, const char *caption) {
    return UI_LABEL(label, igSliderFloat("##", value, 0, 1, caption, ui_slider_flags));
}

int ui_mat33(const char *label, float M[9]) {
//    if( label && ui_filter && ui_filter[0] ) if( !strstri(label, ui_filter) ) return 0;

//    ui_num_signs = 3;
    int changed = 0;
    changed |= ui_label(label);
    changed |= ui_float3(NULL, M);
    changed |= ui_float3(NULL, M+3);
    changed |= ui_float3(NULL, M+6);
    return changed;
}
int ui_mat34(const char *label, float M[12]) {
//    if( label && ui_filter && ui_filter[0] ) if( !strstri(label, ui_filter) ) return 0;

//    ui_num_signs = 3;
    int changed = 0;
    changed |= ui_label(label);
    changed |= ui_float4(NULL, M);
    changed |= ui_float4(NULL, M+4);
    changed |= ui_float4(NULL, M+8);
    return changed;
}
int ui_mat44(const char *label, float M[16]) {
//    if( label && ui_filter && ui_filter[0] ) if( !strstri(label, ui_filter) ) return 0;

//    ui_num_signs = 4;
    int changed = 0;
    changed |= ui_label(label);
    changed |= ui_float4(NULL, M);
    changed |= ui_float4(NULL, M+4);
    changed |= ui_float4(NULL, M+8);
    changed |= ui_float4(NULL, M+12);
    return changed;
}


int ui_color3f(const char *label, float color[3]) { //[0..1]
    // |ImGuiColorEditFlags_PickerHueWheel
    return UI_LABEL(label, igColorEdit3("##", color, ImGuiColorEditFlags_Float));
}
int ui_color3(const char *label, unsigned *color32) { //[0..255]
    ImVec4 color; igColorConvertU32ToFloat4(&color,(ImU32)*color32);
    int touched = ui_color3f(label, &color.x);
    if( touched ) *color32 = (unsigned)igColorConvertFloat4ToU32(color);
    return touched;
}

int ui_color4f(const char *label, float color[4]) { //[0..1]
    // |ImGuiColorEditFlags_PickerHueWheel
    return UI_LABEL(label, igColorEdit4("##", color, ImGuiColorEditFlags_Float|ImGuiColorEditFlags_AlphaPreviewHalf|ImGuiColorEditFlags_AlphaBar));
}
int ui_color4(const char *label, unsigned *color32) { //[0..255]
    ImVec4 color; igColorConvertU32ToFloat4(&color,(ImU32)*color32);
    int touched = ui_color4f(label, &color.x);
    if( touched ) *color32 = (unsigned)igColorConvertFloat4ToU32(color);
    return touched;
}

int ui_button(const char *label) {
    igPushID_Int(++ui_global_id);
    int rc = igButton(label, ImVec2(-1,0));
    return igPopID(), rc;
}
int ui_buttons(int buttons, /*labels*/...) {
    const char *labels[16] = {0};
    if( buttons >= COUNTOF(labels) ) die("ui_buttons() capacity exceeded");
        va_list list;
        va_start(list, buttons);
        for( int i = 0; i < buttons; ++i ) {
            labels[i] = va_arg(list, const char*);
        }
        va_end(list);

    int touched = 0;

    igPushID_Int(++ui_global_id);

    // special case
    if( buttons == 2 && !strcmp(labels[1], "...") ) {
        igSetNextItemAllowOverlap();
        if( igButton(labels[0], ImVec2(-1,0)) ) touched = 1;
        igSameLine(0,0);
        igSetCursorPosX(igGetCursorPosX()-20);
        if( igSmallButton(labels[1]) ) touched = 2;
    }

    // general case
    else {
        igColumns(buttons, "##", false);
        for( int i = 0; i < buttons; ++i ) {
            if( igButton(labels[i], ImVec2(-1,0)) )
                touched = i + 1;
            igNextColumn();
        }
    }

    igPopID();

    return touched;
}
int ui_toolbar(const char *label, int icons, ...) {
    if( !UI_LABEL_1OF2(label) )
        return 0;

    const char *labels[16] = {0};
    if( icons >= COUNTOF(labels) ) die("ui_toolbar() capacity exceeded");
        va_list list;
        va_start(list, icons);
        for( int i = 0; i < icons; ++i ) {
            labels[i] = va_arg(list, const char*);
        }
        va_end(list);

    igPushStyleVar_Float(ImGuiStyleVar_FrameRounding, 0);

    int icon = 0;
    for( int i = 0; i < icons ; ++i ) {
        if( /*igSmallButton*/igButton(labels[i],ImVec2(0,0))) {
            icon = i+1;
        }
        if( i < (icons-1) ) igSameLine(0,0);
    }

    igPopStyleVar(1);

    return UI_LABEL_2OF2(icon);
}

int ui_browse(const char **outfile, bool *inlined) {// may be embedded inside a window or inside a panel
    return ui_label("UI_BROWSE() @todo"), 0;
}

// @fixme: show arg not needed?
int ui_dialog(const char *label, const char *text, int choices, bool *show) {// @fixme: return
    int selected = 0;

        if (igButton(label,ImVec2(0,0)))
            *show = 1, igOpenPopup_Str(label,0);

        if( *show )
        if (igBeginPopupModal(label, NULL, 0/*ImGuiWindowFlags_MenuBar*/))
        {
            igText("%s", text);

            if( choices > 0 ) {
                selected = ui_buttons(choices > 3 ? 3 : choices, "OK", "Cancel", "Retry");
                if( selected ) {
                    *show = 0, igCloseCurrentPopup();
                }
            }
            else {
                if (igButton("Close",ImVec2(0,0)))
                    selected = 1, *show = 0, igCloseCurrentPopup();
            }

            igEndPopup();
        }
        return selected;

    return ui_label("UI_DIALOG() @todo"), 0;
}
int ui_alert(const char *message) {
    bool dummy = 0;
    return ui_dialog("Alert", message, 1, &dummy);
}
int ui_bitmask(const char *label, int numbits, void *ptr) {
    if( !UI_LABEL_1OF2(label) )
        return 0;

    uintptr_t addr = 0;
    memcpy(&addr, ptr, numbits/8);

    int touched = 0;
    for( int i = 0; i < numbits ; ++i ) {
        if( i > 0 ) igSameLine(0,0);
        igPushID_Int(i);
        if( igSmallButton(addr & (1ull<<i) ? "1" : "0") ) {
            touched = i+1;
            addr ^= (1ull<<i);
            memcpy(ptr, &addr, numbits/8);
        }
        igPopID();
    }

    return UI_LABEL_2OF2(touched);
}

int ui_list(const char *label, const char **items, int num_items, int *selector) {
    const int height_in_items = num_items > 4 ? 4 : num_items;
    return UI_LABEL(label, igListBox_Str_arr("##",selector,items,num_items,height_in_items));
}
int ui_section(const char *label) { // @fixme: push different font face
//    ImU32 color = igGetColorU32_Col(ImGuiCol_Text, 2.0f);
//    igPushStyleColor_U32(ImGuiCol_Text, color);
    igSeparatorText(label);
//    igPopStyleColor(1);
    return 1;
}

int ui_subimage(const char *label, unsigned id, unsigned w, unsigned h, unsigned u0, unsigned v0, unsigned u1, unsigned v1) {
    if( !UI_LABEL_1OF2(label) )
        return 0;

    ImVec2 origin;
    igGetCursorPos(&origin);

    w = w ? w : 64, h = h ? h : 64;
    u1 = u1 ? u1 : w, v1 = v1 ? v1 : h;

    igImage(id, ImVec2(w,h), ImVec2(u0/w,v0/h), ImVec2(u1/w,v1/h), ImVec4(255,255,255,255), ImVec4(0,0,0,0));

    int ret = 0;
    if( igIsItemClicked(ImGuiMouseButton_Left) ) {
        ImVec2 click;
        igGetMousePos(&click);
        int px = 0x100 * (click.x - origin.x) / w;
        int py = 0x100 * (click.y - origin.y) / h;
        ret = px * 0x100 + py; printf("%d %d xy:%d\n", px, py, ret);
    }

    return UI_LABEL_2OF2(ret); // !!ret;
}

int ui_image(const char *label, unsigned id, unsigned w, unsigned h) {
    return ui_subimage(label, id, w,h, 0,0,w,h);
}

int ui_is_enabled = 1;
int ui_enable(bool enable) {
    if( enable ^ ui_is_enabled ) {
        if( enable ) igEndDisabled(); else igBeginDisabled(1);
    }
    ui_is_enabled = enable;
    return 1;
}
int ui_enabled() {
    return ui_is_enabled;
}


int ui_hide(bool hidden) {
    ui_is_hidden = hidden;
    return 1;
}
int ui_hidden() {
    return ui_is_hidden;
}

int ui_active() {
    return igIsAnyItemActive();
}
int ui_hovered() {
    return igIsAnyItemHovered();
}

int ui_demo(int do_windows) {
    static int integer = 42;
    static bool toggle = true;
    static bool boolean = true;
    static float floating = 3.14159;
    static float float2[2] = {1,2};
    static float float3[3] = {1,2,3};
    static float float4[4] = {1,2,3,4};
    static float rgbf[3] = {0.84,0.67,0.17};
    static float rgbaf[4] = {0.67,0.90,0.12,0.5};
    static unsigned rgb = 0x22446688;
    static unsigned rgba = 0x88664422;
    static float slider = 0.5f;
    static float slider2 = 0.5f;
    static char string[64] = "hello world 123";
    static int item = 0; const char *list[] = {"one","two","three"};
    static bool show_dialog = false;
    static bool show_browser = false;
    static const char* browsed_file = "";
    static uint8_t bitmask = 0x55;
    static int hits = 0;
    static int window1 = 0, window2 = 0, window3 = 0;
    static int disable_all = 0;

    if( ui_panel("UI", 0) ) {
        int choice = ui_toolbar(NULL, 4,"Browser","Toast@Notify","Toggle on/off","Alert");
            if(choice == 1) show_browser = true;
            if(choice == 2) ui_notify(va("My random toast (%d)", rand()), va("This is notification #%d", ++hits));
            if(choice == 3) disable_all ^= 1;
            if(choice == 4) ui_alert("Button was pressed");

        if( disable_all ) ui_enable(0);

        if( ui_browse(&browsed_file, &show_browser) ) puts(browsed_file);

        if( ui_section("Labels")) {}
        if( ui_label("my label")) {}
        if( ui_label("my label with tooltip@built on " __DATE__ " " __TIME__)) {}
        if( ui_toolbar("my toolbar", 4, ICON_MD_STAR,ICON_MD_STAR_OUTLINE,ICON_MD_BOOKMARK,ICON_MD_BOOKMARK_BORDER) ) {}
//        if( ui_label2_toolbar("my toolbar", ICON_MD_STAR ICON_MD_STAR_OUTLINE ICON_MD_BOOKMARK ICON_MD_BOOKMARK_BORDER) ) {}
        //if( ui_label2_wrap("my long label", "and some long long long long text wrapped")) {}

        if( ui_section("Types")) {}
        if( ui_bool("my bool", &boolean) ) puts("bool changed");
        if( ui_int("my int", &integer) ) puts("int changed");
        if( ui_float("my float", &floating) ) puts("float changed");
        if( ui_buffer("my string", string, 64) ) puts("string changed");

        if( ui_section("Vectors") ) {}
        if( ui_float2("my float2", float2) ) puts("float2 changed");
        if( ui_float3("my float3", float3) ) puts("float3 changed");
        if( ui_float4("my float4", float4) ) puts("float4 changed");

        if( ui_section("Lists")) {}
        if( ui_list("my list", list, 3, &item ) ) puts("list changed");

        if( ui_section("Colors")) {}
        if( ui_color3("my color3", &rgb) ) puts("color3 changed");
        if( ui_color4("my color4@this is a tooltip", &rgba) ) puts("color4 changed");
        if( ui_color3f("my color3f", rgbf) ) puts("color3f changed");
        if( ui_color4f("my color4f@this is a tooltip", rgbaf) ) puts("color4f changed");

        if( ui_section("Sliders")) {}
        if( ui_slider("my slider", &slider)) puts("slider changed");
        if( ui_slider2("my slider 2", &slider2, va("%.2f", slider2))) puts("slider2 changed");
        if( ui_percent("my bar %", slider2));
        if( ui_percent("my bar INF", NAN));

        if( do_windows ) {
        if( ui_section("Windows")) {}
        int show = ui_buttons(3, "Container", "SubPanel", "SubRender");
        if( show == 1 ) window1 = 1;
        if( show == 2 ) window2 = 1;
        if( show == 3 ) window3 = 1;
        }

        if( ui_section("Others")) {}
        if( ui_bitmask("my bitmask", 8, &bitmask) ) printf("bitmask changed %x\n", bitmask);
        if( ui_toggle("my toggle", &toggle) ) printf("toggle %s\n", toggle ? "on":"off");

        int xy;
        if( (xy = ui_image("my image", 1/*image id #1*/, 0, 0)) ) { printf("image clicked at %d\n", xy); }

        if( ui_separator() ) {}
        if( ui_button("my button") ) { puts("button clicked"); show_dialog = true; }
        if( ui_buttons(2, "yes", "no") ) { puts("button clicked"); }
        if( ui_buttons(3, "yes", "no", "maybe") ) { puts("button clicked"); }
        if( ui_dialog("my dialog", __FILE__ "\n" __DATE__ "\n" "Public Domain.", 2/*two buttons*/, &show_dialog) ) {}

        if( disable_all ) ui_enable(1); // restore enabled state

        ui_panel_end();
    }

    if( !do_windows ) return 0;

    // window api showcasing
    if( ui_window("Container demo", &window1) ) {
        ui_label("label #1");
        if( ui_bool("my bool", &boolean) ) puts("bool changed");
        if( ui_int("my int", &integer) ) puts("int changed");
        if( ui_float("my float", &floating) ) puts("float changed");
        if( ui_buffer("my string", string, 64) ) puts("string changed");
        ui_window_end();
    }

    if( ui_window("SubPanel demo", &window2) ) {
        if( ui_panel("panel #2", 0) ) {
            ui_label("label #2");
            if( ui_bool("my bool", &boolean) ) puts("bool changed");
            if( ui_int("my int", &integer) ) puts("int changed");
            if( ui_float("my float", &floating) ) puts("float changed");
            if( ui_buffer("my string", string, 64) ) puts("string changed");
            ui_panel_end();
        }
        ui_window_end();
    }

    if( ui_window("SubRender demo", &window3) ) {
        if( ui_panel("panel #3A", 0) ) {
            if( ui_bool("my bool", &boolean) ) puts("bool changed");
            if( ui_int("my int", &integer) ) puts("int changed");
            if( ui_float("my float", &floating) ) puts("float changed");
            if( ui_buffer("my string", string, 64) ) puts("string changed");
            if( ui_separator() ) {}
            if( ui_slider("my slider", &slider)) puts("slider changed");
            if( ui_slider2("my slider 2", &slider2, va("%.2f", slider2))) puts("slider2 changed");
            ui_panel_end();
        }
        if( ui_panel("panel #3B", 0) ) {
            if( ui_bool("my bool", &boolean) ) puts("bool changed");
            if( ui_int("my int", &integer) ) puts("int changed");
            if( ui_float("my float", &floating) ) puts("float changed");
            if( ui_buffer("my string", string, 64) ) puts("string changed");
            if( ui_separator() ) {}
            if( ui_slider("my slider", &slider)) puts("slider changed");
            if( ui_slider2("my slider 2", &slider2, va("%.2f", slider2))) puts("slider2 changed");
            ui_panel_end();
        }

#if HAS_IMAGE
        const char *title = "SubRender demo";
        struct nk_window *win = nk_window_find(ui_ctx, title);
        if( win ) {
            enum { menubar_height = 65 }; // title bar (~32) + menu bounds (~25)
            struct nk_rect bounds = win->bounds; bounds.y += menubar_height; bounds.h -= menubar_height;
#if 1
            ddraw_flush();

            // @fixme: this is breaking rendering when post-fxs are in use. edit: cannot reproduce
            static texture_t tx = {0};
            if( texture_rec_begin(&tx, bounds.w, bounds.h )) {
                glClearColor(0.15,0.15,0.15,1);
                glClear(GL_COLOR_BUFFER_BIT);
                ddraw_grid(10);
                ddraw_flush();
                texture_rec_end(&tx);
            }
            struct nk_image image = nk_image_id((int)tx.id);
            nk_draw_image_flipped(nk_window_get_canvas(ui_ctx), bounds, &image, nk_white);
#else
            static video_t *v = NULL;
            do_once v = video( "bjork-all-is-full-of-love.mp4", VIDEO_RGB );

            texture_t *textures = video_decode( v );

            struct nk_image image = nk_image_id((int)textures[0].id);
            nk_draw_image(nk_window_get_canvas(ui_ctx), bounds, &image, nk_white);
#endif
        }
#endif

        ui_window_end();
    }
    return 0;
}

#endif

#if 0
// How to do a toolbar in Dear ImGui.
// [src] https://gist.github.com/rmitton/f80cbb028fca4495ab1859a155db4cd8

enum { toolbarSize = 50 }; // cant be probably smaller than padding amounts added up together
float menuBarHeight;

void DockSpaceUI() {
    ImGuiViewport* viewport = igGetMainViewport();
    igSetNextWindowPos(ImVec2(viewport->Pos.x,viewport->Pos.y + toolbarSize), 0, ImVec2(0,0));
    igSetNextWindowSize(ImVec2(viewport->Size.x,viewport->Size.y - toolbarSize), 0);
    igSetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = 0
        | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    igPushStyleVar_Float(ImGuiStyleVar_WindowRounding, 0.0f);
    igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.0f);
    igBegin("Master DockSpace", NULL, window_flags);
    ImGuiID dockMain = igGetID_Str("MyDockspace");
    
    // Save off menu bar height for later.
    menuBarHeight = igGetCurrentWindow()->MenuBarHeight;

    igDockSpace(dockMain, ImVec2(0,0), 0, NULL);
    igEnd();
    igPopStyleVar(3);
}

void ToolbarUI() {
    ImGuiViewport* viewport = igGetMainViewport();
    igSetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menuBarHeight), 0, ImVec2(0,0));
    igSetNextWindowSize(ImVec2(viewport->Size.x, toolbarSize), 0);
    igSetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = 0
        | ImGuiWindowFlags_NoDocking 
        | ImGuiWindowFlags_NoTitleBar 
        | ImGuiWindowFlags_NoResize 
        | ImGuiWindowFlags_NoMove 
        | ImGuiWindowFlags_NoScrollbar 
        | ImGuiWindowFlags_NoSavedSettings
        ;
    igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0);
    igBegin("TOOLBAR", NULL, window_flags);
  
    igButton("Toolbar goes here", ImVec2(0,37));
  
    igEnd();

    igPopStyleVar(1);
}

void ProgramUI() {
    DockSpaceUI();
    ToolbarUI();
}
#endif

#if 0

API int   ui_collapse(const char *label, const char *id);
API int   ui_collapseo(const char *label, const char *id);
API int   ui_contextual();
API int    ui_short(const char *label, short *value);
API int    ui_mat33(const char *label, float mat33[9]);
API int    ui_mat34(const char *label, float mat34[12]);
API int    ui_mat44(const char *label, float mat44[16]);
API int    ui_double(const char *label, double *value);
API int    ui_button_transparent(const char *label);
API int    ui_radio(const char *label, int *selector, int num_items, /* const char* */...);
API int    ui_printf(const char *fmt, ...);
API int    ui_console(const char *optional_cmd);
API int    ui_clampf_(const char *label, float *value, float minf, float maxf, float step);
API int   ui_contextual_end(int close);
API int   ui_collapse_clicked();
API int   ui_collapse_end();

API int  ui_show(const char *panel_or_window_title, int enabled);
API int  ui_dims(const char *panel_or_window_title, float width, float height);
API int  ui_visible(const char *panel_or_window_title); // @todo: include ui_collapse() items that are open as well?
API vec2 ui_get_dims();

API int ui_menu_editbox(char *buf, int bufcap);

API int ui_popups(); // ui_any_popup()? ui_has_popups()?

#endif
