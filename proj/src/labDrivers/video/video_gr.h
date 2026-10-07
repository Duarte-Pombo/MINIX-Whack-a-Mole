#ifndef _VIDEO_GR_H_
#define _VIDEO_GR_H_

#include <lcom/lcf.h>
#include <lcom/vbe.h>
#include <lcom/xpm.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <minix/sysutil.h>
#include "sprites/quit_button.xpm"
#include "sprites/whack.xpm"
#include "sprites/whackMenu.xpm"
#include "sprites/hammerCursor.xpm"
#include "sprites/hammerCursorClick.xpm"
#include "sprites/moleUp.xpm"
#include "sprites/moleDown.xpm"
#include "sprites/settings_screen.xpm"
#include "sprites/selector.xpm"
#include "sprites/loss_esc_screen.xpm"
#include "sprites/win_esc_screen.xpm"
#include "sprites/background.xpm"

// global variables
extern uint8_t *video_mem;
extern unsigned h_res;     
extern unsigned v_res;        
extern vbe_mode_info_t vmi; 
extern uint8_t* cursor_pixmap;
extern xpm_image_t cursor_img;
extern uint8_t* mole_icon_pixmap;
extern xpm_image_t mole_icon_img;
extern uint8_t* hammerCursor_pixmap;
extern xpm_image_t hammerCursor_img;
extern uint8_t* hammerCursorClick_pixmap;
extern xpm_image_t hammerCursorClick_img;
extern uint8_t* moleUp_pixmap;
extern xpm_image_t moleUp_img;
extern uint8_t* moleDown_pixmap;
extern xpm_image_t moleDown_img;

extern uint8_t* settings_screen_pixmap;
extern xpm_image_t settings_screen_img;
extern uint8_t* selector_pixmap;
extern xpm_image_t selector_img;

// Function prototypes
void *(vg_init)(uint16_t mode);
int (vg_exit)(void);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int (vg_set_palette_entry)(uint8_t index, uint32_t color);
int (vg_read_pixel)(uint16_t x, uint16_t y, uint32_t *color);
void (draw_cursor)(int16_t x, int16_t y);
void (erase_cursor)(int16_t x, int16_t y);

// XPM functions
uint8_t* load_xpm(xpm_map_t xpm, xpm_image_t* img);
int draw_xpm(uint8_t* pixmap, xpm_image_t* img, uint16_t x, uint16_t y);
int draw_xpm_transparent(uint8_t* pixmap, xpm_image_t* img, uint16_t x, uint16_t y);
void free_xpm(uint8_t* pixmap);
void load_allxpm();
void free_allxpm();
void drawImage(uint8_t* pixmap, xpm_image_t* img, int16_t x, int16_t y);
#endif 
