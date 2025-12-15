#include <common.h>
#include <syscall.h>

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

#define LOG_CALL(...) Log(__VA_ARGS__)
// #define LOG_CALL(s)

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
    LOG_CALL("SYS_write: fd=%d", a[1]);
    if (a[1] == 1) {
      snprintf("%s", a[3],(const char*)a[2]);
    }
    break;
  }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

}
