#ifndef _VIDEO_GR_H_
#define _VIDEO_GR_H_

#include <lcom/lcf.h>
#include <lcom/vbe.h>
#include <lcom/xpm.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <minix/sysutil.h>
#include "sprites/cursor.xpm"

// global variables
static void *video_mem;        
static void *video_mem_sec;
static unsigned bits_per_pixel;
extern unsigned h_res;     
extern unsigned v_res;        
extern vbe_mode_info_t vmi; 
extern uint8_t* cursor_pixmap;
extern xpm_image_t cursor_img;

// Function prototypes
void *(vg_init)(uint16_t mode);
int (vg_exit)(void);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int (vg_set_palette_entry)(uint8_t index, uint32_t color);
int (vg_read_pixel)(uint16_t x, uint16_t y, uint32_t *color);


// XPM functions
uint8_t* load_xpm(xpm_map_t xpm, xpm_image_t* img);
int draw_xpm(uint8_t* pixmap, xpm_image_t* img, uint16_t x, uint16_t y);
void free_xpm(uint8_t* pixmap);
void load_allxpm();
void free_allxpm();
#endif 
