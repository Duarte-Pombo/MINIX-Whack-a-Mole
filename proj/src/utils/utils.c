#include <lcom/lcf.h>
#include "utils.h"
#include <stdint.h>

//int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
//  uint8_t *lsb = val & 0xFF;
//  return 0;
//}

//int(util_get_MSB)(uint16_t val, uint8_t *msb) {
//  uint8_t *lsb 0 (val >> 8) & 0xFF;
//  return 0;
//}

int util_sys_inb(int port, uint8_t *byte) {
    
  uint32_t temp; // sys calls return 4 byte value even if sys_inb only reads a byte (stored in LSB of temp)
  
  // error handeling
  if (byte == NULL)
    return 1;

  if (sys_inb(port, &temp) != OK)
    return 1;
  
  *byte = (uint8_t)temp; // by converting uint32_t -> uint8_t we are extracting the only important data from the sys call sys_inb (the LSB)
  return 0;
}

int subscribe_interrupts(uint8_t *mouse_bit_no, uint8_t *timer_bit_no, uint8_t *keyboard_bit_no) {
    if (mouse_send_cmd_with_ack(MOUSE_ENABLE_DATA_REPORT) != 0) {
        printf("error getting mouse reports\n");
        return 1;
    }

    if (mouse_subscribe(mouse_bit_no) != 0) {
        printf("error subbing mouse\n");
        return 1;
    } else {
        printf("subbing mouse\n");
    }
    
    if (keyboard_subscribe_int(keyboard_bit_no) != 0) {
        printf("error subbing kbd\n");
        mouse_unsubscribe();
        return 1;
    } else {
        printf("subbing kbd\n");
    }
    
    if (timer_subscribe_int(timer_bit_no) != 0) {
        printf("error subbing timer\n");
        keyboard_unsubscribe_int();
        mouse_unsubscribe();
        return 1;
    } else {
        printf("subbing timer\n");
    }

    return 0;
}

int unsubscribe_interrupts(void) {
    int res = 0;
    
    if (mouse_send_cmd_with_ack(MOUSE_DISABLE_DATA_REPORT) != 0) {
        printf("error disable mouse data report\n");
        res = 1;
    }
    
    if (mouse_unsubscribe() != 0) {
        printf("error to unsub mouse\n");
        res = 1;
    }
    
    if (keyboard_unsubscribe_int() != 0) {
        printf("erro to unsub kbd\n");
        res = 1;
    }
    
    if (timer_unsubscribe_int() != 0) {
        printf("error to unsub timer\n");
        res = 1;
    }
    
    return res;
}


