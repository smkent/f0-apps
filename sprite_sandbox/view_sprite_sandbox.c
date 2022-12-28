#include "view_sprite_sandbox.h"
#include "sprite_sandbox_icons.h"

#include <gui/view.h>

static const uint8_t sprite_dim = 16;
static const uint8_t max_x = 128 / sprite_dim;
static const uint8_t max_y = 64 / sprite_dim;

typedef struct {
    FuriTimer* timer;
} LocalCtx;

enum WalkDirection {
    WalkDown,
    WalkUp,
    WalkLeft,
    WalkRight,
};

typedef struct {
    IconAnimation* anim_walk_active;
    IconAnimation* anim_walk_up;
    IconAnimation* anim_walk_down;
    IconAnimation* anim_walk_left;
    IconAnimation* anim_walk_right;
    bool anim;
    uint8_t x;
    uint8_t y;
    enum WalkDirection walk_direction;
} Model;

static inline void model_init(Model* model) {
    model->anim_walk_up = icon_animation_alloc(&A_PokemonPlayerUp_16);
    model->anim_walk_down = icon_animation_alloc(&A_PokemonPlayerDown_16);
    model->anim_walk_left = icon_animation_alloc(&A_PokemonPlayerLeft_16);
    model->anim_walk_right = icon_animation_alloc(&A_PokemonPlayerRight_16);
    model->anim_walk_active = model->anim_walk_down;
    model->anim = false;
    model->x = 0;
    model->y = 0;
    model->walk_direction = WalkDown;
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
        view->view,
        Model * model,
        {
            icon_animation_free(model->anim_walk_up);
            icon_animation_free(model->anim_walk_down);
            icon_animation_free(model->anim_walk_left);
            icon_animation_free(model->anim_walk_right);
        },
        false);
}

static uint32_t handle_back(void* ctx) {
    UNUSED(ctx);
    return VIEW_NONE;
}

static bool handle_input_update_model(InputEvent* event, Model* model) {
    IconAnimation* anim_new;
    if(event->type == InputTypePress) {
        enum WalkDirection walk_direction;
        switch(event->key) {
        case InputKeyDown:
            walk_direction = WalkDown;
            anim_new = model->anim_walk_down;
            break;
        case InputKeyUp:
            walk_direction = WalkUp;
            anim_new = model->anim_walk_up;
            break;
        case InputKeyLeft:
            walk_direction = WalkLeft;
            anim_new = model->anim_walk_left;
            break;
        case InputKeyRight:
            walk_direction = WalkRight;
            anim_new = model->anim_walk_right;
            break;
        default:
            return false;
            break;
        }
        model->anim = true;
        model->walk_direction = walk_direction;
        if(model->anim_walk_active && model->anim_walk_active != anim_new) {
            icon_animation_stop(model->anim_walk_active);
        }
        model->anim_walk_active = anim_new;
        icon_animation_start(model->anim_walk_active);
        return true;
    } else if(event->type == InputTypeRelease) {
        model->anim = false;
        if(model->anim_walk_active) {
            icon_animation_stop(model->anim_walk_active);
        }
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
    canvas_draw_icon_animation(
        canvas, model->x * sprite_dim, model->y * sprite_dim, model->anim_walk_active);
}

ViewConfig view_sprite_sandbox_config = {
    .id = ViewSpriteSandbox,
    .handle_alloc = handle_alloc,
    .handle_free = handle_free,
    .handle_back = handle_back,
    .handle_input = handle_input,
    .handle_draw = handle_draw,
};
