ENTRY(start);

. = 0x00000000;

SECTIONS {
  .text : ALIGN(16) {
    *(.init);
    *(.text);
  }
  .bss : ALIGN(16) {
    PROVIDE(bss_start = .);
    *(.bss);
    PROVIDE(bss_end = .);
    . += 4096;
    PROVIDE(stack_top = .);
    PROVIDE(global_pointer = .);
  }
  .rodata : ALIGN(16) {
    *(.rodata);
  }
  .data : ALIGN(16) {
    *(.data);
  }
}
