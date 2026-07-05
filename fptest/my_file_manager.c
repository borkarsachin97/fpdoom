#include "my_file_manager.h"

static lv_obj_t * fm_scr;
static lv_group_t * input_group;
static lv_obj_t * old_scr;

static void exit_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        if(key == LV_KEY_PREV) { // Exit
            lv_group_t * app_group = lv_indev_get_group(lv_indev_active());
            lv_indev_set_group(lv_indev_active(), input_group); // Restore home group
            lv_group_delete(app_group);
            lv_screen_load(old_scr);
            lv_obj_delete_async(fm_scr);
        }
    }
}

void create_file_manager_app(lv_group_t * g, lv_obj_t * parent_scr) {
    old_scr = parent_scr;
    input_group = g;

    fm_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(fm_scr, lv_color_hex(0xEEEEEE), 0);

    lv_group_t * app_group = lv_group_create();
    lv_indev_set_group(lv_indev_active(), app_group);

    // File explorer
#if LV_USE_FILE_EXPLORER
    lv_obj_t * file_explorer = lv_file_explorer_create(fm_scr);
    lv_obj_set_size(file_explorer, LV_PCT(100), LV_PCT(100));
    lv_obj_center(file_explorer);
    lv_group_add_obj(app_group, file_explorer);
    lv_obj_add_event_cb(file_explorer, exit_event_cb, LV_EVENT_KEY, NULL);
    lv_group_focus_obj(file_explorer);
#else
    lv_obj_t * label = lv_label_create(fm_scr);
    lv_label_set_text(label, "File Explorer Disabled");
    lv_obj_center(label);
#endif

    lv_obj_add_event_cb(fm_scr, exit_event_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(app_group, fm_scr);

    lv_screen_load(fm_scr);
}
