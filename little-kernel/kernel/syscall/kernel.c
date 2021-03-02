#include <stdint.h>
#include "sys_write.h"
#include "string.h"
#include "malloc.h"
#include "file.h"
#include "disk.h"


#define ARENA_INIT                        \
  top_chunk = (chunk_header*)0x400000;    \
  top_chunk->size = 0x200000;             \
  top_chunk->next = 0

#define FILE_INIT                             \
  root_FCB = (FCB*)_int_malloc(0x30);         \
  current_file = root_FCB;                    \
  root_FCB->file_name = _int_malloc(0x20);    \
  memset(root_FCB->file_name, 0, 0x20);       \
  *root_FCB->file_name = '/'                  

#define DISK_INIT                 \
  disk_mem = (void*)0x600000;     \
  memset(bitmap,0,16*8)

#define MSR_STAR 0xc0000081 /* legacy mode SYSCALL target */
#define MSR_LSTAR 0xc0000082 /* long mode SYSCALL target */
#define MSR_CSTAR 0xc0000083 /* compat mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084

int register_syscall() {
  asm volatile(
    "xor rax, rax;"
    "mov rdx, 0x00200008;"
    "mov ecx, %[msr_star];"
    "wrmsr;"

    "mov eax, %[fmask];"
    "xor rdx, rdx;"
    "mov ecx, %[msr_fmask];"
    "wrmsr;"

    "lea rax, [rip + syscall_entry];"
    "mov rdx, %[base] >> 32;"
    "mov ecx, %[msr_syscall];"
    "wrmsr;"
    :: [msr_star]"i"(MSR_STAR),
       [fmask]"i"(0x3f7fd5), [msr_fmask]"i"(MSR_SYSCALL_MASK),
       [base]"i"(0x0llu), [msr_syscall]"i"(MSR_LSTAR)
    : "rax", "rdx", "rcx");
  return 0;
}

void kernel_init()
{
  ARENA_INIT;
  FILE_INIT;
  DISK_INIT;
}


int kernel_main() {
  kernel_init();
  char* tips = "Switching to user......\n";
  if(register_syscall() != 0) return 1;
  _int_write(tips, 25);
    asm volatile(
        "mov [rip + kernel_stack], rsp;"
        "mov rcx, %[entry];" /* rip */
        "mov r11, 0x2;"      /* rflags */
        "mov rsp, %[rsp];"
        /* clean up registers */
        "xor rax, rax;"
        "xor rbx, rbx;"
        "xor rdx, rdx;"
        "xor rdi, rdi;"
        "xor rsi, rsi;"
        "xor rbp, rbp;"
        "xor r8, r8;"
        "xor r9, r9;"
        "xor r10, r10;"
        "xor r12, r12;"
        "xor r13, r13;"
        "xor r14, r14;"
        "xor r15, r15;"
        "xor rbp, rbp;"
        ".byte 0x48;"
        "sysret"
        :: [entry]"i"(0x200000), [rsp]"i"(0x300000)
        : "r11", "rcx"
    );
  return 0;
}
