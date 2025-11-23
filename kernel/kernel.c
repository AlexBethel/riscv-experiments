#include <stddef.h>
#include <stdint.h>

unsigned char *uart = (unsigned char *)0x10000000;
int putchar(int c) {
  *uart = (unsigned char)c;
  return (int)(unsigned char)c;
}

void print(const char *str) {
  while (*str)
    putchar(*str++);
}

void kmain() {
  print("Hello World!\n");
  while (1) {
    putchar(*uart);
  }
}
