#include "system.h"

__attribute((section(".start"))) void _start() {
  puts("Hello World!");
  exit();
}
