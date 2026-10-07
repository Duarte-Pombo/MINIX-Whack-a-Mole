#include "cursor.h"
#include <lcom/lcf.h>
#include "../projDrivers/video/video.h"
#include "../labDrivers/mouse/mouse.h"  
#include "../labDrivers/video/video_gr.h"
#include "../settings.h"

#define INITIAL_CURSOR_X 512
#define INITIAL_CURSOR_Y 384

cursor _cursor = {0, 0, 0, 0, false, false, true, NULL, 0, 0};

extern uint8_t* hammerCursor_pixmap;
extern xpm_image_t hammerCursor_img;
extern uint8_t* hammerCursorClick_pixmap;
extern xpm_image_t hammerCursorClick_img;
extern uint8_t* whackMenu_pixmap;
extern xpm_image_t whackMenu_img;

extern unsigned h_res;
extern unsigned v_res;
extern uint8_t *video_mem;

int cursor_init(void) {
    printf("initializing cursor\n");
    _cursor.x = INITIAL_CURSOR_X;
    _cursor.y = INITIAL_CURSOR_Y;
    _cursor.prev_x = INITIAL_CURSOR_X;
    _cursor.prev_y = INITIAL_CURSOR_Y;
    _cursor.click = false;
    _cursor.prev_click = false;
    _cursor.needs_redraw = true;
    
    // allocate bg buffer
    _cursor.bg_width = (hammerCursor_img.width > hammerCursorClick_img.width) ? 
                       hammerCursor_img.width : hammerCursorClick_img.width;
    _cursor.bg_height = (hammerCursor_img.height > hammerCursorClick_img.height) ? 
                        hammerCursor_img.height : hammerCursorClick_img.height;
    
    _cursor.background_buffer = malloc(_cursor.bg_width * _cursor.bg_height * 3);
    if (!_cursor.background_buffer) {
        printf("Failed to allocate cursor background buffer\n");
        return 1;
    }
    
    return 0;
}

void cursor_cleanup(void) {
    if (_cursor.background_buffer) {
        free(_cursor.background_buffer);
        _cursor.background_buffer = NULL;
    }
}

void cursor_save_background(void) {
    if (!_cursor.background_buffer) return;
    
    uint16_t width = _cursor.click ? hammerCursorClick_img.width : hammerCursor_img.width;
    uint16_t height = _cursor.click ? hammerCursorClick_img.height : hammerCursor_img.height;

    uint16_t max_width = ((uint16_t)_cursor.x + width > h_res) ? h_res - (uint16_t)_cursor.x : width;
    uint16_t max_height = ((uint16_t)_cursor.y + height > v_res) ? v_res - (uint16_t)_cursor.y : height;
    
    for (uint16_t row = 0; row < max_height; row++) {
        for (uint16_t col = 0; col < max_width; col++) {
            uint32_t screen_offset = (((uint16_t)_cursor.y + row) * h_res + ((uint16_t)_cursor.x + col)) * 3;
            uint32_t buffer_offset = (row * _cursor.bg_width + col) * 3;
            
            _cursor.background_buffer[buffer_offset]     = video_mem[screen_offset];     // R
            _cursor.background_buffer[buffer_offset + 1] = video_mem[screen_offset + 1]; // G
            _cursor.background_buffer[buffer_offset + 2] = video_mem[screen_offset + 2]; // B
        }
    }
}

void cursor_restore_background(void) {
    if (!_cursor.background_buffer) return;
    
    uint16_t width = _cursor.prev_click ? hammerCursorClick_img.width : hammerCursor_img.width;
    uint16_t height = _cursor.prev_click ? hammerCursorClick_img.height : hammerCursor_img.height;
    
    uint16_t max_width = ((uint16_t)_cursor.prev_x + width > h_res) ? h_res - (uint16_t)_cursor.prev_x : width;
    uint16_t max_height = ((uint16_t)_cursor.prev_y + height > v_res) ? v_res - (uint16_t)_cursor.prev_y : height;
    
    for (uint16_t row = 0; row < max_height; row++) {
        for (uint16_t col = 0; col < max_width; col++) {
            uint32_t screen_offset = (((uint16_t)_cursor.prev_y + row) * h_res + ((uint16_t)_cursor.prev_x + col)) * 3;
            uint32_t buffer_offset = (row * _cursor.bg_width + col) * 3;
            
            video_mem[screen_offset]     = _cursor.background_buffer[buffer_offset];     // R
            video_mem[screen_offset + 1] = _cursor.background_buffer[buffer_offset + 1]; // G
            video_mem[screen_offset + 2] = _cursor.background_buffer[buffer_offset + 2]; // B
        }
    }
}

int cursor_update_pos(int16_t newX, int16_t newY) {
    if (_cursor.x != newX || _cursor.y != newY) {
        _cursor.prev_x = _cursor.x;
        _cursor.prev_y = _cursor.y;
        _cursor.x = newX;
        _cursor.y = newY;
        _cursor.needs_redraw = true;
    }
    printf("cursor at (%d, %d)\n", newX, newY);
    return 0;
}

int set_click_state(bool click) {
    if (_cursor.click != click) {
        _cursor.prev_click = _cursor.click;
        _cursor.click = click;
        _cursor.needs_redraw = true;
    }
    return 0;
}

void cursor_draw(void) {
    if (_cursor.click)
        draw_xpm_transparent(hammerCursorClick_pixmap, &hammerCursorClick_img, _cursor.x, _cursor.y);
    else
        draw_xpm_transparent(hammerCursor_pixmap, &hammerCursor_img, _cursor.x, _cursor.y);
}

void cursor_update_display(void) {
    if (!_cursor.needs_redraw) return;
    
    if (_cursor.prev_x != _cursor.x || _cursor.prev_y != _cursor.y || _cursor.prev_click != _cursor.click) {
        cursor_restore_background();
    }

    cursor_save_background();

    cursor_draw();
    
    _cursor.needs_redraw = false;
}

bool cursor_inside_box(int16_t boxX, int16_t boxY, int16_t boxW, int16_t boxH) {
    return (_cursor.x >= boxX && _cursor.x <= boxX + boxW &&
            _cursor.y >= boxY && _cursor.y <= boxY + boxH);
}

void cursor_handle_movement(int16_t change_in_x, int16_t change_in_y) { 
    int16_t scaled_x = (int16_t)(change_in_x * game_settings.mouse_sensitivity);
    int16_t scaled_y = (int16_t)(change_in_y * game_settings.mouse_sensitivity);

    int16_t new_x = _cursor.x + scaled_x;
    int16_t new_y = _cursor.y - scaled_y;

    if (new_x < 0) 
        new_x = 0;
    if ((uint16_t)new_x >= h_res) 
        new_x = (int16_t)(h_res - 1);
    if (new_y < 0) 
        new_y = 0;
    if ((uint16_t)new_y >= v_res) 
        new_y = (int16_t)(v_res - 1);
    
    cursor_update_pos(new_x, new_y);
}

void cursor_handle_packet(struct packet *pp) {
    cursor_handle_movement(pp->delta_x, pp->delta_y);
    set_click_state(pp->lb);
    
    cursor_update_display();
}
