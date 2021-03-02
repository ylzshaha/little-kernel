#include <string.h>
#include<stdint.h>

int64_t _int_write(char* buf, uint64_t len)
{
                
    char* current_ptr = buf;
    __asm__ volatile(
        ".globl loop_2;"
        "mov rbx, %[current_ptr];"
        "mov rcx, %[len];"
        "loop_2:"
        "xor rax, rax;"
        "mov al, [rbx];"
        "mov dx, 0x217;"
        "out dx, al;"//使用out指令打印一个字节
        "dec rcx;"
        "inc rbx;"
        "cmp rcx, 0;"
        "jnz loop_2;"
        :[len]"=r"(len)
        :[current_ptr]"r"(current_ptr),
          "[len]"(len)
        :"rax","rbx","rdx","rcx","rdi"
    );

    return len;
}
