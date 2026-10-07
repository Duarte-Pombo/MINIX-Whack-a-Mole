// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "video_gr.h"
#include "../keyboard/keyboard.h"


extern unsigned h_res;        
extern unsigned v_res;         

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (vg_init(mode) == NULL) {
    return 1;
  }

  sleep(delay);

  if (vg_exit() != 0) {
    return 1;
  }

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {

    for (int i = 0; i < height; i++) {
        if (vg_draw_hline(x, y + i, width, color) != 0) {
            return 1;
        }
    }

    return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
    if (vg_init(mode) == NULL) {
        return 1;
    }

    uint16_t rect_width = h_res / no_rectangles;
    uint16_t rect_height = v_res / no_rectangles;

    // draw the pattern
    for (int row = 0; row < no_rectangles; row++) {
        for (int col = 0; col < no_rectangles; col++) {
            uint32_t color;
            
            if (vmi.MemoryModel == 0x04) { // indexed color mode
                color = (first + (row * no_rectangles + col) * step) % (1 << vmi.BitsPerPixel);
            } else { // direct color mode
                uint8_t r_first = (first >> vmi.RedFieldPosition) & ((1 << vmi.RedMaskSize) - 1);
                uint8_t g_first = (first >> vmi.GreenFieldPosition) & ((1 << vmi.GreenMaskSize) - 1);
                uint8_t b_first = (first >> vmi.BlueFieldPosition) & ((1 << vmi.BlueMaskSize) - 1);

                uint8_t r = (r_first + col * step) % (1 << vmi.RedMaskSize);
                uint8_t g = (g_first + row * step) % (1 << vmi.GreenMaskSize);
                uint8_t b = (b_first + (col + row) * step) % (1 << vmi.BlueMaskSize);

                color = (r << vmi.RedFieldPosition) | 
                       (g << vmi.GreenFieldPosition) | 
                       (b << vmi.BlueFieldPosition);
            }

            if (vg_draw_rectangle(col * rect_width, row * rect_height, rect_width, rect_height, color) != 0) {
                vg_exit();
                return 1;
            }
        }
    }

    // draw black stripes if needed
    // right stripe
    if (h_res % no_rectangles != 0) {
        uint16_t stripe_width = h_res - (no_rectangles * rect_width);
        if (vg_draw_rectangle(no_rectangles * rect_width, 0, stripe_width, v_res, 0) != 0) {
            vg_exit();
            return 1;
        }
    }

    // bottom stripe
    if (v_res % no_rectangles != 0) {
        uint16_t stripe_height = v_res - (no_rectangles * rect_height);
        if (vg_draw_rectangle(0, no_rectangles * rect_height, h_res, stripe_height, 0) != 0) {
            vg_exit();
            return 1;
        }
    }

    uint8_t bit_no;
    if (keyboard_subscribe_int(&bit_no) != 0) {
        vg_exit();
        return 1;
    }
    int kbd_irq = BIT(bit_no);

    // wait for esc key
    int ipc_status;
    message msg;
    while (1) {
        if (driver_receive(ANY, &msg, &ipc_status) != 0) {
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & kbd_irq) {
                        kbc_ih();
                        if (scancode == 0x81) { 
                            if (keyboard_unsubscribe_int() != 0) {
                                vg_exit();
                                return 1;
                            }
                            if (vg_exit() != 0) {
                                return 1;
                            }
                            return 0;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {

    // init video mode 0x105

    uint8_t* vram = vg_init(0x105);
    if (vram == NULL){
        printf ("error init video mode\n");
        return 1;
    }

    uint8_t* pixmap;
    xpm_image_t img;
    enum xpm_image_type type = XPM_INDEXED;

    //load xpm onto pixmap buffer with dimensions 
    pixmap = xpm_load(xpm , type , &img);
    if (pixmap == NULL){
        printf ("errado \n");
        return 1;
    }

    //draw the pixmap
    for (uint32_t imgRow = 0; imgRow < img.height; imgRow++){
        uint8_t* rowStartVRAM = vram + (y + imgRow) * h_res + x; 
        memcpy(rowStartVRAM, pixmap + imgRow * img.width, img.width);
    }

    
    uint8_t bit_no;
    if (keyboard_subscribe_int(&bit_no) != 0) {
        vg_exit();
        return 1;
    }
    int kbd_irq = BIT(bit_no);
   
   // changed the function to exit on esc
    int ipc_status;
    message msg;
    while (1) {
        if (driver_receive(ANY, &msg, &ipc_status) != 0) {
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & kbd_irq) {
                        kbc_ih();
                        if (scancode == 0x81) {
                            if (keyboard_unsubscribe_int() != 0) {
                                return 1;
                            }
                            // return to text mode
                            if (vg_exit() != 0) {
                                return 1;
                            }
                            return 0;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    //unsub kbd interrupts
    if (keyboard_unsubscribe_int() != 0) {
        printf("error trying to unsub kbd \n");
        // eu acho que é tasse continuar invés de retornar erro
    }

    //retrun to text mode
    if (vg_exit() != 0){
        printf ("error returning to text mode");
        return 1;
    }

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

