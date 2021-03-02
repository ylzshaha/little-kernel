#ifndef _SYSCALL_HANDELER_
#define _SYSCALL_HANDELER_
#include<stdint.h>
#include "sys_read.h"
#include "sys_write.h"

typedef uint64_t (*syscall_func)(uint64_t arg0, uint64_t arg1, uint64_t arg2);

#define SYSCALL_FUNC(name)    uint64_t name(uint64_t arg0, uint64_t arg1, uint64_t arg2)


#define FILE_SYSCALL(name)                              \
    SYSCALL_FUNC(sys_file_##name) {                     \
        uint64_t res = _file_##name(arg0,arg1,arg2);    \
        return res;                                     \
    }    


SYSCALL_FUNC(sys_read);
SYSCALL_FUNC(sys_write);
SYSCALL_FUNC(sys_malloc);
SYSCALL_FUNC(sys_file_ls);
SYSCALL_FUNC(sys_file_cd);
SYSCALL_FUNC(sys_file_mkdir);
SYSCALL_FUNC(sys_file_cat);
SYSCALL_FUNC(sys_file_vim);
SYSCALL_FUNC(sys_file_rm);
SYSCALL_FUNC(sys_disk_malloc);

#endif
