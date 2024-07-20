/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include "local-include/reg.h"

#define R(i) gpr(i)
#define REGS_SIZE (sizeof(cpu.gpr) / sizeof(cpu.gpr[0]))
const char *regs_name[REGS_SIZE] = {
  "x0", "x1", "x2", "x3",
  "x4", "x5", "x6", "x7",
  "x8", "x9", "x10", "x11",
  "x12", "x13", "x14", "x15",
  "x16", "x17", "x18", "x19",
  "x20", "x21", "x22", "x23",
  "x24", "x25", "x26", "x27",
  "x28", "x29", "x30", "x31"
};

const char *regs_abi_name[REGS_SIZE] = {
  "zero", "ra", "sp", "gp",
  "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1",
  "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3",
  "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11",
  "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  log_write("Register Name  ABI Name   Value\n");
  log_write("-------------  ---------  ----------\n");
  for (int i = 0; i < REGS_SIZE; i++) {
    log_write("%-13s  %-9s  0x%08x\n", regs_name[i], regs_abi_name[i], R(i));
  }
}

word_t isa_reg_str2val(const char *s, const uint8_t lenth, bool *success) {
  if ((lenth == 0) || (lenth > 4)) {
    *success = false;
    return 0;
  }
  for (int i = 0; i < REGS_SIZE; i ++) {
    if (strncmp(regs_name[i], s, lenth) == 0 || strncmp(regs_abi_name[i], s, lenth) == 0) {
      *success = true;
      return R(i);
    }
  }
  *success = false;
  return 0;
}
