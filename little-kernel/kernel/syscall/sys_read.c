#include "string.h"
#include<stdint.h>

int64_t _int_read(char* buf, uint64_t len)
{
    char* current_ptr = buf;  
    asm volatile(
        ".globl loop, finish;"
        "mov rbx, %[current_ptr];"
        "mov rcx, %[len];"
        "loop:"
        "xor rax, rax;"
        "mov dx, 0x217;"
        "in al, dx;"//使用in指令读入一个字节的数据
        "cmp al, 0x0a;"
        "jz finish;"
        "mov [rbx], al;"
        "dec rcx;"
        "inc rbx;"
        "cmp rcx, 0;"
        "jnz loop;"
        "finish:"
        :[len]"=r"(len)
        :[current_ptr]"r"(current_ptr),
          "[len]"(len)
        :"rax","rbx","rcx","rdx"
    );   
    return len;
}



