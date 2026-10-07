#ifndef I8042_H
#define I8042_H

// Ports
#define KBC_STATUS_REG 0x64  
#define KBC_OUT_BUF    0x60  
#define KBC_CMD_REG    0x64
#define KBC_IN_BUF     0x60

//IRQ LINE 

#define IRQ12_MOUSE 12
#define IRQ0_TIMER 0
#define IRQ1_KEYBOARD 1

// Status Register Bits
#define KBC_OUT_BUF_FULL   BIT(0) 
#define KBC_PARITY_ERROR   BIT(7) 
#define KBC_TIMEOUT_ERROR  BIT(6)
#define KBC_AUX            BIT(5)
#define KBC_INH            BIT(4)
#define KBC_A2             BIT(3)
#define KBC_SYS            BIT(2)
#define KBC_IBF            BIT(1)

// Commands
#define KBC_READ_CMD   0x20
#define KBC_WRITE_CMD  0x60
#define KBC_ENABLE_INT BIT(0)
#define WRITE_BYTE_TO_MOUSE 0xD4
#define MOUSE_ENABLE_DATA_REPORT 0xF4
#define MOUSE_DISABLE_DATA_REPORT 0xF5
#define MOUSE_ACK 0xFA

// Timeouts
#define KBC_TIMEOUT    20000

#endif
