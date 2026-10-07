#include <lcom/lcf.h>
#include <lcom/lab2.h>
#include <stdbool.h>
#include <stdint.h>

extern int timer_counter;


int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t config;
  timer_get_conf(timer, &config);
  timer_display_conf(timer, config,field);
  return 0;
}

//int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  //timer_set_frequency (timer, freq);
  //return 0;
//}

int(timer_test_int)(uint8_t time) {
  uint8_t bit_no;
  int r;
  int ipc_status;
  message msg;
  if (timer_subscribe_int(&bit_no) != 0) {
      return 1;
  }
  int irq_set = BIT(bit_no);
  
  
  while (time>0) {
      if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
          printf("driver_receive failed with: %d\n", r);
          continue;
      }
      if (is_ipc_notify(ipc_status)) {
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE:
                if (msg.m_notify.interrupts & irq_set) {
                      timer_int_handler();
                      if (timer_counter%sys_hz()==0){
                        timer_print_elapsed_time();
                        timer_counter=0;
                        time--;
                      }
                  }
                  break;
              default:
                  break;
          }
      }
  }
  if (timer_unsubscribe_int() != 0) {
    printf("Error: timer_unsubscribe_int failed\n");
    return 1;
  }
  return 0;
}
