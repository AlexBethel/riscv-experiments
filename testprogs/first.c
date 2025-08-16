#include "system.h"

__attribute((section(".start"))) void _start() {
  puts("Hello World!");
  puts("This is a test");
  exit();
}
