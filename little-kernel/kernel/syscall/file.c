#include <stdint.h>
#include "file.h"
#include "string.h"
#include "disk.h"
#include "malloc.h"
#include "sys_read.h"
#include "sys_write.h"

FCB* root_FCB;
FCB* current_file;

#define FILE_INIT(file_name, type, num)             \
    file_name->file_type = type;                    \
    file_name->son_dir = 0;                         \
    file_name->parents_dir = current_file;          \
    new_file->hard_num = num;                       \

#define CHECK_FILE_NAME(p_name, p_file)  memcmp(p_name, p_file->file_name, strlen(p_name))

#define GET_DISK_MEM(file) (char*)((file->hard_num * 0x1000) + (char*) disk_mem)



FCB* _file_mkdir(char *name)
{   
    //memcpy(new_file->file_name+ strlen(current_file),file_name,strlen(file_name));
    FCB * new_file = (FCB*)_int_malloc(0x30);
    FILE_INIT(new_file, 0, -1)
    new_file->file_name = _int_malloc(0x20);
    memcpy(new_file->file_name,name,strlen(name));
    if (current_file->son_dir == 0)
        current_file->son_dir = new_file;
    else
    {
        FCB* temp;
        for(temp = current_file->son_dir; temp != 0 && temp->bro_dir != 0; temp = temp->bro_dir);
        temp->bro_dir = new_file;
    }
    return new_file;//插入链表
}

uint64_t _file_ls()
{
    if(current_file->son_dir != 0)
        {
          for(FCB* i = current_file->son_dir; i != 0; i = i->bro_dir)
            {
                _int_write(i->file_name,strlen(i->file_name));
                _int_write("\t",1);
            }
                _int_write("\n",1);
        }
    return 0;
}

FCB* _file_cd(char* name)
{
    FCB* i;
    if(!(memcmp(name,"..",2)))//如果是..返回上层目录
    {
        if(current_file->parents_dir != 0)
        {
            current_file = current_file->parents_dir;
            return current_file;
        }
        else 
        return current_file;
    }
    if(current_file->son_dir != 0)//在目录中寻找和要求名字相同的目录
    {
        for( i = current_file->son_dir; i!= 0 && (CHECK_FILE_NAME(name,i)); i = i->bro_dir);

        if(i->file_type == 1) return (FCB*)current_file;

        if(i != 0 && (!memcmp(i->file_name, name, strlen(i->file_name))))
        {
            return i;//将当前目录设置为找到的目录
        }
        else
        {
            return current_file;
        }
    }
    else
    {
        return current_file;
    }

}

FCB* _file_vim(char *name,char* content)
{
    FCB* new_file = (FCB*)_int_malloc(0x20);
    FILE_INIT(new_file, 1, 0)
    new_file->file_name = (char*)_int_malloc(0x20);
    memset(new_file->file_name, 0, 0x20);
    memcpy(new_file->file_name,name ,strlen(name));

    new_file->hard_num = disk_malloc();//申请磁盘空间

    if (current_file->son_dir == 0)
        current_file->son_dir = new_file;
    else
    {
        FCB* temp;
        for(temp = current_file->son_dir; temp != 0 && temp->bro_dir != 0; temp = temp->bro_dir);
        temp->bro_dir = new_file;
    }
    memcpy(GET_DISK_MEM(new_file) , content, strlen(content));//将内容写入磁盘空间中
    return new_file;

}

uint64_t _file_cat(char *name)
{
    if(current_file->son_dir != 0)
    {
        FCB* i; 
        for( i = current_file->son_dir; i!= 0 && (CHECK_FILE_NAME(name,i)); i = i->bro_dir);
        
        if(i->file_type == 0) return 0;
        if(i != 0 && (!CHECK_FILE_NAME(name, i)))
        {
            _int_write(GET_DISK_MEM(i),strlen(GET_DISK_MEM(i)));//根据磁盘的基地址找到磁盘块的位置
            _int_write("\n",1);
            return 0;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

}

uint64_t _file_rm(char *name)
{

    if(current_file->son_dir != 0)
    {
        FCB* i; 
        FCB* j = 0;
        for( i = current_file->son_dir; i!= 0 && (CHECK_FILE_NAME(name,i)); i = i->bro_dir)
            j = i;
        if(i->file_type == 0 && i != 0 && (!CHECK_FILE_NAME(name, i)))//如果是目录
        {
            if(!CHECK_FILE_NAME(name,current_file->son_dir))//从目录的链表中删除
            {
                current_file->son_dir = i->bro_dir;
                _int_free(i);
                return 0;
            }
            else
            {
                j->bro_dir = i->bro_dir;
                _int_free(i);
                return 0;
            }
        }
        else if(i->file_type == 1 && i != 0 && (!CHECK_FILE_NAME(name, i)))
        {
            if(!CHECK_FILE_NAME(name,current_file->son_dir))//如果是文件
            {
                current_file->son_dir = i->bro_dir;
                uint64_t mask_number = 0x10 << (i->hard_num % 64);
                uint64_t idx = i->hard_num / 64;
                bitmap[idx] ^= mask_number;//如果是文件释放申请的磁盘空间
                _int_free(i);
                return 0;
            }
            else
            {
                j->bro_dir = i->bro_dir;
                uint64_t mask_number = 0x10 << (i->hard_num % 64);
                uint64_t idx = i->hard_num / 64;
                bitmap[idx] ^= mask_number;
                _int_free(i);
                return 0;
            }
        }
        else if(i == 0)
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
    return 0;

}