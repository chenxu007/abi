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

#include <windows.h>
#include <wdc_lib.h>
#include <wdc_defs.h>

#include <bht_L0.h>

#define LICENSE_10_2 "6C3CC2CFE89E7AD0424A070D434A6F6DC4950E31.hwacreate"

/* windows pci device control block */
typedef struct
{
    bht_L0_device_t *device;
    WD_PCI_CARD_INFO card_info;
    WDC_DEVICE_HANDLE wd_handle;
    bht_L0_sem pci_sem;
}win_pci_device_cb_t;

static bht_L0_u32 devices_cb[16][256][16] = {0}; /* Backplane type, board type, board */

static void pci_card_info_dump(WD_PCI_CARD_INFO * card_info)
{
    bht_L0_u32 index;
    
    if(NULL == card_info)
        return;
    
    printf("dwBus      : %d\n",card_info->pciSlot.dwBus);
    printf("dwSlot     : %d\n",card_info->pciSlot.dwSlot);
    printf("dwFunction : %d\n",card_info->pciSlot.dwFunction);
    printf("dwItems : %d\n",card_info->Card.dwItems);
    
    for(index = 0; index < card_info->Card.dwItems; index++)
    {
        printf("item type    : %d\n",card_info->Card.Item[index].item);
        printf("fNotSharable : %d\n",card_info->Card.Item[index].fNotSharable);
        printf("dwOptions    : 0x%08x\n",card_info->Card.Item[index].dwOptions);
        switch(card_info->Card.Item[index].item)
        {
            case ITEM_MEMORY:
                printf("dwPhysicalAddr : 0x%08x\n", card_info->Card.Item[index].I.Mem.dwPhysicalAddr);
                printf("dwBytes : 0x%08x\n", card_info->Card.Item[index].I.Mem.dwBytes);
                printf("dwUserDirectAddr : 0x%08x\n", card_info->Card.Item[index].I.Mem.dwUserDirectAddr);
                printf("dwCpuPhysicalAddr : 0x%08x\n", card_info->Card.Item[index].I.Mem.dwCpuPhysicalAddr);
                printf("dwBar : 0x%08x\n", card_info->Card.Item[index].I.Mem.dwBar);
                break;
            case ITEM_IO:
                printf("dwAddr : 0x%08x\n", card_info->Card.Item[index].I.IO.dwAddr);
                printf("dwBytes : 0x%08x\n", card_info->Card.Item[index].I.IO.dwBytes);
                printf("dwBar : 0x%08x\n", card_info->Card.Item[index].I.IO.dwBar);
                break;
            case ITEM_INTERRUPT:
                printf("dwInterrupt : 0x%08x\n", card_info->Card.Item[index].I.Int.dwInterrupt);
                printf("dwOptions : 0x%08x\n", card_info->Card.Item[index].I.Int.dwOptions);
                printf("hInterrupt : 0x%08x\n", card_info->Card.Item[index].I.Int.hInterrupt);
                break;
            default:
                printf("UNKNOWN ITEM TYPE -> %d\n", card_info->Card.Item[index].item);
                break;
        }
    }
}

static bht_L0_s32 bht_L0_init(void)
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
//    WDC_Sleep(1000 * msdelay, WDC_SLEEP_BUSY);//此处文档上说低于17000，不起作用
    Sleep(msdelay);
}

bht_L0_s32 
bht_L0_device_scan(bht_L0_dtype_e dtype)
{
    bht_L0_s32 result = BHT_SUCCESS;
    bht_L0_itype_e itype;
    bht_L0_u16 device_id;
    WDC_PCI_SCAN_RESULT scan_result;

    if(dtype >= BHT_L0_DEVICE_TYPE_MAX)
        return BHT_ERR_BAD_INPUT;

    itype = bht_L0_dtypeinfo_items[dtype];
    
    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:
            /* device id*/
            if((BHT_L0_DEVICE_TYPE_PMCA429 == dtype) || (BHT_L0_DEVICE_TYPE_PCIA429 == dtype) || 
               (BHT_L0_DEVICE_TYPE_CPCIA429 == dtype) || (BHT_L0_DEVICE_TYPE_PXIA429 == dtype))
            {
                device_id = BHT_PCI_DEVICE_ID_PMC429;
            }
            else
                return BHT_ERR_UNSUPPORTED_DEVICE_TYPE;
            /* scan */
            if((WD_STATUS_SUCCESS != WDC_PciScanDevices(BHT_PCI_VENDOR_ID, device_id, &scan_result))
                || (scan_result.dwNumDevices < 0))
                return BHT_ERR_LOW_LEVEL_DRIVER_ERR;

            return scan_result.dwNumDevices;
        default:
            return BHT_ERR_BAD_INPUT;
    }
}

/* Function : bht_L0_map_memory
 * Description : This function maps the requested amount of memory and stores the memory 
                 pointer internally for use with the memory read/write functions.
 * @param msdelay, how manny ms do you want to delay
 * @return void
 */
bht_L0_u32 
bht_L0_map_memory(bht_L0_device_t *device, 
        void * arg)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 pci_device_id;
    const bht_L0_itype_e itype = device->itype;
    const bht_L0_dtype_e dtype = device->dtype;
    const bht_L0_u32 device_no = device->device_no;

    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:
            do
            {
                DWORD pci_vendor_id = BHT_PCI_VENDOR_ID;
                DWORD pci_device_id = 0;
                WDC_PCI_SCAN_RESULT scan_result;
				WD_PCI_CARD_INFO card_info;

                if(NULL != device->lld_hand)
                    break;

                /* device id*/
                if((BHT_L0_DEVICE_TYPE_PMCA429 == dtype) || (BHT_L0_DEVICE_TYPE_PCIA429 == dtype) || 
                   (BHT_L0_DEVICE_TYPE_CPCIA429 == dtype) || (BHT_L0_DEVICE_TYPE_PXIA429 == dtype))
                {
                    pci_device_id = BHT_PCI_DEVICE_ID_PMC429;
                }
                else
                    return BHT_ERR_UNSUPPORTED_DEVICE_TYPE;
                /* scan */
                if((WD_STATUS_SUCCESS != WDC_PciScanDevices(BHT_PCI_VENDOR_ID, pci_device_id, &scan_result))
                    || (scan_result.dwNumDevices < 0))
                    return BHT_ERR_LOW_LEVEL_DRIVER_ERR;
                
                if(scan_result.dwNumDevices < (device_no + 1))
                    return BHT_ERR_NO_DEVICE;

                card_info.pciSlot.dwBus = scan_result.deviceSlot[device_no].dwBus;
                card_info.pciSlot.dwFunction = scan_result.deviceSlot[device_no].dwFunction;
                card_info.pciSlot.dwSlot = scan_result.deviceSlot[device_no].dwSlot;
                
                if(WD_STATUS_SUCCESS != WDC_PciGetDeviceInfo(&card_info))
                    return BHT_ERR_NO_DEVICE;
                
                if(WD_STATUS_SUCCESS != WDC_PciDeviceOpen(&(WDC_DEVICE_HANDLE device->lld_hand), \
                    &card_info, NULL, NULL, NULL, NULL))
                    return BHT_ERR_CANT_OPEN_DEV;
            }while(0); 
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_DTYPE;
    }

    return result;
}

bht_L0_s32 bht_L0_unmap_memory(bht_L0_device_t *device)
{
    bht_L0_u32 result = BHT_SUCCESS;	

    switch(device->itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:            
            if(NULL != device->lld_hand)
            {
                if(WD_STATUS_SUCCESS != WDC_PciDeviceClose((WDC_DEVICE_HANDLE )device->lld_hand))
                    result = BHT_ERR_DEVICE_CLOSE_FAIL;    
                else
                    device->lld_hand = NULL;
            }
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_DTYPE;
    }

    return result;
}

bht_L0_s32 
bht_L0_read_mem32(bht_L0_device_t *device, 
        bht_L0_u32 offset, 
        bht_L0_u32 *data, 
        bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 idx;
    bht_L0_itype_e itype = device->itype;

    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:            
            if(NULL != device->lld_hand)
            {
                idx = 0;
//                bht_L0_sem_take(pci_device_cb->pci_sem, BHT_WAITFOREVER);
                while(count != idx)
                {
                    if(WD_STATUS_SUCCESS != WDC_ReadAddr32((WDC_DEVICE_HANDLE)device->lld_hand, 2, \
                        (offset + 4 * idx), &data[idx]))
                    {
                        result = BHT_ERR_DRIVER_READ_FAIL;
                        break;
                    }
                    idx++;
                }
//                bht_L0_sem_give(pci_device_cb->pci_sem);
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_DTYPE;
    }

    return result;
}

bht_L0_s32 bht_L0_read_mem32_dma(bht_L0_device_t *device, bht_L0_u32 offset, bht_L0_u32 *data, bht_L0_u32 count)
{
    return BHT_FAILURE;
}

bht_L0_s32 bht_L0_read_mem16(bht_L0_device_t *device, bht_L0_u32 offset, bht_L0_u16 *data, bht_L0_u32 count)
{
    return BHT_FAILURE;
}

bht_L0_s32 bht_L0_write_mem16(bht_L0_device_t *device, bht_L0_u32 offset, bht_L0_u16 *data, bht_L0_u32 count)
{
    return BHT_FAILURE;
}

bht_L0_s32 
bht_L0_write_mem32(bht_L0_device_t *device, 
        bht_L0_u32 offset, 
        bht_L0_u32 *data, 
        bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 idx;
    bht_L0_itype_e itype = device->itype;

    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:            
            if(NULL != device->lld_hand)
            {
                idx = 0;
//                bht_L0_sem_take(pci_device_cb->pci_sem, BHT_WAITFOREVER);
                while(count != idx)
                {
                    if(WD_STATUS_SUCCESS != WDC_WriteAddr32((WDC_DEVICE_HANDLE)device->lld_hand, 2, \
                        (offset + 4 * idx), data[idx]))
                    {
                        result = BHT_ERR_DRIVER_WRITE_FAIL;
                        break;
                    }
                    idx++;
                }
//                bht_L0_sem_give(pci_device_cb->pci_sem);
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_DTYPE;
    }

    return result;
}

bht_L0_s32 
bht_L0_read_setupmem32(bht_L0_device_t *device, 
        bht_L0_u32 offset, 
        bht_L0_u32 *data, 
        bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 idx;
    bht_L0_itype_e itype = device->itype;
    
    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:            
            if(NULL != device->lld_hand)
            {
                idx = 0;
//                bht_L0_sem_take(pci_device_cb->pci_sem, BHT_WAITFOREVER);
                while(count != idx)
                {
                    if(WD_STATUS_SUCCESS != WDC_ReadAddr32((WDC_DEVICE_HANDLE)device->lld_hand,\
                        0, (offset + 4 * idx), &data[idx]))
                    {
                        result = BHT_ERR_DRIVER_READ_FAIL;
                        break;
                    }
                    idx++;
                }
//                bht_L0_sem_give(pci_device_cb->pci_sem);
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_DTYPE;
    }

	return result;
}

bht_L0_s32 
bht_L0_write_setupmem32(bht_L0_device_t *device, 
        bht_L0_u32 offset, 
        bht_L0_u32 *data, 
        bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 idx;
    bht_L0_itype_e itype = device->itype;
    
    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:            
            if(NULL != device->lld_hand)
            {
                idx = 0;
//                bht_L0_sem_take(pci_device_cb->pci_sem, BHT_WAITFOREVER);
                while(count != idx)
                {
                    if(WD_STATUS_SUCCESS != WDC_WriteAddr32((WDC_DEVICE_HANDLE)device->lld_hand, \
                        0, (offset + 4 * idx), data[idx]))
                    {
                        result = BHT_ERR_DRIVER_WRITE_FAIL;
                        break;
                    }
                    idx++;
                }
//                bht_L0_sem_give(pci_device_cb->pci_sem);
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_DTYPE;
    }

	return result;
}

bht_L0_s32 
bht_L0_read_setupmem16(bht_L0_device_t *device, 
        bht_L0_u32 offset, 
        bht_L0_u16 *data, 
        bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 idx;
    bht_L0_itype_e itype = device->itype;

    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:            
            if(NULL != device->lld_hand)
            {
                idx = 0;
//                bht_L0_sem_take(pci_device_cb->pci_sem, BHT_WAITFOREVER);
                while(count != idx)
                {
                    if(WD_STATUS_SUCCESS != WDC_ReadAddr16((WDC_DEVICE_HANDLE )device->lld_hand, 0,\
                        (offset + 2 * idx), &data[idx]))
                    {
                        result = BHT_ERR_DRIVER_READ_FAIL;
                        break;
                    }
                    idx++;
                }
//                bht_L0_sem_give(pci_device_cb->pci_sem);
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_DTYPE;
    }

	return result;
}

bht_L0_s32 
bht_L0_write_setupmem16(bht_L0_device_t *device, 
        bht_L0_u32 offset, 
        bht_L0_u16 *data, 
        bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_u32 idx;
    bht_L0_itype_e itype = device->itype;

    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:            
            if(NULL != device->lld_hand)
            {
                idx = 0;
//                bht_L0_sem_take(pci_device_cb->pci_sem, BHT_WAITFOREVER);
                while(count != idx)
                {
                    if(WD_STATUS_SUCCESS != WDC_WriteAddr16((WDC_DEVICE_HANDLE)device->lld_hand, 0,\
                        (offset + 2 * idx), data[idx]))
                    {
                        result = BHT_ERR_DRIVER_WRITE_FAIL;
                        break;
                    }
                    idx++;
                }
//                bht_L0_sem_give(pci_device_cb->pci_sem);
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_DTYPE;
    }

	return result;
}

bht_L0_s32 
bht_L0_attach_inthandler(bht_L0_device_t *device, 
        bht_L0_u32 chan_regoffset, 
        BHT_L0_USER_ISRFUNC isr, 
        void * arg)
{
    DWORD ret;
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_itype_e itype = device->itype;

    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:            
            if(NULL != device->lld_hand)
            {
            
//                WD_TRANSFER*pTrans = (WD_TRANSFER*) calloc(3, sizeof(WD_TRANSFER));
//                WDC_DEVICE *dev = (WDC_DEVICE *)pci_device_cb->wd_handle;
#ifndef BHT_A429_INTR_CLR
#define BHT_A429_INTR_CLR					0X0024
#endif
#ifndef BHT_A429_INTR_STATE
#define BHT_A429_INTR_STATE					0X0028
#endif
#if 1
//                (pTrans)->dwPort = (dev->pAddrDesc+2)->kptAddr + BHT_A429_INTR_STATE;
//                (pTrans)->cmdTrans = RM_DWORD;
//                (pTrans)->cmdTrans = WDC_ADDR_IS_MEM(dev->pAddrDesc+2) ? RM_DWORD : RP_DWORD;
//                
//                (pTrans+1)->cmdTrans = CMD_MASK;
//                (pTrans+1)->Data.Dword = BIT4;
//                
//                (pTrans+2)->dwPort = (dev->pAddrDesc+2)->kptAddr + BHT_A429_INTR_CLR;
//                (pTrans+2)->cmdTrans = WM_DWORD;
//                (pTrans+2)->cmdTrans = WDC_ADDR_IS_MEM(dev->pAddrDesc+2) ? WM_DWORD : WP_DWORD;

//                (pTrans+2)->Data.Dword = BIT0;
     
				if(WD_STATUS_SUCCESS != (ret = WDC_IntEnable((WDC_DEVICE_HANDLE)device->lld_hand, \
                    NULL, 0, INTERRUPT_CMD_COPY, (INT_HANDLER)isr, (PVOID)arg, WDC_IS_KP((WDC_DEVICE_HANDLE)device->lld_hand))))
                {
                    printf("func[%s] line[%d] ret = %d\n", __FUNCTION__, __LINE__, ret);
                    result = BHT_ERR_DRIVER_INT_ATTACH_FAIL;
                }
#else
                if(WD_STATUS_SUCCESS != WDC_IntEnable(pci_device_cb->wd_handle, NULL, 0, 0, \
                    (INT_HANDLER)isr, (PVOID)arg, WDC_IS_KP(pci_device_cb->wd_handle)))
                    result = BHT_ERR_DRIVER_INT_ATTACH_FAIL;
#endif
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_DTYPE;
    }
    
    return result;
}

bht_L0_s32 bht_L0_detach_inthandler(bht_L0_device_t *device)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_itype_e itype = device->itype;
    
    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:            
            if(NULL != device->lld_hand)
            {
                if(!WDC_IntIsEnabled((WDC_DEVICE_HANDLE)device->lld_hand))
            	{
            		return BHT_ERR_DRIVER_INT_DETACH_FAIL;
            	}
                
				if(WD_STATUS_SUCCESS != WDC_IntDisable((WDC_DEVICE_HANDLE)device->lld_hand))
                    result = BHT_ERR_DRIVER_INT_DETACH_FAIL;
            }
            else
                result = BHT_ERR_DEV_NOT_INITED;
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_DTYPE;
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

bht_L0_sem
bht_L0_semm_create(void)
{
    HANDLE semm;

    semm = CreateSemaphore(NULL, 1, 1, NULL);

    return (bht_L0_sem)semm;
}

bht_L0_s32 
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

bht_L0_s32 
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

bht_L0_u32
bht_L0_sem_destroy(bht_L0_sem sem)
{
    DWORD ret;

    ret = CloseHandle((HANDLE)sem);
    if(BHT_TRUE == ret)
        return BHT_SUCCESS;
    else
        return BHT_FAILURE;
}

#endif

