#include "my_calendar.h"

static lv_obj_t * calendar_scr;
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
            lv_obj_delete_async(calendar_scr);
        }
    }
}

void create_calendar_app(lv_group_t * g, lv_obj_t * parent_scr) {
    old_scr = parent_scr;
    input_group = g;

    calendar_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(calendar_scr, lv_color_hex(0xEEEEEE), 0);

    lv_group_t * app_group = lv_group_create();
    lv_indev_set_group(lv_indev_active(), app_group);

    lv_obj_t * calendar = lv_calendar_create(calendar_scr);
    lv_obj_set_size(calendar, LV_PCT(100), LV_PCT(100));
    lv_obj_center(calendar);

    // Some basic setup
    lv_calendar_set_today_date(calendar, 2026, 7, 5);
    lv_calendar_set_showed_date(calendar, 2026, 7);

    // Include headers
    lv_calendar_header_arrow_create(calendar);

    lv_group_add_obj(app_group, calendar);
    lv_group_focus_obj(calendar);

    lv_obj_add_event_cb(calendar_scr, exit_event_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(calendar, exit_event_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(app_group, calendar_scr);

    lv_screen_load(calendar_scr);
}
