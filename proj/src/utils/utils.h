#ifndef _UTILS_H_
#define _UTILS_H_

#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <lcom/vbe.h>
#include <lcom/xpm.h>
#include <lcom/utils.h>

#include <stdbool.h>
#include <stdint.h>

#include "../labDrivers/video/video_gr.h"
#include "../labDrivers/mouse/mouse.h"
#include "../labDrivers/keyboard/keyboard.h"
#include "../labDrivers/keyboard/kbc.h"
#include "../labDrivers/keyboard/i8042.h"
#include "../projDrivers/timer/i8254.h"
#include "../projDrivers/timer/timer.h"
#include "../uiElements/cursor.h"  // Add this line to include cursor functions

int util_sys_inb(int port, uint8_t *byte);
int subscribe_interrupts(uint8_t *mouse_bit_no, uint8_t *timer_bit_no, uint8_t *keyboard_bit_no);
int unsubscribe_interrupts(void);

#endif
