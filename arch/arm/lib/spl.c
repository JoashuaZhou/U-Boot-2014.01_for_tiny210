/*
 * (C) Copyright 2010-2012
 * Texas Instruments, <www.ti.com>
 *
 * Aneesh V <aneesh@ti.com>
 * Tom Rini <trini@ti.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <config.h>
#include <spl.h>
#include <image.h>
#include <linux/compiler.h>

/* Pointer to as well as the global data structure for SPL */
DECLARE_GLOBAL_DATA_PTR;
gd_t gdata __attribute__ ((section(".data")));

//UART
#define ULCON0      (*(volatile unsigned int *)0xE2900000)
#define UCON0       (*(volatile unsigned int *)0xE2900004)
#define UTRSTAT0    (*(volatile unsigned int *)0xE2900010)
#define UTXH0       (*(volatile unsigned char *)0xE2900020)
#define URXH0       (*(volatile unsigned char *)0xE2900024)
#define UBRDIV0     (*(volatile unsigned int *)0xE2900028)
#define UDIVSLOT0   (*(volatile unsigned int *)0xE290002C)
//GPA0
#define GPA0CON     (*(volatile unsigned int *)0xE0200000)

typedef u32(*copy_sd_mmc_to_mem)
(u32 channel, u32 start_block, u16 block_size, u32 *trg, u32 init);

void uart_init (void)
{
        GPA0CON = (2<<0) | (2<<4);

        ULCON0 = (0<<6) | (0<<3) | (0<<2) | (3<<0);

        UCON0 = (0<<10) | (1<<2) | (1<<0);

        UBRDIV0 = 34;
        UDIVSLOT0 = 0xDDDD;
}

void putc(char c)
{
	while (!(UTRSTAT0 & (1<<2)));
	UTXH0 = c;
}

void putss(const char *s)
{
	while(*s != '\0')
	{
		putc(*s++);
	}
	putc('\n');
}

void copy_uboot_to_ram(void)
{
        unsigned long ch;

        ch = *(volatile unsigned int *)(0xD0037488);
        copy_sd_mmc_to_mem copy_bl2 =
            (copy_sd_mmc_to_mem) (*(unsigned int *) (0xD0037F98));

        unsigned int ret;

        //printf ("boot mmc chanel: %x\r\n", ch);
        if (ch == 0xEB000000) {
                ret = copy_bl2(0, 49, 1024, (unsigned int *)CONFIG_SYS_TEXT_BASE, 0);
		putc('j');
		putc('u');
		putc('m');
		putc('p');
		putc(' ');
		putc('t');
		putc('o');
		putc(' ');
		putc('R');
		putc('A');
		putc('M');
		putc('\n');
        }

        if (ret == 0) {
                //printf ("copy error\r\n");
                while (1);
        }
        else
                return;
}

/*
 * In the context of SPL, board_init_f must ensure that any clocks/etc for
 * DDR are enabled, ensure that the stack pointer is valid, clear the BSS
 * and call board_init_f.  We provide this version by default but mark it
 * as __weak to allow for platforms to do this in their own way if needed.
 */
void __weak board_init_f(ulong dummy)
{
	__attribute__((noreturn)) void (*uboot)(void);
	
//	uart_init();
	copy_uboot_to_ram();

	uboot = (void *)CONFIG_SYS_TEXT_BASE;
	(*uboot)();
}

/*
 * This function jumps to an image with argument. Normally an FDT or ATAGS
 * image.
 * arg: Pointer to paramter image in RAM
 */
#ifdef CONFIG_SPL_OS_BOOT
void __noreturn jump_to_image_linux(void *arg)
{
	unsigned long machid = 0xffffffff;
#ifdef CONFIG_MACH_TYPE
	machid = CONFIG_MACH_TYPE;
#endif

	debug("Entering kernel arg pointer: 0x%p\n", arg);
	typedef void (*image_entry_arg_t)(int, int, void *)
		__attribute__ ((noreturn));
	image_entry_arg_t image_entry =
		(image_entry_arg_t) spl_image.entry_point;
	cleanup_before_linux();
	image_entry(0, machid, arg);
}
#endif
