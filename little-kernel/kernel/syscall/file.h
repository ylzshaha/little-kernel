#ifndef _FILE_
#define _FILE_
#include<stdint.h>


typedef struct FCB
{
  char* file_name;
  int64_t file_type;//0: dir 1: binary
  struct FCB* son_dir;//目录下第一个子文件
  struct FCB* bro_dir;//同一个目录下其他的文件
  struct FCB* parents_dir;//父亲文件
  uint64_t hard_num;//硬盘怕盘块号
}FCB;

extern FCB* root_FCB;
extern FCB* current_file;

FCB* _file_mkdir(char *name);//创建目录
uint64_t _file_ls();//打印目录
FCB* _file_cd(char* name);//切换工作目录
FCB* _file_vim(char *name,char* content);//创建文件，并向其中写入
uint64_t _file_cat(char *name);//打印文件内容
uint64_t _file_rm(char *name);//删除目录
#endif