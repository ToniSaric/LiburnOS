# Compiler, linker and other tools
ASM         := nasm
CC          := gcc
LD          := ld
OBJCOPY     := objcopy

#The Directories, Source, Includes, Objects, Binary and Resources
BUILD_DIR        := build
BOOT_BIN         := $(BUILD_DIR)/boot.bin
KERNEL_BIN       := $(BUILD_DIR)/kernel.bin
KERNEL_ELF       := $(BUILD_DIR)/kernel.elf
KERNEL_ENTRY_OBJ := $(BUILD_DIR)/kernel_entry.o
KERNEL_OBJ       := $(BUILD_DIR)/kernel.o
IMAGE_BIN        := $(BUILD_DIR)/os-image.bin

# Flags
ASFLAGS=-f bin
CFLAGS  := -m32 -ffreestanding

LDFLAGS := -m elf_i386 -T linker.ld
KERNEL_SECTORS := 15
SECTOR_SIZE := 512
#---------------------------------------------------------------------------------
#							DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
all: $(IMAGE_BIN)

$(BOOT_BIN) : boot.asm print_string.asm print_string_pm.asm gdt.asm switch_to_pm.asm disk_load.asm load_kernel.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) $(ASFLAGS) boot.asm -o $(BOOT_BIN)

$(KERNEL_ENTRY_OBJ): kernel_entry.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f elf32 kernel_entry.asm -o $(KERNEL_ENTRY_OBJ)

$(KERNEL_OBJ): kernel.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c kernel.c -o $(KERNEL_OBJ)

$(KERNEL_ELF): $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $(KERNEL_ELF) $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ)


$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_BIN)

$(IMAGE_BIN) : $(BOOT_BIN) $(KERNEL_BIN)
	@mkdir -p $(BUILD_DIR)
	dd if=/dev/zero of=$(IMAGE_BIN) bs=$(SECTOR_SIZE) count=$$((1 + $(KERNEL_SECTORS))) status=none
	dd if=$(BOOT_BIN)  of=$(IMAGE_BIN) conv=notrunc bs=$(SECTOR_SIZE) seek=0 status=none
	dd if=$(KERNEL_BIN) of=$(IMAGE_BIN) conv=notrunc bs=$(SECTOR_SIZE) seek=1 status=none

run: all
	qemu-system-x86_64 -drive format=raw,file=$(IMAGE_BIN)

check:
	@test $$(stat -c "%s" build/boot.bin) -eq 512

clean:
	rm -rf build