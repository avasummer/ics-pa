#include <common.h>
#include <syscall.h>

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

#define LOG_CALL(s) Log("calling: %s", s)

  switch (a[0]) {
  case SYS_yield: {
    LOG_CALL("SYS_YIELD");
    yield();
    c->GPRx = 0;
    return;
  }
  case SYS_exit: {
    LOG_CALL("SYS_exit");
    halt(c->GPR1);
    return;
  }
  case SYS_write: {
    LOG_CALL("SYS_write");
    if (c->GPR1 == 1) {
      snprintf("%s", c->GPR3,(const char*)c->GPR2);
    }
    return;
  }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

}
