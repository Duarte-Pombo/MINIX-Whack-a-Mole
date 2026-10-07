#include "settings.h"
#include "uiElements/cursor.h"
#include "projDrivers/video/video.h"



// variavel settings, sensibilidade 1 e dificuldade 1: easy
Settings game_settings = {1.0, 1};

uint8_t* settings_screen_pixmap;
xpm_image_t settings_screen_img;
uint8_t* selector_pixmap;
xpm_image_t selector_img;

DifficultyChoice difficulty_choice = DIFFICULTY_EASY; 
DifficultyChoice previous_difficulty_choice = DIFFICULTY_EASY; 

bool shortcuts_enabled = false; 

void update_selector_position() {
    switch (previous_difficulty_choice) {
        case DIFFICULTY_EASY:
            vg_draw_rectangle(DIFFICULTY_EASY_X + 100, DIFFICULTY_EASY_Y + HEIGHT + 110, 35, 35, 0xFFF8ED); 
            break;
        case DIFFICULTY_MEDIUM:
            vg_draw_rectangle(DIFFICULTY_MEDIUM_X + 100, DIFFICULTY_MEDIUM_Y + HEIGHT + 110, 35, 35, 0xFFF8ED); 
            break;
        case DIFFICULTY_HARD:
            vg_draw_rectangle(DIFFICULTY_HARD_X + 100, DIFFICULTY_HARD_Y + HEIGHT + 110, 35, 35, 0xFFF8ED); 
            break;
    }

    switch (difficulty_choice) {
        case DIFFICULTY_EASY:
            drawImage(selector_pixmap, &selector_img, DIFFICULTY_EASY_X + 100, DIFFICULTY_EASY_Y + HEIGHT + 110);
            break;
        case DIFFICULTY_MEDIUM:
            drawImage(selector_pixmap, &selector_img, DIFFICULTY_MEDIUM_X + 100, DIFFICULTY_MEDIUM_Y + HEIGHT + 110);
            break;
        case DIFFICULTY_HARD:
            drawImage(selector_pixmap, &selector_img, DIFFICULTY_HARD_X + 100, DIFFICULTY_HARD_Y + HEIGHT + 110);
            break;
    }
    previous_difficulty_choice = difficulty_choice;
}


void draw_settings_menu() {
    vg_draw_rectangle(0, 0, h_res, v_res, 0xFFFFFF); // background

    drawImage(settings_screen_pixmap, &settings_screen_img, 0, 0); 



    switch (difficulty_choice) {
        case DIFFICULTY_EASY:
            drawImage(selector_pixmap, &selector_img, DIFFICULTY_EASY_X + 100, DIFFICULTY_EASY_Y + HEIGHT + 110);
            break;
        case DIFFICULTY_MEDIUM:
            drawImage(selector_pixmap, &selector_img, DIFFICULTY_MEDIUM_X + 100, DIFFICULTY_MEDIUM_Y + HEIGHT + 110);
            break;
        case DIFFICULTY_HARD:
            drawImage(selector_pixmap, &selector_img, DIFFICULTY_HARD_X + 100, DIFFICULTY_HARD_Y + HEIGHT + 110);
            break;
    }
    
    previous_difficulty_choice = difficulty_choice;

}
