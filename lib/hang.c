// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2013
 * Andreas Bießmann <andreas@biessmann.org>
 *
 * This file consolidates all the different hang() functions implemented in
 * u-boot.
 */

#include <common.h>
#include <bootstage.h>
#include <os.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <led.h>
#include <asm/io.h>

#define  WKUP_GPIO_32						0		/* WKUP_GPIO0 bank 2: Status Red  */
#define  WKUP_GPIO_24						24		/* WKUP_GPIO0 bank 1: Status Green*/

#define  WKUP_GPIO0_BASE					0x42110000
#define  GPIO_DIRECTION01_OFFSET				0x0010
#define  GPIO_SETDATA01_OFFSET					0x0018
#define  GPIO_DIRECTION23_OFFSET				0x0038
#define  GPIO_SETDATA23_OFFSET					0x0040
#define  WKUP_CTRL_MMR0_CFG0					0x43000000
#define  CTRLMMR_WKUP_GPIO32_PADCONFIG20_OFFSET			0x1C050		/* WKUP_GPIO_32  */
#define  CTRLMMR_WKUP_GPIO24_PADCONFIG12_OFFSET			0x1C030		/* WKUP_GPIO_24  */

#define  MUX_MODE_GENERAL_GPIO					0x07
#define  TX_DIS_FIELD						21		/* tx disable filed */
#define  RED							0
#define  GREEN							1
#define  TURN_ON						1
#define  TURN_OFF						0

/**
 * set_iot2050_status_led: set iot2050 status led
 *
 * @colour: status red/green led  [ red: RED ; green: GREEN ]
 * @state:  turn on/off the led   [ on: TURN_ON ; off: TURN_OFF ]
 *
 */

static void set_iot2050_status_led(unsigned int colour,unsigned int state)
{
	u32 pad_config,dir_reg,data_reg;
	void *dirreg, *outreg, *padconfig;
	u32 WKUP_GPIO;

	if (colour == GREEN) {
		dirreg = (void*)(WKUP_GPIO0_BASE+GPIO_DIRECTION01_OFFSET);
		outreg = (void*)(WKUP_GPIO0_BASE+GPIO_SETDATA01_OFFSET);
		padconfig = (void*)(WKUP_CTRL_MMR0_CFG0+CTRLMMR_WKUP_GPIO24_PADCONFIG12_OFFSET);
		WKUP_GPIO = WKUP_GPIO_24;

	} else if (colour == RED) {
		dirreg = (void*)(WKUP_GPIO0_BASE+GPIO_DIRECTION23_OFFSET);
		outreg = (void*)(WKUP_GPIO0_BASE+GPIO_SETDATA23_OFFSET);
		padconfig = (void*)(WKUP_CTRL_MMR0_CFG0+CTRLMMR_WKUP_GPIO32_PADCONFIG20_OFFSET);
		WKUP_GPIO = WKUP_GPIO_32;
	}

	/* 	config pinmux */
	pad_config = __raw_readl(padconfig);
	pad_config |= MUX_MODE_GENERAL_GPIO;
	pad_config &= ~(1 << TX_DIS_FIELD);
	__raw_writel(pad_config, padconfig);

	/* 	set direction out */
	dir_reg = __raw_readl(dirreg);
	dir_reg &= ~(1 << WKUP_GPIO);
	__raw_writel(dir_reg, dirreg);

	/* 	set the value high or low */
	data_reg = __raw_readl(outreg);
	if (state == TURN_ON)
		data_reg |=  (1 << WKUP_GPIO);
	else if (state == TURN_OFF)
		data_reg &= ~(1 << WKUP_GPIO);

	__raw_writel(data_reg, outreg);
}



static void set_iot2050_error_status_led_on(void)
{
	set_iot2050_status_led(GREEN,TURN_OFF);
	set_iot2050_status_led(RED,TURN_ON);
}
/**
 * hang - stop processing by staying in an endless loop
 *
 * The purpose of this function is to stop further execution of code cause
 * something went completely wrong.  To catch this and give some feedback to
 * the user one needs to catch the bootstage_error (see show_boot_progress())
 * in the board code.
 */
void hang(void)
{
	/*Turn on the red led for IOT2050 platform*/
	set_iot2050_error_status_led_on();
#if !defined(CONFIG_SPL_BUILD) || \
		(CONFIG_IS_ENABLED(LIBCOMMON_SUPPORT) && \
		 CONFIG_IS_ENABLED(SERIAL_SUPPORT))
	puts("### ERROR ### Please RESET the board ###\n");
#endif
	bootstage_error(BOOTSTAGE_ID_NEED_RESET);
	if (IS_ENABLED(CONFIG_SANDBOX))
		os_exit(1);
	for (;;)
		;
}
