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
const char *regs[] = {
  "x0(zero)", "x1(ra)", "x2(sp)", "x3(gp)",
  "x4(tp)", "x5(t0)", "x6(t1)", "x7(t2)",
  "x8(s0/fp)", "x9(s1)", "x10(a0)", "x11(a1)",
  "x12(a2)", "x13(a3)", "x14(a4)", "x15(a5)",
  "x16(a6)", "x17(a7)", "x18(s2)", "x19(s3)",
  "x20(s4)", "x21(s5)", "x22(s6)", "x23(s7)",
  "x24(s8)", "x25(s9)", "x26(s10)", "x27(s11)",
  "x28(t3)", "x29(t4)", "x30(t5)", "x31(t6)"
};

void isa_reg_display() {
  for (int i = 0; i < 32; i ++) {
    log_write("%-12s: 0x%08x\n", regs[i], R(i));
  }
}

word_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}
