#include "lvgl.h"
#include "syscode.h"
#include "cmd_def.h"

// Convert fpdoom keycodes to LVGL keycodes
static uint32_t keycode_to_lvgl(int key) {
    switch (key) {
        case 0x04: return LV_KEY_UP;
        case 0x05: return LV_KEY_DOWN;
        case 0x06: return LV_KEY_LEFT;
        case 0x07: return LV_KEY_RIGHT;
        case 0x0d: return LV_KEY_ENTER; // CENTER
        case 0x08: return LV_KEY_PREV;  // LSOFT
        case 0x09: return LV_KEY_NEXT;  // RSOFT

        case 0x30: return '0';
        case 0x31: return '1';
        case 0x32: return '2';
        case 0x33: return '3';
        case 0x34: return '4';
        case 0x35: return '5';
        case 0x36: return '6';
        case 0x37: return '7';
        case 0x38: return '8';
        case 0x39: return '9';

        case 0x2a: return '*';
        case 0x23: return '#';

        default: return 0;
    }
}

static uint32_t last_key = 0;
static lv_indev_state_t last_state = LV_INDEV_STATE_RELEASED;

// Keypad read callback for LVGL
void my_keypad_read(lv_indev_t * indev_drv, lv_indev_data_t * data) {
    int key_event;
    int type = sys_event(&key_event);

    if (type == EVENT_KEYDOWN || type == EVENT_KEYUP) {
        int key_val = key_event & 0x7fff;
        last_key = keycode_to_lvgl(key_val);
        last_state = (type == EVENT_KEYDOWN) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    }

    data->key = last_key;
    data->state = last_state;

    // Process multiple events if available
    if (type != EVENT_END) {
        data->continue_reading = true;
    }
}
