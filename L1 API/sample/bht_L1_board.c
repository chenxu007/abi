/* bht_L1_board.c */

/*
 * Copyright (c) 2017 Bin Hong Tech, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Bin Hong Tech license agreement.
 */

/*
modification history
--------------------
01a,17may17,cx_  add file
*/

#include <bht_L0.h>
#include <bht_L1.h>
#include <bht_L0_plx9056.h>
#include <bht_L1_a429.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>


#define FPGA_ADDR_EEPROM_WRITE         0x4000
#define FPGA_ADDR_EEPROM_READ          0x4004
#define FPGA_ADDR_EEPROM_STATUS        0x4008
#define FPGA_ADDR_EEPROM_DATA          0x400C
#define FPGA_EEPROM_SIZE               0x10000

bht_L0_u32
bht_L1_bd_fpga_eeprom_read(bht_L0_device_t *device,
        bht_L0_u16 addr,
        bht_L0_u8 *data)
{
    bht_L0_u32 result;
    bht_L0_u32 value;

    //wait until not busy
    do
    {
        if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, FPGA_ADDR_EEPROM_STATUS, &value, 1)))
            goto read_end;
    }while(!(value & BIT0));

    //write the address where you want to read
    value = addr << 16;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, FPGA_ADDR_EEPROM_READ, &value, 1)))
        goto read_end;

    //wait until read ok
    do
    {
        if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, FPGA_ADDR_EEPROM_STATUS, &value, 1)))
            goto read_end;
    }while(!(value & BIT0));

    //get the data
    if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, FPGA_ADDR_EEPROM_DATA, &value, 1)))
        goto read_end;
    *data = (bht_L0_u8) (0xFF & value);

read_end:

    return result;
}

bht_L0_u32
bht_L1_bd_fpga_eeprom_write(bht_L0_device_t *device,
        bht_L0_u16 addr,
        bht_L0_u8 data)
{
    bht_L0_u32 result;
    bht_L0_u32 value;

    //wait until not busy
    do
    {
        if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, FPGA_ADDR_EEPROM_STATUS, &value, 1)))
            goto read_end;
    }while(!(value & BIT0));

    //write the address where you want to read
    value = (addr << 16) | data;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, FPGA_ADDR_EEPROM_WRITE, &value, 1)))
        goto read_end;

    //wait until write ok
    do
    {
        if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, FPGA_ADDR_EEPROM_STATUS, &value, 1)))
            goto read_end;
    }while(!(value & BIT0));

read_end:

    return result;
}

bht_L0_u32
bht_L1_bd_fpga_eeprom_test(bht_L0_device_t *device)
{
    bht_L0_u32 result;
    bht_L0_u8 wdata, rdata, temp;
    bht_L0_u32 addr;
    
    srand(time(NULL));
    wdata = (bht_L0_u8)rand();

    printf("test in process ");
    for(addr = 0; addr < FPGA_EEPROM_SIZE; addr++)
    {
        //read temp
        if(BHT_SUCCESS != (result = bht_L1_bd_fpga_eeprom_read(dev_id, addr, &temp)))
        {
            printf("\nfpga eeprom read temp failed\n");
			result = BHT_FAILURE;
            return result;
        }
        //write
        if(BHT_SUCCESS != (result = bht_L1_bd_fpga_eeprom_write(dev_id, addr, wdata)))
        {
            printf("\nfpga eeprom write wdata failed\n");
			result = BHT_FAILURE;
            return result;
        }
        //read 
        if(BHT_SUCCESS != (result = bht_L1_bd_fpga_eeprom_read(dev_id, addr, &rdata)))
        {
            printf("\nfpga eeprom read rdata failed\n");
			result = BHT_FAILURE;
            return result;
        }
        //compare
        if(rdata != wdata)
        {
            printf("\nfpga eeprom test failed, addr = 0x%03x\n", addr);
			result = BHT_FAILURE;
            return result;
        }
        //write temp
        if(BHT_SUCCESS != (result = bht_L1_bd_fpga_eeprom_write(dev_id, addr, temp)))
        {
            printf("\nfpga eeprom write temp failed\n");
            result = BHT_FAILURE;
            return result;
        }
        if(addr % 1024 == 0)
            printf(".");
    }
    printf("\n");

    return result;
}



