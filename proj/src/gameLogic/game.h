#ifndef _GAME_H_
#define _GAME_H_

#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <lcom/vbe.h>
#include <lcom/xpm.h>
#include <lcom/utils.h>

#include <stdbool.h>
#include <stdint.h>

#include "uiElements/cursor.h"
#include "settings.h"
#include "labDrivers/video/video_gr.h"
#include "labDrivers/mouse/mouse.h"
#include "projDrivers/keyboard/keyboard.h"
#include "projDrivers/keyboard/kbc.h"
#include "projDrivers/keyboard/i8042.h"
#include "projDrivers/timer/i8254.h"
#include "utils/utils.h"

#include "uiElements/mole.h"

extern unsigned int timer_counter;
extern unsigned h_res;
extern unsigned v_res;
extern unsigned bits_per_pixel;
extern uint8_t* secondary_buffer;

extern uint8_t* cursor_pixmap;
extern xpm_image_t cursor_img;
extern uint8_t* quit_button_pixmap;
extern xpm_image_t quit_button_img;
extern uint8_t* mole_icon_pixmap;
extern xpm_image_t mole_icon_img;
extern uint8_t* whackMenu_pixmap;
extern xpm_image_t whackMenu_img;
extern uint8_t* hammerCursor_pixmap;
extern xpm_image_t hammerCursor_img;

typedef enum {
    MENU_STATE,
    MAIN_GAME_STATE,
    SETTINGS_STATE,
    EXIT_STATE
} gameState;

typedef struct {
    gameState currState;
    bool running; 
} game;

void handle_menu_input (game *game_);
void handle_game_input (game *game_);
void handle_settings_input (game *game_);



void load(gameState state);
#endif
