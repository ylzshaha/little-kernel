#include<stdint.h>
#include "libc.h"

int64_t puts(char* buf, int64_t size)
{
    asm volatile(
      "mov rax, 1;"
      "mov rdi, %[buf];"
      "mov rsi, %[size];"
      "syscall;"
      ::[buf]"m"(buf),
        [size]"m"(size)
      :"rax","rdi","rsi"
    );
    return (int64_t)buf;
}

int64_t gets(char* buf, int64_t size)
{
    asm volatile(
      "mov rax, 0;"
      "mov rdi, %[buf];"
      "mov rsi, %[size];"
      "syscall;"
      ::[buf]"m"(buf),
        [size]"m"(size)
      :"rax","rdi","rsi"
    );
    return (int64_t)buf;
}

uint64_t libc_malloc(uint64_t size)
{
    uint64_t res;
    asm volatile(
      "mov rax, 2;"
      "syscall;"
      :"=a"(res)
      :[size]"r"(size)
      :"rdi"
    );
    return (uint64_t)res; 
}


uint64_t user_ls(uint64_t arg0, uint64_t arg1)
{
    asm volatile(
      "mov rax, 3;"
      "syscall;"
    );
    return (uint64_t)1;
}

uint64_t user_cd(uint64_t name, uint64_t arg1)
{

    asm volatile(
      "mov rax, 4;"
      "mov rdi, %[name];"
      "syscall;"
      ::[name]"r"(name)
      :"rax","rdi"
    );
    return (uint64_t)1;
}

uint64_t user_mkdir(uint64_t name, uint64_t arg1)
{
  uint64_t res;
    asm volatile(
      "mov rax, 5;"
      "syscall;"
      :"=a"(res)
      ::"rdi"
    );
    return (uint64_t)res;
}

uint64_t user_vim(uint64_t name, uint64_t content)
{
  uint64_t res;
    asm volatile(
      "mov rax, 6;"
      "mov rdi, %[name];"
      "mov rsi, %[content];"
      "syscall;"
      "mov rax, rbx;"
      :"=b"(res)
      :[name]"r"(name),
       [content]"r"(content)
      :"rdi","rsi","rax"
    );
    return (uint64_t)res;
}

uint64_t user_cat(uint64_t name, uint64_t arg1)
{
    asm volatile(
      "mov rax, 7;"
      "syscall;"
      ::[name]"r"(name)
      :"rax","rdi","rax"
    );
    return (uint64_t)1;
}

uint64_t user_rm(uint64_t name, uint64_t arg1)
{
    asm volatile(
      "mov rax, 8;"
      "syscall;"
      ::[name]"r"(name)
      :"rax","rdi","rax"
    );
    return (uint64_t)1;
}

uint64_t strnlen(const char *s, uint64_t maxlen) {
  uint64_t i = 0;
  while(i < maxlen) {
    if(*s == 0) return i;
    i++; s++;
  }
  return maxlen;
}

uint64_t strlen(const char *s) {
  return strnlen(s, (1ull << 63) - 1);
}

void *memset(void *b, int c, uint64_t len) {
  for(int i=0;i<len;i++)
    ((uint8_t*)b)[i] = (uint8_t)c;
  return b;
}

void *memcpy(void *dst, const void *src, uint64_t n) {
  asm(
    "mov rcx, %[n];"
    "rep movsb byte ptr [%[dst]], byte ptr [%[src]];"
    :: [n]"r"(n), [dst]"D"(dst), [src]"S"(src) : "rcx"
    );
  return dst;
}

int memcmp(const void *s1, const void *s2, uint64_t n) {
  unsigned char u1, u2;
  for(; n--; s1++, s2++) {
    u1 = *(unsigned char *) s1;
    u2 = *(unsigned char *) s2;
    if (u1 != u2) return u1 - u2;
  }
  return 0;
}

