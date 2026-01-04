TARGET=build/boot.bin
ASM=nasm
ASFLAGS=-f bin

all: $(TARGET)

$(TARGET): boot.asm print_string.asm
	mkdir -p build
	$(ASM) $(ASFLAGS) boot.asm -o $(TARGET)

run: all
	qemu-system-x86_64 -drive format=raw,file=$(TARGET)

clean:
	rm -rf build