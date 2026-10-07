#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdbool.h>
#include <stdint.h>

// Timer interrupt handling
int (timer_subscribe_int)(uint8_t *bit_no);
int (timer_unsubscribe_int)();
void (timer_int_handler)();


// Timer counter variable
extern unsigned int timer_counter;

#endif 
