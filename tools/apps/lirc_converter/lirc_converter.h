#pragma once

#include <dialogs/dialogs.h>
#include <gui/gui.h>
#include <gui/modules/widget.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <bit_lib/bit_lib.h>
#include "lirc_converter_icons.h"
#include "scenes/lirc_converter_scene.h"

#include <storage/storage.h>

#define TAG "LIRC Converter"

#define LIRC_CONVERTER_APP_PATH_FOLDER STORAGE_EXT_PATH_PREFIX
#define LIRC_CONVERTER_APP_EXTENSION "*"

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Widget* widget;

    FuriString* input_file_path;
    FuriString* output_file_path;
} LIRCConverter;

typedef enum {
    LIRCConverterViewIdWidget,
} LircConverterViewId;
