.globl _start, hlt
.extern main
.intel_syntax noprefix
_start:
  call main
hlt:
  hlt
  jmp hlt