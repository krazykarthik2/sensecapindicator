#include "../ui.h"

// Define screens
lv_obj_t * ui_screen_games;
lv_obj_t * ui_screen_chess;

// UI elements for Games screen
lv_obj_t * ui_games_title;
lv_obj_t * ui_chess_btn;
lv_obj_t * ui_chess_lbl;

// UI elements for Chess screen
lv_obj_t * ui_chess_board;
lv_obj_t * ui_chess_squares[64];
lv_obj_t * ui_chess_status;
lv_obj_t * ui_chess_white_btn;
lv_obj_t * ui_chess_black_btn;
lv_obj_t * ui_chess_random_btn;
lv_obj_t * ui_chess_back_btn;

void ui_event_screen_games(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        // swipe right -> back to todo list
        extern lv_obj_t * ui_screen_chartgpt_1;
        _ui_screen_change(ui_screen_chartgpt_1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
    }
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) {
        // swipe left -> go to setting list
        extern lv_obj_t * ui_screen_setting;
        _ui_screen_change(ui_screen_setting, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
    }
}

void chess_btn_cb(lv_event_t * e) {
    _ui_screen_change(ui_screen_chess, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
}

void ui_screen_games_screen_init(void) {
    ui_screen_games = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_screen_games, lv_color_hex(0x222222), 0);
    lv_obj_clear_flag(ui_screen_games, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(ui_screen_games, ui_event_screen_games, LV_EVENT_ALL, NULL);

    // Title
    ui_games_title = lv_label_create(ui_screen_games);
    lv_label_set_text(ui_games_title, "Games");
    lv_obj_set_style_text_font(ui_games_title, &lv_font_montserrat_24, 0);
    lv_obj_align(ui_games_title, LV_ALIGN_TOP_MID, 0, 10);

    // Chess button
    ui_chess_btn = lv_btn_create(ui_screen_games);
    lv_obj_set_size(ui_chess_btn, 120, 50);
    lv_obj_align(ui_chess_btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(ui_chess_btn, chess_btn_cb, LV_EVENT_CLICKED, NULL);

    ui_chess_lbl = lv_label_create(ui_chess_btn);
    lv_label_set_text(ui_chess_lbl, "Chess");
    lv_obj_set_style_text_font(ui_chess_lbl, &lv_font_montserrat_20, 0);
    lv_obj_center(ui_chess_lbl);
}

// ---------------------------------------------
// Chess Screen
// ---------------------------------------------
void chess_back_cb(lv_event_t * e) {
    _ui_screen_change(ui_screen_games, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
}

void chess_play_cb(lv_event_t * e) {
    int color = (int)(intptr_t)lv_event_get_user_data(e);
    // 0 = white, 1 = black, 2 = random
    // start chess engine...
    lv_label_set_text(ui_chess_status, "Playing...");
    lv_obj_add_flag(ui_chess_white_btn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_chess_black_btn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_chess_random_btn, LV_OBJ_FLAG_HIDDEN);
}

void ui_screen_chess_screen_init(void) {
    ui_screen_chess = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_screen_chess, lv_color_hex(0x222222), 0);
    lv_obj_clear_flag(ui_screen_chess, LV_OBJ_FLAG_SCROLLABLE);

    // Status label
    ui_chess_status = lv_label_create(ui_screen_chess);
    lv_label_set_text(ui_chess_status, "Select Color");
    lv_obj_set_style_text_font(ui_chess_status, &lv_font_montserrat_20, 0);
    lv_obj_align(ui_chess_status, LV_ALIGN_TOP_MID, 0, 10);

    // Back button
    ui_chess_back_btn = lv_btn_create(ui_screen_chess);
    lv_obj_set_size(ui_chess_back_btn, 60, 40);
    lv_obj_align(ui_chess_back_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_t * back_lbl = lv_label_create(ui_chess_back_btn);
    lv_label_set_text(back_lbl, "<-");
    lv_obj_center(back_lbl);
    lv_obj_add_event_cb(ui_chess_back_btn, chess_back_cb, LV_EVENT_CLICKED, NULL);

    // Chess board (8x8 grid)
    ui_chess_board = lv_obj_create(ui_screen_chess);
    lv_obj_set_size(ui_chess_board, 320, 320);
    lv_obj_clear_flag(ui_chess_board, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(ui_chess_board, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_layout(ui_chess_board, LV_LAYOUT_GRID);
    
    // Create grid layout
    static lv_coord_t col_dsc[] = {40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(ui_chess_board, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(ui_chess_board, row_dsc, 0);
    lv_obj_set_style_pad_all(ui_chess_board, 0, 0);

    for(int i=0; i<64; i++) {
        ui_chess_squares[i] = lv_btn_create(ui_chess_board);
        lv_obj_set_grid_cell(ui_chess_squares[i], LV_GRID_ALIGN_STRETCH, i%8, 1,
                                                LV_GRID_ALIGN_STRETCH, i/8, 1);
        int r = i/8;
        int c = i%8;
        if ((r+c)%2 == 0) {
            lv_obj_set_style_bg_color(ui_chess_squares[i], lv_color_hex(0xFFCE9E), 0);
        } else {
            lv_obj_set_style_bg_color(ui_chess_squares[i], lv_color_hex(0xD18B47), 0);
        }
        lv_obj_set_style_radius(ui_chess_squares[i], 0, 0);
        lv_obj_set_style_border_width(ui_chess_squares[i], 0, 0);
        
        lv_obj_t * lbl = lv_label_create(ui_chess_squares[i]);
        lv_label_set_text(lbl, "");
        lv_obj_center(lbl);
        lv_obj_set_style_text_color(lbl, lv_color_hex(0x000000), 0);
    }

    // Play options
    ui_chess_white_btn = lv_btn_create(ui_screen_chess);
    lv_obj_set_size(ui_chess_white_btn, 80, 40);
    lv_obj_align(ui_chess_white_btn, LV_ALIGN_BOTTOM_LEFT, 40, -10);
    lv_obj_t * white_lbl = lv_label_create(ui_chess_white_btn);
    lv_label_set_text(white_lbl, "White");
    lv_obj_center(white_lbl);
    lv_obj_add_event_cb(ui_chess_white_btn, chess_play_cb, LV_EVENT_CLICKED, (void *)0);

    ui_chess_black_btn = lv_btn_create(ui_screen_chess);
    lv_obj_set_size(ui_chess_black_btn, 80, 40);
    lv_obj_align(ui_chess_black_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t * black_lbl = lv_label_create(ui_chess_black_btn);
    lv_label_set_text(black_lbl, "Black");
    lv_obj_center(black_lbl);
    lv_obj_add_event_cb(ui_chess_black_btn, chess_play_cb, LV_EVENT_CLICKED, (void *)1);

    ui_chess_random_btn = lv_btn_create(ui_screen_chess);
    lv_obj_set_size(ui_chess_random_btn, 80, 40);
    lv_obj_align(ui_chess_random_btn, LV_ALIGN_BOTTOM_RIGHT, -40, -10);
    lv_obj_t * rand_lbl = lv_label_create(ui_chess_random_btn);
    lv_label_set_text(rand_lbl, "Random");
    lv_obj_center(rand_lbl);
    lv_obj_add_event_cb(ui_chess_random_btn, chess_play_cb, LV_EVENT_CLICKED, (void *)2);
}
