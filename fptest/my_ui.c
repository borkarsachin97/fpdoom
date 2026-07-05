#include "my_ui.h"
#include "my_snake.h"
#include "my_pushbox.h"

// Define a style for the icons
static lv_style_t style_icon;
static lv_style_t style_icon_pr;
static lv_style_t style_icon_focus;

static lv_group_t * main_group;

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = (lv_obj_t*)lv_event_get_target(e);

    if(code == LV_EVENT_CLICKED) {
        // We stored the app index in the user_data
        int app_id = (int)(intptr_t)lv_event_get_user_data(e);

        if (app_id == 0) {
            // Launch Snake
            create_snake_game(main_group, lv_screen_active());
        } else if (app_id == 1) {
            // Launch Pushbox
            create_pushbox_game(main_group, lv_screen_active());
        } else {
            LV_LOG_USER("Other icon clicked");
        }
    }
}

void create_phone_ui(lv_group_t * g)
{
    main_group = g;

    // Screen background (wallpaper)
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x202020), 0); // Dark grey wallpaper
    lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, 0);

    // Create a status bar at the top
    lv_obj_t * status_bar = lv_obj_create(scr);
    lv_obj_set_size(status_bar, LV_PCT(100), 30);
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(status_bar, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(status_bar, LV_OPA_50, 0);
    lv_obj_set_style_border_width(status_bar, 0, 0);
    lv_obj_set_style_radius(status_bar, 0, 0);
    lv_obj_set_style_pad_all(status_bar, 5, 0);

    // Status bar contents
    lv_obj_t * time_label = lv_label_create(status_bar);
    lv_label_set_text(time_label, "12:00");
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * battery_label = lv_label_create(status_bar);
    lv_label_set_text(battery_label, LV_SYMBOL_BATTERY_FULL);
    lv_obj_set_style_text_color(battery_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(battery_label, LV_ALIGN_RIGHT_MID, -5, 0);

    lv_obj_t * signal_label = lv_label_create(status_bar);
    lv_label_set_text(signal_label, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(signal_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(signal_label, LV_ALIGN_LEFT_MID, 5, 0);


    // Initialize styles for app icons
    lv_style_init(&style_icon);
    lv_style_set_radius(&style_icon, 8); // smaller radius
    lv_style_set_bg_opa(&style_icon, LV_OPA_COVER);
    lv_style_set_border_width(&style_icon, 1); // smaller border
    lv_style_set_border_color(&style_icon, lv_color_hex(0xFFFFFF));
    lv_style_set_border_opa(&style_icon, LV_OPA_30);
    lv_style_set_shadow_width(&style_icon, 5); // smaller shadow
    lv_style_set_shadow_color(&style_icon, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&style_icon, LV_OPA_30);
    lv_style_set_text_color(&style_icon, lv_color_hex(0xFFFFFF));

    // Pressed style
    lv_style_init(&style_icon_pr);
    lv_style_set_bg_opa(&style_icon_pr, LV_OPA_80);
    lv_style_set_transform_scale(&style_icon_pr, 240); // slightly smaller when pressed

    // Focused style (for keypad nav)
    lv_style_init(&style_icon_focus);
    lv_style_set_border_color(&style_icon_focus, lv_color_hex(0x00FF00)); // Green border when focused
    lv_style_set_border_opa(&style_icon_focus, LV_OPA_COVER);
    lv_style_set_transform_scale(&style_icon_focus, 270); // slightly larger when focused

    // Create a flex container for the grid of apps
    lv_obj_t * app_grid = lv_obj_create(scr);
    lv_obj_set_size(app_grid, LV_PCT(100), LV_PCT(100) - 30);
    lv_obj_align(app_grid, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(app_grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(app_grid, 0, 0);

    // Set flex layout
    lv_obj_set_layout(app_grid, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(app_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(app_grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_obj_set_style_pad_all(app_grid, 2, 0); // reduced padding
    lv_obj_set_style_pad_row(app_grid, 5, 0); // reduced row gap

    // App definitions
    const char * app_icons[] = {
        LV_SYMBOL_PLAY,     // Snake
        LV_SYMBOL_DUMMY,    // Pushbox
        LV_SYMBOL_CALL,
        LV_SYMBOL_DIRECTORY,
        LV_SYMBOL_IMAGE,
        LV_SYMBOL_SETTINGS,
        LV_SYMBOL_AUDIO,
        LV_SYMBOL_VIDEO,
        LV_SYMBOL_ENVELOPE
    };

    uint32_t app_colors[] = {
        0x00FF00, // Snake - Green
        0x8B4513, // Pushbox - Brown
        0x4CAF50, // Call - Green
        0x2196F3, // Contacts - Blue
        0xFFC107, // Gallery - Yellow
        0x9E9E9E, // Settings - Grey
        0x9C27B0, // Music - Purple
        0xF44336, // Video - Red
        0x00BCD4  // Messages - Cyan
    };

    const char * app_names[] = {
        "Snake", "Pushbox", "Call", "Contacts",
        "Gallery", "Settings", "Music", "Video", "Messages"
    };

    // Create the apps
    for(int i = 0; i < 9; i++) {
        // App container (to hold icon and label)
        lv_obj_t * app_cont = lv_obj_create(app_grid);
        lv_obj_set_size(app_cont, 38, 55); // smaller container
        lv_obj_set_style_bg_opa(app_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(app_cont, 0, 0);
        lv_obj_set_style_pad_all(app_cont, 0, 0);

        // The interactive button (the icon itself)
        lv_obj_t * btn = lv_btn_create(app_cont);
        lv_obj_set_size(btn, 32, 32); // smaller icon
        lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 0);

        // Apply styles
        lv_obj_add_style(btn, &style_icon, 0);
        lv_obj_add_style(btn, &style_icon_pr, LV_STATE_PRESSED);
        lv_obj_add_style(btn, &style_icon_focus, LV_STATE_FOCUSED);

        // Set specific color for this app
        lv_obj_set_style_bg_color(btn, lv_color_hex(app_colors[i]), 0);

        // Add symbol
        lv_obj_t * symbol = lv_label_create(btn);
        lv_label_set_text(symbol, app_icons[i]);
        lv_obj_align(symbol, LV_ALIGN_CENTER, 0, 0);

        // Add app name label
        lv_obj_t * name_label = lv_label_create(app_cont);
        lv_label_set_text(name_label, app_names[i]);
        lv_obj_set_style_text_color(name_label, lv_color_hex(0xFFFFFF), 0);
        lv_obj_align(name_label, LV_ALIGN_BOTTOM_MID, 0, 0);

        // Add to group for keypad navigation
        lv_group_add_obj(g, btn);

        // Add event
        lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    }
}
