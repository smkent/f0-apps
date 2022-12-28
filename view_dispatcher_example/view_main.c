#include "view_main.h"

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
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, "Test App");
}

struct ViewConfig view_main_config = {
    .id = ViewMain,
    .handle_back = handle_back,
    .handle_input = handle_input,
    .handle_draw = handle_draw,
};
