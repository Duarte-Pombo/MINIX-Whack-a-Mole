#ifndef _MOUSE_H_
#define _MOUSE_H_

#include <stdbool.h>
#include <stdint.h>

// Function declarations


// Interrupt handling
int mouse_subscribe(uint8_t *bit_no);
int mouse_unsubscribe();

void (mouse_ih)();

// Mouse command helpers
int mouse_write_cmd(uint8_t cmd); 
int mouse_send_cmd_with_ack(uint8_t cmd); 
void process_packet(struct packet *pp, uint8_t packet_bytes[3]);

// Variables
extern uint8_t mouse_byte;
extern bool mouse_error;

#endif 
