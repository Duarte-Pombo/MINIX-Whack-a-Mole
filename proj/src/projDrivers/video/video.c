#include "video.h"
#include <lcom/lcf.h>
#include <lcom/vbe.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <minix/sysutil.h>

void *(vg_init)(uint16_t mode) {
    reg86_t reg86;

    if (vbe_get_mode_info(mode, &vmi) != 0) {
        return NULL;
    }

    h_res = vmi.XResolution;
    v_res = vmi.YResolution;
    bits_per_pixel = vmi.BitsPerPixel;

    // Allocating space for vram
    int r;
    struct minix_mem_range mr;
    unsigned int vram_base = vmi.PhysBasePtr;
    unsigned int vram_size = h_res * v_res * (bits_per_pixel / 8);

    mr.mr_base = (phys_bytes)vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if ((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)) != 0) {
        return NULL;
    }

    // Map VRAM allocated space
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    if (video_mem == MAP_FAILED) {
        return NULL;
    }

    memset(&reg86, 0, sizeof(reg86));
    reg86.ah = 0x4F;
    reg86.al = 0x02; 
    reg86.bx = mode | BIT(14); 
    reg86.intno = 0x10; 

    if (sys_int86(&reg86) != OK) {
        return NULL;
    }
    return video_mem;
}
