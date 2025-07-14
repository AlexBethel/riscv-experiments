ENTRY(_start)
/* OUTPUT_FORMAT(binary) */
SECTIONS {
  . = 0x00;
  .start : { *(.start) }
  .text : { *(.text) }
  /* . = 0x1234; */
  .data : { *(.data) }

  /DISCARD/ : {
    *(.eh_frame)
    *(.note.gnu.property)
    *(.comment)
    *(.got)
    /* *(.got.plt) */
    *(.riscv.attributes)
  }
}
