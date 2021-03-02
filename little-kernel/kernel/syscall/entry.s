.globl _start, hlt
.extern kernel_main
.intel_syntax noprefix
_start:
  call kernel_main
hlt:
  hlt
  jmp hlt