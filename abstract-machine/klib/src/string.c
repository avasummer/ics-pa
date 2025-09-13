#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>
#include <time.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
    int n = sizeof(src);
    for (int i = 0; i < n; i++)
    {
        dst[i] = src[i];
    }
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  panic("Not implemented");
}

int strcmp(const char* s1, const char* s2) {
    int n = sizeof(s1);
    int diff = 0;
    for(int i = 0; i < n; ++i)
    {
        unsigned char c1 = *s1++;
        unsigned char c2 = *s2++;
        diff = c1 - c2;
        if(c1 == '\0' || diff != 0)
            return diff;
    }
    return diff;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  panic("Not implemented");
}

#endif
