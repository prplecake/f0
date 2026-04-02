#include "lirc_converter.h"

static bool lirc_converter_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    LIRCConverter* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool lirc_converter_back_event_callback(void* context) {
    furi_assert(context);
    LIRCConverter* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

LIRCConverter* lirc_converter_alloc() {
    LIRCConverter* app = malloc(sizeof(LIRCConverter));
    app->gui = furi_record_open(RECORD_GUI);

    // Scene additions
    app->view_dispatcher = view_dispatcher_alloc();

    app->scene_manager = scene_manager_alloc(&lirc_converter_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, lirc_converter_back_event_callback);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, lirc_converter_custom_event_callback);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, LIRCConverterViewIdWidget, widget_get_view(app->widget));

    app->input_file_path = furi_string_alloc();
    app->output_file_path = furi_string_alloc();

    return app;
}

void lirc_converter_app_free(LIRCConverter* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    // View dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, LIRCConverterViewIdWidget);
    widget_free(app->widget);

    view_dispatcher_free(app->view_dispatcher);

    furi_string_free(app->input_file_path);
    furi_string_free(app->output_file_path);

    furi_record_close(RECORD_GUI);
    free(app);
}

extern int32_t lirc_converter_app(void* p) {
    LIRCConverter* app = lirc_converter_alloc();

    do {
        if(p && strlen(p)) {
            furi_string_set(app->input_file_path, (const char*)p);
        } else {
            furi_string_set(app->input_file_path, LIRC_CONVERTER_APP_PATH_FOLDER);

            DialogsFileBrowserOptions browser_options;
            dialog_file_browser_set_basic_options(
                &browser_options, LIRC_CONVERTER_APP_EXTENSION, &I_text_10px);
            browser_options.hide_ext = false;

            DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
            bool res = dialog_file_browser_show(
                dialogs, app->input_file_path, app->input_file_path, &browser_options);

            furi_record_close(RECORD_DIALOGS);
            if(!res) {
                break;
            }
        }

        scene_manager_next_scene(app->scene_manager, LIRCConverterSceneConvert);
        view_dispatcher_run(app->view_dispatcher);
    } while(false);

    lirc_converter_app_free(app);

    return 0;
}
