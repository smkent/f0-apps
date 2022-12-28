#include "view_sprite_sandbox.h"
#include "sprite_sandbox_icons.h"

#include <gui/view.h>

#include "sprite_walk.h"

static const uint8_t fps = 5;
static const uint8_t sprite_dim = 16;
static const uint8_t max_x = 128 / sprite_dim;
static const uint8_t max_y = 64 / sprite_dim;

typedef struct {
    FuriTimer* timer;
} LocalCtx;

typedef enum {
    MoveNone = 0,
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
} MoveDirection;

typedef struct {
    SpriteWalk* sprite_walk;
    uint8_t x;
    uint8_t y;
    int8_t sprite_offset_x;
    int8_t sprite_offset_y;
    uint8_t move_remaining;
    MoveDirection move_direction;
    MoveDirection move_next;
} Model;

static inline void model_init(Model* model) {
    model->sprite_walk = sprite_walk_alloc();
    model->x = 0;
    model->y = 0;
    model->sprite_offset_x = 0;
    model->sprite_offset_y = 0;
    model->move_remaining = 0;
    model->move_direction = MoveNone;
    model->move_next = MoveNone;
}

static inline void model_tick(Model* model) {
    if(model->move_remaining > 0) {
        switch(model->move_direction) {
        case MoveNone:
            break;
        case MoveUp:
            model->sprite_offset_y -= 1;
            break;
        case MoveDown:
            model->sprite_offset_y += 1;
            break;
        case MoveLeft:
            model->sprite_offset_x -= 1;
            break;
        case MoveRight:
            model->sprite_offset_x += 1;
            break;
        }
        model->move_remaining -= 1;
        if(model->move_remaining == 0) {
            model->move_direction = MoveNone;
        } else {
            return;
        }
    }
    if(model->move_next) {
        switch(model->move_next) {
        case MoveNone:
            break;
        case MoveUp:
            sprite_walk_set_direction(model->sprite_walk, SpriteWalkUp);
            if(model->y <= 0) {
                model->move_next = MoveNone;
                break;
            }
            model->y -= 1;
            model->sprite_offset_y = 15;
            break;
        case MoveDown:
            sprite_walk_set_direction(model->sprite_walk, SpriteWalkDown);
            if(model->y >= max_y) {
                model->move_next = MoveNone;
                break;
            }
            model->y += 1;
            model->sprite_offset_y = -15;
            break;
        case MoveLeft:
            sprite_walk_set_direction(model->sprite_walk, SpriteWalkLeft);
            if(model->x <= 0) {
                model->move_next = MoveNone;
                break;
            }
            model->x -= 1;
            model->sprite_offset_x = 15;
            break;
        case MoveRight:
            sprite_walk_set_direction(model->sprite_walk, SpriteWalkRight);
            if(model->x >= max_x) {
                model->move_next = MoveNone;
                break;
            }
            model->x += 1;
            model->sprite_offset_x = -15;
            break;
        }
        model->move_direction = model->move_next;
        if(model->move_direction != MoveNone) {
            model->move_remaining = 15;
        }
    }
    switch(model->move_direction) {
    case MoveNone:
        sprite_walk_animation_stop(model->sprite_walk);
        break;
    default:
        sprite_walk_animation_start(model->sprite_walk);
        break;
    }
}

static void handle_timer(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    with_view_model(
        view->view, Model * model, { model_tick(model); }, true);
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
    with_view_model(
        view->view, Model * model, { sprite_walk_free(model->sprite_walk); }, false);
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

static bool handle_input_update_model(InputEvent* event, Model* model) {
    if(event->type == InputTypePress) {
        switch(event->key) {
        case InputKeyDown:
            model->move_next = MoveDown;
            break;
        case InputKeyUp:
            model->move_next = MoveUp;
            break;
        case InputKeyLeft:
            model->move_next = MoveLeft;
            break;
        case InputKeyRight:
            model->move_next = MoveRight;
            break;
        default:
            return false;
            break;
        }
        return true;
    } else if(event->type == InputTypeRelease) {
        model->move_next = MoveNone;
        return true;
    }
    return false;
}

static bool handle_input(InputEvent* event, void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    bool consumed = false;
    with_view_model(
        view->view, Model * model, { consumed = handle_input_update_model(event, model); }, true);
    return consumed;
}

static void handle_draw(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    Model* model = ctx;
    canvas_clear(canvas);
    sprite_walk_draw(
        canvas,
        model->x * sprite_dim + model->sprite_offset_x,
        model->y * sprite_dim + model->sprite_offset_y,
        model->sprite_walk);
}

ViewConfig view_sprite_sandbox_config = {
    .id = ViewSpriteSandbox,
    .handle_alloc = handle_alloc,
    .handle_free = handle_free,
    .handle_enter = handle_enter,
    .handle_exit = handle_exit,
    .handle_back = handle_back,
    .handle_input = handle_input,
    .handle_draw = handle_draw,
};
