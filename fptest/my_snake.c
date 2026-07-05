#include "my_snake.h"
#include <stdlib.h>

#define GRID_W 12
#define GRID_H 15
#define CELL_SIZE 10

static lv_obj_t * game_scr;
static lv_timer_t * timer;
static lv_group_t * input_group;
static lv_obj_t * old_scr;

static lv_obj_t * game_container;
static lv_obj_t * snake_objs[100];
static lv_obj_t * food_obj;

static int snake_x[100];
static int snake_y[100];
static int snake_len;
static int dir_x;
static int dir_y;
static int food_x;
static int food_y;
static int game_over;

static void spawn_food(void) {
    food_x = rand() % GRID_W;
    food_y = rand() % GRID_H;
    lv_obj_set_pos(food_obj, food_x * CELL_SIZE, food_y * CELL_SIZE);
}

static void draw_game(void) {
    for (int i = 0; i < snake_len; i++) {
        if (!snake_objs[i]) {
            snake_objs[i] = lv_obj_create(game_container);
            lv_obj_set_size(snake_objs[i], CELL_SIZE, CELL_SIZE);
            lv_obj_set_style_bg_color(snake_objs[i], lv_color_hex(0x00FF00), 0);
            lv_obj_set_style_border_width(snake_objs[i], 0, 0);
            lv_obj_set_style_radius(snake_objs[i], 0, 0);
        }
        lv_obj_set_pos(snake_objs[i], snake_x[i] * CELL_SIZE, snake_y[i] * CELL_SIZE);
    }
}

static void game_timer_cb(lv_timer_t * t) {
    (void)t; // unused
    if (game_over) return;

    // Move body
    for (int i = snake_len - 1; i > 0; i--) {
        snake_x[i] = snake_x[i-1];
        snake_y[i] = snake_y[i-1];
    }

    // Move head
    snake_x[0] += dir_x;
    snake_y[0] += dir_y;

    // Wrap around
    if (snake_x[0] < 0) snake_x[0] = GRID_W - 1;
    if (snake_x[0] >= GRID_W) snake_x[0] = 0;
    if (snake_y[0] < 0) snake_y[0] = GRID_H - 1;
    if (snake_y[0] >= GRID_H) snake_y[0] = 0;

    // Collision with self
    for (int i = 1; i < snake_len; i++) {
        if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i]) {
            game_over = 1;
        }
    }

    // Eat food
    if (snake_x[0] == food_x && snake_y[0] == food_y) {
        if (snake_len < 100) snake_len++;
        spawn_food();
    }

    draw_game();
}

static void game_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        if (key == LV_KEY_UP && dir_y == 0) { dir_x = 0; dir_y = -1; }
        else if (key == LV_KEY_DOWN && dir_y == 0) { dir_x = 0; dir_y = 1; }
        else if (key == LV_KEY_LEFT && dir_x == 0) { dir_x = -1; dir_y = 0; }
        else if (key == LV_KEY_RIGHT && dir_x == 0) { dir_x = 1; dir_y = 0; }
        else if (key == LV_KEY_ENTER && game_over) {
            // Restart logic
            for(int i=3; i<snake_len; i++) {
                if (snake_objs[i]) {
                    lv_obj_delete(snake_objs[i]); // Revert to sync for restart, safe here
                    snake_objs[i] = NULL;
                }
            }
            snake_len = 3;
            snake_x[0] = GRID_W/2; snake_y[0] = GRID_H/2;
            for(int i=1; i<3; i++) { snake_x[i] = snake_x[0]-i; snake_y[i] = snake_y[0]; }
            dir_x = 1; dir_y = 0;
            game_over = 0;
            spawn_food();
            draw_game();
        }
        else if (key == LV_KEY_PREV) {
            // Exit
            lv_timer_delete(timer);
            lv_group_t * game_group = lv_indev_get_group(lv_indev_active());
            lv_indev_set_group(lv_indev_active(), input_group); // Restore home group
            lv_group_delete(game_group); // Prevent memory leak
            lv_screen_load(old_scr);
            lv_obj_delete_async(game_scr);
        }
    }
}

void create_snake_game(lv_group_t * g, lv_obj_t * parent_scr) {
    old_scr = parent_scr;
    input_group = g;

    game_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(game_scr, lv_color_hex(0x000000), 0);

    game_container = lv_obj_create(game_scr);
    lv_obj_set_size(game_container, GRID_W * CELL_SIZE, GRID_H * CELL_SIZE);
    lv_obj_center(game_container);
    lv_obj_set_style_bg_color(game_container, lv_color_hex(0x222222), 0);
    lv_obj_set_style_border_width(game_container, 0, 0);
    lv_obj_set_style_pad_all(game_container, 0, 0);

    food_obj = lv_obj_create(game_container);
    lv_obj_set_size(food_obj, CELL_SIZE, CELL_SIZE);
    lv_obj_set_style_bg_color(food_obj, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_border_width(food_obj, 0, 0);
    lv_obj_set_style_radius(food_obj, 0, 0);

    for(int i=0; i<100; i++) snake_objs[i] = NULL;

    snake_len = 3;
    snake_x[0] = GRID_W/2; snake_y[0] = GRID_H/2;
    for(int i=1; i<3; i++) { snake_x[i] = snake_x[0]-i; snake_y[i] = snake_y[0]; }
    dir_x = 1; dir_y = 0;
    game_over = 0;

    spawn_food();
    draw_game();

    // Create a new group exclusively for the game so focus isn't stolen by the home screen
    lv_group_t * game_group = lv_group_create();
    lv_indev_set_group(lv_indev_active(), game_group);

    // Add event handler to the screen itself
    lv_obj_add_event_cb(game_scr, game_event_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(game_group, game_scr);
    lv_group_focus_obj(game_scr);

    lv_screen_load(game_scr);

    timer = lv_timer_create(game_timer_cb, 200, NULL);
}
