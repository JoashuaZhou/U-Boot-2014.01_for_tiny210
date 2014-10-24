/*
 *  Copyright (C) 2008-2009 Samsung Electronics
 *  Minkyu Kang <mk7.kang@samsung.com>
 *  Kyungmin Park <kyungmin.park@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/sromc.h>
#include <asm/arch/gpio.h>
#include <netdev.h>

DECLARE_GLOBAL_DATA_PTR;

/*
static void dm9000_pre_init(void)
{
unsigned int tmp;
        SROM_BW_REG &= ~(0xf << 4); 
        SROM_BW_REG |= (0x1 << 4); 
        SROM_BC1_REG = ((0<<28)|(0<<24)|(5<<16)|(0<<12)|(0<<8)|(0<<4)|(0<<0));

        tmp = MP01CON_REG;
        tmp &=~(0xf<<4);                                                                tmp |=(2<<4);
        MP01CON_REG = tmp;
}
*/

int board_init(void)
{
//	smc9115_pre_init(); 

	gd->bd->bi_arch_number = MACH_TYPE_TINY210; // Joshua
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);

	return 0;
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
}

/* Joshua  */
#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	printf("Board:\tTiny210\n");
	return 0;
}
#endif

int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_SMC911X
	rc = smc911x_initialize(0, CONFIG_SMC911X_BASE);
#endif
/* Joshua */
#ifdef CONFIG_DRIVER_DM9000
	rc = dm9000_initialize(bis);
#endif
	return rc;
}
