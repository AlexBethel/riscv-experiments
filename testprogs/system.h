#ifndef SYSTEM_H
#define SYSTEM_H

static inline void ecall(unsigned int arg0, unsigned int arg1) {
#ifdef __riscv
  register unsigned int call asm("a0") = arg0;
  register unsigned int arg asm("a1") = arg1;
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

static inline int putchar(int c) {
  ecall(1, c);
  return c;
}

static inline int puts(const char *str) {
  for (; *str; str++)
    putchar(*str);
  putchar('\n');
  return 0;
}

/* Main function. */
extern int main();
asm("  .section .start,\"ax\",@progbits\n"
    "  .align 2\n"
    "  .globl _start\n"
    "  .type _start, @function\n"
    "_start:\n"
    "  li sp, 0xfff0\n" // Initialize the stack pointer
    "  call main\n" // main()
    "  li a0, 0\n"  // exit()
    "  li a1, 0\n"
    "  ecall\n");

#endif /* SYSTEM_H */
