# SPDX-License-Identifier: GPL-2.0
#
# Copyright (C) 2018 Texas Instruments, Incorporated - http://www.ti.com/
#	Andrew F. Davis <afd@ti.com>

%.dtb_HS: %.dtb FORCE
	$(call if_changed,k3secureimg,$(dir $(KEY))/x509-sysfw-template.txt)

$(obj)/u-boot-spl-nodtb.bin_HS: $(obj)/u-boot-spl-nodtb.bin FORCE
	$(call if_changed,k3secureimg,$(dir $(KEY))/x509-sysfw-template.txt)

tispl.bin_HS: $(obj)/u-boot-spl-nodtb.bin_HS $(patsubst %,$(obj)/dts/%.dtb_HS,$(subst ",,$(CONFIG_SPL_OF_LIST))) $(SPL_ITS) FORCE
	$(call if_changed,mkfitimage)

MKIMAGEFLAGS_u-boot.img_HS = -f auto -A $(ARCH) -T firmware -C none -O u-boot \
	-a $(CONFIG_SYS_TEXT_BASE) -e $(CONFIG_SYS_UBOOT_START) \
	-n "U-Boot $(UBOOTRELEASE) for $(BOARD) board" -E \
	$(patsubst %,-b arch/$(ARCH)/dts/%.dtb_HS,$(subst ",,$(CONFIG_OF_LIST)))

OF_LIST_TARGETS = $(patsubst %,arch/$(ARCH)/dts/%.dtb,$(subst ",,$(CONFIG_OF_LIST)))
$(OF_LIST_TARGETS): dtbs

u-boot-nodtb.bin_HS: u-boot-nodtb.bin FORCE
	$(call if_changed,k3secureimg,$(dir $(KEY))/x509-sysfw-template.txt)

u-boot.img_HS: u-boot-nodtb.bin_HS u-boot.img $(patsubst %.dtb,%.dtb_HS,$(OF_LIST_TARGETS)) FORCE
	$(call if_changed,mkimage)
