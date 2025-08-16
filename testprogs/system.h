#ifndef SYSTEM_H
#define SYSTEM_H

static inline void ecall(unsigned int arg0, unsigned int arg1) {
#ifdef __riscv
  register unsigned int call asm("s8") = arg0;
  register unsigned int arg asm("s9") = arg1;
#else
  unsigned int call = arg0;
  unsigned int arg = arg1;
#endif
  asm volatile (
    "ecall"
    : /* no outputs */
    : "r" (call), "r" (arg)
  );
}

static inline __attribute((noreturn)) void exit() {
  ecall(0, 0);
  __builtin_unreachable();
}

static inline void putchar(char c) {
  ecall(1, c);
}

static inline void puts(const char *str) {
  for (; *str; str++)
    putchar(*str);
  putchar('\n');
}

#endif /* SYSTEM_H */
