# Whack-a-Mole | Low-Level Interactive System in MINIX 3

An interactive real-time game developed in C for the MINIX 3 microkernel operating system. Built from the ground up without third-party game engines or standard high-level graphical APIs, this project implements bare-metal device drivers, custom memory management, asynchronous interrupt servicing, and a hardware-synchronized rendering pipeline.

---

## Technical Highlights

* **Bare-Metal Device Drivers:** Implemented low-level drivers from scratch to control system peripherals directly via I/O port programming (`sys_inb`, `sys_outb`) and kernel interrupt subscriptions.
* **Event-Driven IPC Architecture:** Orchestrated an asynchronous main event loop utilizing MINIX Inter-Process Communication (`driver_receive`) to handle hardware notifications concurrently.
* **Direct Framebuffer Graphics:** Configured VESA BIOS Extensions (VBE Mode `0x118`: 1024x768, 24-bit direct color) via physical video RAM mapping into process virtual address space.
* **Flicker-Free Optimized Blitting:** Built a dirty-rectangle saving and restoration pipeline for dynamic cursor rendering without full-screen repaints, preserving system throughput.
* **Multi-Layered Peripheral Integration:** Seamlessly merged 60 Hz timer ticks, PS/2 mouse packet assembly, and raw scancode keyboard processing.

---

## Architecture & Hardware Subsystems

The application interfaces directly with four fundamental PC architectural components:

### 1. Video Subsystem (VESA/VBE Graphics Controller)
* **Initialization:** Invoked VBE BIOS services (`sys_int86`, interrupt `0x10`, service `0x4F02`) to initialize linear frame buffer mode `0x118`.
* **Memory Mapping:** Allocated process virtual memory mapped directly to the physical base address of VRAM using `vm_map_phys` and `sys_privctl`.
* **Rendering Engine:** Implemented transparent XPM pixmap decompression and custom blitting buffers to eliminate screen tearing during cursor and mole updates.

### 2. Programmable Interval Timer (Intel 8254 PIT)
* **Frequency & Synchronization:** Programmed Channel 0 running at 60 Hz to drive the deterministic game loop and mole life-cycle state transitions.
* **Interrupt Servicing:** Subscribed to IRQ 0 with policy `IRQ_REENABLE` to track system uptime and countdown counters.

### 3. PS/2 Mouse Controller
* **Packet Deserialization:** Captured raw bytes on IRQ 12, synchronizing stream alignment via byte-0 bit indicators and assembling full 3-byte movement/button state packets.
* **Dynamic Sensitivity Engine:** Scaled relative positional deltas ($\Delta X$, $\Delta Y$) against real-time user sensitivity settings and screen boundaries.

### 4. Keyboard Controller (Intel 8042 KBC)
* **Scancode Parser:** Read single-byte and two-byte (`0xE0` prefixed) make and break scancodes from the output buffer with status verification (parity and timeout checks).
* **Fast-Path Targeting:** Implemented direct keypad shortcut mappings (`Q`, `W`, `E`, `A`, `S`, `D`) for instantaneous targeted input handling.

---

## Project Structure

```text
.
├── src/
│   ├── proj.c               # Kernel loop, interrupt multiplexing & dispatching
│   ├── gameLogic/           # Game FSM, scoring rules, collision, and state transitions
│   ├── uiElements/          # Dynamic sprite buffers (cursor tracking, mole rendering)
│   ├── projDrivers/         # Custom device drivers optimized for application runtime
│   │   ├── video/           # VBE mode configuration, pixel blitting, and page handling
│   │   ├── timer/           # PIT 8254 interrupt configuration and timekeeping
│   │   ├── keyboard/        # KBC 8042 low-level scancode processing
│   │   └── mouse/           # PS/2 packet synchronization and coordinate resolution
│   ├── labDrivers/          # Foundational hardware register abstraction layers
│   ├── sprites/             # Pre-compiled XPM sprite and background assets
│   └── settings.c           # Configuration logic (sensitivity, key bindings, difficulty)
└── Makefile                 # Compilation directives under the LCOM Framework

```

---

## Build & Execution Instructions

This project targets the **MINIX 3** OS within the LCOM Framework environment.

### 1. Build

Compile the application using the native Makefile:

```bash
make

```

### 2. Execution

Run the binary with privileged driver access:

```bash
lcom_run proj

```

### 3. Termination

To exit cleanly, press `ESC` in-game. Alternatively, abort execution via the terminal:

```bash
lcom_stop proj

```

---

## Controls

* **Mouse:** Move to position the hammer; Left-Click to strike moles or navigate menus.
* **Quick-Strike Keys:** Direct mole attacks using keys `Q`, `W`, `E` (upper row) and `A`, `S`, `D` (lower row).
* **ESC:** Immediately exit or terminate the program.
