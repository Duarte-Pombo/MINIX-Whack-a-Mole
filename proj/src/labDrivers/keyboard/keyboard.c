#include <lcom/lcf.h>

#include <lcom/lab3.h>


#include <stdbool.h>
#include <stdint.h>


#include "i8042.h"
#include "keyboard.h"



bool has_e0 = false;
uint8_t scancode = 0;
static int keyboard_hook_id = 1; 

#ifdef LAB3
int sys_inb_counter = 0;
#endif

// Bottom Layer: Direct KBC register access
int kbc_read_status(uint8_t *status) {
    uint32_t status32;
    #ifdef LAB3
    sys_inb_counter++;
    #endif
    if (sys_inb(KBC_STATUS_REG, &status32) != OK) return 1;
    *status = (uint8_t)status32;
    return 0;
}

int kbc_read_data(uint8_t *data) {
    uint32_t data32;
    #ifdef LAB3
    sys_inb_counter++;
    #endif
    if (sys_inb(KBC_OUT_BUF, &data32) != OK) return 1;
    *data = (uint8_t)data32;
    return 0;
}

int kbc_write_cmd(uint8_t cmd) {
    uint8_t status;
    int attempts = 0;
    
    while (attempts < 3) {
        if (kbc_read_status(&status) != 0) return 1;
        
        if (!(status & KBC_IBF)) {
            if (sys_outb(KBC_CMD_REG, cmd) != OK) return 1;
            return 0;
        }
        
        attempts++;
        tickdelay(micros_to_ticks(KBC_TIMEOUT));
    }
    return 1;
}

int kbc_write_data(uint8_t data) {
    uint8_t status;
    int attempts = 0;
    
    while (attempts < 3) {
        if (kbc_read_status(&status) != 0) return 1;
        
        if (!(status & KBC_IBF)) {
            if (sys_outb(KBC_IN_BUF, data) != OK) return 1;
            return 0;
        }
        
        attempts++;
        tickdelay(micros_to_ticks(KBC_TIMEOUT));
    }
    return 1;
}

// Middle Layer: KBC command protocol
int kbc_read_cmd_byte(uint8_t *cmd) {
    if (kbc_write_cmd(KBC_READ_CMD) != 0) return 1;
    if (kbc_read_data(cmd) != 0) return 1;
    return 0;
}

int kbc_write_cmd_byte(uint8_t cmd) {
    if (kbc_write_cmd(KBC_WRITE_CMD) != 0) return 1;
    if (kbc_write_data(cmd) != 0) return 1;
    return 0;
}

// Top Layer: High-level KBC operations
int(keyboard_re_enable_int)() {
    uint8_t cmd;
    if (kbc_read_cmd_byte(&cmd) != 0) return 1;
    cmd |= KBC_ENABLE_INT;
    if (kbc_write_cmd_byte(cmd) != 0) return 1;
    return 0;
}

int read_byte() {
    uint8_t status, data;
    
    if (kbc_read_status(&status) != 0) return 1;

    if (status & KBC_OUT_BUF_FULL) {
        if (status & (KBC_PARITY_ERROR | KBC_TIMEOUT_ERROR)) {
            return 1; 
        }

        if (kbc_read_data(&data) != 0) return 1;

        if (data == 0xE0) { 
            has_e0 = true; 
            return 0; 
        }
        if (has_e0) { 
            scancode = (0xE0 << 8) | data; 
            has_e0 = false;
        } else {
            scancode = data; 
        }
        return 0;
    }
    return 1;
}

void (kbc_ih)() {
    read_byte();
}

int(keyboard_subscribe_int)(uint8_t *bit_no) {
    int policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
    if (sys_irqsetpolicy(IRQ1_KEYBOARD, policy, &keyboard_hook_id) != OK) {
        printf("Error setting keyboard interrupt policy\n");
        return 1;
    }
    *bit_no = IRQ1_KEYBOARD;
    return 0;
}

int(keyboard_unsubscribe_int)() {
    if (sys_irqrmpolicy(&keyboard_hook_id) != OK) {
        printf("Error removing keyboard interrupt policy\n");
        return 1;
    }
    return 0;
}

