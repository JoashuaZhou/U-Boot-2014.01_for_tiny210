/*
 * Copyright (C) 2009 Samsung Electronics
 * Kyungmin Park <kyungmin.park@samsung.com>
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <version.h>
#include <asm/arch/cpu.h>
#include <asm/arch/power.h>

/* SystemClock */
#define APLL_LOCK     0xE0100000  
#define MPLL_LOCK     0xE0100008 
#define EPLL_LOCK     0xE0100010  
#define VPLL_LOCK     0xE0100020  
#define APLL_CON0     0xE0100100  
#define APLL_CON1     0xE0100104  
#define MPLL_CON      0xE0100108  
#define EPLL_CON0     0xE0100110  
#define EPLL_CON1     0xE0100114  
#define VPLL_CON      0xE0100120  
#define CLK_SRC0      0xE0100200  
#define CLK_DIV0      0xE0100300  
  
#define SETPLL(mdiv, pdiv, sdiv) ((1<<31)|(mdiv<<16)|(pdiv<<8)|(sdiv<<0))  

/* APLL_FOUT = 1000MHz  */  
#define APLL_MDIV   125  
#define APLL_PDIV   3  
#define APLL_SDIV   1  
#define APLL_CON0_VAL SETPLL (APLL_MDIV, APLL_PDIV, APLL_SDIV)  

/* MPLL_FOUT = 667MHz  */
#define MPLL_MDIV   667  
#define MPLL_PDIV   12  
#define MPLL_SDIV   1  
#define MPLL_CON_VAL SETPLL (MPLL_MDIV, MPLL_PDIV, MPLL_SDIV)  
  
#define PCLK_PSYS_RATIO   1  
#define HCLK_PSYS_RATIO   4   
#define PCLK_DSYS_RATIO   1  
#define HCLK_DSYS_RATIO   3  
#define PCLK_MSYS_RATIO   1  
#define HCLK_MSYS_RATIO   4  
#define A2M_RATIO         4  
#define APLL_RATIO        0  
#define CLK_DIV0_VAL ( (APLL_RATIO<<0)|(A2M_RATIO<<4)|(HCLK_MSYS_RATIO<<8)|(PCLK_MSYS_RATIO<<12)|(HCLK_DSYS_RATIO<<16)|(PCLK_DSYS_RATIO<<20)|(HCLK_PSYS_RATIO<<24)|(PCLK_PSYS_RATIO<<28) )  

/* UART */
#define ULCON0      0xE2900000
#define UFCON0      0xE2900008
#define UMCON0      0xE290000C
#define UCON0       0xE2900004
#define UTRSTAT0    0xE2900010
#define UTXH0       0xE2900020
#define URXH0       0xE2900024
#define UBRDIV0     0xE2900028
#define UDIVSLOT0   0xE290002C

//GPA0CON & GPA1CON
#define GPA0CON     0xE0200000
#define GPA1CON     0xE0200020

/* DM9000 netcard */
#define SROM_BW     0xE8000000
#define SROM_BC1    0xE8000008
#define MP0_1CON    0xE02002E0

/*
 * Register usages:
 *
 * r5 has zero always
 */

_TEXT_BASE:
	.word	CONFIG_SYS_TEXT_BASE

	.globl lowlevel_init
lowlevel_init:
	mov	r9, lr

#ifdef	CONFIG_SPL_BUILD
	/* memory initialization */
	bl mem_ctrl_asm_init

	/* initialize SROM controller bank1 for DM9000 */
	bl srom_ctrl_asm_init

	/* 1000MHz */
	/*bl system_clock_init */
	
	/* for UART */
	bl uart_asm_init

#endif
#if 0
	/* for TZPC */
	bl tzpc_asm_init
#endif
1:
	mov	lr, r9
	mov	pc, lr

/*
 * system_clock_init: Initialize core clock and bus clock.
 * void system_clock_init(void)
 */
system_clock_init:

	ldr	r0, =APLL_CON0
	ldr	r1, =APLL_CON0_VAL
	str	r1, [r0]
	
	ldr	r0, =MPLL_CON
	ldr	r1, =MPLL_CON_VAL
	str	r1, [r0]

	ldr	r0, =CLK_SRC0
	ldr	r1, =0x1111
	str	r1, [r0]	

	ldr	r0, =CLK_DIV0
	ldr	r1, =CLK_DIV0_VAL
	str	r1, [r0]

	mov	pc, lr

/*
 * uart_asm_init: Initialize UART's pins
 */
uart_asm_init:
	ldr	r0, =GPA0CON
	ldr	r1, =0x22222222
	str	r1, [r0]			@ GPA0_CON
	ldr	r0, =GPA1CON
	ldr	r1, =0x2222
	str	r1, [r0]			@ GPA1_CON

	ldr	r0, =UFCON0
	ldr	r1, =0x1
	str	r1, [r0]
	ldr	r0, =UMCON0
	ldr	r1, =0x0
	str	r1, [r0]
	ldr	r0, =ULCON0
	ldr	r1, =0x3
	str	r1, [r0]
	ldr	r0, =UCON0
	ldr	r1, =0x5
	str	r1, [r0]

	ldr	r0, =UBRDIV0
	ldr	r1, =35
	str	r1, [r0]

	ldr	r0, =UDIVSLOT0
	ldr	r1, =0x1
	str	r1, [r0]

	mov	pc, lr

/*
 * tzpc_asm_init: Initialize TZPC
 */
tzpc_asm_init:
	ldr	r0, =0xE3800000
	mov	r1, #0x0
	str	r1, [r0]
	mov	r1, #0xff
	str	r1, [r0, #0x804]
	str	r1, [r0, #0x810]

	ldr	r0, =0xE2800000
	str	r1, [r0, #0x804]
	str	r1, [r0, #0x810]
	str	r1, [r0, #0x81C]

	ldr	r0, =0xE2900000
	str	r1, [r0, #0x804]
	str	r1, [r0, #0x810]

	mov	pc, lr

srom_ctrl_asm_init:

	/* configure MP0_1 as SROM chip select mode */
#if 0
	ldr	r0, =MP0_1CON
	ldr	r1, =(0x22553322)
#endif

	/* Bus width setting
	 * 16-bit, SROM_ADDR is half-word base address, disables wait, not using UB/LB is dedicated nWBE
	 */
	ldr	r0, =SROM_BW
	mov	r1, #0x39
	str	r1, [r0]

	/* Bank1 setting */
	ldr	r0, =SROM_BC1
	ldr	r1, =(0x1051000)
	str	r1, [r0]

	mov	pc, lr
