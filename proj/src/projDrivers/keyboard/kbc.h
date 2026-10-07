#ifndef _KBC_H_
#define _KBC_H_

#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"

// KBC Constants
#define ESC_BREAK 0x81
#define ESC_MAKE 0x01

// KBC Functions
int (kbc_init)(void);
int (kbc_read_scancode)(uint8_t *scancode);

#endif 
