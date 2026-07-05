NAME = rtos
TWO_STAGE = 0
SYS_EXTRA = sdio microfat sfc

# FreeRTOS Core
FREERTOS_DIR = kernel
FREERTOS_SRCS = $(FREERTOS_DIR)/tasks.c \
                $(FREERTOS_DIR)/queue.c \
                $(FREERTOS_DIR)/list.c \
                $(FREERTOS_DIR)/portable/MemMang/heap_4.c \
                $(FREERTOS_DIR)/portable/GCC/ARM926EJS/port.c \
                $(FREERTOS_DIR)/portable/GCC/ARM926EJS/portISR.c

# Our application tasks and integration
APP_SRCS = apps/fptest_task kernel/main
APP_OBJS1 = $(APP_SRCS:%=$(OBJDIR)/%.o) \
            $(FREERTOS_SRCS:%.c=$(OBJDIR)/%.o)
APP_OBJS2 =

include ../buildinc.make

# FreeRTOS Includes
APP_CFLAGS += -I$(FREERTOS_DIR)/include \
              -I$(FREERTOS_DIR)/portable/GCC/ARM926EJS \
              -I../rtos \
              -I$(SYSDIR) \
              -std=gnu99 -pedantic -Wno-unused-parameter

# FreeRTOS requires ARM mode, not THUMB mode, for context switching
APP_CFLAGS += -marm

# Override CFLAGS specifically for LTO issues with inline assembly in thumb mode during linking
CFLAGS := $(filter-out -mthumb, $(CFLAGS))
CFLAGS += -marm

# Allow FreeRTOS inline assembly in THUMB interworking
APP_CFLAGS += -DTHUMB_INTERWORK
CFLAGS += -DTHUMB_INTERWORK

# Ensure object directories are built
$(OBJDIR)/kernel/%.o: kernel/%.c | objdir
	@mkdir -p $(dir $@)
	$(call compile_cc,$(APP_CFLAGS))

# Explicitly define pack_reloc processing for rtos.bin (Overrides generic make)
../pack_reloc/pack_reloc:
	$(MAKE) -C ../pack_reloc


rtos.bin: $(OBJDIR)/rtos.elf ../pack_reloc/pack_reloc
	../pack_reloc/pack_reloc $< $(OBJDIR)/rtos.rel
	$(OBJCOPY) -O binary -j .text $< $@
	cat $(OBJDIR)/rtos.rel >> $@

$(OBJDIR)/kernel/portable/MemMang/%.o: kernel/portable/MemMang/%.c | objdir
	@mkdir -p $(dir $@)
	$(call compile_cc,$(APP_CFLAGS))

$(OBJDIR)/kernel/portable/GCC/ARM926EJS/%.o: kernel/portable/GCC/ARM926EJS/%.c | objdir
	@mkdir -p $(dir $@)
	$(call compile_cc,$(APP_CFLAGS))

$(OBJDIR)/apps/%.o: apps/%.c | objdir
	@mkdir -p $(dir $@)
	$(call compile_cc,$(APP_CFLAGS))