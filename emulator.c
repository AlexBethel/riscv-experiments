#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "util.h"

static struct registers {
  u32 pc;
  u32 xreg[32];
} registers;

u8 memory[KB(64)];

static u32 seg(u32 inst, u32 high, u32 low) {
  u32 width = (high + 1) - low;
  u32 mask = (1 << width) - 1;
  return (inst >> low) & mask;
}

#define BLOCK(name, high, low)                                                 \
  static u32 name(u32 inst) { return seg(inst, high, low); }
BLOCK(funct7, 31, 25);
BLOCK(rs2, 24, 20);
BLOCK(rs1, 19, 15);
BLOCK(funct3, 14, 12);
BLOCK(rd, 11, 7);
BLOCK(opcode, 6, 0);
#undef BLOCK
static u32 i_imm(u32 inst) {
  return seg(inst, 31, 20);
}
static u32 s_imm(u32 inst) {
  u32 a = seg(inst, 31, 25);
  u32 b = seg(inst, 11, 7);
  return (a << 5) | b;
}
static u32 u_imm(u32 inst) {
  return seg(inst, 31, 12);
}

// these are most likely incorrect, at least partially
static u32 b_imm(u32 inst) {
  u32 imm12 = seg(inst, 31, 30);
  u32 imm10_5 = seg(inst, 30, 25);
  u32 imm4_1 = seg(inst, 11, 8);
  u32 imm11 = seg(inst, 7, 6);
  return (imm4_1 << 1) | (imm10_5 << 5) | (imm11 << 11) | (imm12 << 12);
}
static u32 j_imm(u32 inst) {
  u32 imm20 = seg(inst, 31, 30);
  u32 imm10_1 = seg(inst, 30, 21);
  u32 imm11 = seg(inst, 20, 19);
  u32 imm19_12 = seg(inst, 19, 12);
  return (imm10_1 << 1) | (imm11 << 11) | (imm19_12 << 12) | (imm20 << 20);
}

#define unimplemented(feat)                \
  fprintf(stderr, "unimplemented: " feat); \
  exit(1);

static void exec_inst(u32 inst) {
#define RS1 (registers.xreg[rs1(inst)])
#define RS2 (registers.xreg[rs2(inst)])
#define RD (registers.xreg[rd(inst)])

  fprintf(stderr, "executing instruction %08x\n", inst);
  fprintf(stderr, "opcode %08x\n", opcode(inst));

  switch (opcode(inst)) {
  case 0b0110111: {             // LUI
    RD = u_imm(inst) << 12;
  } break;
  case 0b0010111: {             // AUIPC
    RD = (u_imm(inst) << 12) + registers.pc;
  } break;
  case 0b1101111: {             // JAL
    RD = registers.pc + 4;
    registers.pc += (i32)j_imm(inst) - 4; // 4 will be added back at the end of exec
  } break;
  case 0b1100111: {             // JALR
    fprintf(stderr, "JALR register %d, store to %d\n", rs1(inst), rd(inst));
    RD = registers.pc + 4;
    fprintf(stderr, "computed jump location [%08x + %08x - 4] = %08x\n",
            RS1, i_imm(inst), (i32)RS1 + (i32)i_imm(inst) - 4);
    registers.pc = (i32)RS1 + (i32)i_imm(inst) - 4;
    registers.pc &= ~1;
    fprintf(stderr, "jumped to %08x\n", registers.pc + 4);
  } break;
  case 0b1100011: {
    switch (funct3(inst)) {
    case 0b000: {               // BEQ
      if (RS1 == RS2)
        registers.pc += b_imm(inst);
    } break;
    case 0b001: {               // BNE
      if (RS1 != RS2)
        registers.pc += b_imm(inst);
    } break;
    case 0b100: {               // BLT
      if ((i32)RS1 < (i32)RS2)
        registers.pc += b_imm(inst);
    } break;
    case 0b101: {               // BGE
      if ((i32)RS1 >= (i32)RS2)
        registers.pc += b_imm(inst);
    } break;
    case 0b110: {               // BLTU
      if (RS1 < RS2)
        registers.pc += b_imm(inst);
    } break;
    case 0b111: {               // BGEU
      if (RS1 >= RS2)
        registers.pc += b_imm(inst);
    } break;
    default: {
      unimplemented("bad instruction\n");
    } break;
    }
  } break;
  case 0b0000011: {
    switch (funct3(inst)) {
    case 0b000: {               // LB
      RD = *(i8 *)(memory + RS1 + i_imm(inst));
    } break;
    case 0b001: {               // LH
      RD = *(i16 *)(memory + RS1 + i_imm(inst));
    } break;
    case 0b010: {               // LW
      RD = *(i32 *)(memory + RS1 + i_imm(inst));
    } break;
    case 0b100: {               // LBU
      RD = *(u8 *)(memory + RS1 + i_imm(inst));
    } break;
    case 0b101: {               // LHU
      RD = *(u16 *)(memory + RS1 + i_imm(inst));
    } break;
    default: {
      unimplemented("bad instruction\n");
    } break;
    }
  } break;
  case 0b0100011: {
    fprintf(stderr, "store\n");
    fprintf(stderr, "store imm %08x\n", s_imm(inst));
    fprintf(stderr, "computed destination %08x\n", RS1 + s_imm(inst));
    fprintf(stderr, "writing reg %d value %08x\n", rs2(inst), RS2);
    switch (funct3(inst)) {
    case 0b000: {               // SB
      *(u8 *)(memory + RS1 + s_imm(inst)) = RS2;
    } break;
    case 0b001: {               // SH
      *(u16 *)(memory + RS1 + s_imm(inst)) = RS2;
    } break;
    case 0b010: {               // SW
      *(u32 *)(memory + RS1 + s_imm(inst)) = RS2;
    } break;
    default: {
      unimplemented("bad instruction\n");
    } break;
    }
  } break;
  case 0b0010011: {
    fprintf(stderr, "arithmetic funct3 %08x\n", funct3(inst));
    switch (funct3(inst)) {
    case 0b000: {               // ADDI
      fprintf(stderr, "addi %08x + %08x\n", RS1, i_imm(inst));
      RD = RS1 + i_imm(inst);
      fprintf(stderr, "result %08x in register %d\n", RD, rd(inst));
    } break;
    case 0b010: {               // SLTI
      // this has wrong sign extension
      RD = (i32)RS1 < (i32)i_imm(inst);
    } break;
    case 0b011: {               // SLTIU
      RD = RS1 < i_imm(inst);
    } break;
    case 0b100: {               // XORI
      // wrong sign extension
      RD = RS1 ^ i_imm(inst);
    } break;
    case 0b110: {               // ORI
      RD = RS1 | i_imm(inst);
    } break;
    case 0b111: {               // ANDI
      RD = RS1 & i_imm(inst);
    } break;
    case 0b001: {               // SLLI
      RD = RS1 << (i_imm(inst) & 0x1f);
    } break;
    case 0b101: {
      switch (funct7(inst)) {
      case 0b0000000: {         // SRLI
        RD = RS1 >> (i_imm(inst) & 0x1f);
      } break;
      case 0b0100000: {         // SRAI
        RD = (i32)RS1 >> (i_imm(inst) & 0x1f);
      } break;
      default: {
        unimplemented("bad instruction\n");
      } break;
      }
    } break;
    default: {
      unimplemented("bad instruction\n");
    } break;
    }
  } break;
  case 0b0110011: {
    switch (funct3(inst)) {
    case 0b000: {
      switch (funct7(inst)) {
      case 0b0000000: {         // ADD
        RD = RS1 + RS2;
      } break;
      case 0b0100000: {         // SUB
        RD = RS1 - RS2;
      } break;
      default: {
        unimplemented("bad instruction\n");
      } break;
      }
    } break;
    case 0b001: {               // SLL
      RD = RS1 << (RS2 & 0x1f);
    } break;
    case 0b010: {               // SLT
      RD = (i32)RS1 < (i32)RS2;
    } break;
    case 0b011: {               // SLTU
      RD = RS1 < RS2;
    } break;
    case 0b100: {               // XOR
      RD = RS1 ^ RS2;
    } break;
    case 0b101: {
      switch (funct7(inst)) {
      case 0b0000000: {         // SRL
        RD = RS1 >> (RS2 & 0x1f);
      } break;
      case 0b0100000: {         // SRA
        RD = (i32)RS1 >> (RS2 & 0x1f);
      } break;
      default: {
        unimplemented("bad instruction\n");
      } break;
      }
    } break;
    case 0b110: {               // OR
      RD = RS1 | RS2;
    } break;
    case 0b111: {               // AND
      RD = RS1 & RS2;
    } break;
    default: {
      unimplemented("bad instruction\n");
    } break;
    }
  } break;
  case 0b0001111: {             // FENCE
    // do nothing, this is a single-threaded emulator
  } break;
  case 0b1110011: {
    switch (i_imm(inst)) {
    case 0b00000000000: {       // ECALL
      unimplemented("ECALL");
    } break;
    case 0b00000000001: {       // EBREAK
      unimplemented("EBREAK");
    } break;
    default: {
      unimplemented("bad instruction\n");
    } break;
    }
  } break;
  default: {
    unimplemented("bad instruction\n");
  } break;
  }
  fprintf(stderr, " !!! advance PC %08x -> %08x\n", registers.pc, registers.pc + 4);
  registers.pc += 4;
  registers.xreg[0] = 0;
}

int main(int argc, char *argv[]) {
  ASSERT(argc == 2, "usage: emulator <flat executable file>\n");
  int fd = open(argv[1], O_RDONLY);
  ASSERT_ERRNO(fd != -1, "error opening %s", argv[1]);
  ASSERT_ERRNO(read(fd, memory, sizeof(memory)) != 0, "read() on %s", argv[1]);
  registers.xreg[1] = 0xfffffff0;  // sentinel return address
  for (int i = 0; i < 7; i++) {
    fprintf(stderr, " -> r1 is %08x\n", registers.xreg[1]);
    exec_inst(*(u32 *)(memory + registers.pc));
    if (registers.pc == 0xfffffff0) {
      fprintf(stderr, "program terminated!\n");
      return 0;
    }
  }
}
