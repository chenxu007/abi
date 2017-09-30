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
bht_L1_device_load(bht_L1_device_handle_t device,
        const char *filename)
{
    int fd;
    bht_L0_s32 len, idx, value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = device;
    bht_L0_u8 buffer[512];

    if(BHT_L0_INTERFACE_TYPE_PCI == device0->itype)
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

    DEBUG_PRINTF("open a device handle %p\n", device0);

    device0->device_status = BHT_L0_DEVICE_STATUS_OPENED;
    
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

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, close_err);

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

    device0->device_status = BHT_L0_DEVICE_STATUS_CLOSED;

    DEBUG_PRINTF("start unmap\n");
    if(BHT_SUCCESS != (result = bht_L0_unmap_memory(device0)))
        goto close_err;
    DEBUG_PRINTF("unmap succ\n");

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

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, release_sem);

    if(BHT_L0_LOGIC_TYPE_A429 == device0->ltype)
    {
        if(BHT_SUCCESS != (result = bht_L0_read_mem32(device0, BHT_A429_DEVICE_VERSION, version, 1)))
            result = BHT_ERR_DRIVER_READ_FAIL;
    }
    else
    {
        result = BHT_ERR_UNSUPPORTED_DTYPE;
    }
release_sem:
    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}

bht_L0_u32
bht_L1_device_hw_version(bht_L1_device_handle_t device,
        bht_L0_u32 *version)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t *)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

release_sem:
    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
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

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
    
    if ((NULL == filename) || ((fp = fopen(filename, "rb")) == NULL))
        return BHT_FAILURE;
    
    //load tree from xml
    tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);    

    fclose(fp);

    if(!tree)
    {
        result = BHT_FAILURE;
        goto release_sem;
    }

    //phase tree and config the device
    Device = tree;
    if(BHT_L0_LOGIC_TYPE_A429 == device0->ltype)
    {
        while(NULL != (Device = mxmlFindElement(Device, tree, "Device", "DevType", STRING_DEVTYPE_A429, MXML_DESCEND)))
        {
            attr = mxmlElementGetAttr(Device, "DevID");
            DevID = atoi(attr);          

            Channel = Device;
            while(NULL != (Channel = mxmlFindElement(Channel, Device, "Channel", NULL, NULL, MXML_DESCEND)))
            {
                bht_L0_u32 ChanID;
    			bht_L0_u32 Period = 0;
                bht_L1_chan_type_e ChanType;
    			bht_L1_able_e LoopEnable;
                bht_L1_a429_baud_rate_e baud;
                bht_L1_a429_parity_e parity;
                bht_L1_a429_err_type_e err_type;
                bht_L1_a429_chan_stat_e chan_status;
                bht_L1_a429_send_mode_e send_mode;
                bht_L1_able_e filter_enable;
                bht_L1_a429_recv_mode_e recv_mode;
                bht_L0_u16 threshold_count,threshold_time;
                
                attr = mxmlElementGetAttr(Channel, "ChanType");
                if(!strcmp(attr,STRING_CHANTYPE_RX))
                    ChanType = BHT_L1_CHAN_TYPE_RX;
                else if(!strcmp(attr,STRING_CHANTYPE_TX))
                    ChanType = BHT_L1_CHAN_TYPE_TX;
                else
                    goto config_failed;

                attr = mxmlElementGetAttr(Channel, "ChanID");
                ChanID = atoi(attr);
    			if(ChanID > 16)
    				goto config_failed;

                attr = mxmlElementGetAttr(Channel, "Baud");
                baud = atoi(attr);
                if(BHT_SUCCESS != bht_L1_a429_chan_baud(device, ChanID, ChanType, &baud, BHT_L1_PARAM_OPT_SET))
                        goto config_failed;

                attr = mxmlElementGetAttr(Channel, "Verify");
                if(!strcmp(attr,STRING_VERIFY_ODD))
                    parity = BHT_L1_A429_PARITY_ODD;
                else if(!strcmp(attr,STRING_VERIFY_EVEN))
                    parity = BHT_L1_A429_PARITY_EVEN;
                else if(!strcmp(attr,STRING_VERIFY_NONE))
                    parity = BHT_L1_A429_PARITY_NONE;
                else
                    goto config_failed;
                if(BHT_SUCCESS != bht_L1_a429_chan_parity(device, ChanID, ChanType, &parity, BHT_L1_PARAM_OPT_SET))
                        goto config_failed;

    			if(NULL == (Param = mxmlFindElement(Channel, Channel, "Param", NULL, NULL, MXML_DESCEND)))
                    goto config_failed;

    			if(NULL == (node = mxmlFindElement(Param, Param, "ChannelStatus", NULL, NULL, MXML_DESCEND)))
                    goto config_failed;
    			if(NULL == (attr = mxmlGetText(node, NULL)))
    				goto config_failed;
    			if(!strcmp(attr,STRING_WORKMODE_OPEN))
                    if(BHT_SUCCESS != bht_L1_a429_chan_open(device, ChanID, ChanType))
                        goto config_failed;
                else if(!strcmp(attr,STRING_WORKMODE_CLOSE))
                    if(BHT_SUCCESS != bht_L1_a429_chan_close(device, ChanID, ChanType))
                        goto config_failed;
                else if(!strcmp(attr,STRING_WORKMODE_CLOSEANDCLEARFIFO))
                    if(BHT_SUCCESS != bht_L1_a429_chan_close_and_clear_fifo(device, ChanID, ChanType))
                        goto config_failed;
                else
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
    				if(BHT_SUCCESS != bht_L1_a429_chan_loop(device, ChanID, &LoopEnable, BHT_L1_PARAM_OPT_SET))
    					goto config_failed;

                    if(NULL == (node = mxmlFindElement(Param, Param, "PeriodAttr", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;
    				if(NULL == (attr = mxmlElementGetAttr(node, "IsPeriodChannel")))
    					goto config_failed;
    	            if(!strcmp(attr,"Yes"))
                	{
    					if(NULL == (node = mxmlFindElement(node, node, "Period", NULL, NULL, MXML_DESCEND)))
    				        goto config_failed;
                        send_mode = BHT_L1_A429_SEND_MODE_PERIOD;
    					if(!(Period = atoi(mxmlGetText(node, NULL))))
    						goto config_failed;
                        if(BHT_SUCCESS != bht_L1_a429_tx_chan_send_mode(device, ChanID, &send_mode, BHT_L1_PARAM_OPT_SET))
        					goto config_failed;
                        if(BHT_SUCCESS != bht_L1_a429_tx_chan_period(device, ChanID, &Period, BHT_L1_PARAM_OPT_SET))
        					goto config_failed;                       
                	}
    	            else if(!strcmp(attr,"No"))
                    {   
    	                send_mode = BHT_L1_A429_SEND_MODE_NONPERIOD;
                        if(BHT_SUCCESS != bht_L1_a429_tx_chan_send_mode(device, ChanID, &send_mode, BHT_L1_PARAM_OPT_SET))
        					goto config_failed;
                    }
    	            else
    	                goto config_failed;

    				if(NULL == (ErrInject = mxmlFindElement(Param, Param, "ErrInjectType", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;
                    if(NULL == (attr = mxmlGetText(ErrInject, NULL)))
        				goto config_failed;
        			if(!strcmp(attr,STRING_ERRINJECTTYPE_NONE))
                        err_type = BHT_L1_A429_ERR_TYPE_NONE;
                    else if(!strcmp(attr,STRING_ERRINJECTTYPE_31BIT))
                        err_type = BHT_L1_A429_ERR_TYPE_31BIT;
                    else if(!strcmp(attr,STRING_ERRINJECTTYPE_33BIT))
                        err_type = BHT_L1_A429_ERR_TYPE_33BIT;
                    else if(!strcmp(attr,STRING_ERRINJECTTYPE_2GAP))
                        err_type = BHT_L1_A429_ERR_TYPE_2GAP;
                    else if(!strcmp(attr,STRING_ERRINJECTTYPE_PARITY))
                        err_type = BHT_L1_A429_ERR_TYPE_PARITY;
                    else
                        goto config_failed;
                    if(BHT_SUCCESS != bht_L1_a429_tx_chan_err_inject(device, ChanID, &err_type, BHT_L1_PARAM_OPT_SET))
    					goto config_failed;    				
    			}
    			else
    			{
    			    if(NULL == (node = mxmlFindElement(Param, Param, "FilterEnable", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;
    				if(NULL == (attr = mxmlGetText(node, NULL)))
    					goto config_failed;
    				if(!strcmp(attr,STRING_ENABLE))
    	                filter_enable = BHT_L1_ENABLE;
    	            else if(!strcmp(attr,STRING_DISABLE))
    	                filter_enable = BHT_L1_DISABLE;
    	            else
    	                goto config_failed;
                    if(BHT_SUCCESS != bht_L1_a429_rx_chan_filter(device, ChanID, &filter_enable, BHT_L1_PARAM_OPT_SET))
    					goto config_failed;   

    				if(NULL == (ReceiveMode = mxmlFindElement(Param, Param, "ReceiveMode", NULL, NULL, MXML_DESCEND)))
    	                goto config_failed;
                    if(NULL == (attr = mxmlElementGetAttr(ReceiveMode, "Mode")))
    					goto config_failed;
    	            if(!strcmp(attr,"List"))
                	{
                	    recv_mode = BHT_L1_A429_RECV_MODE_LIST;

    					if(NULL == (node = mxmlFindElement(ReceiveMode, ReceiveMode, "ListDepthThreshold", NULL, NULL, MXML_DESCEND)))
    		                goto config_failed;
                	    if(0 == (threshold_count = atoi(mxmlGetText(node, NULL))))
    						goto config_failed;

    					if(NULL == (node = mxmlFindElement(ReceiveMode, ReceiveMode, "ListTimeThreshold", NULL, NULL, MXML_DESCEND)))
    		                goto config_failed;
                	    if(0 == (threshold_time = atoi(mxmlGetText(node, NULL))))
    						goto config_failed;
                        if(BHT_SUCCESS != bht_L1_a429_rx_chan_recv_mode(device, ChanID, &recv_mode, BHT_L1_PARAM_OPT_SET))
        					goto config_failed;   
                        if(BHT_SUCCESS != bht_L1_a429_rx_chan_int_threshold(device, ChanID, &threshold_count, &threshold_time, BHT_L1_PARAM_OPT_SET))
        					goto config_failed;   
                	}
    	            else if(!strcmp(attr,"Sample"))
                	{
    	                recv_mode = BHT_L1_A429_RECV_MODE_SAMPLE;
    					if(BHT_SUCCESS != bht_L1_a429_rx_chan_recv_mode(device, ChanID, &recv_mode, BHT_L1_PARAM_OPT_SET))
        					goto config_failed;   
                	}
    	            else
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
release_sem:
    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}
#endif

bht_L0_u32
bht_L1_device_default_param_save(bht_L1_device_handle_t device)
{
    bht_L0_u32 result;
    bht_L0_u32 value;
    bht_L0_u32 count = 0;
    bht_L0_device_t *device0 = (bht_L0_device_t *)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

    value = BIT0;
    BHT_L1_WRITE_MEM32(device0, BHT_A429_SAVE_DEFAULT_PARAM_CTRL, &value, 1, result, release_sem);

    printf("saving...\n\ntime escape %5d ms", count);
    do
    {
        bht_L0_msleep(10);

        printf("\b\b\b\b\b\b\b\b%5d ms", (count += 10));
        
        BHT_L1_READ_MEM32(device0, BHT_A429_SAVE_DEFAULT_PARAM_STATUS, &value, 1, result, release_sem);
    }while(0 != value);
    printf("\n");

release_sem:
    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}

