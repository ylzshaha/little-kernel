#include "syscall_handler.h"
#include "malloc.h"
#include "file.h"
#include "disk.h"
#include "string.h"

enum syscall_type{
  SYS_READ,
  SYS_WRITE,
  SYS_MALLOC,
  SYS_FILE_LS,
  SYS_FILE_CD,
  SYS_FILE_MKDIR,
  SYS_FILE_VIM,
  SYS_FILE_CAT,
  SYS_FILE_RM,
  SYS_DISK_MALLOC
};


static const syscall_func  syscall_entry[] = {
  sys_read,
  sys_write,
  sys_malloc,
  sys_file_ls,
  sys_file_cd,
  sys_file_mkdir,
  sys_file_vim,
  sys_file_cat,
  sys_file_rm,
  sys_disk_malloc,
  };


uint64_t syscall_handler(uint64_t arg0, uint64_t arg1, uint64_t arg2) {
  uint32_t nr;
  asm volatile (
    "mov %[nr], eax;"
    : [nr] "=b"(nr)
    );
  if(nr > SYS_DISK_MALLOC|| syscall_entry[nr] == 0) return -1;
  return syscall_entry[nr](arg0, arg1, arg2);
}


SYSCALL_FUNC(sys_read)
{
  uint64_t len = _int_read((char*)arg0, arg1);
  return len;
}

SYSCALL_FUNC(sys_write)
{
  uint64_t len = _int_write((char*)arg0, arg1);
  return len;
}

SYSCALL_FUNC(sys_malloc)
{
  uint64_t mem_ptr = (uint64_t)_int_malloc(arg0);
  return mem_ptr;
}

SYSCALL_FUNC(sys_file_ls)
{
  uint64_t res = _file_ls(arg0);
  return res;
}

SYSCALL_FUNC(sys_file_cd)
{
  uint64_t res = (uint64_t)_file_cd((char*)arg0);
  current_file = (FCB *)res;
  return res;
}

SYSCALL_FUNC(sys_file_mkdir)
{
  uint64_t res = (uint64_t)_file_mkdir((char*)arg0);
  return res;
}

SYSCALL_FUNC(sys_file_vim)
{
  uint64_t res = (uint64_t)_file_vim((char*)arg0, (char*)arg1);
  return res;
}

SYSCALL_FUNC(sys_file_cat)
{
  uint64_t res = _file_cat((char*)arg0);
  return res;
}

SYSCALL_FUNC(sys_file_rm)
{
  uint64_t res = _file_rm((char*)arg0);
  return res;
}

SYSCALL_FUNC(sys_disk_malloc)
{
  uint64_t res = disk_malloc();
  return res;
}

