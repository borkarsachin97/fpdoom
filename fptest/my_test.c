#include "my_test.h"

static lv_obj_t * test_scr;
static lv_group_t * input_group;
static lv_obj_t * old_scr;

static void dummy_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Dummy test clicked!");
    }
}

static void exit_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        if(key == LV_KEY_PREV) { // Exit
            lv_indev_set_group(lv_indev_active(), input_group); // Restore home group
            lv_screen_load(old_scr);
            lv_obj_delete(test_scr);
        }
    }
}

void create_test_app(lv_group_t * g, lv_obj_t * parent_scr) {
    old_scr = parent_scr;
    input_group = g;

    test_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(test_scr, lv_color_hex(0x202020), 0);

    // Flex container for buttons
    lv_obj_t * cont = lv_obj_create(test_scr);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_center(cont);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Create a new group exclusively for the app so focus isn't stolen by the home screen
    lv_group_t * app_group = lv_group_create();
    lv_indev_set_group(lv_indev_active(), app_group);

    const char * btn_names[] = {"Test RAM", "Test CPU", "Test Flash", "Test Audio", "Test Vibro"};

    for(int i = 0; i < 5; i++) {
        lv_obj_t * btn = lv_btn_create(cont);
        lv_obj_set_size(btn, 100, 20); // Small button

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, btn_names[i]);
        lv_obj_center(label);

        lv_group_add_obj(app_group, btn);
        lv_obj_add_event_cb(btn, dummy_event_cb, LV_EVENT_CLICKED, NULL);
    }

    lv_obj_add_event_cb(test_scr, exit_event_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(app_group, test_scr);

    lv_screen_load(test_scr);
}
