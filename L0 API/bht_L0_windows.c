/* bht_L0_windows.c - Layer 0 implement in windows*/

/*
 * Copyright (c) 2017-2023 Bin Hong Tech, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01a,17may17,cx_  add support in windows
*/
#ifdef WINDOWS_OPS

#include <wdc_lib.h>
#include <wdc_defs.h>

#include <bht_L0.h>

#define LICENSE_10_2 "6C3CC2CFE89E7AD0424A070D434A6F6DC4950E1D.BibHong-tech"

/* windows pci device control block */
typedef struct
{
    bht_L0_u32 dev_id;
    WD_PCI_CARD_INFO card_info;
    WDC_DEVICE_HANDLE wd_handle;
}win_pci_device_cb_t;

static bht_L0_u32 devices_cb[16][256][16] = {0}; /* Backplane type, board type, board */

static bht_L0_u32 wd_lib_init(void)
{
    static bht_L0_u32 is_wd_lib_inited = 0;
    DWORD dwStatus = WD_STATUS_SUCCESS; 

    if(is_wd_lib_inited)
        return BHT_SUCCESS;
    
    //打开Windriver驱动.
    dwStatus = WDC_DriverOpen(WDC_DRV_OPEN_DEFAULT, LICENSE_10_2);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
    	WDC_Err("Error:%d - Failed to open driver.\n", dwStatus); 
    	return BHT_ERR_WINDRIVER_INIT_FAIL;
    }

    //设置调试选项
    dwStatus = WDC_SetDebugOptions(WDC_DBG_DEFAULT, NULL);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
    	WDC_Err("Error:%d - Failed to set debug option.\n", dwStatus);  
    	return BHT_ERR_WINDRIVER_INIT_FAIL;
    }

#if defined(WD_DRIVER_NAME_CHANGE)
    /* Set the driver name */
    if (!WD_DriverName(TEST5064_DEFAULT_DRIVER_NAME))
    {
    	WDC_Err("Failed to set the driver name for WDC library.\n");
    	return BHT_ERR_WINDRIVER_INIT_FAIL;
    }
#endif

    is_wd_lib_inited = 1;

    return BHT_SUCCESS;
}
/* Function : bht_L0_msleep
 * Description : ms sleep function
 * @param msdelay, how manny ms do you want to delay
 * @return void
 */
void bht_L0_msleep(bht_L0_u32 msdelay)
{
    WDC_Sleep(1000 * msdelay, WDC_SLEEP_NON_BUSY);
}

/* Function : bht_L0_map_memory
 * Description : This function maps the requested amount of memory and stores the memory 
                 pointer internally for use with the memory read/write functions.
 * @param msdelay, how manny ms do you want to delay
 * @return void
 */
bht_L0_u32 bht_L0_map_memory(bht_L0_u32 dev_id, void * arg)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 backplane_type, board_type, board_num, channel_type;
    
    backplane_type = dev_id & 0xF0000000;
	board_type     = dev_id & 0x0FF00000;
	board_num      = dev_id & 0x000F0000;
	channel_type   = dev_id & 0x0000FF00;

    switch(backplane_type)
    {
        case BHT_DEVID_BACKPLANETYPE_PCI:
            do
            {
                DWORD pci_vendor_id = BHT_PCI_VENDOR_ID;
                DWORD pci_device_id = 0;
                WDC_PCI_SCAN_RESULT scan_result;
				win_pci_device_cb_t *pci_device_cb = NULL;
                
                if(devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16] != 0)
                    break;

                if(BHT_SUCCESS != (result = wd_lib_init()))
                    break;

                if(board_type == BHT_DEVID_BOARDTYPE_PMC1553)
                    pci_device_id = BHT_PCI_DEVICE_ID_PMC1553;
                else if(board_type == BHT_DEVID_BOARDTYPE_PMCA429)
                    pci_device_id = BHT_PCI_DEVICE_ID_PMCA429;
                else
                {
                    result = BHT_ERR_UNSUPPORTED_BOARDTYPE;
                    break;
                }
                
				pci_device_cb = (win_pci_device_cb_t *)calloc(1, sizeof(win_pci_device_cb_t));
                if(NULL == pci_device_cb)
                {
                    result = BHT_ERR_MEM_ALLOC_FAIL;
                    break;
                }

                if(WD_STATUS_SUCCESS != WDC_PciScanDevices(pci_vendor_id, pci_device_id, &scan_result))
                    result = BHT_ERR_NO_DEVICE;
                else if(scan_result.dwNumDevices < ((board_num >> 16) + 1))
                    result = BHT_ERR_NO_DEVICE;
                else
                {
                    pci_device_cb->card_info.pciSlot.dwBus = scan_result.deviceSlot[board_num >> 16].dwBus;
                    pci_device_cb->card_info.pciSlot.dwFunction = scan_result.deviceSlot[board_num >> 16].dwFunction;
                    pci_device_cb->card_info.pciSlot.dwSlot = scan_result.deviceSlot[board_num >> 16].dwSlot;
                    
                    if(WD_STATUS_SUCCESS != WDC_PciGetDeviceInfo(&pci_device_cb->card_info))
                    {
                        free(pci_device_cb);
                        return  BHT_ERR_NO_DEVICE;
                    }
                    
                    if(WD_STATUS_SUCCESS != WDC_PciDeviceOpen(&pci_device_cb->wd_handle, &pci_device_cb->card_info, NULL, NULL, NULL, NULL))
                    {
                        free(pci_device_cb);
                        result = BHT_ERR_CANT_OPEN_DEV;
                    }
                    else
                        devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16] = (bht_L0_u32)pci_device_cb;
                }
            }while(0); 
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_BACKPLANE;
    }

    return result;
}

bht_L0_u32 bht_L0_unmap_memory(bht_L0_u32 dev_id)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 backplane_type, board_type, board_num, channel_type;
    
    backplane_type = dev_id & 0xF0000000;
	board_type     = dev_id & 0x0FF00000;
	board_num      = dev_id & 0x000F0000;
	channel_type   = dev_id & 0x0000FF00;

    switch(backplane_type)
    {
        case BHT_DEVID_BACKPLANETYPE_PCI:            
            if(devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16] != 0)
            {
                win_pci_device_cb_t * pci_device_cb = (win_pci_device_cb_t *)devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16];

                if(WD_STATUS_SUCCESS != WDC_PciDeviceClose(pci_device_cb->wd_handle))
                    result = BHT_ERR_DEVICE_CLOSE_FAIL;
                else
                {
                    free(pci_device_cb);
                    devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16] = 0;
                }
            }
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_BACKPLANE;
    }

    return result;
}

bht_L0_u32 bht_L0_read_mem32(bht_L0_u32 dev_id, bht_L0_u32 offset, bht_L0_u32 *data, bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 backplane_type, board_type, board_num, channel_type, idx;
    
    backplane_type = dev_id & 0xF0000000;
	board_type     = dev_id & 0x0FF00000;
	board_num      = dev_id & 0x000F0000;
	channel_type   = dev_id & 0x0000FF00;

    switch(backplane_type)
    {
        case BHT_DEVID_BACKPLANETYPE_PCI:            
            if(devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16] != 0)
            {
                win_pci_device_cb_t * pci_device_cb = (win_pci_device_cb_t *)devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16];

                idx = 0;
                while(count != idx)
                {
                    if(WD_STATUS_SUCCESS != WDC_ReadAddr32(pci_device_cb->wd_handle, 2, (offset + 4 * idx), &data[idx]))
                    {
                        result = BHT_ERR_DRIVER_READ_FAIL;
                        break;
                    }
                    idx++;
                }
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_BACKPLANE;
    }

    return result;
}

bht_L0_u32 bht_L0_read_mem32_dma(bht_L0_u32 dev_id, bht_L0_u32 offset, bht_L0_u32 *data, bht_L0_u32 count)
{
    return BHT_FAILURE;
}

bht_L0_u32 bht_L0_read_mem16(bht_L0_u32 dev_id, bht_L0_u32 offset, bht_L0_u16 *data, bht_L0_u32 count)
{
    return BHT_FAILURE;
}

bht_L0_u32 bht_L0_write_mem16(bht_L0_u32 dev_id, bht_L0_u32 offset, bht_L0_u16 *data, bht_L0_u32 count)
{
    return BHT_FAILURE;
}

bht_L0_u32 bht_L0_write_mem32(bht_L0_u32 dev_id, bht_L0_u32 offset, bht_L0_u32 *data, bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 backplane_type, board_type, board_num, channel_type, idx;
    
    backplane_type = dev_id & 0xF0000000;
	board_type     = dev_id & 0x0FF00000;
	board_num      = dev_id & 0x000F0000;
	channel_type   = dev_id & 0x0000FF00;

    switch(backplane_type)
    {
        case BHT_DEVID_BACKPLANETYPE_PCI:            
            if(devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16] != 0)
            {
                win_pci_device_cb_t * pci_device_cb = (win_pci_device_cb_t *)devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16];

                idx = 0;
                while(count != idx)
                {
                    if(WD_STATUS_SUCCESS != WDC_WriteAddr32(pci_device_cb->wd_handle, 2, (offset + 4 * idx), data[idx]))
                    {
                        result = BHT_ERR_DRIVER_WRITE_FAIL;
                        break;
                    }
                    idx++;
                }
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_BACKPLANE;
    }

    return result;
}

bht_L0_u32 bht_L0_read_setupmem32(bht_L0_u32 dev_id, bht_L0_u32 offset, bht_L0_u32 *data, bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 backplane_type, board_type, board_num, channel_type, idx;
    
    backplane_type = dev_id & 0xF0000000;
	board_type     = dev_id & 0x0FF00000;
	board_num      = dev_id & 0x000F0000;
	channel_type   = dev_id & 0x0000FF00;

    switch(backplane_type)
    {
        case BHT_DEVID_BACKPLANETYPE_PCI:            
            if(devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16] != 0)
            {
                win_pci_device_cb_t * pci_device_cb = (win_pci_device_cb_t *)devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16];

                idx = 0;
                while(count != idx)
                {
                    if(WD_STATUS_SUCCESS != WDC_ReadAddr32(pci_device_cb->wd_handle, 0, (offset + 4 * idx), &data[idx]))
                    {
                        result = BHT_ERR_DRIVER_READ_FAIL;
                        break;
                    }
                    idx++;
                }
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_BACKPLANE;
    }

	return result;
}

bht_L0_u32 bht_L0_write_setupmem32(bht_L0_u32 dev_id, bht_L0_u32 offset, bht_L0_u32 *data, bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 backplane_type, board_type, board_num, channel_type, idx;
    
    backplane_type = dev_id & 0xF0000000;
	board_type     = dev_id & 0x0FF00000;
	board_num      = dev_id & 0x000F0000;
	channel_type   = dev_id & 0x0000FF00;

    switch(backplane_type)
    {
        case BHT_DEVID_BACKPLANETYPE_PCI:            
            if(devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16] != 0)
            {
                win_pci_device_cb_t * pci_device_cb = (win_pci_device_cb_t *)devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16];

                idx = 0;
                while(count != idx)
                {
                    if(WD_STATUS_SUCCESS != WDC_WriteAddr32(pci_device_cb->wd_handle, 0, (offset + 4 * idx), data[idx]))
                    {
                        result = BHT_ERR_DRIVER_WRITE_FAIL;
                        break;
                    }
                    idx++;
                }
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_BACKPLANE;
    }

	return result;
}

bht_L0_u32 bht_L0_attach_inthandler(bht_L0_u32 dev_id, bht_L0_u32 chan_regoffset, BHT_L0_USER_ISRFUNC isr, void * arg)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 backplane_type, board_type, board_num;
    
    backplane_type = dev_id & 0xF0000000;
	board_type     = dev_id & 0x0FF00000;
	board_num      = dev_id & 0x000F0000;

    switch(backplane_type)
    {
        case BHT_DEVID_BACKPLANETYPE_PCI:            
            if(devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16] != 0)
            {
                win_pci_device_cb_t * pci_device_cb = (win_pci_device_cb_t *)\
                    devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16];
                WD_TRANSFER*pTrans = (WD_TRANSFER*) calloc(3, sizeof(WD_TRANSFER));
                WDC_DEVICE *dev = (WDC_DEVICE *)pci_device_cb->wd_handle;
#ifndef BHT_A429_INTR_CLR
#define BHT_A429_INTR_CLR					0X0024
#endif
#ifndef BHT_A429_INTR_STATE
#define BHT_A429_INTR_STATE					0X0028
#endif
                (pTrans)->dwPort = (dev->pAddrDesc+2)->kptAddr + BHT_A429_INTR_STATE;
                (pTrans)->cmdTrans = RM_DWORD;
                
                (pTrans+1)->cmdTrans = CMD_MASK;
                (pTrans+1)->Data.Dword = BIT4;
                
                (pTrans+2)->dwPort = (dev->pAddrDesc+2)->kptAddr + BHT_A429_INTR_CLR;
                (pTrans+2)->cmdTrans = WM_DWORD;
                (pTrans+2)->Data.Dword = BIT0;
     
				if(WD_STATUS_SUCCESS != WDC_IntEnable(pci_device_cb->wd_handle, pTrans, 3, 0, \
                    (INT_HANDLER)isr, (PVOID)arg, FALSE))
                    result = BHT_ERR_DRIVER_INT_ATTACH_FAIL;
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_BACKPLANE;
    }
    
    return result;
}

bht_L0_u32 bht_L0_detach_inthandler(bht_L0_u32 dev_id)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 backplane_type, board_type, board_num;
    
    backplane_type = dev_id & 0xF0000000;
	board_type     = dev_id & 0x0FF00000;
	board_num      = dev_id & 0x000F0000;

    switch(backplane_type)
    {
        case BHT_DEVID_BACKPLANETYPE_PCI:            
            if(devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16] != 0)
            {
                win_pci_device_cb_t * pci_device_cb = (win_pci_device_cb_t *)\
                    devices_cb[backplane_type >> 28][board_type >> 20][board_num >> 16];
				if(WD_STATUS_SUCCESS != WDC_IntDisable(pci_device_cb->wd_handle))
                    result = BHT_ERR_DRIVER_INT_DETACH_FAIL;
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_BACKPLANE;
    }
    
    return result;
}

bht_L0_sem
bht_L0_semc_create(bht_L0_u32 initial_cnt, bht_L0_u32 max_cnt)
{
    HANDLE semc;

    semc = CreateSemaphore(NULL, initial_cnt, max_cnt, NULL);

    return (bht_L0_sem)semc;
}

bht_L0_u32 
bht_L0_sem_take(bht_L0_sem sem, bht_L0_s32 timeout_ms)
{
    DWORD ret;
    DWORD wtimeout;
    HANDLE wsem = (HANDLE)sem;

    if(BHT_WAITFOREVER == timeout_ms)
        wtimeout = INFINITE;
    else
        wtimeout = (DWORD)timeout_ms;
        
    ret = WaitForSingleObject(wsem, wtimeout);

    if(ret == 0)
        return BHT_SUCCESS;
    else
        return BHT_FAILURE;
}

bht_L0_u32 
bht_L0_sem_give(bht_L0_sem sem)
{
    DWORD ret;
    HANDLE wsem = (HANDLE)sem;

    ret = (DWORD)ReleaseSemaphore(wsem, 1, NULL);

    if(BHT_TRUE == ret)
        return BHT_SUCCESS;
    else
        return BHT_FAILURE;
}

#endif

