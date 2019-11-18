// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

/* #define	DEBUG	*/

#include <common.h>
#include <autoboot.h>
#include <cli.h>
#include <command.h>
#include <console.h>
#include <env.h>
#include <version.h>
#include <asm/io.h>
#include <dm/uclass.h>
#include <led.h>
/*
 * Board-specific Platform code can reimplement show_boot_progress () if needed
 */
__weak void show_boot_progress(int val) {}

static void run_preboot_environment_command(void)
{
	char *p;

	p = env_get("preboot");
	if (p != NULL) {
		int prev = 0;

		if (IS_ENABLED(CONFIG_AUTOBOOT_KEYED))
			prev = disable_ctrlc(1); /* disable Ctrl-C checking */

		run_command_list(p, -1, 0);

		if (IS_ENABLED(CONFIG_AUTOBOOT_KEYED))
			disable_ctrlc(prev);	/* restore Ctrl-C checking */
	}
}

#if defined(CONFIG_TARGET_IOT2050_ADVANCED_A53) || defined(CONFIG_TARGET_IOT2050_BASIC_A53)
static void iot2050_show_set_light(const char *label,enum led_state_t state)
{
	struct udevice *led_dev;
	int error_status;
	error_status = led_get_by_label(label,&led_dev);
	if(!error_status)
	{
		led_set_state(led_dev, state);
	}
}
#endif

/* We come here after U-Boot is initialised and ready to process commands */
void main_loop(void)
{
	const char *s;

	bootstage_mark_name(BOOTSTAGE_ID_MAIN_LOOP, "main_loop");

	if (IS_ENABLED(CONFIG_VERSION_VARIABLE))
		env_set("ver", version_string);  /* set version variable */

	cli_init();

	if (IS_ENABLED(CONFIG_USE_PREBOOT))
		run_preboot_environment_command();

	if (IS_ENABLED(CONFIG_UPDATE_TFTP))
		update_tftp(0UL, NULL, NULL);

	s = bootdelay_process();
	if (cli_process_fdt(&s))
		cli_secure_boot_cmd(s);

	autoboot_command(s);

#if defined(CONFIG_TARGET_IOT2050_ADVANCED_A53) || defined(CONFIG_TARGET_IOT2050_BASIC_A53)
	/* Turn on the LED when OS is not started  */
	iot2050_show_set_light("redled",LEDST_ON);
	iot2050_show_set_light("greenled",LEDST_OFF);
#endif
	cli_loop();
	panic("No CLI available");
}
