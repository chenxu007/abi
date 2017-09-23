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
#include <bht_L0_plx9056.h>
#include <bht_L1.h>
#include <bht_L1_defs.h>
#include <bht_L1_a429.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

#define FPGA_ADDR_EEPROM_WRITE         0x4000
#define FPGA_ADDR_EEPROM_READ          0x4004
#define FPGA_ADDR_EEPROM_STATUS        0x4008
#define FPGA_ADDR_EEPROM_DATA          0x400C
#define FPGA_EEPROM_SIZE               0x10000

bht_L0_u32
bht_L1_bd_fpga_eeprom_read(bht_L1_device_handle_t device,
        bht_L0_u16 addr,
        bht_L0_u8 *data)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end); 

    //wait until not busy
    do
    {
        BHT_L1_READ_MEM32(device0, FPGA_ADDR_EEPROM_STATUS, &value, 1, result, release_sem);
    }while(!(value & BIT0));

    //write the address where you want to read
    value = addr << 16;
    BHT_L1_WRITE_MEM32(device0, FPGA_ADDR_EEPROM_READ, &value, 1, result, release_sem);

    //wait until read ok
    do
    {
        BHT_L1_READ_MEM32(device0, FPGA_ADDR_EEPROM_STATUS, &value, 1, result, release_sem);
    }while(!(value & BIT0));

    //get the data
    BHT_L1_READ_MEM32(device0, FPGA_ADDR_EEPROM_DATA, &value, 1, result, release_sem);
    *data = (bht_L0_u8) (0xFF & value);

release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
	return result;
}

bht_L0_u32
bht_L1_bd_fpga_eeprom_write(bht_L1_device_handle_t device,
        bht_L0_u16 addr,
        bht_L0_u8 data)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end); 

    //wait until not busy
    do
    {
        BHT_L1_READ_MEM32(device0, FPGA_ADDR_EEPROM_STATUS, &value, 1, result, release_sem);
    }while(!(value & BIT0));

    //write the address where you want to read
    value = (addr << 16) | data;
    BHT_L1_WRITE_MEM32(device0, FPGA_ADDR_EEPROM_WRITE, &value, 1, result, release_sem);

    //wait until write ok
    do
    {
        BHT_L1_READ_MEM32(device0, FPGA_ADDR_EEPROM_STATUS, &value, 1, result, release_sem);
    }while(!(value & BIT0));

release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}

bht_L0_u32
bht_L1_bd_fpga_eeprom_test(bht_L1_device_handle_t device)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L0_u8 wdata, rdata, temp;
    bht_L0_u32 addr;
    
    srand(time(NULL));
    wdata = (bht_L0_u8)rand();

    printf("test in process ");
    for(addr = 0; addr < FPGA_EEPROM_SIZE; addr++)
    {
        //read temp
        if(BHT_SUCCESS != (result = bht_L1_bd_fpga_eeprom_read(device0, addr, &temp)))
        {
            printf("\nfpga eeprom read temp failed\n");
			result = BHT_FAILURE;
            return result;
        }
        //write
        if(BHT_SUCCESS != (result = bht_L1_bd_fpga_eeprom_write(device0, addr, wdata)))
        {
            printf("\nfpga eeprom write wdata failed\n");
			result = BHT_FAILURE;
            return result;
        }
        //read 
        if(BHT_SUCCESS != (result = bht_L1_bd_fpga_eeprom_read(device0, addr, &rdata)))
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
        if(BHT_SUCCESS != (result = bht_L1_bd_fpga_eeprom_write(device0, addr, temp)))
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

#define EESK  0x01000000   /* bit 24 RW */
#define EECS  0x02000000   /* bit 25 RW */
#define EEDI  0x04000000   /* bit 26 RW */
#define EEDO  0x08000000   /* bit 27 RO */
#define EEDS  0x80000000   /* bit 31 RW */
#define EEPROM_SIZE 128
//#define ACTION_READ_ALL
//#define ACTION_ERASE
#define ACTION_INIT_EEPROM

static int hatoi(bht_L0_u8* str, bht_L0_u32 length)     
{     
    bht_L0_u8  revstr[16]={0};  //根据十六进制字符串的长度，这里注意数组不要越界  
    bht_L0_u32   num[16]={0};     
    bht_L0_u32   count=1;     
    bht_L0_u32   result=0;  
	bht_L0_s32 i;

    strcpy(revstr,str);

    for   (i = length-1; i >= 0; i--)     
    {     
        if ((revstr[i]>='0') && (revstr[i]<='9'))     
            num[i]=revstr[i]-48;//字符0的ASCII值为48  
        else if ((revstr[i]>='a') && (revstr[i]<='f'))     
            num[i]=revstr[i]-'a'+10;     
        else if ((revstr[i]>='A') && (revstr[i]<='F'))     
            num[i]=revstr[i]-'A'+10;     
        else     
            num[i]=0;   
        result=result+num[i]*count;     
        count=count*16;//十六进制(如果是八进制就在这里乘以8)      
    }     
    return result;     
}

bht_L0_u32 
bht_L1_bd_plx_eeprom_read(bht_L1_device_handle_t device,
        bht_L0_u32 addr,
        bht_L0_u16 *buf, 
        bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L0_u32 copy;  
    bht_L0_u32 i,x;

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

    bht_L0_read_setupmem32 (device0, PLX9056_CNTRL, &copy, 1);

    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDO;
    copy &= ~EEDS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* select chip  (step 2 3) */
    /* (2) Set EECS high */
    copy |=  EECS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (3) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* start bit 1 (step 4,5) */
    /* (4) Set the EEDI bit high (Start bit) */
    copy |=  EEDI;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (5) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* opcode 10 read (step 6,7,8) */
    /* (6) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);


    /* (7) Clear EEDI */
    copy &= (~EEDI);

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (8) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* address A7~A0 for 8 times EESK (EEDI is 0) */
    /* (9) Toggle EESK high, then low, 8 times (serial EEPROM address 0) */
    for (i = 0; i< 8; i++)
    {
        if((addr<<i) & 0x80)
           copy|=EEDI;
        else
           copy&=~EEDI;


        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

        copy |=  EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    }

    /* Enable EEDO */
    /* (10) Set the EEDO Input Enable to 1 to float the EEDO input for reading */
    copy |=EEDS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);


    /* (11) Toggle EESK high, then low, 16 times, (12) reading in a bit after each clock */
    for (i = 0; i < count; i++) 
    {
        buf[i] = 0;
        for (x = 0; x < 16; x++)
        {
            buf[i] = buf[i] << 1;
            
            copy |=  EESK;

            bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

            copy &= ~EESK;

            bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

            /*bht_L0_read_setupmem32 (PCI_BAR0(cardnum) + PLX9056_CNTRL, &readValue); 	*/	
            bht_L0_read_setupmem32 (device0, PLX9056_CNTRL, &copy, 1);

            if (copy & EEDO)
            {
                buf[i] |= 0x01;
            }
            else
            {
                buf[i] &= 0xFFFE;
            }
        }
    }

    /* (13) Clear the EECS bit */
    copy &= ~EECS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (14) Toggle EESK high, then low.  The read is complete */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);


    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
//    copy &= ~EECS;
//    copy &= ~EESK;
//    copy &= ~EEDI;
//    copy &= ~EEDS;

    /* bht_L0_write_setupmem32 (PCI_BAR0(cardnum) + PLX9056_CNTRL, copy);  */ 
//    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return BHT_SUCCESS;
}

static  bht_L0_u32 
bht_L1_bd_plx_eeprom_write_enable(bht_L1_device_handle_t device)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L0_u32 copy;  
    bht_L0_u32 i;

    bht_L0_read_setupmem32 (device0, PLX9056_CNTRL, &copy, 1);    

    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDO;
    copy &= ~EEDS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* select chip  (step 2 3) */
    /* (2) Set EECS high */
    copy |=  EECS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (3) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* start bit 1 (step 4,5) */
    /* (4) Set the EEDI bit high (Start bit) */
    copy |=  EEDI;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (5) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* opcode 00 write enable */
    /* (6) Toggle EESK high, then low */
    copy &=  (~EEDI);

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (8) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* address A7~A0 for 8 times EESK (EEDI is 0) */
    /* (9) Toggle EESK high, then low, 8 times (serial EEPROM address 0) */
    copy |= EEDI;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    for (i = 0; i< 8; i++)
    {
        copy |=  EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    }

    /* Clear the EECS bit */
    copy &= ~EECS;
    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    return BHT_SUCCESS;
}

static bht_L0_u32 
bht_L1_bd_plx_eeprom_write_disable(bht_L1_device_handle_t device)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L0_u32 copy;  
	bht_L0_u32 i;

    bht_L0_read_setupmem32 (device0, PLX9056_CNTRL, &copy, 1);    

    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDO;
    copy &= ~EEDS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* select chip  (step 2 3) */
    /* (2) Set EECS high */
    copy |=  EECS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (3) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* start bit 1 (step 4,5) */
    /* (4) Set the EEDI bit high (Start bit) */
    copy |=  EEDI;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (5) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* opcode 00 write disable */
    /* (6) Toggle EESK high, then low */
    copy &=  (~EEDI);

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (8) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* address A7~A0 for 8 times EESK (EEDI is 0) */
    /* (9) Toggle EESK high, then low, 8 times (serial EEPROM address 0) */
    copy &= (~EEDI);

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    for (i = 0; i< 8; i++)
    {
        copy |=  EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    }

    /* Clear the EECS bit */
    copy &= ~EECS;
    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    return BHT_SUCCESS;
}

bht_L0_u32 
bht_L1_bd_plx_eeprom_reload(bht_L1_device_handle_t device)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L0_u32 copy;  

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
    
    bht_L0_read_setupmem32 (device0, PLX9056_CNTRL, &copy, 1);

    copy |= PLX9056_CNTRL_RELOAD_CONFIG;
    copy |= PLX9056_CNTRL_RESET;
    bht_L0_write_setupmem32 (device0, PLX9056_CNTRL, &copy, 1);

    bht_L0_msleep(1);

    copy &= (~PLX9056_CNTRL_RESET);
    bht_L0_write_setupmem32 (device0, PLX9056_CNTRL, &copy, 1);

    /* wait initialize complete */
    bht_L0_msleep(1000);

    /* clear reload bit */
    copy &= (~PLX9056_CNTRL_RELOAD_CONFIG);
    bht_L0_write_setupmem32 (device0, PLX9056_CNTRL, &copy, 1);
//    do
//    {
//        bht_L0_write_setupmem32 (device0, PLX9056_CNTRL, &copy, 1);
//    }while();

    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
       
}



bht_L0_u32 
bht_L1_bd_plx_eeprom_write(bht_L1_device_handle_t device,
        bht_L0_u32 addr,
        bht_L0_u16 *buf, 
        bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L0_u32 copy;  
    bht_L0_u16 *promValuesPtr = buf;   
    bht_L0_u32 i,x;

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
    
    bht_L1_bd_plx_eeprom_write_enable(device0);
    
    bht_L0_read_setupmem32 (device0, PLX9056_CNTRL, &copy, 1);    

    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDO;
    copy &= ~EEDS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* select chip  (step 2 3) */
    /* (2) Set EECS high */
    copy |=  EECS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (3) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* start bit 1 (step 4,5) */
    /* (4) Set the EEDI bit high (Start bit) */
    copy |=  EEDI;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (5) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* opcode 10 read (step 6,7,8) */
    /* (6) Toggle EESK high, then low */
    copy &=  (~EEDI);

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);


    /* (7) Clear EEDI */
    copy |= (EEDI);

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (8) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* address A7~A0 for 8 times EESK (EEDI is 0) */
    /* (9) Toggle EESK high, then low, 8 times (serial EEPROM address 0) */
    for (i = 0; i< 8; i++)
    {
        if((addr<<i) & 0x80)
           copy|=EEDI;
        else
           copy&=~EEDI;


        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

        copy |=  EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    }

    /* Enable EEDO */
    /* (10) Set the EEDO Input Enable to 1 to float the EEDO input for reading */
//    copy |=EEDS;

//    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);


    /* (11) Toggle EESK high, then low, 16 times, (12) reading in a bit after each clock */
    for (i = 0; i < count; i++) 
    {
//        buf[i] = 0;
        for (x = 0; x < 16; x++)
        {
            if((buf[i] << x) & 0x8000)
                copy |= EEDI;
            else
                copy &= ~EEDI;

            bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
            
            copy |=  EESK;

            bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

            copy &= ~EESK;

            bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
        }
    }

    /* (13) Clear the EECS bit */
    copy &= ~EECS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* Enable EEDO */
    copy |= EEDS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy |= EECS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* wait write complete */
    do
    {
        bht_L0_read_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    }while(!(copy & EEDO));

    /* (14) Toggle EESK high, then low.  The read is complete */
//    copy |=  EESK;

//    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

//    copy &= ~EESK;

//    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);


    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDS;

    /* bht_L0_write_setupmem32 (PCI_BAR0(cardnum) + PLX9056_CNTRL, copy);  */ 
    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    bht_L1_bd_plx_eeprom_write_disable(device0);

    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}

bht_L0_u32 
bht_L1_bd_plx_eeprom_write_all(bht_L1_device_handle_t device,
        bht_L0_u16 value)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L0_u32 copy;  
    bht_L0_u32 i,x;

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
    
    bht_L1_bd_plx_eeprom_write_enable(device0);

    bht_L0_read_setupmem32 (device0, PLX9056_CNTRL, &copy, 1);    

    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDO;
    copy &= ~EEDS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* select chip  (step 2 3) */
    /* (2) Set EECS high */
    copy |=  EECS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (3) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* start bit 1 (step 4,5) */
    /* (4) Set the EEDI bit high (Start bit) */
    copy |=  EEDI;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (5) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* opcode 00 write all (step 6,7,8) */
    /* (6) Toggle EESK high, then low */
    copy &=  (~EEDI);

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* (8) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* address A7~A0 for 8 times EESK (EEDI is 0) */
    /* (9) Toggle EESK high, then low, 8 times (serial EEPROM address 0) */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    
    copy |= EEDI;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    
    for (i = 0; i < 7; i++)
    {
        copy |=  EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    }


    for (x = 0; x < 16; x++)
    {
        if((value << x) & 0x8000)
            copy |= EEDI;
        else
            copy &= ~EEDI;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
        
        copy |=  EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    }

    /* (13) Clear the EECS bit */
    copy &= ~EECS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* Enable EEDO */
    copy |= EEDS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    copy |= EECS;

    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    /* wait write complete */
    do
    {
        bht_L0_read_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);
    }while(!(copy & EEDO));

    /* (14) Toggle EESK high, then low.  The read is complete */
//    copy |=  EESK;

//    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

//    copy &= ~EESK;

//    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);


    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDS;

    /* bht_L0_write_setupmem32 (PCI_BAR0(cardnum) + PLX9056_CNTRL, copy);  */ 
    bht_L0_write_setupmem32 (device0,  PLX9056_CNTRL,&copy, 1);

    bht_L1_bd_plx_eeprom_write_disable(device0);

    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}


bht_L0_u32
bht_L1_bd_plx_eeprom_rfsetting (bht_L1_device_handle_t device,
        const char* filename)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    int fd;
	bht_L0_u32 value;
    bht_L0_u16 value16;
    bht_L0_u32 addr = 0;
    char buffer[100];

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
    /* plx9056 init */
    /* check 9056 initiate stat */
    if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(device0, PLX9056_LMISC1, &value, 1)))
        goto end;
    
    if((value & BIT2) == 0)
        goto end;
    
    /* enable memory space */
    if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(device0, PLX9056_PCICR, &value, 1)))
        goto end;
    
    value |= BIT1;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(device0, PLX9056_PCICR, &value, 1)))
        goto end;

    /* config INTCSR */
    value = BIT8 | BIT11 | BIT16 | BIT18 | BIT19;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(device0, PLX9056_INTCSR, &value, 1)))
        goto end;
    
    /* command 0xc-read mult line; 0xe- read line; 0x6- read */
    value = 0xd767c;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(device0, PLX9056_CNTRL, &value, 1)))
        goto end;
#if 0
#ifdef ACTION_READ_ALL

    if(0 > (fd = open("eeprom-value.txt", O_RDWR | O_CREAT | O_TRUNC, 0666)))
	{
	    goto end;
	}
    /* read the entire eeprom data */
    for(addr = 0; addr < EEPROM_SIZE; addr += 1)
    {
        bht_L1_bd_plx_eeprom_read(device0, addr, &value16, 1);
        
        sprintf(buffer, "%04x\n", value16);

        write(fd, buffer, strlen(buffer));

        printf(buffer);
    }

    close(fd);

	printf("eeprom read succ! data stored in eeprom-value.txt.\n");
    
#endif	

#ifdef ACTION_ERASE
    value16 = 0xFFFF;
    bht_L1_bd_plx_eeprom_write_enable(device0);
	for(addr = 0; addr < EEPROM_SIZE; addr++)
	{
		bht_L1_bd_plx_eeprom_write(device0, addr, &value16, 1);
	}
    bht_L1_bd_plx_eeprom_write_disable(device0);
	printf("eeprom erased succ!\n");
#endif
#endif

#ifdef ACTION_INIT_EEPROM
    if(0 > (fd = open(filename, O_BINARY | O_RDONLY, 0666)))
	{
	    goto end;
	}
    
    bht_L1_bd_plx_eeprom_write_enable(device0);
    
    for(addr = 0; addr < EEPROM_SIZE; addr++)
	{
		memset(buffer, 0x00, sizeof(buffer));
	    /* read value from file */
	    read(fd, buffer, 4);
		value16 = hatoi(buffer, 4);
        /* write to eeprom */
		bht_L1_bd_plx_eeprom_write(device0, addr, &value16, 1);
        /* read the '\n' char */
        read(fd, buffer, 2);
	}
    
    bht_L1_bd_plx_eeprom_write_disable(device0);

	close(fd);

#endif

    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);

end:
	return result;
}





