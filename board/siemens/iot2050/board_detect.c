/*
 * Library to support early IOT2050 EEPROM handling
 * (C) Copyright 2019 Siemens AG
 *   Le Jin <le.jin@siemens.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/hardware.h>
#include <asm/omap_common.h>
#include <dm/uclass.h>
#include <i2c.h>
// #include <environment.h>
#include <version.h>

#include "board_detect.h"

bool __maybe_unused board_is(char *name_tag)
{
	struct iot2050_eeprom *ep = IOT2050_EEPROM_DATA;

	if (ep->header != IOT2050_EEPROM_HEADER_MAGIC)
		return false;
	return !strncmp(ep->name, name_tag, IOT2050_EEPROM_HDR_NAME_LEN);
}

char * __maybe_unused board_iot2050_get_name(void)
{
	struct iot2050_eeprom *ep = IOT2050_EEPROM_DATA;

	if (ep->header != IOT2050_EEPROM_HEADER_MAGIC)
		return NULL;
	return ep->name;
}

void __maybe_unused
board_iot2050_get_eth_mac_addr(int index,
			      u8 mac_addr[IOT2050_EEPROM_MAC_ADDR_LEN])
{
	struct iot2050_eeprom *ep = IOT2050_EEPROM_DATA;

	if (ep->header != IOT2050_EEPROM_HEADER_MAGIC)
		goto fail;

	if (index < 0 || index >= ep->mac_addr_cnt)
		goto fail;

	memcpy(mac_addr, ep->mac_addr[index], IOT2050_EEPROM_MAC_ADDR_LEN);
	return;
fail:
	memset(mac_addr, 0, IOT2050_EEPROM_MAC_ADDR_LEN);
}

void __maybe_unused set_board_info_env_iot2050(void)
{
	struct iot2050_eeprom *ep = IOT2050_EEPROM_DATA;
	u8 need_update = 0;
	u8 mac_cnt;
	char env_name[16];
	char mac_str[32];

	/* name */
	if(strcmp(env_get("board_name"), ep->name))
	{
		env_set("board_name", ep->name);
		need_update = 1;
	}
	if(strcmp(env_get("board"), ep->name))
	{
		env_set("board", ep->name);
		need_update = 1;
	}
	/* serial */
	if(strcmp(env_get("board_serial"), ep->serial))
	{
		env_set("board_serial", ep->serial);
		need_update = 1;
	}
	/* mlfb */
	if(strcmp(env_get("mlfb"), ep->mlfb))
	{
		env_set("mlfb", ep->mlfb);
		need_update = 1;
	}
	/* uuid */
	if(strcmp(env_get("board_uuid"), ep->uuid))
	{
		env_set("board_uuid", ep->uuid);
		need_update = 1;
	}
	/* a5e */
	if(strcmp(env_get("board_a5e"), ep->a5e))
	{
		env_set("board_a5e", ep->a5e);
		need_update = 1;
	}
	/* fw version */
	if(strcmp(env_get("fw_version"), PLAIN_VERSION))
	{
		env_set("fw_version", PLAIN_VERSION);
		need_update = 1;
	}
	/* MAC address */
	for(mac_cnt=0; mac_cnt<ep->mac_addr_cnt; mac_cnt++)
	{
		snprintf(env_name, 16, "eth%daddr", mac_cnt+1);
		char *mac = env_get(env_name);
		snprintf(mac_str, 32, "%02x:%02x:%02x:%02x:%02x:%02x",
								ep->mac_addr[mac_cnt][0],
								ep->mac_addr[mac_cnt][1],
								ep->mac_addr[mac_cnt][2],
								ep->mac_addr[mac_cnt][3],
								ep->mac_addr[mac_cnt][4],
								ep->mac_addr[mac_cnt][5]);
		if((mac == NULL) || strcmp(mac, mac_str))
		{
			env_set(env_name, mac_str);
			need_update = 1;

			/* Set the MAC address environment variable that ICSSG0-PRU eth0 will use in u-boot */
			if(0 == mac_cnt)
			{
				env_set("ethaddr", mac_str);
			}
		}
	}
	if(need_update)
	{
		env_save();
	}
}

void board_iot2050_set_ethaddr(int index, int count)
{
	u8 mac_addr[6];
	int i;

	for (i = 0; i < count; i++) {
		board_iot2050_get_eth_mac_addr(i, mac_addr);
		if (is_valid_ethaddr(mac_addr))
			eth_env_set_enetaddr_by_index("eth", i + index,
						      mac_addr);
	}
}

bool __maybe_unused board_iot2050_was_eeprom_read(void)
{
	struct iot2050_eeprom *ep = IOT2050_EEPROM_DATA;

	if (ep->header == IOT2050_EEPROM_HEADER_MAGIC)
		return true;
	else
		return false;
}
