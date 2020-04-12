/*
 *  DN1004S board support
 *
 *  Copyright (C) 2011 dongyuqi <729650915@qq.com>
 *  Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2013 alzhao <alzhao@gmail.com>
 *  Copyright (C) 2014 Michel Stempin <michel.stempin@wanadoo.fr>
 *  Copyright (C) 2020 Jaume Clarens <jclarens@engel.es>
 * 
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
*/

#include <linux/gpio.h>

#include <asm/mach-ath79/ath79.h>

#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define DN1004S_GPIO_LED_WLAN		   1
#define DN1004S_GPIO_LED_LAN		   16
#define DN1004S_GPIO_LED_WAN		   27 

#define DN1004S_GPIO_BIN_USB         6
#define DN1004S_GPIO_ETH_RESET         7
#define DN1004S_GPIO_ETH_PWR         8
#define DN1004S_GPIO_BTN_RESET	   11

#define DN1004S_KEYS_POLL_INTERVAL   20	/* msecs */
#define DN1004S_KEYS_DEBOUNCE_INTERVAL	(3 * DN1004S_KEYS_POLL_INTERVAL)

#define DN1004S_MAC0_OFFSET	0x0000
#define DN1004S_MAC1_OFFSET	0x0000
#define DN1004S_CALDATA_OFFSET	0x1000
#define DN1004S_WMAC_MAC_OFFSET	0x0000

static struct gpio_led dn1004s_leds_gpio[] __initdata = {
	{
		.name = "dn1004s:orange:wlan",
		.gpio = DN1004S_GPIO_LED_WLAN,
		.active_low = 0,
	},
	{
		.name = "dn1004s:green:lan",
		.gpio = DN1004S_GPIO_LED_LAN,
		.active_low = 0,
	},
	{
		.name = "dn1004s:green:wan",
		.gpio = DN1004S_GPIO_LED_WAN,
		.active_low = 0,
 		.default_state = 1,
	},
	{
		.name = "dn1004s:usbpow",
		.gpio = DN1004S_GPIO_BIN_USB,
		.active_low = 1,
 		.default_state = 1,
	},
	{
		.name = "dn1004s:ethpow",
		.gpio = DN1004S_GPIO_ETH_PWR,
		.active_low = 1,
 		.default_state = 1,
	},
	{
		.name = "dn1004s:ethrst",
		.gpio = DN1004S_GPIO_ETH_RESET,
		.active_low = 0,
 		.default_state = 0,
	},
};

static struct gpio_keys_button dn1004s_gpio_keys[] __initdata = {
	{
		.desc = "reset",
		.type = EV_KEY,
		.code = KEY_RESTART,
		.debounce_interval = DN1004S_KEYS_DEBOUNCE_INTERVAL,
		.gpio = DN1004S_GPIO_BTN_RESET,
		.active_low = 0,
	},
};

static void __init dn1004s_setup(void)
{

	/* ART base address */
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	/* disable PHY_SWAP and PHY_ADDR_SWAP bits */
	ath79_setup_ar933x_phy4_switch(false, false);

	/* register flash. */
	ath79_register_m25p80(NULL);

	/* register gpio LEDs and keys */
	ath79_register_leds_gpio(-1, ARRAY_SIZE(dn1004s_leds_gpio),
				 dn1004s_leds_gpio);
	ath79_register_gpio_keys_polled(-1, DN1004S_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(dn1004s_gpio_keys),
					dn1004s_gpio_keys);

#if 0 //JCB
	/* enable usb */
	gpio_request_one(DN1004S_GPIO_BIN_USB,
				 GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT_DIR_FIXED,
	 			 "USB power");
#endif
	ath79_register_usb();
	
	/* register eth0 as WAN, eth1 as LAN */
	ath79_init_mac(ath79_eth0_data.mac_addr, art+DN1004S_MAC0_OFFSET, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, art+DN1004S_MAC1_OFFSET, 0);
	ath79_register_mdio(0, 0x0);
	ath79_register_eth(0);
	ath79_register_eth(1);

	/* register wireless mac with cal data */
	ath79_register_wmac(art + DN1004S_CALDATA_OFFSET, art + DN1004S_WMAC_MAC_OFFSET);
}

MIPS_MACHINE(ATH79_MACH_DN1004S, "DN1004S", "EngelCast DN1004S", dn1004s_setup);
