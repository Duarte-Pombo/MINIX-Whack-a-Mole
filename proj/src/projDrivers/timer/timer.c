#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "timer.h"
#include "i8254.h"
#include "../keyboard/i8042.h"

extern unsigned int timer_counter;

unsigned int timer_counter = 0;
static int timer_hook_id = 0;

void (timer_int_handler)(void) {
    timer_counter++;
} 

int (timer_subscribe_int)(uint8_t *bit_no) {
  if (sys_irqsetpolicy(IRQ0_TIMER, IRQ_REENABLE, &timer_hook_id) != OK) {
    printf("Error setting timer interrupt policy\n");
    return -1;
}
*bit_no = IRQ0_TIMER;
return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&timer_hook_id) != OK) {
      printf("Error removing timer interrupt policy\n");
      return -1;
  }

  return 0;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
    //error handeling
    if (timer > 2 || st == NULL)
      return 1;

    uint8_t rb_command = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer); // generate a Read Back Command

    //error handeling
    if (sys_outb(TIMER_CTRL, rb_command) != OK)
      return 1;

    return util_sys_inb(TIMER_0 + timer, st);
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  uint8_t init_mode_bits = (st >> 4) & 0x03;
  union timer_status_field_val conf_val;

  switch (field) {
        case tsf_all:
            conf_val.byte = st;
            break;

        case tsf_initial:
            switch (init_mode_bits) {
                case 0:
                    conf_val.in_mode = INVAL_val;
                    break;
                case 1:
                    conf_val.in_mode = LSB_only;
                    break;
                case 2:
                    conf_val.in_mode = MSB_only;
                    break;
                case 3:
                    conf_val.in_mode = MSB_after_LSB;
                    break;
                default:
                    conf_val.in_mode = INVAL_val;
                    break;
            }
            break;

        case tsf_mode:
            conf_val.count_mode = (st >> 1) & 0x07;
            break;

        case tsf_base:
            conf_val.bcd = st & 0x01;
            break;
        default:
            return 1;
    }
    timer_print_config(timer, field, conf_val);

    return 0;
}
