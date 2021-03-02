#ifndef LIBC_H_
#define LIBC_H_
#include <stdint.h>


uint64_t libc_malloc(uint64_t size);
int64_t puts(char* buf, int64_t size);
int64_t gets(char* buf, int64_t size);
uint64_t strlen(const char *s);
uint64_t strnlen(const char *s, uint64_t maxlen);
void *memset(void *b, int c, uint64_t len);
void *memcpy(void *dst, const void *src, uint64_t n);
int memcmp(const void *s1, const void *s2, uint64_t n);
uint64_t user_ls(uint64_t arg0,uint64_t arg1);
uint64_t user_cd(uint64_t name, uint64_t arg1);
uint64_t user_mkdir(uint64_t name, uint64_t arg1);
uint64_t user_vim(uint64_t name, uint64_t content);
uint64_t user_cat(uint64_t name,uint64_t arg1);
uint64_t user_rm(uint64_t name, uint64_t arg1);
#endif