#include "video_gr.h"
#include <lcom/lcf.h>
#include <lcom/vbe.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <minix/sysutil.h>


uint8_t *video_mem;   
unsigned h_res;       
unsigned v_res;        
static unsigned bits_per_pixel; 
vbe_mode_info_t vmi;

uint8_t* cursor_pixmap;
xpm_image_t cursor_img;
uint8_t* quit_button_pixmap;
xpm_image_t quit_button_img;
uint8_t* mole_icon_pixmap;
xpm_image_t mole_icon_img;
uint8_t* whackMenu_pixmap;
xpm_image_t whackMenu_img;
uint8_t* hammerCursor_pixmap;
xpm_image_t hammerCursor_img;
uint8_t* hammerCursorClick_pixmap;
xpm_image_t hammerCursorClick_img;
uint8_t* moleUp_pixmap;
xpm_image_t moleUp_img;
uint8_t* moleDown_pixmap;
xpm_image_t moleDown_img;
uint8_t* loss_esc_screen_pixmap;
xpm_image_t loss_esc_screen_img;
uint8_t* win_esc_screen_pixmap;
xpm_image_t win_esc_screen_img;
uint8_t* background_pixmap;
xpm_image_t background_img;

extern uint8_t* settings_screen_pixmap;
extern xpm_image_t settings_screen_img;
extern uint8_t* selector_pixmap;
extern xpm_image_t selector_img;

int (vg_set_palette_entry)(uint8_t index, uint32_t color) {
    reg86_t reg86;

    memset(&reg86, 0, sizeof(reg86));
    reg86.ah = 0x4F;
    reg86.al = 0x09; 
    reg86.bh = 0x00;  
    reg86.bl = index; 
    reg86.cx = 1;     
    reg86.dx = (color >> 16) & 0xFF;  
    reg86.es = (color >> 8) & 0xFF;   
    reg86.di = color & 0xFF;         
    reg86.intno = 0x10;

    if (sys_int86(&reg86) != OK) {
        return 1;
    }

    return 0;
}

void *(vg_init)(uint16_t mode) {
    reg86_t reg86;

    if (vbe_get_mode_info(mode, &vmi) != 0) {
        return NULL;
    }

    h_res = vmi.XResolution;
    v_res = vmi.YResolution;
    bits_per_pixel = vmi.BitsPerPixel;

    // Allocating space for vram
    int r;
    struct minix_mem_range mr;
    unsigned int vram_base = vmi.PhysBasePtr;
    unsigned int vram_size = h_res * v_res * (bits_per_pixel / 8);

    mr.mr_base = (phys_bytes)vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if ((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)) != 0) {
        return NULL;
    }

    // Map VRAM allocated space
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    if (video_mem == MAP_FAILED) {
        return NULL;
    }

    memset(&reg86, 0, sizeof(reg86));
    reg86.ah = 0x4F;
    reg86.al = 0x02; 
    reg86.bx = mode | BIT(14); 
    reg86.intno = 0x10; 

    if (sys_int86(&reg86) != OK) {
        return NULL;
    }
    return video_mem;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    if (x >= h_res || y >= v_res || x + len > h_res) {return 1;}

    int bytes_per_pixel = bits_per_pixel / 8;
    
    uint8_t *ptr = video_mem + (y * h_res + x) * bytes_per_pixel;

    for (int i = 0; i < len; i++) {
        switch (bytes_per_pixel) {
            case 1: // 8 bit 
                if (vmi.MemoryModel == 0x04) { 
                    *ptr = (uint8_t)color;  
                }
                break;

            case 2: // 15/16 bit
                if (vmi.MemoryModel == 0x06) { // Direct color
                    uint16_t pixel_color;
                    if (bits_per_pixel == 15) { // 5:5:5 (dosnt work idk why)
                        pixel_color = ((color >> 16) & 0x1F) << 10 |  
                                    ((color >> 8) & 0x1F) << 5  |  
                                    (color & 0x1F);        
                    } else { // 5:6:5
                        pixel_color = ((color >> 19) & 0x1F) << 11 |
                                    ((color >> 10) & 0x3F) << 5  |
                                    ((color >> 3)  & 0x1F);
                    }   
                    *(uint16_t *)ptr = pixel_color;
                }
                break;

            case 3: // 24 bit
                ptr[0] = (uint8_t)(color);       
                ptr[1] = (uint8_t)(color >> 8); 
                ptr[2] = (uint8_t)(color >> 16); 
                break;

            case 4: // 32 bit
                *(uint32_t *)ptr = color;
                break;
        }
        ptr += bytes_per_pixel;
    }

    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (int i = 0; i < height; i++) {
        if (vg_draw_hline(x, y + i, width, color) != 0) {
            return 1;
        }
    }
    return 0;
}


uint8_t* load_xpm(xpm_map_t xpm, xpm_image_t* img) {
    if (!img) return NULL;
    uint8_t* pixmap = xpm_load(xpm, XPM_8_8_8, img);
    if (!pixmap) return NULL;
    
    return pixmap;
}

int draw_xpm(uint8_t* pixmap, xpm_image_t* img, uint16_t x, uint16_t y) {
    if (!pixmap || !img || !video_mem) return 1;

    uint16_t width = img->width;
    uint16_t height = img->height;

    // dont draw offscreen
    if (x >= h_res || y >= v_res) return 0;

    // logic to draw mouse on border if tries to leave screen
    uint16_t draw_width = (x + width > h_res) ? h_res - x : width;
    uint16_t draw_height = (y + height > v_res) ? v_res - y : height;

    for (uint16_t row = 0; row < draw_height; row++) {
        for (uint16_t col = 0; col < draw_width; col++) {
            uint32_t px = (row * width + col) * 3;
            uint32_t vram = ((y + row) * h_res + (x + col)) * 3;

            // RGB order
            video_mem[vram]     = pixmap[px];     // R
            video_mem[vram + 1] = pixmap[px + 1]; // G
            video_mem[vram + 2] = pixmap[px + 2]; // B
        }
    }

    return 0;
}

int draw_xpm_transparent(uint8_t* pixmap, xpm_image_t* img, uint16_t x, uint16_t y) {
    if (!pixmap || !img || !video_mem) return 1;

    uint16_t width = img->width;
    uint16_t height = img->height;

    if (x >= h_res || y >= v_res) return 0;

    uint16_t draw_width = (x + width > h_res) ? h_res - x : width;
    uint16_t draw_height = (y + height > v_res) ? v_res - y : height;

    for (uint16_t row = 0; row < draw_height; row++) {
        for (uint16_t col = 0; col < draw_width; col++) {
            uint32_t px = (row * width + col) * 3;
            uint32_t vram = ((y + row) * h_res + (x + col)) * 3;
            
            // skip transparent pixels
            if ((pixmap[px] == 0 && pixmap[px+1] == 0 && pixmap[px+2] == 0)) // black = transparent
                continue;
            
            video_mem[vram]     = pixmap[px];     // R
            video_mem[vram + 1] = pixmap[px + 1]; // G
            video_mem[vram + 2] = pixmap[px + 2]; // B
        }
    }

    return 0;
}

void free_xpm(uint8_t* pixmap) {
    if (pixmap) {
        free(pixmap);
    }
}

void load_allxpm(){
    cursor_pixmap = load_xpm((xpm_map_t)hammerCursor, &hammerCursor_img);
    hammerCursor_pixmap = load_xpm((xpm_map_t)hammerCursor, &hammerCursor_img);
    hammerCursorClick_pixmap = load_xpm((xpm_map_t)hammerCursorClick, &hammerCursorClick_img);
    whackMenu_pixmap = load_xpm((xpm_map_t)whackMenu, &whackMenu_img);
    quit_button_pixmap = load_xpm((xpm_map_t)quit_button, &quit_button_img);
    moleUp_pixmap = load_xpm((xpm_map_t)moleUp, &moleUp_img);
    moleDown_pixmap = load_xpm((xpm_map_t)moleDown, &moleDown_img);
    background_pixmap = load_xpm((xpm_map_t)background, &background_img);
    loss_esc_screen_pixmap = load_xpm((xpm_map_t)loss_esc_screen, &loss_esc_screen_img);
    win_esc_screen_pixmap = load_xpm((xpm_map_t)win_esc_screen, &win_esc_screen_img);
    settings_screen_pixmap = load_xpm((xpm_map_t)settings_screen_xpm, &settings_screen_img);
    selector_pixmap = load_xpm((xpm_map_t)selector_xpm, &selector_img);
}

void drawImage(uint8_t* pixmap, xpm_image_t* img, int16_t x, int16_t y) {
    draw_xpm(pixmap, img, x, y);
}

void free_allxpm(){
    free_xpm(cursor_pixmap);
    free_xpm(hammerCursor_pixmap);
    free_xpm(hammerCursorClick_pixmap);
    free_xpm(whackMenu_pixmap);
    free_xpm(quit_button_pixmap);
    free_xpm(moleUp_pixmap);
    free_xpm(moleDown_pixmap);
    free_xpm(background_pixmap);
    free_xpm(loss_esc_screen_pixmap);
    free_xpm(win_esc_screen_pixmap);
    free_xpm(settings_screen_pixmap);
    free_xpm(selector_pixmap);
}
