REP_INC_DIR += src/bootstrap/board/imx6q_sabrelite

SRC_S   += bootstrap/spec/arm/crt0.s

SRC_CC  += bootstrap/spec/arm/cpu.cc
SRC_CC  += bootstrap/spec/arm/cortex_a9_mmu.cc
SRC_CC  += bootstrap/spec/arm/gicv2.cc
SRC_CC  += bootstrap/spec/arm/imx6_platform.cc
SRC_CC  += bootstrap/spec/arm/arm_v7_cpu.cc
SRC_CC  += hw/spec/32bit/memory_map.cc

include $(call select_from_repositories,lib/mk/bootstrap-hw.inc)
