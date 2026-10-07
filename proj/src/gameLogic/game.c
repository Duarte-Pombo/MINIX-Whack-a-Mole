#include "game.h"
#include "uiElements/cursor.h"

//remover este commentario mas acho q ter isto assim definido acaba por facilitar apesar de ficar ganda textao
#define BUTTON_W 150
#define BUTTON_H 100

#define START_BUTTON_X 200
#define START_BUTTON_Y 450
#define EXIT_BUTTON_X 400
#define EXIT_BUTTON_Y 450
#define SETTINGS_BUTTON_X 600
#define SETTINGS_BUTTON_Y 450

void handle_menu_input (game *game_){
    // check if cursor is inside menu boxes
    if (_cursor.click) {
        if (cursor_inside_box(START_BUTTON_X, START_BUTTON_Y, BUTTON_W, BUTTON_H)) {
            printf("Start button clicked\n");
            game_->currState = MAIN_GAME_STATE;
        }
        else if (cursor_inside_box(SETTINGS_BUTTON_X, SETTINGS_BUTTON_Y, BUTTON_W, BUTTON_H)) {
            printf("Settings button clicked\n");
            game_->currState = SETTINGS_STATE;
        }
        else if (cursor_inside_box(EXIT_BUTTON_X, EXIT_BUTTON_Y, BUTTON_W, BUTTON_H)) {
            printf("Exit button clicked\n");
            game_->currState = EXIT_STATE;
        }
        _cursor.click = false; // mesma cena q meti no meu handle settings
    }
}

void handle_game_input (game *game_){

    if (moles_is_game_over()) {
        return; 
    }

    //game logic
    if (_cursor.click) {
        mole_check_collision(_cursor.x, _cursor.y);
    }
}

void handle_settings_input (game *game_){

    //settings_input(_cursor, game_); //funçao em settings.c, ainda nao sei se meto diretamente aqui ou se deixo noutra file

    if (_cursor.click) { 
        if (cursor_inside_box(SENSITIVITY_MINUS_X, SENSITIVITY_MINUS_Y, SENSITIVITY_MINUS_W, HEIGHT)) {

            if (game_settings.mouse_sensitivity < 0.4) { 
                game_settings.mouse_sensitivity = 0.4;
            }
            game_settings.mouse_sensitivity -= 0.022;
            printf("sensitivity decreased: %.3f\n", game_settings.mouse_sensitivity);
        } else if (cursor_inside_box(SENSITIVITY_PLUS_X, SENSITIVITY_PLUS_Y, SENSITIVITY_PLUS_W, HEIGHT)) {

            if (game_settings.mouse_sensitivity > 3) { 
                game_settings.mouse_sensitivity = 3;
            }
            game_settings.mouse_sensitivity += 0.11;
            printf("sensitivity increased: %.3f\n", game_settings.mouse_sensitivity);
        } else if (cursor_inside_box(DIFFICULTY_EASY_X, DIFFICULTY_EASY_Y, DIFFICULTY_EASY_W, HEIGHT)) {
            difficulty_choice = DIFFICULTY_EASY;
            game_settings.difficulty = 1;
            printf("difficulty set to Easy: %d\n", game_settings.difficulty);
        } else if (cursor_inside_box(DIFFICULTY_MEDIUM_X, DIFFICULTY_MEDIUM_Y, DIFFICULTY_MEDIUM_W, HEIGHT)) {
            difficulty_choice = DIFFICULTY_MEDIUM;
            game_settings.difficulty = 2;
            printf("difficulty set to Medium: %d\n", game_settings.difficulty);
        } else if (cursor_inside_box(DIFFICULTY_HARD_X, DIFFICULTY_HARD_Y, DIFFICULTY_HARD_W, HEIGHT)) {
            difficulty_choice = DIFFICULTY_HARD;
            game_settings.difficulty = 3;
            printf("difficulty set to Hard: %d\n", game_settings.difficulty);
        } else if (cursor_inside_box(SHORTCUT_ON_X, SHORTCUT_ON_Y, SHORTCUT_ON_W, HEIGHT)) {
            shortcuts_enabled = true;
            printf("Shortcuts enabled\n");
        } else if (cursor_inside_box(SHORTCUT_OFF_X, SHORTCUT_OFF_Y, SHORTCUT_OFF_W, HEIGHT)) {
            shortcuts_enabled = false;
            printf("Shortcuts disabled\n");
        } else if (cursor_inside_box(BACK_TO_MENU_X, BACK_TO_MENU_Y, BACK_TO_MENU_W, HEIGHT)) {
            printf("returning to menu\n");
            game_->currState = MENU_STATE;
        }

        update_selector_position();
    }
}

void load(gameState state) {
    switch(state) {
        case MENU_STATE:
            // placeholder for drawing menu xpm
            drawImage(whackMenu_pixmap, &whackMenu_img, 0, 0);
            break;
        case MAIN_GAME_STATE:
            drawImage(background_pixmap, &background_img, 0, 0);
            
            switch (game_settings.difficulty) {
                case 1: // EASY
                    moles_init(EASY);
                    break;
                case 2: // MEDIUM
                    moles_init(MEDIUM);
                    break;
                case 3: // HARD
                    moles_init(HARD);
                    break;
            }
            
            moles_draw();
            break;

        case SETTINGS_STATE:
            // placeholder
            draw_settings_menu();
            printf("Loading settings state\n");
            break;
        case EXIT_STATE:
            // placeholder
            printf("Exiting game\n");
            break;
    }
}



