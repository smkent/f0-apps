#include "view_sprite_sandbox.h"
#include "sprite_sandbox_icons.h"

#include <gui/view.h>

#include "sprite_walk.h"

static const uint8_t sprite_dim = 16;
static const uint8_t max_x = 128 / sprite_dim;
static const uint8_t max_y = 64 / sprite_dim;

typedef struct {
    FuriTimer* timer;
} LocalCtx;

typedef struct {
    SpriteWalk* sprite_walk;
    bool anim;
    uint8_t x;
    uint8_t y;
} Model;

static inline void model_init(Model* model) {
    model->sprite_walk = sprite_walk_alloc();
    model->anim = false;
    model->x = 0;
    model->y = 0;
    UNUSED(max_x);
    UNUSED(max_y);
}

static void handle_timer(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    UNUSED(view);
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

static uint32_t handle_back(void* ctx) {
    UNUSED(ctx);
    return VIEW_NONE;
}

static bool handle_input_update_model(InputEvent* event, Model* model) {
    if(event->type == InputTypePress) {
        SpriteWalkDirection walk_direction;
        switch(event->key) {
        case InputKeyDown:
            walk_direction = SpriteWalkDown;
            break;
        case InputKeyUp:
            walk_direction = SpriteWalkUp;
            break;
        case InputKeyLeft:
            walk_direction = SpriteWalkLeft;
            break;
        case InputKeyRight:
            walk_direction = SpriteWalkRight;
            break;
        default:
            return false;
            break;
        }
        model->anim = true;
        sprite_walk_set_direction(model->sprite_walk, walk_direction);
        sprite_walk_animation_start(model->sprite_walk);
        return true;
    } else if(event->type == InputTypeRelease) {
        model->anim = false;
        sprite_walk_animation_stop(model->sprite_walk);
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
    sprite_walk_draw(canvas, model->x * sprite_dim, model->y * sprite_dim, model->sprite_walk);
}

ViewConfig view_sprite_sandbox_config = {
    .id = ViewSpriteSandbox,
    .handle_alloc = handle_alloc,
    .handle_free = handle_free,
    .handle_back = handle_back,
    .handle_input = handle_input,
    .handle_draw = handle_draw,
};
