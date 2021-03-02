#include "libc.h"
#include <stdint.h>

#define LIBC_CALL(cmd)                               \
  if(!memcmp(letter_array[0],#cmd,strlen(#cmd)))     \
   {user_##cmd((uint64_t)letter_array[1],(uint64_t)letter_array[2]);continue;}

void print_dir()
{
  char* tips = "jlx@jlx-kvm:";
  puts(tips,strlen(tips));
}

uint64_t skip_space(char* command)
{
  for(;*command == ' ' && *command != 0;(char*)command++);
  return (uint64_t)command;
}

uint64_t find_space(char* command)
{
  for(;*command != ' ' &&  *command != 0;(char*)command++);
  *command = '\x00';
  return (uint64_t)++command;
}

void spiltw(char* command, char** letter_array)
{
  char* letter = (char*)skip_space(command);
  for(int i = 0; (i < 3) && (*letter != 0); i++)
  {
    letter_array[i] = letter;
    letter = (char*)find_space(letter);
    letter = (char*)skip_space(letter);
  }
  return;
}

int main()
{
  char*  command = (char*)libc_malloc(0x30);  
  char* letter_array[4] = {0};
  while(1)
    {
      print_dir();
      memset(command,0,0x20);
      gets(command,0x20);
      spiltw(command, letter_array);
      LIBC_CALL(ls)
      LIBC_CALL(cd)
      LIBC_CALL(mkdir)
      LIBC_CALL(vim)
      LIBC_CALL(cat)
      LIBC_CALL(rm)
      else puts("invalid command!\n",18);
    }
    return 0;
}