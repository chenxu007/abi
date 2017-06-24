/* bht_L0_plx_vpd.c */

/*
 * Copyright (c) 2017 Bin Hong Tech, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01a,17may17,cx_  add file
*/

#include <bht_L0.h>
#include <bht_L1.h>
#include <bht_L1_plx9056.h>
#include <bht_L1_a429.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEVID BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PMCA429 | BHT_DEVID_BOARDNUM_01 
#define EEPROM_SIZE 256
#define EEDO BIT31
#define F_bit BIT15


int main56 (void)
{
    int fd;
    int result;
	bht_L0_u32 value, idx;
    bht_L0_u16 value16;
    bht_L0_u32 addr = 0;    
    char buffer[100];
    char * filename = "eeprom-value.txt";

    if(0 > (fd = open(filename, O_RDWR | O_CREAT)))
        goto plx_vpd_err;
    
    if(BHT_SUCCESS != (result = bht_L0_map_memory(DEVID, NULL)))
        goto plx_vpd_err;

    /* plx9056 init */
    /* check 9056 initiate stat */
    if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(DEVID, PLX9056_LMISC1, &value, 1)))
        goto plx_vpd_err;
    
    if((value & BIT2) == 0)
        goto plx_vpd_err;
    
    /* enable memory space */
    if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(DEVID, PLX9056_PCICR, &value, 1)))
        goto plx_vpd_err;
    
    value |= BIT1;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(DEVID, PLX9056_PCICR, &value, 1)))
        goto plx_vpd_err;

    /* config INTCSR */
    value = BIT8 | BIT11 | BIT16 | BIT18 | BIT19;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(DEVID, PLX9056_INTCSR, &value, 1)))
        goto plx_vpd_err;
    
    /* command 0xc-read mult line; 0xe- read line; 0x6- read */
    value = 0xd767c;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(DEVID, PLX9056_CNTRL, &value, 1)))
        goto plx_vpd_err;

    /* read read the entire eeprom data */
    for(addr = 0x60; addr < EEPROM_SIZE; addr += 4)
    {
        /* 1.disable eedo input (CNTRL[31] = 0) */
        if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(DEVID, PLX9056_CNTRL, &value, 1)))
            goto plx_vpd_err;
        if(value & EEDO)
        {
            value &= (~EEDO);
            if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(DEVID, PLX9056_CNTRL, &value, 1)))
                goto plx_vpd_err;
        }
        
        /* 2.write the serial eeprom destination address in the PVPDAD register, 
           and the F bit to 0(PVPDAD[15] = 0). PVPDAD[0] must be 0 (address is word-aligned) */
//        if(BHT_SUCCESS != (result = bht_L0_read_setupmem16(DEVID, 0x4E, &value16, 1)))
//            goto plx_vpd_err;

        value16 = (addr) & (~F_bit);
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem16(DEVID, 0x4E, &value16, 1)))
            goto plx_vpd_err;
        /* 3.poll the F bit until it changes to 1(PVPDAD[15]=1) to ensure that the Read data is available */
        do
        {
            if(BHT_SUCCESS != (result = bht_L0_read_setupmem16(DEVID, 0x4E, &value16, 1)))
                goto plx_vpd_err;
        }while(!(value16 & F_bit));
        
        /* 4.Read back the PVPDATA register to obtain the requested data */
        if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(DEVID, PLX9056_PVPDATA, &value, 1)))
            goto plx_vpd_err;
        
        sprintf(buffer, "%08x\n", value);

        write(fd, buffer, strlen(buffer));

        printf(buffer);
    }
    
plx_vpd_err:

    if(fd > 0)
    {
        close(fd);
    }

    bht_L0_unmap_memory(DEVID);
    
	system("pause");
    
	return 0;
}



