#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  va_list ap;
  va_start(ap, fmt);
  int result = vsprintf((char*)&buffer, fmt, ap);
  va_end(ap);
  putstr((const char*)&buffer[0]);
  return result;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *optr = out;
  for(char *p = (char*)fmt; *p; p++) {
    if(*p != '%') {
      *optr++ = *p;
      continue;
    }
    switch(*++p) {
    case (p>'0' && p<='9'):
      {
        
      }
    case 's':
      {
        char *s = va_arg(ap, char *);
        int len = strlen(s);
        for(int i = 0; i < len; i++) {
          *optr++ = s[i];
        }
        break;
      }
    case 'l':
      {
        break;
      }
    case 'd':
      {
        long d = va_arg(ap, int);
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

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int result = vsprintf(out, fmt, ap);
  va_end(ap);
  return result;
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
