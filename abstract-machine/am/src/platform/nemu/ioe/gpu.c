#include <am.h>
#include <nemu.h>
#include <stdint.h>

#include "../include/nemu.h"

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
    int i;
    int w = inl(VGACTL_ADDR) >> 16;  // TODO: get the correct width
    int h = inl(VGACTL_ADDR) & 0xffff;  // TODO: get the correct height
    uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
    for (i = 0; i < w * h; i ++) fb[i] = i;
    outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t screen_wh = inl(VGACTL_ADDR);
  uint32_t screen_w = screen_wh >> 16;
  uint32_t screen_h = screen_wh & 0xffff;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = screen_w, .height = screen_h,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
