#include "malloc.h"
#include "string.h"
#include "sys_write.h"
#define chunk_2_mem(p) (char*)((uint64_t)p + 16)

chunk_header* top_chunk = (chunk_header*)0x400000;
chunk_header* free_list = 0;

char* _int_malloc(uint64_t size)
{
  chunk_header* chunk = 0;
  size = (size + 0x10) & (~0xf);
  if(free_list)
  {
    chunk_header* current_chunk = free_list;
    while(current_chunk != 0)
    {
      if(current_chunk->size == size)
      {
        chunk = current_chunk;
        free_list = current_chunk->next;
      }
      current_chunk = current_chunk->next;//看下一个空闲块

    }
  }
  else
  {
    if(top_chunk->size >= size)
    {
      chunk = top_chunk;
      top_chunk = (chunk_header*)((char*)top_chunk + size);
      top_chunk->size = chunk->size - size;
      chunk->size = size;
      chunk->next = 0;
    }
  }
  
  return chunk_2_mem(chunk);
}


void _int_free(void* mem)
{
  chunk_header* header = (chunk_header*)mem - 1;
  if(!free_list)
  {
    free_list = header;
    header->next = 0;
    return;
  }
  header->next = free_list;
  free_list = header;//插入进空闲链表
}