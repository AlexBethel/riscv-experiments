/* #include <stdint.h> */

#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned int
#define uint64_t unsigned long long int
#define int8_t signed char
#define int16_t signed short
#define int32_t signed int
#define int64_t signed long long int

#define uintptr_t size_t

#include <stddef.h>
#include <stdbool.h>

#include "system.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t

void *memcpy(void *dest_, const void *src_, size_t len) {
  char *dest = dest_, *src = (char *)src_;
  for (size_t i = 0; i < len; i++)
    dest[i] = src[i];
  return dest_;
}

typedef struct Arena {
  u8 *mem;
} Arena;

static void Arena_align(Arena *a, u8 align) {
  if ((uintptr_t)(a->mem) & (align - 1))
    a->mem += align - ((uintptr_t)(a->mem) & (align - 1));
}

static u8 *Arena_alloc_unalign(Arena *a, u32 size) {
  u8 *r = a->mem;
  a->mem += size;
  return r;
}

static u8 *Arena_alloc(Arena *a, u32 size) {
  Arena_align(a, 4);
  return Arena_alloc_unalign(a, size);
}

typedef struct str {
  u8 *mem;
  u32 len;
} str;
#define strl(s) \
  (str){ .mem = (u8 *)s, .len = sizeof(s) - 1 }

static str str_concat(Arena *a, str l, str r) {
  if (l.mem + l.len == a->mem) {
    memcpy(a->mem, r.mem, r.len);
    a->mem += r.len;
    l.len += r.len;
    return l;
  }

  u8 *res = a->mem;
  memcpy(a->mem, l.mem, l.len);
  a->mem += l.len;
  memcpy(a->mem, r.mem, r.len);
  a->mem += r.len;
  return (str){ .mem = res, .len = l.len + r.len };
}

static str fnum(Arena *a, s32 n) {
  bool negative = n < 0;
  if (negative) n = -n;

  u8 *buf = Arena_alloc(a, 16);
  u8 wr_index = 16;
  while (n != 0) {
    buf[--wr_index] = '0' + (n % 10);
    n /= 10;
  }

  if (negative)
    buf[--wr_index] = '-';

  return (str) { .mem = buf + wr_index, .len = 16 - wr_index };
}

static void pstr(str s) {
  for (u32 i = 0; i < s.len; i++)
    putchar(s.mem[i]);
}

int main() {
  static u8 mem[1024];
  Arena a = { .mem = mem };

  pstr(str_concat(&a,
    fnum(&a, -1234),
    strl(" is my favorite number!\n"))
  );
}
