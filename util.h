#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <errno.h>
#include <string.h>

#define WITH(init, end)                                                        \
  for (int _once = 1; _once; _once = 0)                                        \
    for (init; _once; end, _once = 0)

#define TRY                                     \
  for (int _once = 1; _once; _once = 0)

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define usize size_t
#define isize ssize_t

#define f32 float
#define f64 double

#define ARRLEN(a) (sizeof(a) / (sizeof((a)[0])))

#define KB(x) ((x) * 1024)
#define MB(x) (KB(x) * 1024)
#define GB(x) (MB(x) * 1024)
#define TB(x) (GB(x) * 1024)

#define ASSERT(val, ...)                                                       \
  do {                                                                         \
    if ((val) == 0) {                                                          \
      fprintf(stderr, __FILE__ ":%d: ", __LINE__);                             \
      fprintf(stderr, __VA_ARGS__);                                            \
      fprintf(stderr, "\n");                                                   \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)
#define ASSERT_ERRNO(val, msg, ...)                                            \
  ASSERT(val, "%s: " msg, strerror(errno), __VA_ARGS__)

#endif /* UTIL_H */
