#include "view_sprite_sandbox.h"
#include "sprite_sandbox_icons.h"

#include <gui/view.h>

static const uint32_t fps = 4;
static const uint8_t sprite_dim = 16;
static const uint8_t max_x = 128 / sprite_dim;
static const uint8_t max_y = 64 / sprite_dim;

typedef struct {
    FuriTimer* timer;
} LocalCtx;

enum WalkIcon {
    WalkIconDown1 = 0,
    WalkIconDownL,
    WalkIconDown2,
    WalkIconDownR,
    WalkIconEnd,
};

enum WalkDirection {
    WalkDown,
    WalkUp,
    WalkLeft,
    WalkRight,
};

typedef struct {
    bool anim;
    uint8_t x;
    uint8_t y;
    enum WalkIcon icon_state;
    enum WalkDirection walk_direction;
} Model;

static inline void model_init(Model* model) {
    model->anim = false;
    model->x = 0;
    model->y = 0;
    model->icon_state = WalkIconDown1;
    model->walk_direction = WalkDown;
    UNUSED(max_x);
    UNUSED(max_y);
}

static const Icon* walk_icon(Model* model) {
    enum WalkIcon icon_state = WalkIconDown1;
    if(model->anim) {
        icon_state = model->icon_state;
    }
    switch(model->walk_direction) {
    case WalkDown:
        switch(icon_state) {
        case WalkIconDown1:
        case WalkIconDown2:
            return &I_pokemon_player_down;
        case WalkIconDownL:
            return &I_pokemon_player_down_walk_l;
        case WalkIconDownR:
            return &I_pokemon_player_down_walk_r;
        default:
            return NULL;
        }
        break;
    case WalkUp:
        switch(icon_state) {
        case WalkIconDown1:
        case WalkIconDown2:
            return &I_pokemon_player_up;
        case WalkIconDownL:
            return &I_pokemon_player_up_walk_l;
        case WalkIconDownR:
            return &I_pokemon_player_up_walk_r;
        default:
            return NULL;
        }
        break;
    case WalkLeft:
        switch(icon_state) {
        case WalkIconDown1:
        case WalkIconDown2:
            return &I_pokemon_player_left;
        case WalkIconDownL:
        case WalkIconDownR:
            return &I_pokemon_player_left_walk;
        default:
            return NULL;
        }
        break;
    case WalkRight:
        switch(icon_state) {
        case WalkIconDown1:
        case WalkIconDown2:
            return &I_pokemon_player_right;
        case WalkIconDownL:
        case WalkIconDownR:
            return &I_pokemon_player_right_walk;
        default:
            return NULL;
        }
        break;
    }
    return NULL;
}

static inline void tick(AppView* view, Model* model) {
    UNUSED(view);
    model->icon_state = (model->icon_state + 1) % WalkIconEnd;
}

static void handle_timer(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    with_view_model(
        view->view, Model * model, { tick(view, model); }, true);
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

static uint32_t handle_back(void* ctx) {
    UNUSED(ctx);
    return VIEW_NONE;
}

static bool handle_input(InputEvent* event, void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    LocalCtx* localctx = (LocalCtx*)view->localctx;
    if(event->type == InputTypePress) {
        enum WalkDirection walk_direction;
        switch(event->key) {
        case InputKeyDown:
            walk_direction = WalkDown;
            break;
        case InputKeyUp:
            walk_direction = WalkUp;
            break;
        case InputKeyLeft:
            walk_direction = WalkLeft;
            break;
        case InputKeyRight:
            walk_direction = WalkRight;
            break;
        default:
            return false;
            break;
        }
        with_view_model(
            view->view,
            Model * model,
            {
                model->anim = true;
                model->walk_direction = walk_direction;
                model->icon_state = WalkIconDownL;
            },
            true);
        uint32_t hz = furi_kernel_get_tick_frequency();
        furi_timer_start(localctx->timer, hz / fps);
        return true;
    } else if(event->type == InputTypeRelease) {
        furi_timer_stop(localctx->timer);
        with_view_model(
            view->view, Model * model, { model->anim = false; }, true);
        return true;
    }
    return false;
}

static void handle_draw(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    Model* model = ctx;
    canvas_clear(canvas);
    canvas_draw_icon(canvas, model->x * sprite_dim, model->y * sprite_dim, walk_icon(model));
}

ViewConfig view_sprite_sandbox_config = {
    .id = ViewSpriteSandbox,
    .handle_alloc = handle_alloc,
    .handle_free = handle_free,
    .handle_back = handle_back,
    .handle_input = handle_input,
    .handle_draw = handle_draw,
};
