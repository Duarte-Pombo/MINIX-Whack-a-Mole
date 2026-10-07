#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdbool.h>
#include <stdint.h>

// Function declarations
int read_byte();
int keyboard_re_enable_int();

// Interrupt handling
int keyboard_subscribe_int(uint8_t *bit_no);
int keyboard_unsubscribe_int();

// Bottom Layer: Direct KBC register access
int kbc_read_status(uint8_t *status);
int kbc_read_data(uint8_t *data);
int kbc_write_cmd(uint8_t cmd);
int kbc_write_data(uint8_t data);

// Middle Layer: KBC command protocol
int kbc_read_cmd_byte(uint8_t *cmd);
int kbc_write_cmd_byte(uint8_t cmd);

// Variables
extern bool has_e0;
extern uint8_t scancode;
extern int sys_inb_counter;

#endif 
