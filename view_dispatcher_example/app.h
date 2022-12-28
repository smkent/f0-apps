#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include <core/common_defines.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <input/input.h>
#include <notification/notification_messages.h>

typedef struct app_t app_t;

typedef struct {
    View* view;
    app_t* app;
} view_main_t;

struct app_t {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    ViewPort* view_port;
    NotificationApp* notifications;
    view_main_t* view_main;
};

typedef enum {
    ViewMain,
} views_t;

struct ViewConfig {
    views_t id;
    void (*handle_enter)(void*);
    void (*handle_exit)(void*);
    uint32_t (*handle_back)(void*);
    bool (*handle_input)(InputEvent* event, void* ctx);
    void (*handle_draw)(Canvas* const canvas, void* ctx);
};

struct AppView {
    app_t* app;
    View* view;
};

struct AppViewState {
    struct ViewConfig* config;
    struct AppView* context;
};
