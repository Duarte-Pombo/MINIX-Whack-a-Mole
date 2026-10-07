#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "keyboard.h"
#include "../timer/timer.h"

extern bool has_e0;
extern uint8_t scancode;

// Timer related variables
extern unsigned int timer_counter;

extern bool shortcuts_enabled;


int(kbd_test_scan)() {
  uint8_t scancode_bytes[2]; // Stores bytes of the scancode (max 2 bytes)
  uint8_t scancode_size = 0; // Tracks how many bytes we've received
  bool make = false;     // True for make codes, false for break codes
  bool esc_released = false;  // Set to true when ESC break code is detected
  int r;
  uint8_t bit_no;

  if (keyboard_subscribe_int(&bit_no) != 0) {
    return 1;
  }

  int rq = BIT(bit_no);

  while (!esc_released) {
      int ipc_status;
      message msg;
      if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
        printf("driver_receive failed with: %d\n", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) {
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE:
                  if (msg.m_notify.interrupts & rq) {
                      kbc_ih();
                      make = !(scancode & 0x80);
                      if (has_e0) { // Two-byte scancode
                        scancode_bytes[0] = 0xE0;
                        scancode_bytes[1] = scancode;
                        scancode_size = 2;
                    } else { // One-byte scancode
                        scancode_bytes[0] = scancode;
                        scancode_size = 1;
                    }
                      kbd_print_scancode(make, scancode_size, scancode_bytes);
                      if (scancode_size == 1 && scancode_bytes[0] == 0x81) esc_released = true;
                }
                  break;
              default:
                  break;
            }
      }
  }

  if (keyboard_unsubscribe_int() != 0) {
    return 1;
  }

  return 0;
}

int (kbd_test_poll)() {
  uint8_t scancode_bytes[2];
  uint8_t scancode_size = 0;
  bool make = false;
  bool esc_released = false;

  while (!esc_released) {
      if (read_byte() == 0) {  // Successfully read a byte
          make = !(scancode & 0x80);
          if (has_e0) {
              scancode_bytes[0] = 0xE0;
              scancode_bytes[1] = scancode;
              scancode_size = 2;
              has_e0 = false;
          } else {
              scancode_bytes[0] = scancode;
              scancode_size = 1;
          }
          
          kbd_print_scancode(make, scancode_size, scancode_bytes);
          
          // Check for ESC break code
          if (scancode_size == 1 && scancode_bytes[0] == 0x81) {
              esc_released = true;
          }
      }
      
      tickdelay(micros_to_ticks(20000)); // Prevent CPU overload
  }

  if (keyboard_re_enable_int() != 0) return 1;

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t scancode_bytes[2];
  uint8_t scancode_size = 0;
  bool make = false;
  bool esc_released = false;
  int r;
  uint8_t kbd_bit_no;
  uint8_t timer_bit_no;

  if (keyboard_subscribe_int(&kbd_bit_no) != 0) {
    return 1;
  }

  if (timer_subscribe_int(&timer_bit_no) != 0) {
    return 1;
  }

  int kbd_irq = BIT(kbd_bit_no);
  int timer_irq = BIT(timer_bit_no);

  while (!esc_released && timer_counter < n * sys_hz()) {  
    int ipc_status;
    message msg;
    
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d\n", r);
      continue;
    }
    
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & timer_irq) {
            timer_int_handler();
          }
          if (msg.m_notify.interrupts & kbd_irq) {
            timer_counter = 0;  // Reset timer when key is pressed
            kbc_ih();
            make = !(scancode & 0x80);
            if (has_e0) {
              scancode_bytes[0] = 0xE0;
              scancode_bytes[1] = scancode;
              scancode_size = 2;
            } else {
              scancode_bytes[0] = scancode;
              scancode_size = 1;
            }
            kbd_print_scancode(make, scancode_size, scancode_bytes);
            if (scancode_size == 1 && scancode_bytes[0] == 0x81) {
              esc_released = true;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  // Unsubscribe from both interrupts - order doesn't matter here
  if (timer_unsubscribe_int() != 0) {
    return 1;
  }

  if (keyboard_unsubscribe_int() != 0) {
    return 1;
  }

  return 0;
}
