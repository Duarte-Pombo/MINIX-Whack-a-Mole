#include <lcom/lcf.h>

#include <lcom/lab4.h>


#include <stdbool.h>
#include <stdint.h>


#include "../keyboard/i8042.h"
#include "mouse.h"

static int mouse_hook_id = 12;
uint8_t mouse_byte = 0;
bool mouse_error = false;

void (mouse_ih)(){
    uint32_t status32, data32;
    uint8_t status, data;
    mouse_error = false;
    if (sys_inb(KBC_STATUS_REG, &status32) != 0) {
        printf("[mouse_ih] Error reading KBC_STATUS_REG\n");
        mouse_error = true;
        return;
    }
    status = (uint8_t)status32;
    if ((status & KBC_OUT_BUF_FULL) && (status & KBC_AUX)) {
        if (sys_inb(KBC_OUT_BUF, &data32) != 0) {
            printf("[mouse_ih] Error reading KBC_OUT_BUF\n");
            mouse_error = true;
            return;
        }
        data = (uint8_t)data32;
        if (status & (KBC_PARITY_ERROR | KBC_TIMEOUT_ERROR)) {
            printf("[mouse_ih] Parity or timeout error\n");
            mouse_error = true;
            return;
        }
        mouse_byte = data;
    } else {
        printf("[mouse_ih] Output buffer not full or not AUX\n");
        mouse_error = true;
    }
}

int mouse_subscribe(uint8_t *bit_no) {
    int policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
    if (sys_irqsetpolicy(IRQ12_MOUSE, policy, &mouse_hook_id) != OK) {
        printf("[mouse_subscribe] Error setting mouse interrupt policy\n");
        return 1;
    }
    *bit_no = IRQ12_MOUSE;
    return 0;
}

int mouse_unsubscribe() {
    if (sys_irqrmpolicy(&mouse_hook_id) != OK) {
        printf("[mouse_unsubscribe] Error removing mouse interrupt policy\n");
        return 1;
    }
    return 0;
}

int mouse_write_cmd(uint8_t cmd) {
    uint32_t status32;
    uint8_t status;
    int attempts = 0;
    while (attempts < 3) {
        if (sys_inb(KBC_STATUS_REG, &status32) != 0) {
            printf("[mouse_write_cmd] Error reading KBC_STATUS_REG\n");
            return 1;
        }
        status = (uint8_t)status32;
        if (!(status & KBC_IBF)) {
            if (sys_outb(KBC_CMD_REG, WRITE_BYTE_TO_MOUSE) != OK) {
                printf("[mouse_write_cmd] Error writing WRITE_BYTE_TO_MOUSE\n");
                return 1;
            }
            if (sys_inb(KBC_STATUS_REG, &status32) != 0) {
                printf("[mouse_write_cmd] Error reading KBC_STATUS_REG (2)\n");
                return 1;
            }
            status = (uint8_t)status32;
            if (!(status & KBC_IBF)) {
                if (sys_outb(KBC_IN_BUF, cmd) != OK) {
                    printf("[mouse_write_cmd] Error writing command to KBC_IN_BUF\n");
                    return 1;
                }
                return 0;
            }
        }
        attempts++;
        tickdelay(micros_to_ticks(KBC_TIMEOUT));
    }
    return 1;
}

int mouse_send_cmd_with_ack(uint8_t cmd) {
    int attempts = 0;
    while (attempts < 5) {
        if (mouse_write_cmd(cmd) != 0) return 1;
        tickdelay(micros_to_ticks(20000));
        uint32_t ack32;
        uint8_t ack;
        if (sys_inb(KBC_OUT_BUF, &ack32) != 0) {
            printf("[mouse_send_cmd_with_ack] Error reading ACK from KBC_OUT_BUF\n");
            return 1;
        }
        ack = (uint8_t)ack32;
        if (ack == MOUSE_ACK) return 0;
        else if (ack == 0xFE) {
            attempts++;
            continue;
        } else if (ack == 0xFC) {
            return 1;
        }
    }

    return 1;
}

void process_packet(struct packet *pp, uint8_t packet_bytes[3]) {
    for (int i = 0; i < 3; ++i) pp->bytes[i] = packet_bytes[i];
    pp->lb = packet_bytes[0] & BIT(0);
    pp->rb = packet_bytes[0] & BIT(1);
    pp->mb = packet_bytes[0] & BIT(2);
    pp->x_ov = packet_bytes[0] & BIT(6);
    pp->y_ov = packet_bytes[0] & BIT(7);
    pp->delta_x = (packet_bytes[0] & BIT(4)) ? (int16_t)(packet_bytes[1] | 0xFF00) : packet_bytes[1];
    pp->delta_y = (packet_bytes[0] & BIT(5)) ? (int16_t)(packet_bytes[2] | 0xFF00) : packet_bytes[2];
}
