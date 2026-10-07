#include "kbc.h"
#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>

extern uint8_t scancode; // Declare external global variable

int (kbc_init)(void) {
    return keyboard_re_enable_int();
}

int (kbc_read_scancode)(uint8_t *scancode_out) {
    if (read_byte() != 0) return 1;
    *scancode_out = scancode; // Use the global scancode variable
    return 0;
} 
