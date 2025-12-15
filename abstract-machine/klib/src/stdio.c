#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define CHUNK_SIZE 2048
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  //not a safe buffer write.
  char buffer[CHUNK_SIZE];
  memset(buffer, 0, sizeof(buffer));

  va_list ap;
  va_start(ap, fmt);

  int result = vsnprintf((char *)&buffer, CHUNK_SIZE, fmt, ap);
  if(result!=strlen(fmt))panic("overflow");
  va_end(ap);
  putstr((const char*)&buffer[0]);
  return result;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
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
  va_list ap;
  va_start(ap, fmt);
  int result = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return result;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  if(n==0) return 0;

  char *optr = out;
  char *p = NULL;

  for (p = (char *)fmt; *p; p++) {
    
    if(*p != '%') {
      *optr++ = *p;
      if(strlen(out) >= n - 1)return p-fmt;
      continue;
    }

   switch(*++p) {
    case 's':
      {
        char *s = va_arg(ap, char *);
        int len = strlen(s);
        int space = n - strlen(out);
        for(int i = 0; i < len && i < space; i++) {
          *optr++ = s[i];
        }
        break;
    }
    case 'p': {
      uintptr_t ptr = (uintptr_t)va_arg(ap, void*);
      if (strlen(out) < n - 1) *optr++ = '0';
      if (strlen(out) < n - 1) *optr++ = 'x';
      char buf[2 * sizeof(uintptr_t) + 1];
      char *t = buf + sizeof(buf) - 1;
      *t = '\0';

      if (ptr == 0) {
        *--t = '0';
      } else {
        while (ptr) {
          int v = ptr & 0xf;
          *--t = (v < 10) ? ('0' + v) : ('a' + (v - 10));
          ptr >>= 4;
        }
      }
      while (*t) {
        if (strlen(out) < n - 1) *optr++ = *t++;
        else break;
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
        char buf[sizeof(long)+2];
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

        if (negative)
          *optr++ = '-';

        while (*p) {
          if(strlen(out) < n - 1)*optr++ = *p++;}
        break;
    }
    case '\0': break;
    default: {
        *optr++ = *p;
      }
    }
  }
  *optr = 0;
  return p-fmt;
}

int puts(const char* str) {
    putstr(str);
  return strlen(str);
}

#endif
