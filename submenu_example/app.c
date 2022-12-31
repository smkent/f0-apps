#include "app.h"
#include "views.h"

static AppViewState views[] = {
    {
        .config = &view_one_config,
    },
    {
        .config = &view_two_config,
    },
};

static const unsigned views_count = COUNT_OF(views);

static uint32_t app_exit(void* ctx) {
    UNUSED(ctx);
    return VIEW_NONE;
}

static void submenu_callback(void* ctx, uint32_t index) {
    furi_assert(ctx);
    App* app = ctx;
    ViewID view_id = ViewMenu;
    switch(index) {
    case ViewOne:
    case ViewTwo:
        view_id = index;
        break;
    default:
        return;
    }
    view_dispatcher_switch_to_view(app->view_dispatcher, view_id);
}

static void app_views_free(App* app) {
    for(unsigned i = 0; i < views_count; i++) {
        furi_assert(views[i].context);
        view_dispatcher_remove_view(app->view_dispatcher, views[i].config->id);
        if(views[i].config->handle_free && views[i].context) {
            (*views[i].config->handle_free)(views[i].context);
        }
        view_free(views[i].context->view);
        free(views[i].context);
    }
    view_dispatcher_remove_view(app->view_dispatcher, ViewMenu);
    submenu_free(app->submenu);
}

static App* app_views_alloc(App* app) {
    app->submenu = submenu_alloc();
    submenu_add_item(app->submenu, "View One", ViewOne, submenu_callback, app);
    submenu_add_item(app->submenu, "View Two", ViewTwo, submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), app_exit);
    view_dispatcher_add_view(app->view_dispatcher, ViewMenu, submenu_get_view(app->submenu));
    for(unsigned i = 0; i < views_count; i++) {
        views[i].context = malloc(sizeof(AppView));
        views[i].context->view = view_alloc();
        views[i].context->app = app;
        if(views[i].config->handle_alloc) {
            (*views[i].config->handle_alloc)(views[i].context);
        }
        view_set_context(views[i].context->view, views[i].context);
        view_set_enter_callback(views[i].context->view, views[i].config->handle_enter);
        view_set_exit_callback(views[i].context->view, views[i].config->handle_exit);
        view_set_draw_callback(views[i].context->view, views[i].config->handle_draw);
        view_set_input_callback(views[i].context->view, views[i].config->handle_input);
        view_dispatcher_add_view(
            app->view_dispatcher, views[i].config->id, views[i].context->view);
        view_set_previous_callback(views[i].context->view, views[i].config->handle_back);
    }
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewMenu);
    return app;
}

static void app_free(App* app) {
    furi_assert(app);
    app_views_free(app);
    furi_record_close(RECORD_NOTIFICATION);
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
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    return app_views_alloc(app);
}

int32_t app_entry_point(void) {
    srand(DWT->CYCCNT);
    App* app = app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    app_free(app);
    return 0;
}
