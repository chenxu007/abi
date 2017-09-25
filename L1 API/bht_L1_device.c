/* bht_L1_device - Layer 1 device function */

/*
 * Copyright (c) 2017 Bin Hong Tech, Inc.
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

#include <bht_L0_types.h>
#include <bht_L0.h>
#include <bht_L0_config.h>
#include <bht_L0_plx9056.h>
#include <bht_L1_a429.h>
#include <bht_L1.h>
#include <bht_L1_defs.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <io.h>

#define BHT_L1_DEVICE_MAX       16
#define BHT_L1_CHANNEL_MAX      16

#define SLAVE_SERIAL
#define NEW_BOARD
#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTF(format, ...)\
do\
{\
	(void)printf(format, ##__VA_ARGS__); \
}while (0);
#else
#define DEBUG_PRINTF(x)
#endif

#ifdef WINDOWS_OPS
#ifdef WINDOWS_BIT64
#define FPGA_UPDATE_FILE_PATH           "c:\\Windows\\SysWOW64\\"
#else
#ifdef NEW_BOARD
#define FPGA_UPDATE_FILE_PATH           "W:\\ARINC_429_k7_config\\"
#else
#define FPGA_UPDATE_FILE_PATH           "W:\\spartan6_config\\"
#endif
//#define FPGA_UPDATE_FILE_PATH           "X:\\doc\\fpga_bin\\"
//#define FPGA_UPDATE_FILE_PATH           "C:\\WINDOWS\\system32\\"
#endif
#else
#define FPGA_UPDATE_FILE_PATH           "/tffs0/"
#endif
//#define FPGA_UPDATE_FILE_NAME           "A429_FPGA_1_0.bin"
#define FPGA_UPDATE_FILE_NAME           "a429_fpga_top.bin"
//#define FPGA_UPDATE_FILE_NAME           "a429_fpga_top(1553).bin"

#ifdef SUPPORT_CONFIG_FROM_XML
#include <mxml.h>
#define STRING_DEVTYPE_A429 "ARINC429"
#define STRING_DEVTYPE_1553 "MIL-STD-1553B"
#define STRING_CHANTYPE_RX "RX"
#define STRING_CHANTYPE_TX "TX"
#define STRING_ENABLE "Enable"
#define STRING_DISABLE "Disable"
#define STRING_VERIFY_ODD "ODD"
#define STRING_VERIFY_EVEN "EVEN"
#define STRING_VERIFY_NONE "NONE"
#define STRING_RECVMODE_LIST "List"
#define STRING_RECVMODE_Sample "Sample"
#define STRING_WORKMODE_OPEN "Open"
#define STRING_WORKMODE_CLOSE "Close"
#define STRING_WORKMODE_CLOSEANDCLEARFIFO "CloseAndClearFIFO"
#endif

typedef struct
{
    bht_L0_dtype_e dtype;
    bht_L0_bool is_scaned;
    bht_L0_u32 tot_num;
}bht_L1_device_scan_t;

static bht_L1_device_scan_t device_scan_table[BHT_L0_DEVICE_TYPE_MAX] = {0};

//static void 
//device_scan_table_init(void)
//{
//    bht_L0_u32 dtype;

//    for(dtype = 0; dtype < BHT_L1_DEVICE_MAX; dtype++)
//    {
//        device_scan_table[dtype].dtype = dtype;
//        device_scan_table[dtype].is_scaned = BHT_L0_FALSE;
//        device_scan_table[dtype].tot_num = 0;
//    } 
//}

#ifdef SLAVE_SERIAL
bht_L0_u32 
bht_L1_device_load(bht_L0_device_t *device,
        const char *filename)
{
    int fd;
    bht_L0_s32 len, idx, bit8, value;
    bht_L0_itype_e itype = bht_L0_dtypeinfo_items[device->dtype].itype;
    bht_L0_ltype_e ltype = bht_L0_dtypeinfo_items[device->dtype].ltype;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u8 buffer[512];

    if(BHT_L0_INTERFACE_TYPE_PCI == itype)
    {
     
        if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(device, PLX9056_CNTRL, &value, 1)))
            return result;
        /* 2.0 PROGRAM_B set 0 */   
        value &= (~BIT16);
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(device, PLX9056_CNTRL, &value, 1)))
            return result;
        bht_L0_msleep(1);
        
        /* 2.1 PROGRAM_B set 1 */   
        value |= BIT16;
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(device, PLX9056_CNTRL, &value, 1)))
            return result;
        bht_L0_msleep(1);
        
        /* 2.2 PROGRAM_B set 0 */
        value &= (~BIT16);
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(device, PLX9056_CNTRL, &value, 1)))
            return result;
        bht_L0_msleep(1);

		/* check done */
		bht_L0_read_setupmem32(device, PLX9056_CNTRL, &value, 1);
        if(!(BIT17 & value))
            assert(0);
        
        /* 2.3 transfer data */
        if(0 > (fd = open(filename, O_BINARY | O_RDONLY)))
        {
            printf("%s %d\n", __FILE__, __LINE__);
            return BHT_ERR_LOAD_FPGA_FAIL;
        }
        while(0 < (len = read(fd, buffer, sizeof(buffer))))
        {
            for(idx = 0; idx < len; idx ++)
            {
                for(bit8 = 7; bit8 >= 0; bit8 --)
                {
                    value = (buffer[idx] >> bit8) & 0x01;
					//value = 0x01 & (buffer[idx] >> bit8);
                    /* write one bit*/
                    if(BHT_SUCCESS != (result = bht_L0_write_mem32(device, 0xfffc, &value, 1)))
                    {
                        close(fd);
                        return result;
                    }
                }
            }
        }

        close(fd);
        
        /* wait 1ms*/
        bht_L0_msleep(1);
        /* output 8 cycles */
		value = 0;
        for(idx = 0; idx < 8; idx++)
            bht_L0_write_mem32(device, 0, &value, 1);
        /* check done */
        for(idx = 1000; idx > 0; idx--)
        {
            bht_L0_msleep(1);

            if(BHT_L0_LOGIC_TYPE_A429 == ltype)
            {
                value = 0;
                bht_L0_read_setupmem32(device, PLX9056_CNTRL, &value, 1);
                if(!(BIT17 & value))
                    break;
            }
            else
                return BHT_ERR_UNSUPPORTED_BOARDTYPE;                
        }
        if(idx <= 0)
        {
            printf("%s %d\n", __FILE__, __LINE__);            
            return BHT_ERR_LOAD_FPGA_FAIL;
        }
    }
    else
        result = BHT_ERR_UNSUPPORTED_BACKPLANE;

    return result;
    
}

#else
bht_L0_u32 
bht_L1_device_load(bht_L0_device_t *device,
        const char *filename)
{
    int fd;
    bht_L0_s32 len, idx, value;
    bht_L0_itype_e itype = bht_L0_dtypeinfo_items[device->dtype].itype;
    bht_L0_ltype_e ltype = bht_L0_dtypeinfo_items[device->dtype].ltype;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u8 buffer[512];

    if(BHT_L0_INTERFACE_TYPE_PCI == itype)
    {
     
        if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(device, PLX9056_CNTRL, &value, 1)))
            return result;
        /* 2.0 PROGRAM_B set 0 */   
        value &= (~BIT16);
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(device, PLX9056_CNTRL, &value, 1)))
            return result;
        bht_L0_msleep(1);
        
        /* 2.1 PROGRAM_B set 1 */   
        value |= BIT16;
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(device, PLX9056_CNTRL, &value, 1)))
            return result;
        bht_L0_msleep(1);
        
        /* 2.2 PROGRAM_B set 0 */
        value &= (~BIT16);
        if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(device, PLX9056_CNTRL, &value, 1)))
            return result;
        bht_L0_msleep(1);
        
        /* 2.3 transfer data */
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
                if(BHT_SUCCESS != (result = bht_L0_write_mem32(device, 0x8000, &value, 1)))
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
            bht_L0_write_mem32(device, 0x8000, &value, 1);
        /* check version */
        for(idx = 1000; idx > 0; idx--)
        {
            bht_L0_msleep(1);
            if(BHT_L0_LOGIC_TYPE_A429 == ltype)
                bht_L0_read_mem32(device, BHT_A429_DEVICE_VERSION, &value, 1);
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
#endif


bht_L0_u32
bht_L1_device_scan(bht_L0_dtype_e dtype)
{
    return bht_L0_device_scan(dtype);
}

bht_L0_u32
bht_L1_device_open(bht_L0_dtype_e dtype,
        bht_L0_u32 device_no,
        bht_L1_device_handle_t *device,
        const char *filename)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = NULL;

    *device = NULL;
    if(BHT_L0_DEVICE_TYPE_MAX <= dtype)
        return BHT_ERR_BAD_INPUT;
    
	device0 = (bht_L0_device_t*)calloc(1, sizeof(bht_L0_device_t));
    if(NULL == device0)
        return BHT_ERR_MEM_ALLOC_FAIL;

    device0->dtype = dtype;
    device0->itype = bht_L0_dtypeinfo_items[dtype].itype;
    device0->ltype = bht_L0_dtypeinfo_items[dtype].ltype;
    
    device0->device_no = device_no;
    device0->lld_hand = NULL;
    
    if(BHT_SUCCESS != (result = bht_L0_map_memory(device0, NULL)))
        goto open_err;

    device0->mutex_sem = bht_L0_semm_create();

    if(0 > device0->mutex_sem)
    {
        result = BHT_ERR_SEM_CREAT_FAIL;
        goto open_err;
    }

    /* pci load */
    if(NULL != filename)
    {
        result = bht_L1_device_load(device0, filename);
        if(BHT_SUCCESS != result)
            goto open_err;
    }

    if(BHT_L0_LOGIC_TYPE_A429 == device0->ltype)
    {
        result = bht_L1_a429_private_alloc(device0);
        if(BHT_SUCCESS != result)
            goto open_err;
        device0->reset = bht_L1_a429_reset;
        DEBUG_PRINTF("bht_L1_a429_private_alloc succ\n");
    }
    else
    {
        result = BHT_ERR_UNSUPPORTED_DTYPE;
        goto open_err;
    }

    *device = device0;
    return result;
    
open_err:
    if(device0)
        free(device0);
    return result;
}

bht_L0_u32
bht_L1_device_close(bht_L1_device_handle_t device)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t *)device;

    if(NULL == device0)
        return BHT_ERR_BAD_INPUT;           

    if(BHT_L0_LOGIC_TYPE_A429 == device0->ltype)
    {
        result = bht_L1_a429_private_free(device0);
        if(BHT_SUCCESS != result)
            goto close_err;
    }
    else
    {
        result = BHT_ERR_UNSUPPORTED_DTYPE;
        goto close_err;
    }

    if(BHT_SUCCESS != (result = bht_L0_unmap_memory(device0)))
        goto close_err;

    if(BHT_SUCCESS != (result = bht_L0_sem_destroy(device0->mutex_sem)))
        goto close_err;   

    free(device0);
    
close_err:
    return result;
}

bht_L0_u32
bht_L1_device_reset(bht_L1_device_handle_t device)
{
    bht_L0_device_t *device0 = (bht_L0_device_t *)device;

    if(NULL == device0)
        return BHT_ERR_DEVICE_NOT_INIT;     

    if(NULL != device0->reset)
        return device0->reset(device0);
    else
        return BHT_ERR_DEVICE_NOT_INIT;
}

bht_L0_u32
bht_L1_device_logic_version(bht_L1_device_handle_t device,
        bht_L0_u32 *version)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t *)device;

    if(NULL == device0)
        return BHT_ERR_BAD_INPUT;

    if(BHT_L0_LOGIC_TYPE_A429 == device0->ltype)
    {
        if(BHT_SUCCESS != (result = bht_L0_read_mem32(device0, BHT_A429_DEVICE_VERSION, version, 1)))
            result = BHT_ERR_DRIVER_READ_FAIL;
    }
    else
    {
        result = BHT_ERR_UNSUPPORTED_DTYPE;
    }
    
    return result;
}

bht_L0_u32
bht_L1_device_hw_version(bht_L1_device_handle_t device,
        bht_L0_u32 *version)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t *)device;

    if(NULL == device0)
        return BHT_ERR_BAD_INPUT;
    
    return result;
}

#ifdef SUPPORT_CONFIG_FROM_XML
bht_L0_u32
bht_L1_device_config_from_xml(bht_L1_device_handle_t device,
        const char *filename)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t *)device;
    FILE * fp = NULL;
    bht_L0_u32 ChannelCount, DevID;
    mxml_node_t	*tree, *node;
	mxml_node_t *Device, *Channel, *Param, *ErrInject, *ReceiveMode;
    char * attr;

    if(NULL == device0)
        return BHT_ERR_BAD_INPUT;
    
    if ((NULL == filename) || ((fp = fopen(filename, "rb")) == NULL))
        return BHT_FAILURE;
    
    //load tree from xml
    tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);    

    fclose(fp);

    if(!tree)
        return BHT_FAILURE;

    //phase tree and config the device
    Device = tree;
    if(BHT_L0_LOGIC_TYPE_A429 == device0->ltype)
    {
        while(NULL != (Device = mxmlFindElement(Device, tree, "Device", "DevType", STRING_DEVTYPE_A429, MXML_DESCEND)))
        {
            attr = mxmlElementGetAttr(Device, "DevID");
            DevID = atoi(attr);
            
            attr = mxmlElementGetAttr(Device, "ChannelCount");
            ChannelCount = atoi(attr);

            Channel = Device;
            while(ChannelCount--)
            {
                bht_L0_u32 ChanID;
    			bht_L0_u32 Period = 0;
                bht_L1_chan_type_e ChanType;
    			bht_L1_able_e LoopEnable;
                bht_L1_a429_chan_comm_param_t comm_param;
                bht_L1_a429_rx_chan_gather_param_t gather_param;
                bht_L1_a429_tx_chan_inject_param_t inject_param;

                if(NULL == (Channel = mxmlFindElement(Channel, Device, "Channel", NULL, NULL, MXML_DESCEND)))
                    goto config_failed;

                attr = mxmlElementGetAttr(Channel, "ChanType");
                if(!strcmp(attr,STRING_CHANTYPE_RX))
                    ChanType = BHT_L1_CHAN_TYPE_RX;
                else if(!strcmp(attr,STRING_CHANTYPE_TX))
                    ChanType = BHT_L1_CHAN_TYPE_TX;
                else
                    goto config_failed;

                attr = mxmlElementGetAttr(Channel, "ChanID");
                ChanID = atoi(attr);
    			if(ChanID > 15)
    				goto config_failed;

                attr = mxmlElementGetAttr(Channel, "Baud");
                comm_param.baud = atoi(attr);

                attr = mxmlElementGetAttr(Channel, "Verify");
                if(!strcmp(attr,STRING_VERIFY_ODD))
                    comm_param.par = BHT_L1_A429_PARITY_ODD;
                else if(!strcmp(attr,STRING_VERIFY_EVEN))
                    comm_param.par = BHT_L1_A429_PARITY_EVEN;
                else if(!strcmp(attr,STRING_VERIFY_NONE))
                    comm_param.par = BHT_L1_A429_PARITY_NONE;
                else
                    goto config_failed;

    			if(NULL == (Param = mxmlFindElement(Channel, Channel, "Param", NULL, NULL, MXML_DESCEND)))
                    goto config_failed;

    			if(NULL == (node = mxmlFindElement(Param, Param, "WorkMode", NULL, NULL, MXML_DESCEND)))
                    goto config_failed;
    			if(NULL == (attr = mxmlGetText(node, NULL)))
    				goto config_failed;
    			if(!strcmp(attr,STRING_WORKMODE_OPEN))
                    comm_param.work_mode = BHT_L1_A429_CHAN_WORK_MODE_OPEN;
                else if(!strcmp(attr,STRING_WORKMODE_CLOSE))
                    comm_param.par = BHT_L1_A429_CHAN_WORK_MODE_CLOSE;
                else if(!strcmp(attr,STRING_WORKMODE_CLOSEANDCLEARFIFO))
                    comm_param.par = BHT_L1_A429_CHAN_WORK_MODE_CLOSE_AND_CLEAR;
                else
                    goto config_failed;

    			if(BHT_SUCCESS != a429_chan_comm_param(dev_id, ChanID + 1, ChanType, &comm_param, BHT_L1_PARAM_OPT_SET))
    				goto config_failed;

    			if(BHT_L1_CHAN_TYPE_TX == ChanType)
    			{
    			    if(NULL == (node = mxmlFindElement(Param, Param, "LoopEnable", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;
    				if(NULL == (attr = mxmlGetText(node, NULL)))
    					goto config_failed;
    				if(!strcmp(attr,STRING_ENABLE))
    	                LoopEnable = BHT_L1_ENABLE;
    	            else if(!strcmp(attr,STRING_DISABLE))
    	                LoopEnable = BHT_L1_DISABLE;
    	            else
    	                goto config_failed;

    				if(BHT_SUCCESS != bht_L1_a429_tx_chan_loop(dev_id, ChanID + 1, LoopEnable))
    					goto config_failed;

                    if(NULL == (node = mxmlFindElement(Param, Param, "PeriodAttr", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;
    				if(NULL == (attr = mxmlElementGetAttr(node, "IsPeriodChannel")))
    					goto config_failed;
    	            if(!strcmp(attr,"Yes"))
                	{
    					if(NULL == (node = mxmlFindElement(node, node, "Period", NULL, NULL, MXML_DESCEND)))
    				        goto config_failed;
    					if(!(Period = atoi(mxmlGetText(node, NULL))))
    						goto config_failed;
                	}
    	            else if(!strcmp(attr,"No"))
    	                Period = 0;
    	            else
    	                goto config_failed;

    				if(BHT_SUCCESS != bht_L1_a429_tx_chan_period_param(dev_id, ChanID + 1, &Period, BHT_L1_PARAM_OPT_SET))
    					goto config_failed;

    				if(NULL == (ErrInject = mxmlFindElement(Param, Param, "ErrInject", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;

    				if(NULL == (node = mxmlFindElement(ErrInject, ErrInject, "WordBits", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;				
    				if(NULL == (attr = mxmlGetText(node, NULL)))
    					goto config_failed;
    				if(!strcmp(attr,"31bit"))
    	                inject_param.tb_bits = BHT_L1_A429_WORD_BIT31;
    				else if(!strcmp(attr,"32bit"))
    	                inject_param.tb_bits = BHT_L1_A429_WORD_BIT32;
    				else if(!strcmp(attr,"33bit"))
    	                inject_param.tb_bits = BHT_L1_A429_WORD_BIT33;
    	            else
    	                goto config_failed;

    				if(NULL == (node = mxmlFindElement(ErrInject, ErrInject, "GapBits", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;				
    				if(NULL == (attr = mxmlGetText(node, NULL)))
    					goto config_failed;
    				if(!strcmp(attr,"2bit"))
    	                inject_param.tb_gap = BHT_L1_A429_GAP_2BIT;
    				else if(!strcmp(attr,"4bit"))
    	                inject_param.tb_bits = BHT_L1_A429_GAP_4BIT;
    	            else
    	                goto config_failed;

    				if(NULL == (node = mxmlFindElement(ErrInject, ErrInject, "VerifyErrEnable", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;				
    				if(NULL == (attr = mxmlGetText(node, NULL)))
    					goto config_failed;
    				if(!strcmp(attr,STRING_ENABLE))
    	                inject_param.tb_par_en = BHT_L1_ENABLE;
    				else if(!strcmp(attr,STRING_DISABLE))
    	                inject_param.tb_par_en = BHT_L1_DISABLE;
    	            else
    	                goto config_failed;

    				if(BHT_SUCCESS != bht_L1_a429_tx_chan_inject_param(dev_id, ChanID + 1, &inject_param, BHT_L1_PARAM_OPT_SET))
    					goto config_failed;
    			}
    			else
    			{
    			    if(NULL == (node = mxmlFindElement(Param, Param, "FilterEnable", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;
    				if(NULL == (attr = mxmlGetText(node, NULL)))
    					goto config_failed;
    				if(!strcmp(attr,STRING_ENABLE))
    	                gather_param.gather_enable = BHT_L1_DISABLE;
    	            else if(!strcmp(attr,STRING_DISABLE))
    	                gather_param.gather_enable = BHT_L1_ENABLE;
    	            else
    	                goto config_failed;

    				if(NULL == (ReceiveMode = mxmlFindElement(Param, Param, "ReceiveMode", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;
                    if(NULL == (attr = mxmlElementGetAttr(ReceiveMode, "Mode")))
    					goto config_failed;
    	            if(!strcmp(attr,"List"))
                	{
                	    gather_param.recv_mode = BHT_L1_A429_RECV_MODE_LIST;

    					if(NULL == (node = mxmlFindElement(ReceiveMode, ReceiveMode, "ListDepthThreshold", NULL, NULL, MXML_DESCEND)))
    		                goto config_failed;
                	    if(0 == (gather_param.threshold_count = atoi(mxmlGetText(node, NULL))))
    						goto config_failed;

    					if(NULL == (node = mxmlFindElement(ReceiveMode, ReceiveMode, "ListTimeThreshold", NULL, NULL, MXML_DESCEND)))
    		                goto config_failed;
                	    if(0 == (gather_param.threshold_time = atoi(mxmlGetText(node, NULL))))
    						goto config_failed;
                	}
    	            else if(!strcmp(attr,"Sample"))
                	{
    	                gather_param.recv_mode = BHT_L1_A429_RECV_MODE_SAMPLE;
    					gather_param.threshold_count = 512;
    					gather_param.threshold_time = 50;
                	}
    	            else
    	                goto config_failed;

    				if(BHT_SUCCESS != bht_L1_a429_rx_chan_gather_param(dev_id, ChanID + 1, &gather_param, BHT_L1_PARAM_OPT_SET))
    					goto config_failed;
    			}
            }
            
        }
    }
    else
    {
        result = BHT_ERR_UNSUPPORTED_DTYPE;
        goto config_failed;
    }

    mxmlDelete(tree);
    return BHT_SUCCESS;
    
config_failed:
    mxmlDelete(tree);
    return result;
}
#endif

bht_L0_u32
bht_L1_device_default_param_save(bht_L1_device_handle_t device)
{
    bht_L0_u32 result;
    bht_L0_u32 value;
    bht_L0_u32 count = 0;
    
    value = BIT0;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(device, BHT_A429_SAVE_DEFAULT_PARAM_CTRL, &value, 1)))
    {
        printf("write SAVE_DEFAULT_PARAM_CTRL failed\n");
        return result;
    }

    printf("saving...\n\ntime escape %3d senconds", ++count);
    do
    {
        bht_L0_msleep(1000);
        printf("\b\b\b\b\b\b\b\b\b\b\b\b%3d senconds", ++count);
        if(BHT_SUCCESS != (result = bht_L0_read_mem32(device, BHT_A429_SAVE_DEFAULT_PARAM_STATUS, &value, 1)))
        {
            printf("read SAVE_DEFAULT_PARAM_STATUS failed\n");
            return result;
        }
    }while(0 != value);

    return result;
}

