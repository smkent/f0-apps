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

typedef struct app_t {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    ViewPort* view_port;
    NotificationApp* notifications;
} App;

typedef enum {
    ViewMain,
} ViewID;

typedef struct {
    ViewID id;
    void (*handle_enter)(void*);
    void (*handle_exit)(void*);
    uint32_t (*handle_back)(void*);
    bool (*handle_input)(InputEvent* event, void* ctx);
    void (*handle_draw)(Canvas* const canvas, void* ctx);
} ViewConfig;

typedef struct {
    App* app;
    View* view;
} AppView;

typedef struct {
    ViewConfig* config;
    AppView* context;
} AppViewState;
