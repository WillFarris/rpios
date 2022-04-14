include gccpath.mk
ARMGNU ?= $(ARMPATH)aarch64-none-elf

C_OPS = -nostdlib -nostartfiles -ffreestanding -mgeneral-regs-only -g -Iheaders
ASM_OPS = -g -Iheaders
QEMU_OPS = -s -M raspi3b -cpu cortex-a53 -serial null -serial stdio# -vnc :1

BUILD_DIR = build
SRC_DIR = src

all: kernel8.img
	$(ARMGNU)-objdump -D $(BUILD_DIR)/kernel8.elf > $(BUILD_DIR)/kernel8.elf.dump

clean:
	rm -rf $(BUILD_DIR) armstub/build *.img

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(C_OPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASM_OPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

qemu: kernel8.img
	qemu-system-aarch64 $(QEMU_OPS) -kernel build/kernel8.elf

qemus: kernel8.img
	qemu-system-aarch64 $(QEMU_OPS) -kernel $(BUILD_DIR)/kernel8.elf -S

gdb:
	aarch64-linux-gnu-gdb -q --se build/kernel8.elf

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	mkdir -p $(BUILD_DIR)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

armstub/build/armstub_s.o: armstub/armstub.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

armstub: armstub/build/armstub_s.o
	$(ARMGNU)-ld --section-start=.text=0 -o armstub/build/armstub.elf armstub/build/armstub_s.o
	$(ARMGNU)-objcopy armstub/build/armstub.elf -O binary armstub-new.bin
