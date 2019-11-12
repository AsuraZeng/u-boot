/*
 * Library to support early IOT2050 EEPROM handling
 * (C) Copyright 2019 Siemens AG
 *   Le Jin <le.jin@siemens.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __IOT2050_BOARD_DETECT_H
#define __IOT2050_BOARD_DETECT_H

/* EEPROM MAGIC Header identifier */
#define IOT2050_EEPROM_HEADER_MAGIC		0xC0DEC0DE

/* IOT2050 EEPROM board info structure */
#define IOT2050_EEPROM_HDR_NAME_LEN				20
#define IOT2050_EEPROM_HDR_SERIAL_LEN			16
#define IOT2050_EEPROM_HDR_MLFB_LEN				18
#define IOT2050_EEPROM_HDR_UUID_LEN				32
#define IOT2050_EEPROM_HDR_A5E_LEN				18


/* IOT2050 EEPROM MAC info structure */
#define IOT2050_EEPROM_MAC_ADDR_LEN				6
#define IOT2050_EEPROM_HDR_NO_OF_MAC_ADDR		8

struct iot2050_eeprom {
	u32 header;
	char name[IOT2050_EEPROM_HDR_NAME_LEN + 1];
	char serial[IOT2050_EEPROM_HDR_SERIAL_LEN + 1];
	char mlfb[IOT2050_EEPROM_HDR_MLFB_LEN + 1];
	char uuid[IOT2050_EEPROM_HDR_UUID_LEN + 1];
	char a5e[IOT2050_EEPROM_HDR_A5E_LEN + 1];
	u8 mac_addr_cnt;
	char mac_addr[IOT2050_EEPROM_HDR_NO_OF_MAC_ADDR][IOT2050_EEPROM_MAC_ADDR_LEN];
};

/* Use scratch SRAM even before DDR is initialzed */
#define IOT2050_EEPROM_DATA ((struct iot2050_eeprom *) \
				TI_SRAM_SCRATCH_BOARD_EEPROM_START)


/**
 * board_is() - Board detection logic for IOT2050 boards
 * @name_tag:	Tag used in eeprom for the board
 *
 * Return: false if board information does not match OR eeprom wasn't read.
 *	   true otherwise
 */
bool board_is(char *name_tag);

/**
 * board_iot2050_get_name() - Get board name for IOT2050
 *
 * Return: Empty string if eeprom wasn't read.
 *	   Board name otherwise
 */
char *board_iot2050_get_name(void);

/**
 * board_iot2050_get_eth_mac_addr() - Get Ethernet MAC address from EEPROM MAC list
 * @index:	0 based index within the list of MAC addresses
 * @mac_addr:	MAC address contained at the index is returned here
 *
 * Does not sanity check the mac_addr. Whatever is stored in EEPROM is returned.
 */
void board_iot2050_get_eth_mac_addr(int index, u8 mac_addr[IOT2050_EEPROM_MAC_ADDR_LEN]);

/**
 * set_board_info_env_iot2050() - Setup commonly used board information environment
 *			      vars for iot2050 boards
 * @name:	Name of the board
 *
 * If name is NULL, default_name is used.
 */
void set_board_info_env_iot2050(void);

/**
 * board_iot2050_set_ethaddr- Sets the ethaddr environment from EEPROM
 * @index: The first eth<index>addr environment variable to set
 * @count: The number of MAC addresses to process
 *
 * EEPROM should be already read before calling this function. The EEPROM
 * contains n dedicated MAC addresses. This function sets the ethaddr
 * environment variable for all the available MAC addresses starting
 * from eth<index>addr.
 */
void board_iot2050_set_ethaddr(int index, int count);

/**
 * board_iot2050_was_eeprom_read() - Check to see if the eeprom contents have been read
 *
 * This function is useful to determine if the eeprom has already been read and
 * its contents have already been loaded into memory. It utiltzes the magic
 * number that the header value is set to upon successful eeprom read.
 */
bool board_iot2050_was_eeprom_read(void);

#endif	/* __IOT2050_BOARD_DETECT_H */
