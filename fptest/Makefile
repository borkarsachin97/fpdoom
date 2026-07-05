NAME = fptest
TWO_STAGE = 0

SYS_EXTRA = sdio microfat sfc

APP_SRCS = main my_keypad my_ui
APP_OBJS1 = $(APP_SRCS:%=$(OBJDIR)/app/%.o)
APP_OBJS2 =


LVGL_PATH ?= ${shell pwd}/lvgl
include $(LVGL_PATH)/lvgl.mk
APP_CFLAGS += -I$(LVGL_PATH)

APP_OBJS2 += $(CSRCS:%.c=$(OBJDIR)/app/%.o)

include ../buildinc.make

#CFLAGS += -DSDIO_VERBOSE=2

APP_CFLAGS += -std=c99 -pedantic
APP_CFLAGS += -I$(SYSDIR)
