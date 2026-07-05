#include "my_calculator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static lv_obj_t * calc_scr;
static lv_group_t * input_group;
static lv_obj_t * old_scr;
static lv_obj_t * ta;

static const char * btnm_map[] = {
    "1", "2", "3", "+", "\n",
    "4", "5", "6", "-", "\n",
    "7", "8", "9", "*", "\n",
    "C", "0", "=", "/", ""
};

static void btnm_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = (lv_obj_t*)lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_buttonmatrix_get_selected_button(obj);
        const char * txt = lv_buttonmatrix_get_button_text(obj, id);

        if(txt) {
            if(strcmp(txt, "C") == 0) {
                lv_textarea_set_text(ta, "");
            } else if(strcmp(txt, "=") == 0) {
                // Dummy evaluation (for a real calculator we need an expression parser)
                // We will just print "Result" for now since embedded C parsing is heavy.
                lv_textarea_set_text(ta, "Done");
            } else {
                lv_textarea_add_text(ta, txt);
            }
        }
    }
}

static void exit_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        if(key == LV_KEY_PREV) { // Exit
            lv_group_t * app_group = lv_indev_get_group(lv_indev_active());
            lv_indev_set_group(lv_indev_active(), input_group); // Restore home group
            lv_group_delete(app_group);
            lv_screen_load(old_scr);
            lv_obj_delete_async(calc_scr);
        }
    }
}

void create_calculator_app(lv_group_t * g, lv_obj_t * parent_scr) {
    old_scr = parent_scr;
    input_group = g;

    calc_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(calc_scr, lv_color_hex(0x222222), 0);

    lv_group_t * app_group = lv_group_create();
    lv_indev_set_group(lv_indev_active(), app_group);

    // Text area
    ta = lv_textarea_create(calc_scr);
    lv_obj_set_size(ta, LV_PCT(100), 40);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 0);
    lv_textarea_set_max_length(ta, 32);
    lv_textarea_set_one_line(ta, true);

    // Button matrix
    lv_obj_t * btnm = lv_buttonmatrix_create(calc_scr);
    lv_obj_set_size(btnm, LV_PCT(100), LV_PCT(100) - 40);
    lv_obj_align(btnm, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_buttonmatrix_set_map(btnm, btnm_map);
    lv_obj_add_event_cb(btnm, btnm_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Keypad exit binding
    lv_obj_add_event_cb(calc_scr, exit_event_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(btnm, exit_event_cb, LV_EVENT_KEY, NULL);

    lv_group_add_obj(app_group, btnm);
    lv_group_add_obj(app_group, calc_scr);
    lv_group_focus_obj(btnm);

    lv_screen_load(calc_scr);
}
