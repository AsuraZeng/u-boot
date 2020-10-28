/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration header file for IOT2050
 * (C) Copyright 2018 Siemens AG
 * Le Jin <le.jin@siemens.com>
 */

#ifndef __CONFIG_IOT2050_H
#define __CONFIG_IOT2050_H

#include <linux/sizes.h>
#include <environment/ti/mmc.h>

// #define CONFIG_ENV_SIZE			(128 << 10)

/* DDR Configuration */
#define CONFIG_SYS_SDRAM_BASE1		0x880000000

/* SPL Loader Configuration */
#if defined(CONFIG_TARGET_IOT2050_BASIC_A53) || defined(CONFIG_TARGET_IOT2050_ADVANCED_A53)
// #define CONFIG_SPL_TEXT_BASE		0x80080000
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SPL_TEXT_BASE +	\
					 CONFIG_SYS_K3_NON_SECURE_MSRAM_SIZE)
// #define CONFIG_SYS_SPI_U_BOOT_OFFS	0x280000
#else
//#define CONFIG_SPL_TEXT_BASE		0x41c00000
/*
 * Maximum size in memory allocated to the SPL BSS. Keep it as tight as
 * possible (to allow the build to go through), as this directly affects
 * our memory footprint. The less we use for BSS the more we have available
 * for everything else.
 */
#define CONFIG_SPL_BSS_MAX_SIZE		0x5000
/*
 * Link BSS to be within SPL in a dedicated region located near the top of
 * the MCU SRAM, this way making it available also before relocation. Note
 * that we are not using the actual top of the MCU SRAM as there is a memory
 * location filled in by the boot ROM that we want to read out without any
 * interference from the C context.
 */
#define CONFIG_SPL_BSS_START_ADDR	(CONFIG_SYS_K3_BOOT_PARAM_TABLE_INDEX -\
					 CONFIG_SPL_BSS_MAX_SIZE)
/* Set the stack right below the SPL BSS section */
#define CONFIG_SYS_INIT_SP_ADDR         CONFIG_SPL_BSS_START_ADDR
/* Configure R5 SPL post-relocation malloc pool in DDR */
#define CONFIG_SYS_SPL_MALLOC_START	0x84000000
#define CONFIG_SYS_SPL_MALLOC_SIZE	SZ_16M
//#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x80000
#endif

#ifdef CONFIG_SYS_K3_SPL_ATF
#define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME	"tispl.bin"
#endif

#ifndef CONFIG_CPU_V7R
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

#define CONFIG_SPL_MAX_SIZE		CONFIG_SYS_K3_MAX_DOWNLODABLE_IMAGE_SIZE

#define CONFIG_SYS_BOOTM_LEN		SZ_64M

#define PARTS_DEFAULT \
	/* Linux partitions */ \
	"uuid_disk=${uuid_gpt_disk};" \
	"name=rootfs,start=0,size=-,uuid=${uuid_gpt_rootfs}\0"

#define EXTRA_ENV_SELECT_FDT                           \
    "fdt_found=no\0"                                  \
    "fdt_select=if env exists board_name; then "       \
                    "if test ${board_name} = IOT2050-BASIC; then "          \
                        "env set fdtfile siemens/iot2050-basic.dtb;"        \
                        "env set fdt_found yes;" \
                    "else if test ${board_name} = IOT2050-ADVANCED; then "  \
                        "env set fdtfile siemens/iot2050-advanced.dtb;"     \
                        "env set fdt_found yes;" \
                    "else "                                                  \
                        "echo ** ERROR: can not find fdt file for ${board_name} **;" \
                        "env set fdt_found no;" \
                    "fi; fi;"     \
                "else "           \
                    "echo ** Warning: board_name is not defined, default boot as IOT2050-BASIC **;" \
                    "env set fdtfile siemens/iot2050-basic.dtb;" \
                    "env set fdt_found yes;" \
                "fi;\0"

/* U-Boot general configuration */
#define EXTRA_ENV_IOT2050_BOARD_SETTINGS				\
	"loadaddr=0x80080000\0"						\
	"scriptaddr=0x83000000\0"					\
	"kernel_addr_r=0x80080000\0"					\
	"ramdisk_addr_r=0x81000000\0"					\
	"fdt_addr_r=0x82000000\0"					\
	"overlay_addr_r=0x83000000\0"   \
	"start_icssg0=rproc start 2; rproc start 3\0" \
	"load_icssg0_pru0_fw=sf read 0x89000000 0x7c0000 0x8000; rproc load 2 0x89000000 0x8000\0" \
	"load_icssg0_rtu0_fw=sf read 0x8a000000 0x7e0000 0x8000; rproc load 3 0x8a000000 0x8000\0" \
	"init_icssg0=rproc init; sf probe; run load_icssg0_pru0_fw; run load_icssg0_rtu0_fw\0" \
	"usb_pgood_delay=900\0"

#ifndef CONFIG_SPL_BUILD
#if defined(CONFIG_TARGET_IOT2050_BASIC_A53)
#define BOOT_TARGET_DEVICES(func) \
        func(MMC, mmc, 0) \
        func(MMC, usb, 0) \
        func(MMC, usb, 1) \
        func(MMC, usb, 2)
#else
#define BOOT_TARGET_DEVICES(func) \
        func(MMC, mmc, 1) \
        func(MMC, mmc, 0) \
        func(MMC, usb, 0) \
        func(MMC, usb, 1) \
        func(MMC, usb, 2)
#endif
#include <config_distro_bootcmd.h>

#define UUID_DEVICES \
        "uuid_devices=" BOOT_TARGET_DEVICES(BOOTENV_DEV_NAME) "\0"
#define UUID_CONFLICT_CHECK   \
        "uuid_conflict_check="   \
            "echo ====== uuid conflict check begin ======;"   \
            "setenv uuid_conflict no;"   \
            "run uuid_get_all;"   \
            "setenv indexa a;"   \
            "for uuida in ${uuidall}; do "  \
                "setenv indexb a;"   \
                "for uuidb in ${uuidall}; do "  \
                    "if test $indexa != $indexb && test ${uuida} = ${uuidb}; then "   \
                        "echo ** uuid conflict: ${uuida} **; echo ====== uuid conflict check end ======; setenv uuid_conflict yes; exit; fi;"   \
                    "setenv indexb ${indexb}a;"   \
                "done;"  \
                "setenv indexa ${indexa}a;"   \
            "done;"   \
            "echo ====== uuid conflict check end ======\0"
#define UUID_GET_DEV(devtypeu, devtypel, instance) \
        "uuid_get_" #devtypel #instance "=" \
            "setenv devtype " #devtypel ";" \
            "setenv devnum " #instance ";" \
            "run uuid_get_" #devtypel "\0"
#define UUID_GET_MMC_SHARE  \
        "uuid_get_mmc=if mmc dev ${devnum}; then run uuid_get; fi\0"
#define UUID_GET_USB_SHARE \
        "uuid_get_usb=if test ${usbreseted} = no; then usb reset; setenv usbreseted yes; fi; if usb dev ${devnum}; then run uuid_get; fi\0"
#define UUID_GET_NET_SHARE \
        "uuid_get_net=echo net does not upport get uuid\0"
#define UUID_GET   \
        "uuid_get="   \
        "part list ${devtype} ${devnum} partlist;"   \
            "if env exists partlist; then "   \
                "for devpart in ${partlist}; do "   \
                    "if part uuid ${devtype} ${devnum}:${devpart} uuid; then " \
                        "setenv uuids ${uuids} ${uuid};"   \
                    "fi;" \
                "done;" \
            "fi\0"
#define UUID_GET_ALL   \
        "uuid_get_all="   \
            "env delete uuidall;"   \
            "setenv usbreseted no;"   \
            "for device in ${uuid_devices}; do "   \
                "env delete uuids;"   \
                "run uuid_get_${device};" \
                "if env exists uuids; then " \
                    "setenv uuidall ${uuidall} ${uuids};" \
                "fi;" \
            "done\0"

#define UUID_CONFLICT_CHECK_ENV   \
        UUID_CONFLICT_CHECK   \
        UUID_DEVICES   \
        UUID_GET   \
        UUID_GET_ALL   \
        UUID_GET_MMC_SHARE   \
        UUID_GET_USB_SHARE   \
        UUID_GET_NET_SHARE   \
        BOOT_TARGET_DEVICES(UUID_GET_DEV)

#ifdef CONFIG_BOOTCOMMAND
#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND   "run uuid_conflict_check; if test $uuid_conflict = no; then run distro_bootcmd; fi"
#endif

/* Incorporate settings into the U-Boot environment */
#define CONFIG_EXTRA_ENV_SETTINGS					\
	BOOTENV								\
	EXTRA_ENV_IOT2050_BOARD_SETTINGS	\
	EXTRA_ENV_SELECT_FDT   \
	UUID_CONFLICT_CHECK_ENV
#endif

#define CONFIG_SUPPORT_EMMC_BOOT

/* MMC ENV related defines */
#ifdef CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_ENV_PART	1
#define CONFIG_ENV_SIZE		(128 << 10)
#define CONFIG_ENV_OFFSET		0x680000
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)
#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#endif

/* Non Kconfig SF configs */
// #define CONFIG_SF_DEFAULT_SPEED		0
// #define CONFIG_SF_DEFAULT_MODE		0
// #ifdef CONFIG_ENV_IS_IN_SPI_FLASH
// #define CONFIG_ENV_OFFSET		0x680000
// #define CONFIG_ENV_SECT_SIZE		0x20000
// #define CONFIG_ENV_OFFSET_REDUND        (CONFIG_ENV_OFFSET + 
// 					 CONFIG_ENV_SECT_SIZE)
// #define CONFIG_SYS_REDUNDAND_ENVIRONMENT
// #endif

/* Now for the remaining common defines */
#include <configs/ti_armv7_common.h>

#endif /* __CONFIG_IOT2050_H */
