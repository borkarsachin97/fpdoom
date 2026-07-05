#include "my_hw_info.h"
#include "syscode.h"
#include <stdio.h>

static lv_obj_t * hw_scr;
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
            lv_obj_delete_async(hw_scr);
        }
    }
}

void create_hw_info_app(lv_group_t * g, lv_obj_t * parent_scr) {
    old_scr = parent_scr;
    input_group = g;

    hw_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(hw_scr, lv_color_hex(0x000000), 0);

    lv_obj_t * label = lv_label_create(hw_scr);
    lv_obj_set_style_text_color(label, lv_color_hex(0x00FF00), 0);
    lv_obj_set_width(label, 120);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 5, 5);

    // Create a new group exclusively for the app so focus isn't stolen by the home screen
    lv_group_t * app_group = lv_group_create();
    lv_indev_set_group(lv_indev_active(), app_group);

    char buf[256];
    snprintf(buf, sizeof(buf),
        "HW INFO\n"
        "Chip ID: %08X\n"
        "Disp W: %d\n"
        "Disp H: %d\n"
        "RAM: %d MB\n"
        "SPI: %X",
        (unsigned int)sys_data.chip_id,
        sys_data.display.w2, sys_data.display.h2,
        (4 << 20) / (1024 * 1024), // Hardcode FIRMWARE_SIZE default fallback 4MB to prevent compile errors
        (unsigned int)sys_data.spi
    );

    lv_label_set_text(label, buf);

    // Handle back button on screen to exit
    lv_obj_add_event_cb(hw_scr, exit_event_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(app_group, hw_scr);
    lv_group_focus_obj(hw_scr);

    lv_screen_load(hw_scr);
}
