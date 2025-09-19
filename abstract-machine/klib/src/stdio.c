#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

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
    putstr(out);
    if(*p != '%') {
      *optr++ = *p;
      continue;
    }
    switch(*++p) {
    case 's':
      {
        char *s = va_arg(arg, char *);
        int len = strlen(s);
        for(int i = 0; i < len; i++) {
          *optr++ = s[i];
        }
        break;
      }
    case 'd':
      {
        long d = va_arg(arg, int);
        int negative = (d < 0);
        unsigned long u = negative ? -d : d;
        char buf[sizeof(long)+1];
        char *p = buf + sizeof(buf) -1;
        *p = 0;
        if (u==0) {
          *--p = '0';
        } else {
          do {
            *--p = u%10 + '0';
            u /= 10;
          } while (u);
        }
        if(negative) *optr++ = '-';
        while(*p) *optr++ = *p++;
        break;
      }
    default:
      {
        *optr++ = *p;
      }
    }
  }
  *optr = 0;
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int puts(const char* str) {
    putstr(str);
  return strlen(str);
}

#endif
