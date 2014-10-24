/*
 * (C) Copyright 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * HeungJun Kim <riverful.kim@samsung.com>
 * Inki Dae <inki.dae@samsung.com>
 *
 * Configuation settings for the SAMSUNG SMDKC100 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* Joshua */
#define MACH_TYPE_TINY210	3466

/* Joshua  */
#define CONFIG_SPL
#define CONFIG_SPL_FRAMEWORK

/* Joshua  */
/* #define CONFIG_GENERIC_MMC */

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_SAMSUNG		1	/* in a SAMSUNG core */
#define CONFIG_S5P		1	/* which is in a S5P Family */
#define CONFIG_S5PC100		1	/* which is in a S5PC100 */
#define CONFIG_SMDKC100		1	/* working with SMDKC100 */

#include <asm/arch/cpu.h>		/* get chip and board defs */

#define CONFIG_ARCH_CPU_INIT

#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* input clock of PLL: SMDKC100 has 12MHz input clock */
#define CONFIG_SYS_CLK_FREQ		24000000

/* DRAM Base, Joshua */
#define CONFIG_SYS_SDRAM_BASE		0x20000000

/* Text Base, Joshua */
#define CONFIG_SYS_TEXT_BASE		0x3FF70000 /* U-boot size < 576KB  */

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_EDITING

/*
 * Size of malloc() pool
 * 1MB = 0x100000, 0x100000 = 1024 * 1024
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (1 << 20))
/*
 * select serial console configuration
 */
#define CONFIG_SERIAL0			1	/* use SERIAL 0 on SMDKC100 */

/* PWM */
#define CONFIG_PWM			1

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_BAUDRATE			115200

/***********************************************************
 * Command definition
 ***********************************************************/
#include <config_cmd_default.h>

#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS
/* #undef CONFIG_CMD_NAND Joshua */
#define CONFIG_CMD_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE	0xB0E00000
#define CONFIG_NAND_S3C2410


#define CONFIG_CMD_CACHE
#define CONFIG_CMD_REGINFO
/* #define CONFIG_CMD_ONENAND Joshua */
#define CONFIG_CMD_ELF
#define CONFIG_CMD_FAT
#define CONFIG_CMD_MTDPARTS

#define CONFIG_BOOTDELAY	3

#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS

#define MTDIDS_DEFAULT		"nand0=tiny210-nand"	/* Joshua  */
#define MTDPARTS_DEFAULT	"mtdparts=tiny210-nand:512k(bootloader)"\
				",5m(recovery)"\
				",5m(kernel)"\
				",3m(ramdisk)"\
				",-(root)"

#define NORMAL_MTDPARTS_DEFAULT MTDPARTS_DEFAULT

#define CONFIG_BOOTCOMMAND      "tftp 20007FC0 uImage; bootm 20007FC0"

#define CONFIG_BOOTARGS "root=/dev/mtdblock4 console=ttySAC0,115200 init=/linuxrc"

#define CONFIG_ENV_OVERWRITE

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser	*/
#define CONFIG_SYS_PROMPT		"Tiny210@Joshua # "
#define CONFIG_SYS_CBSIZE	256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	384	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16	/* max number of command args */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 0x5e00000)
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_SDRAM_BASE

/* SMDKC100 has 1 banks of DRAM, we use only one in U-Boot */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		CONFIG_SYS_SDRAM_BASE	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	(512 << 20)	/* 512M on Tiny210 board Joshua */

#define CONFIG_SYS_MONITOR_BASE	0x00000000

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_NO_FLASH		1

#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* 256 KiB */
#define CONFIG_IDENT_STRING		" for Tiny210"	/* Joshua */

#if !defined(CONFIG_NAND_SPL) && (CONFIG_SYS_TEXT_BASE >= 0xc0000000)
#define CONFIG_ENABLE_MMU
#endif

#ifdef CONFIG_ENABLE_MMU
#define CONFIG_SYS_MAPPED_RAM_BASE	0xc0000000
#else
#define CONFIG_SYS_MAPPED_RAM_BASE	CONFIG_SYS_SDRAM_BASE
#endif

/*-----------------------------------------------------------------------
 * Boot configuration
 * Joshua
 */
/* #define CONFIG_ENV_IS_NOWHERE Joshua, old version */
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SIZE			(128 << 10)	/* 128KiB, 0x20000 */
#define CONFIG_ENV_ADDR			(256 << 10)	/* 256KiB, 0x40000 */
#define CONFIG_ENV_OFFSET		(256 << 10)	/* 256KiB, 0x40000 */

/* #define CONFIG_USE_ONENAND_BOARD_INIT
 * #define CONFIG_SAMSUNG_ONENAND		1
 * #define CONFIG_SYS_ONENAND_BASE		0xE7100000
 * Joshua
 */

/* #define CONFIG_DOS_PARTITION		1 Joshua */

/* Joshua  */
#define CONFIG_SYS_INIT_SP_ADDR		0x3F000000

/*
 * Ethernet Contoller driver
 * Joshua
 */
#define CONFIG_CMD_NET
#ifdef CONFIG_CMD_NET
#define CONFIG_DRIVER_DM9000	1       /* we have a SMC9115 on-board   */
#define CONFIG_DM9000_BASE	0x88000000      /* SMC911X Drive Base   */
#define DM9000_IO		CONFIG_DM9000_BASE      /* SMC911X Drive Base   */
#define DM9000_DATA		(CONFIG_DM9000_BASE + 4)
/*#define CONFIG_ENV_SROM_BANK	3        Select SROM Bank-3 for Ethernet*/
#define CONFIG_IPADDR	192.168.1.103
#define CONFIG_ETHADDR	00:0c:29:59:35:94
#define CONFIG_SERVERIP	192.168.1.112
#define CONFIG_CMD_PING

#endif /* CONFIG_CMD_NET */

#endif	/* __CONFIG_H */
