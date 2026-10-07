#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include <stdint.h>
#include "uiElements/cursor.h"
#include "proj.h"
#include "projDrivers/keyboard/keyboard.h"
#include "projDrivers/keyboard/kbc.h"
#include "projDrivers/keyboard/i8042.h"
#include "labDrivers/video/video_gr.h"

//sensitivity buttons

#define HEIGHT 60

#define SENSITIVITY_MINUS_X 320
#define SENSITIVITY_MINUS_Y 110
#define SENSITIVITY_MINUS_W 60

#define SENSITIVITY_PLUS_X 400
#define SENSITIVITY_PLUS_Y 110
#define SENSITIVITY_PLUS_W 60

//difficulty buttons

#define DIFFICULTY_EASY_X 330
#define DIFFICULTY_EASY_Y 230
#define DIFFICULTY_EASY_W 109

#define DIFFICULTY_MEDIUM_X 440
#define DIFFICULTY_MEDIUM_Y 230
#define DIFFICULTY_MEDIUM_W 159

#define DIFFICULTY_HARD_X 600
#define DIFFICULTY_HARD_Y 230
#define DIFFICULTY_HARD_W 109

//shortcut on-off
#define SHORTCUT_ON_X 330
#define SHORTCUT_ON_Y 370
#define SHORTCUT_ON_W 60

#define SHORTCUT_OFF_X 410
#define SHORTCUT_OFF_Y 370
#define SHORTCUT_OFF_W 60

//back to menu button

#define BACK_TO_MENU_X 340
#define BACK_TO_MENU_Y 540
#define BACK_TO_MENU_W 255


typedef struct {
    float mouse_sensitivity;
    int difficulty;
} Settings;

extern Settings game_settings;

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD,
} DifficultyChoice;

extern DifficultyChoice difficulty_choice;

extern bool shortcuts_enabled;

void draw_settings_menu();
void update_settings(int mouse_sensitivity, int difficulty);
void update_selector_position();


#endif // SETTINGS_H
