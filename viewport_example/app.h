#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include <core/common_defines.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/gui_i.h>
#include <gui/view_dispatcher.h>
#include <input/input.h>

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    ViewPort* view_port;
    FuriMessageQueue* queue;
} App;

typedef enum {
    EventKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent event;
} Event;
