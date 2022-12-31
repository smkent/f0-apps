#include "views.h"

static uint32_t handle_back(void* ctx) {
    UNUSED(ctx);
    return ViewMenu;
}

static bool handle_input(InputEvent* event, void* ctx) {
    UNUSED(ctx);
    UNUSED(event);
    return false;
}

static void handle_draw_one(Canvas* const canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, "View One");
}

static void handle_draw_two(Canvas* const canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 16, AlignCenter, AlignCenter, "View Two");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 48, AlignCenter, AlignCenter, "Bonus Text");
}

ViewConfig view_one_config = {
    .id = ViewOne,
    .handle_back = handle_back,
    .handle_input = handle_input,
    .handle_draw = handle_draw_one,
};

ViewConfig view_two_config = {
    .id = ViewTwo,
    .handle_back = handle_back,
    .handle_input = handle_input,
    .handle_draw = handle_draw_two,
};
