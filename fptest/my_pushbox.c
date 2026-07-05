#include "my_pushbox.h"

#define MAP_W 12
#define MAP_H 15
#define TILE_SIZE 10

// 0: empty, 1: wall, 2: box, 3: target, 4: box on target
static int map[MAP_H][MAP_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,1,0,0,0,0,1},
    {1,0,0,2,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,3,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
};

static int init_map[MAP_H][MAP_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,1,0,0,0,0,1},
    {1,0,0,2,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,3,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
};

static lv_obj_t * game_scr;
static lv_obj_t * game_container;
static lv_group_t * input_group;
static lv_obj_t * old_scr;

static lv_obj_t * tile_objs[MAP_H][MAP_W];
static lv_obj_t * player_obj;

static int px = 2, py = 2; // player pos

static void update_tile(int x, int y) {
    if (!tile_objs[y][x]) {
        tile_objs[y][x] = lv_obj_create(game_container);
        lv_obj_set_style_border_width(tile_objs[y][x], 0, 0);
        lv_obj_set_style_radius(tile_objs[y][x], 0, 0);
    }

    lv_obj_t * obj = tile_objs[y][x];

    if(map[y][x] == 1) { // Wall
        lv_obj_set_size(obj, TILE_SIZE, TILE_SIZE);
        lv_obj_set_pos(obj, x*TILE_SIZE, y*TILE_SIZE);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x888888), 0);
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else if(map[y][x] == 2) { // Box
        lv_obj_set_size(obj, TILE_SIZE-2, TILE_SIZE-2);
        lv_obj_set_pos(obj, x*TILE_SIZE+1, y*TILE_SIZE+1);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x8B4513), 0);
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else if(map[y][x] == 3) { // Target
        lv_obj_set_size(obj, TILE_SIZE-6, TILE_SIZE-6);
        lv_obj_set_pos(obj, x*TILE_SIZE+3, y*TILE_SIZE+3);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x00FF00), 0);
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else if(map[y][x] == 4) { // Box on target
        lv_obj_set_size(obj, TILE_SIZE-2, TILE_SIZE-2);
        lv_obj_set_pos(obj, x*TILE_SIZE+1, y*TILE_SIZE+1);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xFFFF00), 0);
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else { // Empty
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void draw_map(void) {
    for(int y=0; y<MAP_H; y++) {
        for(int x=0; x<MAP_W; x++) {
            update_tile(x, y);
        }
    }

    // Draw player
    lv_obj_set_pos(player_obj, px*TILE_SIZE+2, py*TILE_SIZE+2);
}

static void try_move(int dx, int dy) {
    int nx = px + dx;
    int ny = py + dy;

    if(map[ny][nx] == 1) return; // Wall

    if(map[ny][nx] == 2 || map[ny][nx] == 4) { // Pushing box
        int nnx = nx + dx;
        int nny = ny + dy;

        if(map[nny][nnx] == 0 || map[nny][nnx] == 3) {
            // Move box
            map[ny][nx] = (map[ny][nx] == 4) ? 3 : 0;
            map[nny][nnx] = (map[nny][nnx] == 3) ? 4 : 2;
            px = nx; py = ny;
        }
    } else {
        // Move player
        px = nx; py = ny;
    }
    draw_map();
}

static void pb_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        if(key == LV_KEY_UP) try_move(0, -1);
        else if(key == LV_KEY_DOWN) try_move(0, 1);
        else if(key == LV_KEY_LEFT) try_move(-1, 0);
        else if(key == LV_KEY_RIGHT) try_move(1, 0);
        else if(key == LV_KEY_ENTER) {
            // Restart
            for(int y=0; y<MAP_H; y++) {
                for(int x=0; x<MAP_W; x++) {
                    map[y][x] = init_map[y][x];
                }
            }
            px = 2; py = 2;
            draw_map();
        }
        else if(key == LV_KEY_PREV) { // Exit
            lv_group_t * game_group = lv_indev_get_group(lv_indev_active());
            lv_indev_set_group(lv_indev_active(), input_group); // Restore home group
            lv_group_delete(game_group); // Prevent memory leak
            lv_screen_load(old_scr);
            lv_obj_delete_async(game_scr);
        }
    }
}

void create_pushbox_game(lv_group_t * g, lv_obj_t * parent_scr) {
    old_scr = parent_scr;
    input_group = g;

    game_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(game_scr, lv_color_hex(0x000000), 0);

    game_container = lv_obj_create(game_scr);
    lv_obj_set_size(game_container, MAP_W * TILE_SIZE, MAP_H * TILE_SIZE);
    lv_obj_center(game_container);
    lv_obj_set_style_bg_color(game_container, lv_color_hex(0x222222), 0);
    lv_obj_set_style_border_width(game_container, 0, 0);
    lv_obj_set_style_pad_all(game_container, 0, 0);

    for(int y=0; y<MAP_H; y++) {
        for(int x=0; x<MAP_W; x++) {
            tile_objs[y][x] = NULL;
            map[y][x] = init_map[y][x];
        }
    }

    player_obj = lv_obj_create(game_container);
    lv_obj_set_size(player_obj, TILE_SIZE-4, TILE_SIZE-4);
    lv_obj_set_style_bg_color(player_obj, lv_color_hex(0x0000FF), 0);
    lv_obj_set_style_border_width(player_obj, 0, 0);
    lv_obj_set_style_radius(player_obj, 0, 0);

    px = 2; py = 2;
    draw_map();

    // Create a new group exclusively for the game so focus isn't stolen by the home screen
    lv_group_t * game_group = lv_group_create();
    lv_indev_set_group(lv_indev_active(), game_group);

    lv_obj_add_event_cb(game_scr, pb_event_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(game_group, game_scr);
    lv_group_focus_obj(game_scr);

    lv_screen_load(game_scr);
}
