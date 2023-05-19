/*
 *                           0BSD 
 * 
 *                    BSD Zero Clause License
 * 
 *  Copyright (c) 2020 Hermann Meyer
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
#include <string>
#include <libgen.h>
#include "xcustommap.h"
#include "xmkeyboard.h"
#include "xmessage-dialog.h"

#include <unistd.h>

static const char * const notes_5edo[] = {"C","D","E","G","A"};
static const char * const notes_6edo[] = {"C","D","E","F#","G#","B"};
static const char * const notes_7edo[] = {"C","D","E","F","G","A","B"};
static const char * const notes_8edo[] = {"C","D","E","F#","Fb","Gb","Ab","B#"};
static const char * const notes_9edo[] = {"C","D","E","F#","F","G","A","B","C#"};
static const char * const notes_10edo[] = {"C","Db","D","E","Fb","F","G","Ab","A","B"};
static const char * const notes_11edo[] = {"C","Db","D","Eb","E","F","Gb","G","A","Bb","B"};
static const char * const notes_12edo[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
static const char * const notes_13edo[] = {"C","C#","D","D#","E","F","F#","G","G#","H","A","A#","B"};
static const char * const notes_14edo[] = {"C","^C","D","^D","E","^E","F","^F","G","^G","A","^A","B","^B"};
static const char * const notes_15edo[] = {"C","C#","D","D#","E","E#","F","F#","G","G#","Ab","A","A#","B","B#"};
static const char * const notes_16edo[] = {"C","D#","D","E#","E","Eb","F#","F","G#""G","A#","A","B#","B","Bb","C#"};
static const char * const notes_17edo[] = {"C","Db","C#","D","Eb","E#","E","F","Gb","F#","G","Ab","G#","A","Bb","A#","B"};
static const char * const notes_18edo[] = {"C","Db","C#","D","Eb","D#","E","F","Gb","F#","G","Hb","G#","H","A","Bb","A#","B"};
static const char * const notes_19edo[] = {"C","C#","Db","D","D#","Eb","E","Fb","F","F#","Gb","G","G#","Ab","A","A#","Bb","B","Cb"};
static const char * const notes_20edo[] = {"C","C#","Db","D","D#","Eb","E","E#","Fb","F","F#","Gb","G","Ab","A","A#","Bb","B","B#","Cb"};
static const char * const notes_21edo[] = {"C","C#","Db","D","D#","Eb","E","Fb","F","F#","Gb","G","G#","Hb","H","Ab","A","A#","Bb","B","Cb"};
static const char * const notes_22edo[] = {"C","C#","Db","D","D#","Eb","E","E#","Fb","F","F#","Gb","G","G#","Abb","Ab","A","A#","Bb","B","B#","Cb"};
static const char * const notes_23edo[] = {"C","Cb","D#","D","Db","E#","E","Eb","Ebb","F#","F","Fb","G#","G","Gb","A#","A","Ab","B#","B","Bb","Bbb","C#"};


static const char * const * const note_names[] = {notes_5edo, notes_6edo, notes_7edo, notes_8edo, notes_9edo,
        notes_10edo, notes_11edo, notes_12edo, notes_13edo, notes_14edo, notes_15edo, notes_16edo,
        notes_17edo, notes_18edo, notes_19edo, notes_20edo, notes_21edo, notes_22edo, notes_23edo};

static const char * const octaves[] = {"-1","0","1","2","3","4","5","6","7","8","9", "10"};

typedef struct {
    int active;
    int pre_active;
    int changed;
    int nsize;
    int ratio;
    const char * const * notes;
    char* keymapfile;
    long multikeys[128][2];
    Widget_t *keyboard;
    Widget_t *keyboardmap;
    Widget_t *km;
} CustomKeymap;

void draw_custom_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    set_pattern(w,&w->app->color_scheme->selected,&w->app->color_scheme->normal,BACKGROUND_);
    cairo_paint (w->crb);

    cairo_pattern_t* pat;

    pat = cairo_pattern_create_linear (20.0, 20, 380, 330);
    cairo_pattern_add_color_stop_rgba (pat, 0,  0.25, 0.25, 0.25, 1.0);
    cairo_pattern_add_color_stop_rgba (pat, 0.25,  0.2, 0.2, 0.2, 1.0);
    cairo_pattern_add_color_stop_rgba (pat, 0.5,  0.15, 0.15, 0.15, 1.0);
    cairo_pattern_add_color_stop_rgba (pat, 0.75,  0.1, 0.1, 0.1, 1.0);
    cairo_pattern_add_color_stop_rgba (pat, 1,  0.05, 0.05, 0.05, 1.0);

    cairo_set_source (w->crb, pat);
    cairo_rectangle(w->crb, 20.0, 20.0,360,405-w->scale.scale_y);
    cairo_set_line_width(w->crb,2);
    cairo_stroke(w->crb);
    cairo_pattern_destroy (pat);
    pat = NULL;
    pat = cairo_pattern_create_linear (20, 20, 500, 550);
    cairo_pattern_add_color_stop_rgba (pat, 1,  0.25, 0.25, 0.25, 1.0);
    cairo_pattern_add_color_stop_rgba (pat, 0.75,  0.2, 0.2, 0.2, 1.0);
    cairo_pattern_add_color_stop_rgba (pat, 0.5,  0.15, 0.15, 0.15, 1.0);
    cairo_pattern_add_color_stop_rgba (pat, 0.25,  0.1, 0.1, 0.1, 1.0);
    cairo_pattern_add_color_stop_rgba (pat, 0,  0.05, 0.05, 0.05, 1.0);

    cairo_set_source (w->crb, pat);
    cairo_rectangle(w->crb, 26.0, 26.0,348,393-w->scale.scale_y);
    cairo_stroke(w->crb);
    cairo_pattern_destroy (pat);
    pat = NULL;

}

void draw_viewslider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    int v = (int)w->adj->max_value;
    if (!v) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    float sliderstate = adj_get_state(w->adj);
    use_bg_color_scheme(w, get_color_state(w));
    cairo_rectangle(w->crb, 0,0,width,height);
    cairo_fill_preserve(w->crb);
    use_shadow_color_scheme(w, NORMAL_);
    cairo_fill(w->crb);
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0,(height-10)*sliderstate,width,10);
    cairo_fill(w->crb);
}

void draw_custom_keymap(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    int v = (int)w->adj->max_value;
    if (!v) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    int i = 4;
    int a = 0;
    int n = 0;
    int o = 0;
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb,0,0,width,height);
    cairo_fill(w->crb);
    use_fg_color_scheme(w, NORMAL_);
    cairo_text_extents_t extents;
    char s[64];
    for (;i<height/5;i++) {
        if (customkeys->active == a) {
            use_base_color_scheme(w, SELECTED_);
        } else {
            use_fg_color_scheme(w, NORMAL_);
        }
        snprintf(s, 63,"%d",  a);
        cairo_set_font_size (w->crb, w->app->big_font/w->scale.ascale);
        cairo_text_extents(w->crb, s, &extents);
        cairo_move_to (w->crb, 10, (i*5));
        cairo_show_text(w->crb, s);
        cairo_move_to (w->crb, 280, (i*5));
        cairo_show_text(w->crb, customkeys->notes[n]);
        cairo_move_to (w->crb, 305, (i*5));
        cairo_show_text(w->crb, octaves[o]);
        if (customkeys->multikeys[a][0]) {
            int b = 70;
            cairo_move_to (w->crb, b, (i*5));
            int c = 0;
            for (;c<2;c++) {
                if (customkeys->multikeys[a][c] != 0) {
                    cairo_show_text(w->crb, XKeysymToString(customkeys->multikeys[a][c]));
                    b += 100;
                    cairo_move_to (w->crb, b, (i*5));
                }
            }
        }
        cairo_rectangle(w->crb,60,(i*5)-20,200,25);
        cairo_stroke(w->crb);
        i +=5;
        a++;
        n++;
        if (n > customkeys->nsize - 1) {
            n = 0;
            o++;
        }
    }
}

void custom_motion(void *w_, void* xmotion_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    XMotionEvent *xmotion = (XMotionEvent*)xmotion_;
    //fprintf(stderr, "xmotion->x %i xmotion->y %i \n", xmotion->x, xmotion->y);
    if((xmotion->x < 280) && (xmotion->x > 0)) {
        int i = 4;
        int a = 0;
        for (;i<height/5;i++) {
            if (xmotion->y < (i*5)+5 && xmotion->y > (i*5)-20) {
                if (customkeys->active != a) {
                    customkeys->active = a;
                    expose_widget(w);
                }
                return;
            }
            i +=5;
            a++;
        }
    }
}

void leave_keymap(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    customkeys->pre_active = customkeys->active;
    customkeys->active = -2;
    expose_widget(w);
}

void enter_keymap(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    customkeys->active = customkeys->pre_active;
    customkeys->pre_active = -2;
    expose_widget(w);
}

void key_press(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    XKeyEvent *key = (XKeyEvent*)key_;
    if (!key) return;
    if (key->state & ControlMask) {
        return;
    } else {
        KeySym keysym  = XLookupKeysym (key, 0);
        if (keysym == XK_BackSpace) {
            int c = 1;
            for (;c>=0;c--) {
                if (customkeys->multikeys[customkeys->active][c] != 0) {
                    customkeys->multikeys[customkeys->active][c] = 0;
                    break;
                }
            }
            expose_widget(w);
            return;
        } else if (keysym == XK_Up) {
            if (customkeys->active) customkeys->active -=1;
            return;
        } else if (keysym == XK_Down) {
            if (customkeys->active) customkeys->active +=1;
            return;
        } else if (customkeys->active) {
            if (customkeys->multikeys[customkeys->active][0] == 0) {
                customkeys->multikeys[customkeys->active][0] = keysym;
            } else {
                customkeys->multikeys[customkeys->active][1] = keysym;
            }
            customkeys->changed = 1;
            expose_widget(w);
        }
    }
}

void set_viewpoint(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = (Widget_t*)w->parent;
    Widget_t *slider = p->childlist->childs[0];
    adj_set_state(slider->adj, adj_get_state(w->adj));
    int v = (int)max(0,adj_get_value(w->adj));
    XMoveWindow(w->app->dpy,w->widget,0, -10*v);
}

void set_viewport(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = (Widget_t*)w->parent;
    Widget_t *viewport = p->childlist->childs[1];
    adj_set_state(viewport->adj, adj_get_state(w->adj));
}

void adjust_viewport(void *w_, void* user_data) {
    Widget_t *parent = (Widget_t*)w_;
    Widget_t *w = parent->childlist->childs[1];
    XWindowAttributes attrs;
    XGetWindowAttributes(parent->app->dpy, (Window)parent->widget, &attrs);
    int height_t = attrs.height;
    XGetWindowAttributes(parent->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    float d = (float)height/(float)height_t;
    float max_value = (float)((float)height/((float)(height_t/(float)(height-height_t))*(d*10.0)));
    float value = adj_get_value(w->adj);
    w->adj_y->max_value = max_value;
    if (max_value < value) adj_set_value(w->adj,value);
}

static void customkeys_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    free(customkeys->keymapfile);
    free(customkeys);
}

Widget_t* add_viewport(Widget_t *parent, int width, int height) {
    Widget_t *slider = add_vslider(parent, "", 330, 0, 10, 385);
    slider->func.expose_callback = draw_viewslider;
    slider->adj_y = add_adjustment(slider,0.0, 0.0, 0.0, 1.0,0.0085, CL_VIEWPORTSLIDER);
    slider->adj = slider->adj_y;
    slider->func.value_changed_callback = set_viewport;
    slider->scale.gravity = NORTHWEST;
    slider->flags &= ~USE_TRANSPARENCY;
    slider->flags |= NO_AUTOREPEAT | NO_PROPAGATE;

    Widget_t *wid = create_widget(parent->app, parent, 0, 0, width, height);
    XSelectInput(wid->app->dpy, wid->widget,StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask|KeyReleaseMask
                    |ButtonPressMask|Button1MotionMask|PointerMotionMask);
    Widget_t *p = (Widget_t*)parent->parent;
    wid->parent_struct = p->parent_struct;
    wid->scale.gravity = NONE;
    wid->flags &= ~USE_TRANSPARENCY;
    wid->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    XWindowAttributes attrs;
    XGetWindowAttributes(parent->app->dpy, (Window)parent->widget, &attrs);
    int height_t = attrs.height;
    float d = (float)height/(float)height_t;
    float max_value = (float)((float)height/((float)(height_t/(float)(height-height_t))*(d*10.0)));
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0,max_value ,3.0, CL_VIEWPORT);
    wid->adj = wid->adj_y;
    wid->func.adj_callback = set_viewpoint;
    wid->func.expose_callback = draw_custom_keymap;
    wid->func.motion_callback = custom_motion;
    wid->func.leave_callback = leave_keymap;
    wid->func.enter_callback = enter_keymap;
    wid->func.key_press_callback = key_press;
    adj_set_value(wid->adj,max_value/9.5);

    parent->func.configure_notify_callback = adjust_viewport;

    return wid;
}

void cancel_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *parent = (Widget_t*)w->parent;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        destroy_widget(parent,parent->app);
    }
}

void open_keymap(Widget_t *w, const char* keymapfile, long keys[128][2]) {
    if( access(keymapfile, F_OK ) != -1 ) {
        FILE *fp;
        if((fp=fopen(keymapfile, "rb"))==NULL) {
            Widget_t *dia = open_message_dialog(w, ERROR_BOX, keymapfile, 
            _("Couldn't load file, sorry"),NULL);
            XSetTransientForHint(w->app->dpy, dia->widget, w->widget);
            return;
        }

        if(fread(keys, sizeof(long), 128*2, fp) != 128*2) {
            if(feof(fp)) {
                Widget_t *dia = open_message_dialog(w, ERROR_BOX, keymapfile, 
                _("Premature end of file"),NULL);
                XSetTransientForHint(w->app->dpy, dia->widget, w->widget);
            } else {
                Widget_t *dia = open_message_dialog(w, ERROR_BOX, keymapfile, 
                _("File read error"),NULL);
                XSetTransientForHint(w->app->dpy, dia->widget, w->widget);
            }
        }
        fclose(fp);
    }
}

void save_custom_keymap(Widget_t *w) {
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    MambaKeyboard *keys = (MambaKeyboard*)customkeys->keyboard->parent_struct;
    FILE *fp;
    if((fp=fopen(w->label, "wb"))==NULL) {
        Widget_t *dia = open_message_dialog(w, ERROR_BOX, w->label, 
        _("Couldn't load file, sorry"),NULL);
        XSetTransientForHint(w->app->dpy, dia->widget, w->widget);
        return;
    }

    if(fwrite(customkeys->multikeys, sizeof(long), 128*2, fp) != 128*2) {
        Widget_t *dia = open_message_dialog(w, ERROR_BOX, w->label, 
        _("File write error"),NULL);
        XSetTransientForHint(w->app->dpy, dia->widget, w->widget);
    }
    fclose(fp);
    mamba_read_keymap(keys, w->label,keys->custom_keys);
}

void save_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *parent = (Widget_t*)w->parent;
    CustomKeymap *customkeys = (CustomKeymap*)parent->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        customkeys->changed = 0;
        save_custom_keymap(parent);
        adj_set_value(customkeys->km->adj, 4.0);
    }
}

void exit_response(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *parent = (Widget_t*)w->parent;
   if(user_data !=NULL) {
        int response = *(int*)user_data;
        if(response == 0) {
            destroy_widget(parent,parent->app);
        }
    }
}

void exit_callback(void *w_, void* button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    adj_set_value(w->adj,0.0);
    Widget_t *parent = (Widget_t*)w->parent;
    CustomKeymap *customkeys = (CustomKeymap*)parent->parent_struct;
    if (customkeys->changed) {
        Widget_t *dia = open_message_dialog(w, QUESTION_BOX, "Exit", 
                _("Settings have been changed and not saved, exit anyway?"),NULL);
        XSetTransientForHint(w->app->dpy, dia->widget, w->widget);
        w->func.dialog_callback = exit_response;
    } else {
        if (w->flags & HAS_POINTER ){
            destroy_widget(parent,parent->app);
        }
    }
}

void notes_by_ratio(CustomKeymap *customkeys) {
    customkeys->notes = note_names[customkeys->ratio - 5];
    customkeys->nsize = customkeys->ratio;
}

//static
void edo_callback(void *w_, void* user_data)  noexcept{
    Widget_t *w = (Widget_t*)w_;
    Widget_t *parent = (Widget_t*)w->parent;
    CustomKeymap *customkeys = (CustomKeymap*)parent->parent_struct;
    int i = (int)adj_get_value(w->adj);
    switch(i) {
        case(0) :
            customkeys->notes = notes_12edo;
            customkeys->nsize = 12;
        break;
        case(1) :
            customkeys->notes = notes_12edo;
            customkeys->nsize = 12;
        break;
        case(2) :
            notes_by_ratio(customkeys);
        break;
        default:
            customkeys->notes = notes_12edo;
            customkeys->nsize = 12;
        break;
    }
    memset(customkeys->multikeys, 0, 128*2*sizeof customkeys->multikeys[0][0]);
    std::string path = dirname(customkeys->keymapfile);
    Widget_t *menu = w->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    std::string mapfile = path + "/Mamba_" + comboboxlist->list_names[i] +".multikeymap";
    //fprintf(stderr, "%s\n", mapfile.c_str());
    free(customkeys->keymapfile);
    customkeys->keymapfile = NULL;
    if (asprintf(&customkeys->keymapfile, "%s", mapfile.c_str())) {
        parent->label = customkeys->keymapfile;
        open_keymap(parent, parent->label, customkeys->multikeys);
    }
    expose_widget(customkeys->keyboardmap);
}

Widget_t *open_custom_keymap(Widget_t *keyboard, Widget_t *w, Widget_t *k, int sel, int ratio, const char* keymapfile) {
    Widget_t *wid = create_window(w->app, DefaultRootWindow(w->app->dpy), 0, 0, 400, 490);
    XSelectInput(wid->app->dpy, wid->widget,StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask|KeyReleaseMask
                    |ButtonPressMask|Button1MotionMask|PointerMotionMask);

    XSizeHints* win_size_hints;
    win_size_hints = XAllocSizeHints();
    win_size_hints->flags =  PMinSize|PBaseSize|PMaxSize|PWinGravity|PResizeInc;
    win_size_hints->min_width = 400;
    win_size_hints->min_height = 400;
    win_size_hints->base_width = 400;
    win_size_hints->base_height = 490;
    win_size_hints->max_width = 400;
    win_size_hints->max_height = 793;
    win_size_hints->width_inc = 0;
    win_size_hints->height_inc = 30;
    win_size_hints->win_gravity = CenterGravity;
    XSetWMNormalHints(wid->app->dpy, wid->widget, win_size_hints);
    XFree(win_size_hints);

    widget_set_title(wid, _("Custom Keymap - Editor"));
    wid->func.expose_callback = draw_custom_window;
    XSetTransientForHint(w->app->dpy, wid->widget, w->widget);
    CustomKeymap *customkeys = (CustomKeymap*)malloc(sizeof(CustomKeymap));
    wid->parent_struct = customkeys;
    customkeys->active = 12;
    customkeys->pre_active = 12;
    customkeys->changed = 0;
    memset(customkeys->multikeys, 0, 128*2*sizeof customkeys->multikeys[0][0]);
    customkeys->keyboard = keyboard;
    customkeys->km = k;
    customkeys->notes = notes_12edo;
    customkeys->nsize = 12;
    customkeys->ratio = ratio;
    if (asprintf(&customkeys->keymapfile, "%s", keymapfile))
        wid->label = customkeys->keymapfile;
    else 
        wid->label = keymapfile;
    open_keymap(wid, wid->label,customkeys->multikeys);
    wid->flags &= ~USE_TRANSPARENCY;
    wid->flags |= HAS_MEM | NO_AUTOREPEAT | NO_PROPAGATE;
    wid->func.mem_free_callback = customkeys_mem_free;

    Widget_t *view = create_widget(wid->app, wid, 30, 30, 340, 385);
    view->scale.gravity = NORTHWEST;
    customkeys->keyboardmap = add_viewport(view, 330, 3840);

    Widget_t *fs_edo = add_combobox(wid, _("edo"), 30, 440, 90, 30);
    fs_edo->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    combobox_add_entry(fs_edo, "12ji");
    /*for (unsigned int i = 10; i < 24; i++) {
        std::string key = std::to_string(i)+"edo";
        combobox_add_entry(fs_edo, key.c_str());
    }*/
    combobox_add_entry(fs_edo, "12edo");
    combobox_add_entry(fs_edo, "Scala");
    fs_edo->childlist->childs[0]->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    fs_edo->func.value_changed_callback = edo_callback;
    combobox_set_active_entry(fs_edo, sel);
    edo_callback(fs_edo, NULL);

    Widget_t * button = add_button(wid, _("Cancel"), 130, 440, 75, 30);
    button->scale.gravity = SOUTHWEST;
    button->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    button->func.value_changed_callback = cancel_callback;

    button = add_button(wid, _("Save"), 215, 440, 75, 30);
    button->scale.gravity = SOUTHWEST;
    button->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    button->func.value_changed_callback = save_callback;

    button = add_button(wid, _("Exit"), 300, 440, 75, 30);
    button->scale.gravity = SOUTHWEST;
    button->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    button->func.button_release_callback = exit_callback;

    widget_show_all(wid);
    return wid;
}
