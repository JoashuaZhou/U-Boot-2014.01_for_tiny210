/*
 * (C) Copyright 2006 OpenMoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

#include <nand.h>
// #include <asm/arch/s3c24x0_cpu.h> Joshua
#include <asm/io.h>

#define S3C2410_NFCONF_EN          (1<<15)
#define S3C2410_NFCONF_512BYTE     (1<<14)
#define S3C2410_NFCONF_4STEP       (1<<13)
#define S3C2410_NFCONF_INITECC     (1<<12)
#define S3C2410_NFCONF_nFCE        (1<<11)
#define S3C2410_NFCONF_TACLS(x)    ((x)<<8)
#define S3C2410_NFCONF_TWRPH0(x)   ((x)<<4)
#define S3C2410_NFCONF_TWRPH1(x)   ((x)<<0)

#define S3C2410_ADDR_NALE 4
#define S3C2410_ADDR_NCLE 8

#ifdef CONFIG_NAND_SPL

/* in the early stage of NAND flash booting, printf() is not available */
#define printf(fmt, args...)

static void nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	int i;
	struct nand_chip *this = mtd->priv;

	for (i = 0; i < len; i++)
		buf[i] = readb(this->IO_ADDR_R);
}
#endif

static void s3c2410_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
#if 0
	struct nand_chip *chip = mtd->priv;
	struct s3c2410_nand *nand = s3c2410_get_base_nand();

	debug("hwcontrol(): 0x%02x 0x%02x\n", cmd, ctrl);

	if (ctrl & NAND_CTRL_CHANGE) {
		ulong IO_ADDR_W = (ulong)nand;

		if (!(ctrl & NAND_CLE))
			IO_ADDR_W |= S3C2410_ADDR_NCLE;
		if (!(ctrl & NAND_ALE))
			IO_ADDR_W |= S3C2410_ADDR_NALE;

		chip->IO_ADDR_W = (void *)IO_ADDR_W;

		if (ctrl & NAND_NCE)
			writel(readl(&nand->nfconf) & ~S3C2410_NFCONF_nFCE,
			       &nand->nfconf);
		else
			writel(readl(&nand->nfconf) | S3C2410_NFCONF_nFCE,
			       &nand->nfconf);
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, chip->IO_ADDR_W);
#endif
#define NFCMMD   (*(volatile unsigned char *)0xB0E00008)  
#define NFADDR   (*(volatile unsigned char *)0xB0E0000C)  
  
	if (cmd != NAND_CMD_NONE) {  
		if (ctrl & NAND_CLE) {  
			NFCMMD = cmd;  
            		//printf ("cmd: %x\n", cmd);  
            		//writeb(cmd, NFCMMD);  
        	}  
        	else if (ctrl & NAND_ALE) {  
			//writeb(cmd, NFADDR);  
            		NFADDR = cmd;  
            		//printf ("addr: %x\n", cmd);  
        	}  
    	}	  
  
}  

static int s3c2410_dev_ready(struct mtd_info *mtd)
{
#if 0
	struct s3c2410_nand *nand = s3c2410_get_base_nand();
	debug("dev_ready\n");
	return readl(&nand->nfstat) & 0x01;
#endif

	/* Joshua */
	#define NFSTAT   (*(volatile unsigned int *)0xB0E00028)

        while ( (NFSTAT&0x1) == 0 );

        return 1;

}

#ifdef CONFIG_S3C2410_NAND_HWECC
void s3c2410_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	struct s3c2410_nand *nand = s3c2410_get_base_nand();
	debug("s3c2410_nand_enable_hwecc(%p, %d)\n", mtd, mode);
	writel(readl(&nand->nfconf) | S3C2410_NFCONF_INITECC, &nand->nfconf);
}

static int s3c2410_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat,
				      u_char *ecc_code)
{
	struct s3c2410_nand *nand = s3c2410_get_base_nand();
	ecc_code[0] = readb(&nand->nfecc);
	ecc_code[1] = readb(&nand->nfecc + 1);
	ecc_code[2] = readb(&nand->nfecc + 2);
	debug("s3c2410_nand_calculate_hwecc(%p,): 0x%02x 0x%02x 0x%02x\n",
	       mtd , ecc_code[0], ecc_code[1], ecc_code[2]);

	return 0;
}

static int s3c2410_nand_correct_data(struct mtd_info *mtd, u_char *dat,
				     u_char *read_ecc, u_char *calc_ecc)
{
	if (read_ecc[0] == calc_ecc[0] &&
	    read_ecc[1] == calc_ecc[1] &&
	    read_ecc[2] == calc_ecc[2])
		return 0;

	printf("s3c2410_nand_correct_data: not implemented\n");
	return -1;
}
#endif

static void s5pv210_select_chip(struct mtd_info *mtd, int chipnr)  
{  
        #define NFCONT   (*(volatile unsigned int *)0xB0E00004)  
          
        struct nand_chip *chip = mtd->priv;  
      
        switch (chipnr) {  
        case -1:  
            //chip->cmd_ctrl(mtd, NAND_CMD_NONE, 0 | NAND_CTRL_CHANGE);  
            NFCONT |= (1<<1);  
            break;  
        case 0:  
            NFCONT &= ~(1<<1);  
            break;  
      
        default:  
            BUG();  
        }  
}  

int board_nand_init(struct nand_chip *nand)
{
#if 0
	u_int32_t cfg;
	u_int8_t tacls, twrph0, twrph1;
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();
	struct s3c2410_nand *nand_reg = s3c2410_get_base_nand();

	debug("board_nand_init()\n");

	writel(readl(&clk_power->clkcon) | (1 << 4), &clk_power->clkcon);

	/* initialize hardware */
#if defined(CONFIG_S3C24XX_CUSTOM_NAND_TIMING)
	tacls  = CONFIG_S3C24XX_TACLS;
	twrph0 = CONFIG_S3C24XX_TWRPH0;
	twrph1 =  CONFIG_S3C24XX_TWRPH1;
#else
	tacls = 4;
	twrph0 = 8;
	twrph1 = 8;
#endif

	cfg = S3C2410_NFCONF_EN;
	cfg |= S3C2410_NFCONF_TACLS(tacls - 1);
	cfg |= S3C2410_NFCONF_TWRPH0(twrph0 - 1);
	cfg |= S3C2410_NFCONF_TWRPH1(twrph1 - 1);
	writel(cfg, &nand_reg->nfconf);
#endif

	#define MP0_1CON (*(volatile unsigned int *)0xE02002E0)  
    	#define MP0_3CON (*(volatile unsigned int *)0xE0200320)  
    	#define NFCONF   (*(volatile unsigned int *)0xB0E00000)  
    	#define NFCONT   (*(volatile unsigned int *)0xB0E00004)  
    	#define NFCMMD   (*(volatile unsigned char *)0xB0E00008)  
	#define NFADDR   (*(volatile unsigned char *)0xB0E0000C)  
    	#define NFDATA   (*(volatile unsigned char *)0xB0E00010)  
	#define NFSTAT   (*(volatile unsigned int *)0xB0E00028)  
  
	#define NFCONF_VAL ((7<<12)|(7<<8)|(7<<4)|(0<<3)|(0<<2)|(1<<1))  
	#define NFCONT_VAL ((1<<23)|(1<<22)|(1<<2)|(0<<1)|(1<<0))  

	unsigned int tmp;  
  
    	//tmp = MP0_1CON;  
   	//tmp &= ~((0xf<<8)|(0xf<<12));  
   	//tmp |= (0x3<<8) | (0x3<<12);  
   	//MP0_1CON = tmp;  
   	tmp = MP0_1CON;  
    	tmp &= ~(0xf<<16);  
   	tmp |= (0x3<<16);  
	MP0_1CON = tmp;  
  
    	tmp = MP0_3CON;  
   	//tmp &= ~((0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20));  
    	//tmp |= (0x2<<0)|(0x2<<4)|(0x2<<8)|(0x2<<12)|(0x2<<16)|(0x2<<20);  
    	tmp &= ~((0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16));  
   	tmp |= (0x2<<0)|(0x2<<4)|(0x2<<8)|(0x2<<12)|(0x2<<16);  
	MP0_3CON = tmp;  
  
    	NFCONF = NFCONF_VAL;  
   	NFCONT = NFCONT_VAL;  

	/* initialize nand_chip data structure */
	nand->IO_ADDR_R = (volatile unsigned char *)0xB0E00010;
	nand->IO_ADDR_W = (volatile unsigned char *)0xB0E00010;

	nand->select_chip = s5pv210_select_chip;

	/* read_buf and write_buf are default */
	/* read_byte and write_byte are default */
#ifdef CONFIG_NAND_SPL
	nand->read_buf = nand_read_buf;
#endif

	/* hwcontrol always must be implemented */
	nand->cmd_ctrl = s3c2410_hwcontrol;

	nand->dev_ready = s3c2410_dev_ready;

#ifdef CONFIG_S3C2410_NAND_HWECC
	nand->ecc.hwctl = s3c2410_nand_enable_hwecc;
	nand->ecc.calculate = s3c2410_nand_calculate_ecc;
	nand->ecc.correct = s3c2410_nand_correct_data;
	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.size = CONFIG_SYS_NAND_ECCSIZE;
	nand->ecc.bytes = CONFIG_SYS_NAND_ECCBYTES;
	nand->ecc.strength = 1;
#else
	nand->ecc.mode = NAND_ECC_SOFT;
#endif

#ifdef CONFIG_S3C2410_NAND_BBT
	nand->bbt_options |= NAND_BBT_USE_FLASH;
#endif

	debug("end of nand_init\n");

	return 0;
}
