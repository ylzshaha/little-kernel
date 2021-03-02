#ifndef _DISK_
#define _DISK_
#include <stdint.h>


extern uint64_t bitmap[16];//一共1024位的位图管理1024块磁盘块
extern void* disk_mem;

extern uint64_t disk_malloc();//申请磁盘块 文件管理的vi函数回调用

#endif