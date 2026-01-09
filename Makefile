# Compiler, linker and other tools
ASM         := nasm
CC          := gcc
LD          := ld
OBJCOPY     := objcopy

#The Directories, Source, Includes, Objects, Binary and Resources
BUILD_DIR   := build
BOOT_BIN    := $(BUILD_DIR)/boot.bin
KERNEL_BIN  := $(BUILD_DIR)/kernel.bin
KERNEL_ELF  := $(BUILD_DIR)/kernel.elf
IMAGE_BIN   := $(BUILD_DIR)/os-image.bin

# Flags
ASFLAGS=-f bin
CFLAGS  := -m32 -ffreestanding

LDFLAGS := -m elf_i386 -T linker.ld

#---------------------------------------------------------------------------------
#							DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
all: $(IMAGE_BIN)

$(BOOT_BIN) : boot.asm print_string.asm print_string_pm.asm gdt.asm switch_to_pm.asm disk_load.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) $(ASFLAGS) boot.asm -o $(BOOT_BIN)


$(KERNEL_ELF): kernel.c linker.ld
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c kernel.c -o $(BUILD_DIR)/kernel.o
	$(LD) $(LDFLAGS) -o $(KERNEL_ELF) $(BUILD_DIR)/kernel.o


$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_BIN)


$(IMAGE_BIN) : $(BOOT_BIN) $(KERNEL_BIN)
	@mkdir -p $(BUILD_DIR)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(IMAGE_BIN)

run: all
	qemu-system-x86_64 -drive format=raw,file=$(IMAGE_BIN)

check:
	@test $$(stat -c "%s" build/boot.bin) -eq 512

clean:
	rm -rf build