#include "my_settings.h"
#include "syscode.h"

static lv_obj_t * settings_scr;
static lv_group_t * input_group;
static lv_obj_t * old_scr;

static void slider_event_cb(lv_event_t * e) {
    lv_obj_t * slider = (lv_obj_t*)lv_event_get_target(e);
    int32_t val = lv_slider_get_value(slider);

    // Call the hardware brightness control (1-100 range)
    sys_data.brightness = val;
    sys_brightness(val);
}

static void exit_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        if(key == LV_KEY_PREV) { // Exit
            lv_indev_set_group(lv_indev_active(), input_group); // Restore home group
            lv_screen_load(old_scr);
            lv_obj_delete(settings_scr);
        }
    }
}

void create_settings_app(lv_group_t * g, lv_obj_t * parent_scr) {
    old_scr = parent_scr;
    input_group = g;

    settings_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(settings_scr, lv_color_hex(0xEEEEEE), 0);

    // Title
    lv_obj_t * title = lv_label_create(settings_scr);
    lv_label_set_text(title, "Display Settings");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Brightness label
    lv_obj_t * bl_label = lv_label_create(settings_scr);
    lv_label_set_text(bl_label, "Backlight");
    lv_obj_align(bl_label, LV_ALIGN_TOP_LEFT, 10, 40);

    // Create a new group exclusively for the app so focus isn't stolen by the home screen
    lv_group_t * app_group = lv_group_create();
    lv_indev_set_group(lv_indev_active(), app_group);

    // Brightness slider
    lv_obj_t * slider = lv_slider_create(settings_scr);
    lv_obj_set_size(slider, 100, 10);
    lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 60);
    lv_slider_set_range(slider, 1, 100);
    lv_slider_set_value(slider, sys_data.brightness, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_group_add_obj(app_group, slider);
    lv_group_focus_obj(slider);

    // Wallpaper label dummy
    lv_obj_t * wp_label = lv_label_create(settings_scr);
    lv_label_set_text(wp_label, "Wallpaper: Default");
    lv_obj_align(wp_label, LV_ALIGN_TOP_LEFT, 10, 90);

    // Handle back button on screen to exit
    lv_obj_add_event_cb(settings_scr, exit_event_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(app_group, settings_scr);

    lv_screen_load(settings_scr);
}
