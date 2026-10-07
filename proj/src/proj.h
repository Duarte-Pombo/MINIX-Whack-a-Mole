#ifndef PROJ_H
#define PROJ_H

#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <lcom/vbe.h>
#include <lcom/xpm.h>
#include <lcom/utils.h>

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "labDrivers/video/video_gr.h"
#include "labDrivers/mouse/mouse.h"
#include "projDrivers/keyboard/keyboard.h"
#include "projDrivers/keyboard/kbc.h"
#include "projDrivers/keyboard/i8042.h"
#include "projDrivers/timer/i8254.h"
#include "utils/utils.h"
#include "gameLogic/game.h"
#include "uiElements/cursor.h"
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
extern uint8_t* loss_esc_screen_pixmap;
extern xpm_image_t loss_esc_screen_img;
extern uint8_t* win_esc_screen_pixmap;
extern xpm_image_t win_esc_screen_img;
extern uint8_t* background_pixmap;
extern xpm_image_t background_img;

extern uint8_t* settings_screen_pixmap;
extern xpm_image_t settings_screen_img;
extern uint8_t* selector_pixmap;
extern xpm_image_t selector_img;

#define TIME_LIMIT 300
#endif

