#include <common.h>
#include <syscall.h>
int write(const char *str, int len) {
  char *ptr = (char *)str;
  int i = 0;
  for (i = 0; i < len; i++)
    putch(ptr[i]);
  return len;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

// #define LOG_CALL(...) Log(__VA_ARGS__)
#define LOG_CALL(...)

  switch (a[0]) {
  case SYS_yield: {
    LOG_CALL("SYS_YIELD");
    yield();
    c->GPRx = 0;
    break;
  }
  case SYS_exit: {
    LOG_CALL("SYS_exit");
    halt(a[1]);
    break;
  }
  case SYS_write: {
    LOG_CALL("SYS_write: fd=%d ptr=%p len=%d", a[1], a[2], a[3]);
    if (a[1] == 1) {
      c->GPRx = write((const char *)a[2], a[3]);
    }
    break;
  }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

}
