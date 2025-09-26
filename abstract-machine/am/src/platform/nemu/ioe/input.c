#include <am.h>
#include <nemu.h>

#include "../include/nemu.h"

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  if(kbd->keycode != AM_KEY_NONE)kbd->keydown = true;
  kbd->keycode = inw(KBD_ADDR) & ~KEYDOWN_MASK;
}
