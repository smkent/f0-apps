#include "app.h"

static const int queue_size = 8;

static void event_callback(InputEvent* event, FuriMessageQueue* queue) {
    Event message = {
        .type = EventKey,
        .event = *event,
    };
    furi_message_queue_put(queue, &message, FuriWaitForever);
}

static void draw_callback(Canvas* const canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, "ViewPort example app");
}

static void app_free(App* app) {
    furi_assert(app);
    furi_message_queue_free(app->queue);
    view_dispatcher_free(app->view_dispatcher);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_enabled_set(app->view_port, false);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    free(app);
}

static App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->gui = furi_record_open(RECORD_GUI);
    app->view_port = view_port_alloc();
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_enable_queue(app->view_dispatcher);
    app->queue = furi_message_queue_alloc(queue_size, sizeof(Event));
    view_port_input_callback_set(app->view_port, event_callback, app->queue);
    view_port_draw_callback_set(app->view_port, draw_callback, NULL);
    return app;
}

static inline bool handle_key_press(Event* msg) {
    switch(msg->event.type) {
    case InputTypeRelease:
        if(msg->event.key == InputKeyBack) {
            return false;
        }
        break;
    default:
        break;
    }
    return true;
}

static bool event_handler(Event msg) {
    switch(msg.type) {
    case EventKey:
        if(!handle_key_press(&msg)) {
            return false;
        }
        break;
    default:
        break;
    }
    return true;
}

static void run_event_loop(App* app) {
    Event msg = {0};
    while(furi_message_queue_get(app->queue, &msg, FuriWaitForever) == FuriStatusOk) {
        if(!event_handler(msg)) {
            break;
        }
        view_port_update(app->view_port);
    }
}

int32_t app_entry_point(void) {
    srand(DWT->CYCCNT);
    App* app = app_alloc();
    run_event_loop(app);
    app_free(app);
    return 0;
}
