TARGET := kernel.bin

SRCS := entry.s kernel.c sys_read.c sys_write.c file.c disk.c string.c malloc.c syscall_entry.s syscall_handler.c
OBJS :=
DEPS := $(wildcard ./**/*.d)
#SUBOBJS := syscall/syscalls.a utills/utils.a

CFLAGS := -Wall -Werror -fPIE -pie -masm=intel -nostdlib -Os -I.
LDFLAGS := -ffreestanding -fno-builtin -Tlinkscript.ld

.PRECIOUS: %.o
all: $(TARGET)

%.bin: $(SRCS) $(OBJS) $(SUBOBJS)
	$(CC) $(CFLAGS) $^ -o $@ -Xlinker --oformat=binary $(LDFLAGS)

%.o: %.s
	$(AS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c -MMD -MP $< -o $@

$(SUBOBJS): FORCE
	$(MAKE) -C $(dir $@) $(notdir $@)

.PHONY: clean
clean:
	rm *.bin

FORCE:
