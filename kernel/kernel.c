#include <stddef.h>
#include <stdint.h>

#include "system.h"

static void getline(char *buf) {
  do
    *buf = getchar();
  while (*(buf++) != '\n');

  *buf = 0;
}

static size_t strlen(const char *buf) {
  size_t len = 0;
  while (buf[len])
    len++;

  return len - 1;
}

static char *fnum(char *buf, unsigned int num) {
  buf += 16;
  *buf = 0;
  while (num) {
    buf--;
    *buf = (num % 10) + '0';
    num /= 10;
  }
  return buf;
}

char buf[128];
char buf2[32];

void kmain() {
  puts("Hello World!");

  getline(buf);
  puts("I got:");
  puts(fnum(buf2, strlen(buf)));
  puts("characters!");
}
