#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <stdio.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  char *optr = out;
  for(char *p = (char*)fmt; *p; p++) {
    if(*p != '%') {
      *optr++ = *p;
      continue;
    }
    switch(*++p) {
    case 's':
      {
        panic("s");
        char *s = va_arg(arg, char *);
        for(int i = 0; i < strlen(s); i++) {
          *optr++ = *p;
        }
        break;
      }
    case 'd':
      {
        panic("1");
        int d = va_arg(arg, int);
        do {
          *optr++ = ('0' + d % 10);
        } while(d /= 10 != 0);
        break;
      }
    default:
      {
        *optr++ = *p;
      }
    }
  }
  optr = 0;
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
