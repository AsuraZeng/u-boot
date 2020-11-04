// SPDX-License-Identifier: GPL-2.0+
/*
 * Board specific initialization for IOT2050
 * (C) Copyright 2018 Siemens AG
 *	Le Jin <le.jin@siemens.com>
 *
 */

#include <common.h>
#include <dm.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/hardware.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <spl.h>
#include <i2c.h>
#include "board_detect.h"

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_TARGET_IOT2050_A53) || defined(CONFIG_TARGET_IOT2050_ADVANCED_R5)
#define RAM_SIZE (0x80000000)
#elif defined(CONFIG_TARGET_IOT2050_BASIC_A53) || defined(CONFIG_TARGET_IOT2050_BASIC_R5)
#define RAM_SIZE (0x40000000)
#else
#error "Unknown iot2050 platfrom"
#endif

int fdt_disable_node(void *blob, char *node_path);

int board_init(void)
{
	return 0;
}

int dram_init(void)
{
#ifdef CONFIG_PHYS_64BIT
	gd->ram_size = RAM_SIZE;
#else
	gd->ram_size = RAM_SIZE;
#endif

	return 0;
}

ulong board_get_usable_ram_top(ulong total_size)
{
#ifdef CONFIG_PHYS_64BIT
	/* Limit RAM used by U-Boot to the DDR low region */
	if (gd->ram_top > 0x100000000)
		return 0x100000000;
#endif

	return gd->ram_top;
}

int dram_init_banksize(void)
{
	/* Bank 0 declares the memory available in the DDR low region */
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = RAM_SIZE;
	gd->ram_size = RAM_SIZE;

#ifdef CONFIG_PHYS_64BIT
	/* Bank 1 declares the memory available in the DDR high region */
	gd->bd->bi_dram[1].start = CONFIG_SYS_SDRAM_BASE1;
	gd->bd->bi_dram[1].size = 0x00000000;
	gd->ram_size = RAM_SIZE;
#endif

	return 0;
}

#ifdef CONFIG_SPL_LOAD_FIT
int board_fit_config_name_match(const char *name)
{
#if defined(CONFIG_TARGET_IOT2050_ADVANCED_A53)
	if (!strcmp(name, "iot2050-advanced"))
		return 0;
#elif defined(CONFIG_TARGET_IOT2050_BASIC_A53)
	if (!strcmp(name, "iot2050-basic"))
		return 0;
#endif
#if defined(CONFIG_TARGET_IOT2050_ADVANCED_R5)
	if (!strcmp(name, "iot2050-r5-advanced"))
		return 0;
#elif defined(CONFIG_TARGET_IOT2050_BASIC_R5)
	if (!strcmp(name, "iot2050-r5-basic"))
		return 0;
#endif

	return -1;
}
#endif

int do_board_detect(void)
{
	return 0;
}

static void setup_board_eeprom_env(void)
{
	do_board_detect();
	if(board_iot2050_was_eeprom_read())
	{
		set_board_info_env_iot2050();
	}
	else
	{
		pr_err("%s: Board info parsing error!\n", __func__);
	}
}

int board_late_init(void)
{
	struct iot2050_eeprom *ep = IOT2050_EEPROM_DATA;

	setup_board_eeprom_env();
	/*
	 * The first MAC address for ethernet a.k.a. ethernet0 comes from
	 * efuse populated via the am654 gigabit eth switch subsystem driver.
	 * All the other ones are populated via EEPROM, hence continue with
	 * an index of 1.
	 */
	board_iot2050_set_ethaddr(1, ep->mac_addr_cnt);
	return 0;
}

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, bd_t *bd)
{
	int ret;

	ret = fdt_fixup_msmc_ram(blob, "/interconnect@100000", "sram@70000000");
	if (ret)
		printf("%s: fixing up msmc ram failed %d\n", __func__, ret);

#if defined(CONFIG_TI_SECURE_DEVICE)
	/* Make HW RNG reserved for secure world use */
	ret = fdt_disable_node(blob, "/interconnect@100000/trng@4e10000");
	if (ret)
		printf("%s: disabling TRGN failed %d\n", __func__, ret);
#endif

	return ret;
}
#endif

void spl_board_init(void)
{
}
