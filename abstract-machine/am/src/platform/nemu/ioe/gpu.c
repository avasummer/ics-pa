#include <am.h>
#include <nemu.h>
#include <stdint.h>
#include <assert.h>
#include "../include/nemu.h"

#define SYNC_ADDR (VGACTL_ADDR + 4)

void get_size(uint32_t* w, uint32_t* h) {
  uint32_t screen_wh = inl(VGACTL_ADDR);
  uint32_t screen_w = screen_wh >> 16;
  uint32_t screen_h = screen_wh & 0xffff;
  if(screen_w && screen_h)return;
}

void __am_gpu_init() {
    int i;
  uint32_t screen_wh = inl(VGACTL_ADDR);
    uint32_t w = screen_wh >> 16;  // TODO: get the correct width
    uint32_t h =  screen_wh & 0xffff;  // TODO: get the correct height
    uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
    for (i = 0; i < w * h; i ++) fb[i] = i;
    outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t w = 0, h = 0;
  get_size(&w, &h);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  // int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  // uint32_t screen_w=0,screen_h=0;
  // get_size(&screen_w, &screen_h);
  // if(screen_h < y+h || screen_w < x+w)return;
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // uint32_t *pixels = ctl->pixels;
  // for (int i = y; i < y+h; i++) {
  //   for (int j = x; j < x+w; j++) {
  //     fb[screen_w*i+j] = pixels[w*(i-y)+(j-x)];
  //   }
  // }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
