#include <stdint.h>
#include "string.h"
#include "disk.h"

uint64_t bitmap[16];//一共1024位的位图管理1024块磁盘块
void* disk_mem;

uint64_t disk_malloc()
{
    uint64_t block_num = 1;
    for(uint64_t i = 0; i < 16; i++)
    {
        uint64_t mask_number = 0x10;
        for(uint64_t j = 1; j < 64; j++)
        {
            uint64_t result = bitmap[i] & mask_number;
            if(result == 0)//表示这一块还没有分配出去
            {
                void* physics_addr = (void*)((uint64_t)disk_mem + (i * 64 + j)*0x1000);
                memset(physics_addr,0,0x1000);
                bitmap[i] |= mask_number;
                return block_num;
            }
            mask_number = mask_number << 1;
            block_num ++;
        }
    }
    return 0;
}

