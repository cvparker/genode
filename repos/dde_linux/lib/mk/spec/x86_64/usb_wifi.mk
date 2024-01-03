REQUIRES := x86_64

include $(call select_from_repositories,lib/mk/wifi.inc)

TARGET_LIB_DIR := $(REP_DIR)/src/lib/usb_wifi

LIBS    += virt_linux_generated virt_lx_emul

INC_DIR += $(TARGET_LIB_DIR)

SRC_CC  += lx_emul/random.cc
SRC_CC  += dtb_helper_no_dtb.cc

SRC_C   += $(notdir $(wildcard $(TARGET_LIB_DIR)/generated_dummies.c))


SRC_C += lx_emul/shadow/drivers/char/random.c
SRC_C += lx_emul/shadow/fs/libfs.c
SRC_C += lx_emul/shadow/mm/page_alloc.c
SRC_C += lx_emul/shadow/mm/vmalloc.c
SRC_C += lx_emul/shadow/drivers/pci/pci-sysfs.c

SRC_C += lx_emul/virt/common_dummies.c

vpath lx_emul/virt/common_dummies.c $(REP_DIR)/src/lib

CC_OPT_net/mac80211/trace += -I$(LX_SRC_DIR)/net/mac80211
CC_OPT_net/wireless/trace += -I$(LX_SRC_DIR)/net/wireless

vpath %.c  $(TARGET_LIB_DIR)
vpath %.cc $(TARGET_LIB_DIR)
