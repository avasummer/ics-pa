#include <am.h>
#include <nemu.h>
#include <stdint.h>
#include "../include/nemu.h"
#include<klib.h>
#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t k = inw(KBD_ADDR);
  printf("%d\n",k);
  kbd->keydown = (k & KEYDOWN_MASK ? true : false);
  kbd->keycode = k & ~KEYDOWN_MASK;
}
