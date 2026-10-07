#ifndef _CURSOR_H_
#define _CURSOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "labDrivers/video/video_gr.h"

struct packet; 

typedef struct {
    int16_t x, y;
    int16_t prev_x, prev_y;
    bool click;
    bool prev_click;
    bool needs_redraw;           
    uint8_t* background_buffer;  
    uint16_t bg_width, bg_height; 
} cursor;

// global cursor
extern cursor _cursor;

int cursor_init(void);
void cursor_cleanup(void);
void cursor_save_background(void); 
void cursor_restore_background(void);
int cursor_update_pos(int16_t newX, int16_t newY);
int set_click_state(bool click);
void cursor_draw(void);
void cursor_update_display(void);
bool cursor_inside_box(int16_t boxX, int16_t boxY, int16_t boxW, int16_t boxH);
void cursor_handle_packet(struct packet *pp);
void cursor_handle_movement(int16_t delta_x, int16_t delta_y); //checkbounds

#endif
