#ifndef _MALLOC_
#define _MALLOC_
#include <stdint.h>


//内存块的结构体
typedef struct chunk_header
{
  uint64_t size;
  struct chunk_header* next;
} chunk_header;

extern chunk_header* top_chunk;
void _int_free(void* mem);//用于内存的释放
char* _int_malloc(uint64_t size);//用于内存申请

#endif