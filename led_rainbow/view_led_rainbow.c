#include "view_led_rainbow.h"

#include <gui/view.h>

static const uint32_t fps = 24;
static const uint8_t led_increment = 10;

typedef struct {
    FuriTimer* timer;
} LocalCtx;

enum LEDPhase {
    RedToGreenUp,
    RedDownToGreen,
    GreenToBlueUp,
    GreenDownToBlue,
    BlueToRedUp,
    BlueDownToRed,
};

typedef struct {
    enum LEDPhase phase;
    uint8_t led_red;
    uint8_t led_green;
    uint8_t led_blue;
} Model;

static inline void model_tick(Model* model) {
    switch(model->phase) {
    case RedToGreenUp:
        if(model->led_green >= 255 - led_increment) {
            model->phase = RedDownToGreen;
            model->led_green = 255;
            return;
        }
        model->led_green += led_increment;
        break;
    case RedDownToGreen:
        if(model->led_red <= led_increment) {
            model->phase = GreenToBlueUp;
            model->led_red = 0;
            return;
        }
        model->led_red -= led_increment;
        break;
    case GreenToBlueUp:
        if(model->led_blue >= 255 - led_increment) {
            model->phase = GreenDownToBlue;
            model->led_blue = 255;
            return;
        }
        model->led_blue += led_increment;
        break;
    case GreenDownToBlue:
        if(model->led_green <= led_increment) {
            model->phase = BlueToRedUp;
            model->led_green = 0;
            return;
        }
        model->led_green -= led_increment;
        break;
    case BlueToRedUp:
        if(model->led_red >= 255 - led_increment) {
            model->phase = BlueDownToRed;
            model->led_red = 255;
            return;
        }
        model->led_red += led_increment;
        break;
    case BlueDownToRed:
        if(model->led_blue <= led_increment) {
            model->phase = RedToGreenUp;
            model->led_blue = 0;
            return;
        }
        model->led_blue -= led_increment;
        break;
    default:
        break;
    }
}

static inline void tick(AppView* view, Model* model) {
    model_tick(model);
    const NotificationMessage led_red = {
        .type = NotificationMessageTypeLedRed,
        .data.led.value = model->led_red,
    };
    const NotificationMessage led_green = {
        .type = NotificationMessageTypeLedGreen,
        .data.led.value = model->led_green,
    };
    const NotificationMessage led_blue = {
        .type = NotificationMessageTypeLedBlue,
        .data.led.value = model->led_blue,
    };
    const NotificationSequence led_state = {
        &led_red,
        &led_green,
        &led_blue,
        &message_do_not_reset,
        NULL,
    };
    notification_message(view->app->notifications, &led_state);
}

static void handle_timer(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    with_view_model(
        view->view, Model * model, { tick(view, model); }, true);
}

static inline void model_init(Model* model) {
    model->phase = RedToGreenUp;
    model->led_red = 255;
    model->led_green = 0;
    model->led_blue = 0;
}

static void handle_alloc(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    view->localctx = malloc(sizeof(LocalCtx));
    LocalCtx* localctx = view->localctx;
    view_allocate_model(view->view, ViewModelTypeLocking, sizeof(Model));
    with_view_model(
        view->view, Model * model, { model_init(model); }, true);
    localctx->timer = furi_timer_alloc(handle_timer, FuriTimerTypePeriodic, view);
}

static void handle_free(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    LocalCtx* localctx = view->localctx;
    furi_timer_stop(localctx->timer);
    furi_timer_free(localctx->timer);
    free(view->localctx);
}

static void handle_enter(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    uint32_t hz = furi_kernel_get_tick_frequency();
    LocalCtx* localctx = (LocalCtx*)view->localctx;
    furi_timer_start(localctx->timer, hz / fps);
}

static void handle_exit(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    LocalCtx* localctx = (LocalCtx*)view->localctx;
    furi_timer_stop(localctx->timer);
    notification_message(view->app->notifications, &sequence_reset_rgb);
}

static uint32_t handle_back(void* ctx) {
    UNUSED(ctx);
    return VIEW_NONE;
}

static bool handle_input(InputEvent* event, void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    if(event->type == InputTypeShort && event->key == InputKeyOk) {
        view_dispatcher_switch_to_view(view->app->view_dispatcher, VIEW_NONE);
        return true;
    }
    return false;
}

static void handle_draw(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    Model* model = ctx;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "LED Rainbow");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 25, "R");
    canvas_draw_str(canvas, 0, 40, "G");
    canvas_draw_str(canvas, 0, 55, "B");
    canvas_set_font(canvas, FontBigNumbers);
    FuriString* buffer = furi_string_alloc();
    furi_string_printf(buffer, "%d", model->led_red);
    canvas_draw_str_aligned(canvas, 10, 20, AlignLeft, AlignCenter, furi_string_get_cstr(buffer));
    furi_string_printf(buffer, "%d", model->led_green);
    canvas_draw_str_aligned(canvas, 10, 35, AlignLeft, AlignCenter, furi_string_get_cstr(buffer));
    furi_string_printf(buffer, "%d", model->led_blue);
    canvas_draw_str_aligned(canvas, 10, 50, AlignLeft, AlignCenter, furi_string_get_cstr(buffer));
    furi_string_free(buffer);
}

ViewConfig view_led_rainbow_config = {
    .id = ViewLEDRainbow,
    .handle_alloc = handle_alloc,
    .handle_free = handle_free,
    .handle_enter = handle_enter,
    .handle_exit = handle_exit,
    .handle_back = handle_back,
    .handle_input = handle_input,
    .handle_draw = handle_draw,
};
