#include "view_images.h"
#include "images_example_icons.h"

typedef struct {
    IconAnimation* animation_left;
    IconAnimation* animation_middle;
    IconAnimation* animation_right;
} Model;

static void handle_alloc(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    view_allocate_model(view->view, ViewModelTypeLocking, sizeof(Model));
    with_view_model(
        view->view,
        Model * model,
        {
            model->animation_left = icon_animation_alloc(&A_SaturnRight_16x21);
            model->animation_middle = icon_animation_alloc(&A_Saturn_16x21);
            model->animation_right = icon_animation_alloc(&A_SaturnLeft_16x21);
        },
        true);
}

static void handle_free(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    with_view_model(
        view->view,
        Model * model,
        {
            icon_animation_free(model->animation_left);
            icon_animation_free(model->animation_middle);
            icon_animation_free(model->animation_right);
        },
        false);
}

static void handle_enter(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    with_view_model(
        view->view,
        Model * model,
        {
            icon_animation_start(model->animation_left);
            icon_animation_start(model->animation_middle);
            icon_animation_start(model->animation_right);
        },
        true);
}

static void handle_exit(void* ctx) {
    furi_assert(ctx);
    AppView* view = ctx;
    with_view_model(
        view->view,
        Model * model,
        {
            icon_animation_stop(model->animation_left);
            icon_animation_stop(model->animation_middle);
            icon_animation_stop(model->animation_right);
        },
        false);
}

static uint32_t handle_back(void* ctx) {
    UNUSED(ctx);
    return VIEW_NONE;
}

static bool handle_input(InputEvent* event, void* ctx) {
    UNUSED(ctx);
    UNUSED(event);
    return false;
}

static void handle_draw(Canvas* const canvas, void* ctx) {
    UNUSED(ctx);
    UNUSED(canvas);
    Model* model = ctx;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "Images example");
    canvas_draw_icon_animation(canvas, 23, 16 + 4, model->animation_left);
    canvas_draw_icon(canvas, 23 + (1 * (4 + 16)), 16, &I_SaturnTrash_22x29);
    canvas_draw_icon_animation(canvas, 23 + 26 + (1 * (4 + 16)), 16 + 4, model->animation_middle);
    canvas_draw_icon_animation(canvas, 23 + 26 + (2 * (4 + 16)), 16 + 4, model->animation_right);
}

ViewConfig view_images_config = {
    .id = ViewImages,
    .handle_alloc = handle_alloc,
    .handle_free = handle_free,
    .handle_enter = handle_enter,
    .handle_exit = handle_exit,
    .handle_back = handle_back,
    .handle_input = handle_input,
    .handle_draw = handle_draw,
};
