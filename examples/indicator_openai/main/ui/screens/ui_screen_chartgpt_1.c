#include "../ui.h"
#include "../ui_helpers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nvs_flash.h"
#include "nvs.h"


#define MAX_TASKS 20
typedef struct {
    char name[64];
    bool done;
    bool active;
} task_t;
static task_t tasks[MAX_TASKS];

static lv_obj_t * task_list = NULL;
static lv_obj_t * edit_panel = NULL;
static lv_obj_t * edit_textarea = NULL;
static lv_obj_t * edit_keyboard = NULL;
static int current_edit_index = -1;

static void save_tasks(void) {
    nvs_handle_t my_handle;
    if (nvs_open("storage", NVS_READWRITE, &my_handle) == ESP_OK) {
        nvs_set_blob(my_handle, "todos", tasks, sizeof(tasks));
        nvs_commit(my_handle);
        nvs_close(my_handle);
    }
}

static void load_tasks(void) {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    nvs_handle_t my_handle;
    if (nvs_open("storage", NVS_READONLY, &my_handle) == ESP_OK) {
        size_t required_size = sizeof(tasks);
        nvs_get_blob(my_handle, "todos", tasks, &required_size);
        nvs_close(my_handle);
    }
}


static void render_task_list(void);
static void render_task_list_async(void * arg) {
    render_task_list();
}

static void toggle_cb(lv_event_t * e) {
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    tasks[idx].done = !tasks[idx].done;
    save_tasks();
    lv_async_call(render_task_list_async, NULL);
}

static void edit_cb(lv_event_t * e) {
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    current_edit_index = idx;
    lv_textarea_set_text(edit_textarea, tasks[idx].name);
    lv_obj_clear_flag(edit_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(edit_keyboard, LV_OBJ_FLAG_HIDDEN);
}

static void delete_cb(lv_event_t * e) {
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    tasks[idx].active = false;
    save_tasks();
    lv_async_call(render_task_list_async, NULL);
}

static void render_task_list(void) {
    if (!task_list) return;
    lv_obj_clean(task_list);

    for (int i = 0; i < MAX_TASKS; i++) {
        if (!tasks[i].active) continue;

        lv_obj_t * item = lv_obj_create(task_list);
        lv_obj_set_width(item, lv_pct(100));
        lv_obj_set_height(item, 60);
        lv_obj_set_flex_flow(item, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(item, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(item, 5, 0);
        lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);

        // Checkbox button: [ ] or [x]
        lv_obj_t * cb_btn = lv_btn_create(item);
        lv_obj_t * cb_label = lv_label_create(cb_btn);
        lv_label_set_text(cb_label, tasks[i].done ? "[x]" : "[ ]");
        lv_obj_add_event_cb(cb_btn, toggle_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);

        // Task name
        lv_obj_t * name_label = lv_label_create(item);
        lv_label_set_text(name_label, tasks[i].name);
        lv_obj_set_flex_grow(name_label, 1);
        if (tasks[i].done) {
            lv_obj_set_style_text_color(name_label, lv_color_hex(0x888888), 0);
        }

        // Edit button
        lv_obj_t * e_btn = lv_btn_create(item);
        lv_obj_t * e_label = lv_label_create(e_btn);
        lv_label_set_text(e_label, "->");
        lv_obj_add_event_cb(e_btn, edit_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);

        // Delete button
        lv_obj_t * d_btn = lv_btn_create(item);
        lv_obj_t * d_label = lv_label_create(d_btn);
        lv_label_set_text(d_label, LV_SYMBOL_TRASH);
        lv_obj_set_style_bg_color(d_btn, lv_color_hex(0xcc0000), 0);
        lv_obj_add_event_cb(d_btn, delete_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
    }
}

static void add_cb(lv_event_t * e) {
    current_edit_index = -1;
    lv_textarea_set_text(edit_textarea, "");
    lv_obj_clear_flag(edit_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(edit_keyboard, LV_OBJ_FLAG_HIDDEN);
}

static void save_cb(lv_event_t * e) {
    const char * text = lv_textarea_get_text(edit_textarea);
    if (strlen(text) > 0) {
        if (current_edit_index >= 0) {
            strncpy(tasks[current_edit_index].name, text, 63);
            tasks[current_edit_index].name[63] = '\0';
        } else {
            for (int i = 0; i < MAX_TASKS; i++) {
                if (!tasks[i].active) {
                    strncpy(tasks[i].name, text, 63);
                    tasks[i].name[63] = '\0';
                    tasks[i].active = true;
                    tasks[i].done = false;
                    break;
                }
            }
        }
    }
    lv_obj_add_flag(edit_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(edit_keyboard, LV_OBJ_FLAG_HIDDEN);
    save_tasks();
    lv_async_call(render_task_list_async, NULL);
}

static void cancel_cb(lv_event_t * e) {
    lv_obj_add_flag(edit_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(edit_keyboard, LV_OBJ_FLAG_HIDDEN);
}

static void ui_event_screen_chartgpt_1(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) {
        _ui_screen_change(ui_screen_setting, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
    }
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        extern lv_obj_t * ui_screen_games;
        _ui_screen_change(ui_screen_games, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
    }
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP) {
        _ui_screen_change(ui_screen_time, LV_SCR_LOAD_ANIM_MOVE_TOP, 200, 0);
    }
}

void ui_screen_chartgpt_1_screen_init(void) {
    ui_screen_chartgpt_1 = lv_obj_create(NULL);
    load_tasks();
    lv_obj_set_style_bg_color(ui_screen_chartgpt_1, lv_color_hex(0x222222), 0);
    lv_obj_add_event_cb(ui_screen_chartgpt_1, ui_event_screen_chartgpt_1, LV_EVENT_ALL, NULL);

    // Title
    ui_chatgpt_1_title = lv_label_create(ui_screen_chartgpt_1);
    lv_label_set_text(ui_chatgpt_1_title, "To-Do List");
    lv_obj_set_style_text_font(ui_chatgpt_1_title, &lv_font_montserrat_20, 0);
    lv_obj_align(ui_chatgpt_1_title, LV_ALIGN_TOP_MID, 0, 10);

    // Add button
    lv_obj_t * add_btn = lv_btn_create(ui_screen_chartgpt_1);
    lv_obj_set_size(add_btn, 60, 40);
    lv_obj_align(add_btn, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_t * add_lbl = lv_label_create(add_btn);
    lv_label_set_text(add_lbl, "+");
    lv_obj_set_style_text_font(add_lbl, &lv_font_montserrat_20, 0);
    lv_obj_center(add_lbl);
    lv_obj_add_event_cb(add_btn, add_cb, LV_EVENT_CLICKED, NULL);

    // List container
    task_list = lv_obj_create(ui_screen_chartgpt_1);
    lv_obj_set_size(task_list, 440, 360);
    lv_obj_align(task_list, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_flex_flow(task_list, LV_FLEX_FLOW_COLUMN);

    // Edit Panel
    edit_panel = lv_obj_create(ui_screen_chartgpt_1);
    lv_obj_set_size(edit_panel, 400, 150);
    lv_obj_align(edit_panel, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_bg_color(edit_panel, lv_color_hex(0x444444), 0);
    lv_obj_add_flag(edit_panel, LV_OBJ_FLAG_HIDDEN);

    edit_textarea = lv_textarea_create(edit_panel);
    lv_obj_set_size(edit_textarea, 360, 60);
    lv_obj_align(edit_textarea, LV_ALIGN_TOP_MID, 0, 10);
    
    lv_obj_t * save_btn = lv_btn_create(edit_panel);
    lv_obj_align(save_btn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_t * save_lbl = lv_label_create(save_btn);
    lv_label_set_text(save_lbl, "Save");
    lv_obj_add_event_cb(save_btn, save_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * cancel_btn = lv_btn_create(edit_panel);
    lv_obj_align(cancel_btn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_set_style_bg_color(cancel_btn, lv_color_hex(0x888888), 0);
    lv_obj_t * cancel_lbl = lv_label_create(cancel_btn);
    lv_label_set_text(cancel_lbl, "Cancel");
    lv_obj_add_event_cb(cancel_btn, cancel_cb, LV_EVENT_CLICKED, NULL);

    // Keyboard
    edit_keyboard = lv_keyboard_create(ui_screen_chartgpt_1);
    lv_obj_set_size(edit_keyboard, 480, 220);
    lv_obj_align(edit_keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_textarea(edit_keyboard, edit_textarea);
    lv_obj_add_flag(edit_keyboard, LV_OBJ_FLAG_HIDDEN);

    // Initial data removed to allow NVS persistence

    render_task_list();

    // Satisfy ui.h externs
    ui_generate_answer_btn = lv_btn_create(ui_screen_chartgpt_1);
    lv_obj_add_flag(ui_generate_answer_btn, LV_OBJ_FLAG_HIDDEN);
    ui_generate_answer_title = lv_label_create(ui_generate_answer_btn);
    ui_Panel1 = lv_obj_create(ui_screen_chartgpt_1);
    lv_obj_add_flag(ui_Panel1, LV_OBJ_FLAG_HIDDEN);
    ui_text_edit_gpt_request = lv_textarea_create(ui_Panel1);
    ui_Keyboard_chatgpt = lv_keyboard_create(ui_screen_chartgpt_1);
    lv_obj_add_flag(ui_Keyboard_chatgpt, LV_OBJ_FLAG_HIDDEN);
    ui_wifi_st_9 = lv_img_create(ui_screen_chartgpt_1);
    lv_obj_add_flag(ui_wifi_st_9, LV_OBJ_FLAG_HIDDEN);
}
