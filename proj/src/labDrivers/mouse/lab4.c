// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <lcom/lab4.h>

// Any header files included below this line should have been created by you

#include "../keyboard/i8042.h"
#include "mouse.h"
#include "../timer/timer.h"



extern bool mouse_error;
extern uint8_t mouse_byte;
extern unsigned int timer_counter;

int (mouse_test_packet)(uint32_t cnt) {
    if (cnt == 0) return 0;
    uint8_t bit_no;
    if (mouse_send_cmd_with_ack(MOUSE_ENABLE_DATA_REPORT) != 0) return 1;
    if (mouse_subscribe(&bit_no) != 0) return 1;
    int rq = BIT(bit_no);

    int ipc_status, r;
    message msg;
    uint8_t packet_bytes[3];
    uint8_t byte_idx = 0;
    uint32_t packets_printed = 0;

    while (packets_printed < cnt) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d\n", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & rq) {
                        mouse_ih();
                        if (mouse_error) break;
                        uint8_t b = mouse_byte;
                        if (byte_idx == 0 && !(b & BIT(3))) break; // sync: first byte must have bit 3 set
                        packet_bytes[byte_idx++] = b;
                        if (byte_idx == 3) {
                            struct packet pp;
                            process_packet(&pp, packet_bytes);
                            mouse_print_packet(&pp);
                            byte_idx = 0;
                            packets_printed++;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        printf("msg.m_notify.interrupts: 0x%X, BIT(bit_no): 0x%X\n", msg.m_notify.interrupts, BIT(bit_no));
    }
    if (mouse_send_cmd_with_ack(MOUSE_DISABLE_DATA_REPORT) != 0) return 1;
    if (mouse_unsubscribe() != 0) {
        printf("Error removing mouse interrupt policy\n");
        return 1;
    }
    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    if (idle_time == 0) return 0;
    uint8_t mouse_bit_no;
    uint8_t timer_bit_no;
    int mouse_rq, timer_rq;
    if (mouse_send_cmd_with_ack(MOUSE_ENABLE_DATA_REPORT) != 0) return 1;
    if (mouse_subscribe(&mouse_bit_no) != 0) return 1;
    
    if (timer_subscribe_int(&timer_bit_no) != 0) {
        mouse_unsubscribe();
        return 1;
    }

    mouse_rq = BIT(mouse_bit_no);
    timer_rq = BIT(timer_bit_no);


    timer_counter = 0;
    int ipc_status, r;
    message msg;
    uint8_t packet_bytes[3];
    uint8_t byte_idx = 0;

    while (timer_counter < idle_time * sys_hz()) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d\n", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & timer_rq) {
                        timer_int_handler();
                    }
                    if (msg.m_notify.interrupts & mouse_rq) {
                        mouse_ih();
                        if (mouse_error) break;
                        uint8_t b = mouse_byte;
                        if (byte_idx == 0 && !(b & BIT(3))) break; // sync: first byte must have bit 3 set
                        packet_bytes[byte_idx++] = b;
                        if (byte_idx == 3) {
                            struct packet pp;
                            process_packet(&pp, packet_bytes);
                            mouse_print_packet(&pp);
                            byte_idx = 0;
                            timer_counter = 0; // Reset timer only after a full packet
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    if (mouse_send_cmd_with_ack(MOUSE_DISABLE_DATA_REPORT) != 0) return 1;
    mouse_unsubscribe();
    timer_unsubscribe_int();
    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    enum { INIT, DRAW_UP, VERTEX, DRAW_DOWN, DONE } state = INIT;
    int16_t x_drawn = 0, y_drawn = 0;
    uint8_t bit_no;
    if (mouse_send_cmd_with_ack(MOUSE_ENABLE_DATA_REPORT) != 0) return 1;
    if (mouse_subscribe(&bit_no) != 0) return 1;
    int rq = BIT(bit_no);

    int ipc_status, r;
    message msg;
    uint8_t packet_bytes[3];
    uint8_t byte_idx = 0;

    while (state != DONE) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d\n", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & rq) {
                        mouse_ih();
                        if (mouse_error) break;
                        uint8_t b = mouse_byte;
                        if (byte_idx == 0 && !(b & BIT(3))) break; // sync: first byte must have bit 3 set
                        packet_bytes[byte_idx++] = b;
                        if (byte_idx == 3) {
                            struct packet pp;
                            process_packet(&pp, packet_bytes);
                            mouse_print_packet(&pp);

                            int16_t dx = pp.delta_x, dy = pp.delta_y;

                            switch (state) {
                                case INIT:
                                    if (pp.lb && !pp.rb && !pp.mb) {
                                        state = DRAW_UP;
                                        x_drawn = 0; y_drawn = 0;
                                    }
                                    break;
                                case DRAW_UP:
                                    if (pp.lb && !pp.rb && !pp.mb) {
                                        if (dx < -tolerance || dy < -tolerance) { state = INIT; break; }
                                        x_drawn += dx;
                                        y_drawn += dy;
                                    } else if (!pp.lb && !pp.rb && !pp.mb) {
                                        if (abs(x_drawn) >= x_len && abs(y_drawn) >= x_len && abs(y_drawn) > abs(x_drawn) && y_drawn > 0) {
                                            state = VERTEX;
                                        } else {
                                            state = INIT;
                                        }
                                    } else {
                                        state = INIT;
                                    }
                                    break;
                                case VERTEX:
                                    if (abs(dx) > tolerance || abs(dy) > tolerance) { state = INIT; break; }
                                    if (pp.rb && !pp.lb && !pp.mb) {
                                        state = DRAW_DOWN;
                                        x_drawn = 0; y_drawn = 0;
                                    } else if (pp.lb || pp.mb) {
                                        state = INIT;
                                    }
                                    break;
                                case DRAW_DOWN:
                                    if (pp.rb && !pp.lb && !pp.mb) {
                                        if (dx < -tolerance || dy > tolerance) { state = INIT; break; }
                                        x_drawn += dx;
                                        y_drawn += dy;
                                    } else if (!pp.lb && !pp.rb && !pp.mb) {
                                        if (abs(x_drawn) >= x_len && abs(y_drawn) >= x_len && abs(y_drawn) > abs(x_drawn) && y_drawn < 0) {
                                            state = DONE;
                                        } else {
                                            state = INIT;
                                        }
                                    } else {
                                        state = INIT;
                                    }
                                    break;
                                default: break;
                            }
                            byte_idx = 0;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    if (mouse_send_cmd_with_ack(MOUSE_DISABLE_DATA_REPORT) != 0) return 1;
    mouse_unsubscribe();
    return 0;
}

