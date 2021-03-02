#include <fcntl.h>
#include <linux/kvm.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include<unistd.h>
#include "definition.h"


#define PS_LIMIT (0x200000)
#define MEM_SIZE (PS_LIMIT * 0x5)//1 -> kernel, 2 -> user, 3 -> heap, 4,5 -> hard
#define PAGE_TABLE_SIZE (0x5000)// page size
#define KERNEL_STACK_SIZE (0x1000)// kernel stack size
#define MAX_KERNEL_SIZE (PS_LIMIT - PAGE_TABLE_SIZE - KERNEL_STACK_SIZE)

void read_file(const char *filename, uint8_t** content_ptr, size_t* size_ptr) {
  FILE *f = fopen(filename, "rb");
  if(f == NULL) error("Open file '%s' failed.\n", filename);
  if(fseek(f, 0, SEEK_END) < 0) pexit("fseek(SEEK_END)");

  size_t size = ftell(f);
  if(size == 0) error("Empty file '%s'.\n", filename);
  if(fseek(f, 0, SEEK_SET) < 0) pexit("fseek(SEEK_SET)");

  uint8_t *content = (uint8_t*) malloc(size);
  if(content == NULL) error("read_file: Cannot allocate memory\n");
  if(fread(content, 1, size, f) != size) error("read_file: Unexpected EOF\n");

  fclose(f);
  *content_ptr = content;
  *size_ptr = size;
} //读文件



void setup_regs(VM *vm, size_t entry) {
  struct kvm_regs regs;
  if(ioctl(vm->vcpufd, KVM_GET_REGS, &regs) < 0) pexit("ioctl(KVM_GET_REGS)");
  regs.rip = entry;
  regs.rsp = PS_LIMIT - 0x1000; /* temporary stack */
  regs.rdi = PS_LIMIT; /* start of free pages */
  regs.rsi = MEM_SIZE - regs.rdi; /* total length of free pages */
  regs.rflags = 0x2;
  if(ioctl(vm->vcpufd, KVM_SET_REGS, &regs) < 0) pexit("ioctl(KVM_SET_REGS");
}


void setup_paging(VM *vm) {
  struct kvm_sregs sregs;
  if(ioctl(vm->vcpufd, KVM_GET_SREGS, &sregs) < 0) pexit("ioctl(KVM_GET_SREGS)");
  uint64_t pml4_addr = MAX_KERNEL_SIZE;
  uint64_t *pml4 = (void*) (vm->mem + pml4_addr);

  uint64_t pdp_addr = pml4_addr + 0x1000;
  uint64_t *pdp = (void*) (vm->mem + pdp_addr);

  uint64_t pd_addr = pdp_addr + 0x1000;
  uint64_t *pd = (void*) (vm->mem + pd_addr);//页表必须都是4k对齐的
  //内存一共分四页，第一页给内核使用，第二页用户使用，第三页用于动态内存分配，第四，五块给磁盘
  pml4[0] = PDE64_PRESENT | PDE64_RW | PDE64_USER | pdp_addr;
  pdp[0] = PDE64_PRESENT | PDE64_RW | PDE64_USER | pd_addr;
  pd[0] = PDE64_PRESENT | PDE64_RW | PDE64_PS; /* kernel only, no PED64_USER */
  pd[1] = PDE64_PRESENT | PDE64_RW | PDE64_PS | PDE64_USER | PS_LIMIT;// user mem
  pd[2] = PDE64_PRESENT | PDE64_RW | PDE64_PS | PDE64_USER | PS_LIMIT * 2;// heap mem
  pd[3] = PDE64_PRESENT | PDE64_RW | PDE64_PS | PDE64_USER | PS_LIMIT * 3;//hardware disk mem
  pd[4] = PDE64_PRESENT | PDE64_RW | PDE64_PS | PDE64_USER | PS_LIMIT * 4;//hardware disk mem_2
  //设置控制寄存器
  sregs.cr3 = pml4_addr;
  sregs.cr4 = CR4_PAE;
  sregs.cr4 |= CR4_OSFXSR | CR4_OSXMMEXCPT; /* enable SSE instruction */
  sregs.cr0 = CR0_PE | CR0_MP | CR0_ET | CR0_NE | CR0_WP | CR0_AM | CR0_PG;
  sregs.efer = EFER_LME | EFER_LMA;
  sregs.efer |= EFER_SCE; /* enable syscall instruction *///打开系统调用

  if(ioctl(vm->vcpufd, KVM_SET_SREGS, &sregs) < 0) pexit("ioctl(KVM_SET_SREGS)");
}


void setup_seg_regs(VM *vm) {
  struct kvm_sregs sregs;
  if(ioctl(vm->vcpufd, KVM_GET_SREGS, &sregs) < 0) pexit("ioctl(KVM_GET_SREGS)");
  struct kvm_segment seg = {
    .base = 0,
    .limit = 0xffffffff,
    .selector = 1 << 3,
    .present = 1,
    .type = 0xb, /* Code segment */
    .dpl = 0, /* Kernel: level 0 */
    .db = 0,
    .s = 1,
    .l = 1, /* long mode */
    .g = 1
  };
  sregs.cs = seg;
  seg.type = 0x3; /* Data segment */
  seg.selector = 2 << 3;
  sregs.ds = sregs.es = sregs.fs = sregs.gs = sregs.ss = seg;
  if(ioctl(vm->vcpufd, KVM_SET_SREGS, &sregs) < 0) pexit("ioctl(KVM_SET_SREGS)");
}


void setup_long_mode(VM *vm) {
  setup_paging(vm);
  setup_seg_regs(vm);
}



VM* kvm_init(uint8_t code[], size_t len, char* argv) {
  puts("Start Init KVM......");
  int kvmfd = open("/dev/kvm", O_RDWR | O_CLOEXEC);//创建虚拟机
  if(kvmfd < 0) pexit("open(/dev/kvm)");

  
  int vmfd = ioctl(kvmfd, KVM_CREATE_VM, 0);
  if(vmfd < 0) pexit("ioctl(KVM_CREATE_VM)");
  void *mem = mmap(0,
    MEM_SIZE,
    PROT_READ | PROT_WRITE,
    MAP_SHARED | MAP_ANONYMOUS,
    -1, 0);
  if(mem == NULL) pexit("mmap(MEM_SIZE)");
  size_t entry = 0;
  memcpy((void*) mem + entry, code, len);//将内核的指令复制到虚拟机的内存中
    if(!memcmp(argv, "shell", 5))
    {
        uint8_t *code_;
        size_t len_;
        read_file("./kernel/shell/shell.bin", &code_, &len_);
        memcpy((void*) mem + PS_LIMIT, code_, len_);
    }
  
  struct kvm_userspace_memory_region region = {
    .slot = 0,
    .flags = 0,
    .guest_phys_addr = 0,
    .memory_size = MEM_SIZE,
    .userspace_addr = (size_t) mem
  };
  if(ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &region) < 0) {
    pexit("ioctl(KVM_SET_USER_MEMORY_REGION)");//将申请的内存交给KVM使用
  }//set VM

  int vcpufd = ioctl(vmfd, KVM_CREATE_VCPU, 0);
  if(vcpufd < 0) pexit("ioctl(KVM_CREATE_VCPU)");
  size_t vcpu_mmap_size = ioctl(kvmfd, KVM_GET_VCPU_MMAP_SIZE, NULL);
  struct kvm_run *run = (struct kvm_run*) mmap(0,
    vcpu_mmap_size,
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    vcpufd, 0);

  VM *vm = (VM*) malloc(sizeof(VM));
  *vm = (struct VM){
    .mem = mem,
    .mem_size = MEM_SIZE,
    .vcpufd = vcpufd,
    .run = run
  };

  setup_regs(vm, entry);//设置通用寄存器
  setup_long_mode(vm);//设置分页进入64位模式
  puts("Congratuate to you! KVM Init Success!");
  return vm;
}

void execute(VM* vm) {
  while(1) {
    ioctl(vm->vcpufd, KVM_RUN, NULL);
    switch (vm->run->exit_reason) {
    //设置I/O和异常的响应方式
    case KVM_EXIT_HLT:
      //dump_regs(vm);
      fprintf(stderr, "KVM_EXIT_HLT\n");
      return;
    case KVM_EXIT_IO://响应IO中断
      if(vm->run->io.direction == KVM_EXIT_IO_OUT)
        write(1,(((char*)vm->run) + vm->run->io.data_offset),0x1);// out
      else if(vm->run->io.direction == KVM_EXIT_IO_IN)
        read(0,(((char*)vm->run) + vm->run->io.data_offset),0x1);// in
      break;
    case KVM_EXIT_FAIL_ENTRY:
      error("KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason = 0x%llx\n",
        vm->run->fail_entry.hardware_entry_failure_reason);
    case KVM_EXIT_INTERNAL_ERROR:
      error("KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x\n",
        vm->run->internal.suberror);
    case KVM_EXIT_SHUTDOWN:
      //dump_regs(vm);
      error("KVM_EXIT_SHUT_DOWN\n");
      return;
    default:
      error("Unhandled reason: %d\n", vm->run->exit_reason);
    }
  }
}

int main(int argc, char *argv[]) {
  uint8_t *code;
  size_t len;
  read_file(argv[1], &code, &len);
  if(len > MAX_KERNEL_SIZE)
    error("Kernel size exceeded, %p > MAX_KERNEL_SIZE(%p).\n",
      (void*) len,
      (void*) MAX_KERNEL_SIZE);
  VM* vm = kvm_init(code, len, argv[2]);
  execute(vm);
}
