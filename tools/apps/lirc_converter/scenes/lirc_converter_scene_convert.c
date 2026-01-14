#include "../lirc_converter.h"

#define CONVERT_MAX_FILE_SIZE 8000

void lirc_converter_scene_convert_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    furi_assert(context);
    LIRCConverter* app = (LIRCConverter*)context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void lirc_converter_scene_convert_on_enter(void* context) {
    furi_assert(context);
    LIRCConverter* app = context;

    // Load the file
    FuriString* buffer;
    buffer = furi_string_alloc();

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);

    FileInfo fileinfo;
    FS_Error error =
        storage_common_stat(storage, furi_string_get_cstr(app->input_file_path), &fileinfo);
    if(error == FSE_OK) {
        if((fileinfo.size < CONVERT_MAX_FILE_SIZE) && (fileinfo.size > 2)) {
            bool ok = storage_file_open(
                file, furi_string_get_cstr(app->input_file_path), FSAM_READ, FSOM_OPEN_EXISTING);
            if(ok) {
                FURI_LOG_D(TAG, "File opened successfully");

                FuriString* temp_str = furi_string_alloc();
                furi_string_printf(
                    temp_str,
                    "\e#Converting...\e#\nFile: %s\nSize: %llu bytes",
                    furi_string_get_cstr(app->input_file_path),
                    fileinfo.size);
                widget_add_text_box_element(
                    app->widget,
                    0,
                    0,
                    128,
                    64,
                    AlignLeft,
                    AlignTop,
                    furi_string_get_cstr(temp_str),
                    false);
            } else {
                widget_add_text_box_element(
                    app->widget,
                    0,
                    0,
                    128,
                    64,
                    AlignLeft,
                    AlignCenter,
                    "\e#Error:\nStorage file read error\e#",
                    false);
            }
            storage_file_close(file);
        } else if(fileinfo.size < 2) {
            widget_add_text_box_element(
                app->widget,
                0,
                0,
                128,
                64,
                AlignLeft,
                AlignCenter,
                "\e#Error:\nFile is too small\e#",
                false);
        } else {
            widget_add_text_box_element(
                app->widget,
                0,
                0,
                128,
                64,
                AlignLeft,
                AlignCenter,
                "\e#Error:\nFile is too large\e#",
                false);
        }
    } else {
        widget_add_text_box_element(
            app->widget,
            0,
            0,
            128,
            64,
            AlignLeft,
            AlignCenter,
            "\e#Error:\nFile system error\e#",
            false);
    }

    furi_string_free(buffer);

    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    view_dispatcher_switch_to_view(app->view_dispatcher, LIRCConverterViewIdWidget);
}

bool lirc_converter_scene_convert_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    LIRCConverter* app = (LIRCConverter*)context;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        scene_manager_previous_scene(app->scene_manager);
        return true;
    default:
        return false;
    }
}

void lirc_converter_scene_convert_on_exit(void* context) {
    furi_assert(context);
    LIRCConverter* app = (LIRCConverter*)context;

    widget_reset(app->widget);
}