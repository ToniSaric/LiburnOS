#---------------------------------------------------------------------------------
# Compiler, linker and other tools
#---------------------------------------------------------------------------------
ASM         := nasm
CC          := gcc
LD          := ld
OBJCOPY     := objcopy

#---------------------------------------------------------------------------------
# The Directories, Source, Includes, Objects, Binary and Resources
#---------------------------------------------------------------------------------
BUILD_DIR  := build
BOOT_DIR   := boot
KERNEL_DIR := kernel

LINKER_SCRIPT := linker.ld

BOOT_MAIN  := $(BOOT_DIR)/boot.asm
BOOT_BIN   := $(BUILD_DIR)/boot.bin

KERNEL_ELF := $(BUILD_DIR)/kernel.elf
KERNEL_BIN := $(BUILD_DIR)/kernel.bin

IMAGE_BIN  := $(BUILD_DIR)/os-image.bin

KERNEL_ENTRY     := $(KERNEL_DIR)/entry.asm
KERNEL_ENTRY_OBJ := $(BUILD_DIR)/kernel_entry.o

#---------------------------------------------------------------------------------
# Flags
#---------------------------------------------------------------------------------
ASFLAGS=-f bin
CFLAGS  := -m32 -ffreestanding
LDFLAGS := -m elf_i386 -T $(LINKER_SCRIPT)

KERNEL_SECTORS := 15
SECTOR_SIZE := 512
##################################################################################
#							DO NOT EDIT BELOW THIS LINE
##################################################################################

#---------------------------------------------------------------------------------
# Build directory - output
#---------------------------------------------------------------------------------
BOOT_BIN         := $(BUILD_DIR)/boot.bin
KERNEL_BIN       := $(BUILD_DIR)/kernel.bin
KERNEL_ELF       := $(BUILD_DIR)/kernel.elf
KERNEL_ENTRY_OBJ := $(BUILD_DIR)/kernel_entry.o
IMAGE_BIN        := $(BUILD_DIR)/os-image.bin
#---------------------------------------------------------------------------------
# Inputs
#---------------------------------------------------------------------------------
BOOT_ASM         := $(shell find $(BOOT_DIR) -type f -name '*.asm')

KERNEL_BIN       := $(BUILD_DIR)/kernel.bin
KERNEL_ELF       := $(BUILD_DIR)/kernel.elf
KERNEL_ENTRY     := $(KERNEL_DIR)/entry.asm
KERNEL_SRC       := $(shell find $(KERNEL_DIR) -type f -name '*.c')
#---------------------------------------------------------------------------------
# Generate object file names
#---------------------------------------------------------------------------------
KERNEL_OBJ := $(patsubst $(KERNEL_DIR)/%.c,$(BUILD_DIR)/%.o,$(KERNEL_SRC))

all: $(IMAGE_BIN)

$(BOOT_BIN) : $(BOOT_ASM)
	@mkdir -p $(BUILD_DIR)
	$(ASM) $(ASFLAGS) $(BOOT_MAIN) -o $@

$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY)
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f elf32 $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_ELF): $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ) $(LINKER_SCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ)


$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

$(IMAGE_BIN) : $(BOOT_BIN) $(KERNEL_BIN)
	@mkdir -p $(BUILD_DIR)
	@dd if=/dev/zero of=$(IMAGE_BIN) bs=$(SECTOR_SIZE) count=$$((1 + $(KERNEL_SECTORS))) status=none
	@dd if=$(BOOT_BIN)  of=$(IMAGE_BIN) conv=notrunc bs=$(SECTOR_SIZE) seek=0 status=none
	@dd if=$(KERNEL_BIN) of=$(IMAGE_BIN) conv=notrunc bs=$(SECTOR_SIZE) seek=1 status=none
	@echo "Boot sector size: $$(stat -c '%s' $(BOOT_BIN)) bytes"
	@echo "Kernel size: $$(stat -c '%s' $(KERNEL_BIN)) bytes"
	@echo "OS image created: $(IMAGE_BIN)"


run: all
	qemu-system-x86_64 -drive format=raw,file=$(IMAGE_BIN)

check: $(BOOT_BIN)
	@if [ $$(stat -c "%s" $(BOOT_BIN)) -eq 512 ]; then \
		echo "✓ Boot sector is exactly 512 bytes"; \
	else \
		echo "✗ Boot sector is $$(stat -c '%s' $(BOOT_BIN)) bytes (should be 512)"; \
		exit 1; \
	fi

info:
	@echo "=== Build Configuration ==="
	@echo "Boot main file: $(BOOT_MAIN)"
	@echo "Boot dependencies: $(BOOT_ASM)"
	@echo "Kernel C files: $(KERNEL_SRC)"
	@echo "Kernel C objects: $(KERNEL_OBJ)"
	@echo "=========================="

clean:
	rm -rf build