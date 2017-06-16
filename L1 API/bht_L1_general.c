/* bht_L0_genneral.c - Layer 1 implement in windows*/

/*
 * Copyright (c) 2017-2023 Bin Hong Tech, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable BinHong-tech license agreement.
 */

/*
modification history
--------------------
01a,17may18,cx_  add file
*/

#include <bht_L0.h>
#include <bht_L1.h>
#include <bht_L1_a429.h>
#include <bht_L1_plx9056.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>

#ifdef WINDOWS_OPS
#ifdef WINDOWS_BIT64
#define FPGA_UPDATE_FILE_PATH           "c:\\Windows\\SysWOW64\\"
#else
//#define FPGA_UPDATE_FILE_PATH           "W:\\"
#define FPGA_UPDATE_FILE_PATH           "C:\\WINDOWS\\system32\\"
#endif
#else
#define FPGA_UPDATE_FILE_PATH           "/tffs0/"
#endif
//#define FPGA_UPDATE_FILE_NAME           "A429_FPGA_1_0.bin"
#define FPGA_UPDATE_FILE_NAME           "a429_fpga_top.bin"

const char * bht_L1_error_to_string(bht_L0_u32 err_num)
{
    switch (err_num)
    {
    case BHT_SUCCESS:
       return("Function call completed without error");
       break;
    case BHT_FAILURE:
       return("Function call completed with error");
       break;
    case BHT_ERR_MEM_MAP_SIZE:
       return("Layer 0 - Invalid memory map size");
       break;
    case BHT_ERR_NO_DEVICE:
       return("Layer 0 - Device not found");
       break;
    case BHT_ERR_CANT_OPEN_DEV:
       return("Layer 0 - Can't open device");
       break;
    case BHT_ERR_DEV_NOT_INITED:
       return("Layer 0 - Device not initialized");
       break;
    case BHT_ERR_DEV_ALREADY_OPEN:
       return("Layer 0 - Device already open");
       break;
    case BHT_ERR_UNSUPPORTED_BACKPLANE:
       return("Layer 0 - Unsupported backplane in DevID");
       break;
    case BHT_ERR_UNSUPPORTED_BOARDTYPE:
       return("Layer 0 - Unsupported board type in DevID");
       break;
    case BHT_ERR_UNSUPPORTED_CHANNELTYPE:
       return("Layer 0 - Unsupported channel type in DevID");
       break;
    case BHT_ERR_CANT_OPEN_DRIVER:
       return("Layer 0 - Can't open driver");
       break;
    case BHT_ERR_CANT_SET_DRV_OPTIONS:
       return("Layer 0 - Can't set driver options");
       break;
    case BHT_ERR_CANT_GET_DEV_INFO:
       return("Layer 0 - Can't get device info");
       break;
    case BHT_ERR_INVALID_BOARD_NUM:
       return("Layer 0 - Invalid board number");
       break;
    case BHT_ERR_INVALID_CHANNEL_NUM:
       return("Layer 0 - Invalid channel number");
       break;
    case BHT_ERR_DRIVER_READ_FAIL:
       return("Layer 0 - Driver read memory failure");
       break;
    case BHT_ERR_DRIVER_WRITE_FAIL:
       return("Layer 0 - Driver write memory failure");
       break;
    case BHT_ERR_DEVICE_CLOSE_FAIL:
       return("Layer 0 - Device close failure");
       break;
    case BHT_ERR_DRIVER_CLOSE_FAIL:
       return("Layer 0 - Driver close failure");
       break;
    case BHT_ERR_KP_OPEN_FAIL:
       return("Layer 0 - Kernel Plug-In Open failure");
       break;
    case BHT_ERR_BAD_INPUT:
       return("Layer 1 - Bad input parameter");
       break;
    case BHT_ERR_MEM_TEST_FAIL:
       return("Layer 1 - Failed memory test");
       break;
    case BHT_ERR_MEM_MGT_NO_INIT:
       return("Layer 1 - Memory Management not initialized for the device ID");
       break;
    case BHT_ERR_MEM_MGT_INIT:
       return("Layer 1 - Memory Management already initialized for the device ID");
       break;
    case BHT_ERR_MEM_MGT_NO_MEM:
       return("Layer 1 - Not enough memory available");
       break;
    case BHT_ERR_BAD_DEV_TYPE:
       return("Layer 1 - Bad device type in device ID");
       break;
    case BHT_ERR_RT_FT_UNDEF:
       return("Layer 1 - RT Filter Table not defined");
       break;
    case BHT_ERR_RT_SA_UNDEF:
       return("Layer 1 - RT Subaddress not defined");
       break;
    case BHT_ERR_RT_SA_CDP_UNDEF:
       return("Layer 1 - RT SA CDP not defined");
       break;
    case BHT_ERR_IQ_NO_NEW_ENTRY:
       return("Layer 1 - No new entry in interrupt queue");
       break;
    case BHT_ERR_NO_BCCB_TABLE:
       return("Layer 1 - BCCB Table Pointer is zero");
       break;
    case BHT_ERR_BCCB_ALREADY_ALLOCATED:
       return("Layer 1 - BCCB already allocated");
       break;
    case BHT_ERR_BCCB_NOT_ALLOCATED:
       return("Layer 1 - BCCB has not been allocated");
       break;
    case BHT_ERR_BUFFER_FULL:
       return("Layer 1 - 1553-ARINC PB (CDP/PCB or RXP/PXP) buffer is full");
       break;
    case BHT_ERR_TIMEOUT:
       return("Layer 1 - Timeout error");
       break;
    case BHT_ERR_BAD_CHAN_NUM:
       return("Layer 1 - Bad channel number, channel does not exist on this board or is not initialized");
       break;
    case BHT_ERR_BITFAIL:
       return("Layer 1 - Built-In Test failure");
       break;
    case BHT_ERR_DEVICEINUSE:
       return("Layer 1 - Device in use already, or not properly closed");
       break;
    case BHT_ERR_NO_TXCB_TABLE:
       return("Layer 1 - TXCB Table Pointer is zero");
       break;
    case BHT_ERR_TXCB_ALREADY_ALLOCATED:
       return("Layer 1 - TXCB already allocated");
       break;
    case BHT_ERR_TXCB_NOT_ALLOCATED:
       return("Layer 1 - TXCB has not been allocated");
       break;
    case BHT_ERR_PBCB_TOOMANYPXPS:
       return("Layer 1 - PBCB Too Many PXPs For PBCB Allocation");
       break;
    case BHT_ERR_NORXCHCVT_ALLOCATED:
       return("Layer 1 - RX CH - No CVT Option Defined at Init");
       break;
    case BHT_ERR_NO_DATA_AVAILABLE:
       return("Layer 1 - No Data Available");
       break;

    default:
       return("UNKNOWN ERROR/STATUS CODE!");
       break;
    }
}

static bht_L0_u32 bht_L1_device_load(bht_L0_u32 dev_id)
{
    int fd;
    bht_L0_s32 len, idx, value;
    bht_L0_u32 backplane_type, board_type;
    bht_L0_u32 result = BHT_SUCCESS;
    char filename[100];
    bht_L0_u8 buffer[512];

    backplane_type = dev_id & 0xF0000000;
	board_type     = dev_id & 0x0FF00000;

    if(BHT_DEVID_BACKPLANETYPE_PCI == backplane_type)
    {
     /* 2.1 PROGRAM_B set 1 */
        if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(dev_id, PLX9056_CNTRL, &value, 1)))
            return result;
        value |= BIT16;
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(dev_id, PLX9056_CNTRL, &value, 1)))
            return result;
        /* 2.2 PROGRAM_B set 0 */
        value &= (~BIT16);
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(dev_id, PLX9056_CNTRL, &value, 1)))
            return result;
        bht_L0_msleep(1);
        /* 2.3 transfer data */
        sprintf(filename, "%s%s", FPGA_UPDATE_FILE_PATH, FPGA_UPDATE_FILE_NAME);
        if(0 > (fd = open(filename, O_BINARY | O_RDONLY)))
            return BHT_ERR_LOAD_FPGA_FAIL;
        while(0 < (len = read(fd, buffer, sizeof(buffer))))
        {
            for(idx = 0; idx < len; idx += 2)
            {
                value = ((buffer[idx]>>7)&BIT0) | ((buffer[idx]>>5)&BIT1) | ((buffer[idx]>>3)&BIT2)\
                    | ((buffer[idx]>>1) & BIT3) | ((buffer[idx]<<1) & BIT4) | ((buffer[idx]<<3) & BIT5) | \
                    ((buffer[idx]<<5) & BIT6) | ((buffer[idx]<<7) & BIT7);
                if(idx + 1 < len)
                {
                    value = (value << 8) + \
                        (((buffer[idx+1]>>7)&BIT0) | ((buffer[idx+1]>>5)&BIT1) | ((buffer[idx+1]>>3)&BIT2)\
                    | ((buffer[idx+1]>>1) & BIT3) | ((buffer[idx+1]<<1) & BIT4) | ((buffer[idx+1]<<3) & BIT5) | \
                    ((buffer[idx+1]<<5) & BIT6) | ((buffer[idx+1]<<7) & BIT7));
                }

                /* write 16bit*/
                if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, 0x8000, &value, 1)))
                {
                    close(fd);
                    return result;
                }
            }
        }

        close(fd);
        
        /* wait 1ms*/
        bht_L0_msleep(1);
        /* output 8 cycles */
		value = 0;
        for(idx = 0; idx < 8; idx++)
            bht_L0_write_mem32(dev_id, 0x8000, &value, 1);
        /* check version */
        for(idx = 100; idx > 0; idx--)
        {
            bht_L0_msleep(1);
            if(board_type == BHT_DEVID_BOARDTYPE_PMCA429)
                bht_L0_read_mem32(dev_id, BHT_A429_DEVICE_VERSION, &value, 1);
            else
            {
                result = BHT_ERR_UNSUPPORTED_BACKPLANE;
                break;
            }
            if(0 != value)
        	{
        	    printf("version 0x%08x\n", value);
                break;
        	}
        }
        if(idx <= 0)
            return BHT_ERR_LOAD_FPGA_FAIL;
    }
    else
        result = BHT_ERR_UNSUPPORTED_BACKPLANE;

    return result;
    
}

bht_L0_u32 bht_L1_device_softreset(bht_L0_u32 dev_id)
{
    bht_L0_s32 value;
    bht_L0_u32 backplane_type, board_type;
    bht_L0_u32 result = BHT_SUCCESS;

    backplane_type = dev_id & 0xF0000000;
	board_type     = dev_id & 0x0FF00000;

    if(board_type == BHT_DEVID_BOARDTYPE_PMCA429)
    {
        value = 1;
        bht_L0_write_mem32(dev_id, BHT_A429_DEVICE_SOFT_RESET, &value, 1);
        do
        {
            bht_L0_msleep(1);
            bht_L0_read_mem32(dev_id, BHT_A429_DEVICE_STATE, &value, 1);
        }while(value != 0x00000001);
    }
    else
        result = BHT_ERR_UNSUPPORTED_BACKPLANE;

    return result;
}
    
bht_L0_u32 bht_L1_device_probe(bht_L0_u32 dev_id)
{
    bht_L0_s32 value;
    bht_L0_u32 backplane_type, board_type;
    bht_L0_u32 result = BHT_SUCCESS;

    backplane_type = dev_id & 0xF0000000;
	board_type     = dev_id & 0x0FF00000;
    
    /* device map memory  */
    result = bht_L0_map_memory(dev_id, NULL);
    if(BHT_SUCCESS != result)
        return result;

    if(BHT_DEVID_BACKPLANETYPE_PCI == backplane_type)
    {
        /* 1¡¢plx9056 init */
        /* 1.1 check 9056 initiate stat */
        if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(dev_id, PLX9056_LMISC1, &value, 1)))
        {
            return result;
        }
        if((value & BIT2) == 0)
        {
            printf("9056 is not initiated yet.");
            assert(0);
        }
        /* 1.2 enable memory space */
        if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(dev_id, PLX9056_PCICR, &value, 1)))
        {
            return result;
        }
        value |= BIT1;
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(dev_id, PLX9056_PCICR, &value, 1)))
        {
            return result;
        }
        /* 1.3 config INTCSR */
        value = BIT8 | BIT11 | BIT16 | BIT18 | BIT19;
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(dev_id, PLX9056_INTCSR, &value, 1)))
        {
            return result;
        }
        /* 1.4 command 0xc-read mult line; 0xe- read line; 0x6- read */
	    value = 0xd767c;
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(dev_id, PLX9056_CNTRL, &value, 1)))
        {
            return result;
        }
        /* 2 load device */
        if(BHT_SUCCESS != (result = bht_L1_device_load(dev_id)))
            return result;
        
        /* 3 soft reset device */
        result = bht_L1_device_softreset(dev_id);
        
    }
    else
        result = BHT_ERR_UNSUPPORTED_BACKPLANE;

//    /* enable pci interrupt */
//    value = 0x01;
//    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_INTR_EN, &value, 1)))
//        return result;
//    /* attach intterrupt handler */
//    if(BHT_SUCCESS != (result = bht_L0_attach_inthandler(dev_id, 0, (BHT_L0_USER_ISRFUNC)test_isr, dev_id)))
//        return result;
//    else
//        printf("bht_L0_attach_inthandler success\n");
    
    return result;
}

bht_L0_u32 bht_L1_device_remove(bht_L0_u32 dev_id)
{
    return bht_L0_unmap_memory(dev_id);   
}

