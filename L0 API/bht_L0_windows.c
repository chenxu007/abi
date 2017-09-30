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
#include <windrvr.h>
#include <stdio.h>
#include <stdarg.h>
#include <utils.h>

#include <bht_L0.h>
#include <bht_L0_config.h>
#include <stdio.h>
#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTF(format, ...)\
do\
{\
	(void)printf(format, ##__VA_ARGS__); \
}while (0);
#else
#define DEBUG_PRINTF(format, ...)
#endif

#define LICENSE_10_2 "6C3CC2CFE89E7AD0424A070D434A6F6DC4950E31.hwacreate"

typedef void (*PCI9056_INT_HANDLER)(WDC_DEVICE_HANDLE hDev);

/* PCI9056 device information struct */
typedef struct {
    PCI9056_INT_HANDLER   funcDiagIntHandler;
    void *arg;
} PCI9056_DEV_CTX, *PPCI9056_DEV_CTX;

static BOOL DeviceValidate(const PWDC_DEVICE pDev)
{
    DWORD i, dwNumAddrSpaces = pDev->dwNumAddrSpaces;

    /* NOTE: You can modify the implementation of this function in order to
             verify that the device has the resources you expect to find */
    
    /* Verify that the device has at least one active address space */
    for (i = 0; i < dwNumAddrSpaces; i++)
    {
        if (WDC_AddrSpaceIsActive(pDev, i))
            return TRUE;
    }
    
    /* In this sample we accept the device even if it doesn't have any
     * address spaces */ 
    printf("Device does not have any active memory or I/O address spaces\n");
    return TRUE;
}

static inline BOOL IsValidDevice(PWDC_DEVICE pDev, const CHAR *sFunc)
{
    if (!pDev || !WDC_GetDevContext(pDev))
    {
        return FALSE;
    }

    return TRUE;
}

static BOOL IsItemExists(PWDC_DEVICE pDev, ITEM_TYPE item)
{
    int i;
    DWORD dwNumItems = pDev->cardReg.Card.dwItems;

    for (i=0; i<dwNumItems; i++)
    {
        if (pDev->cardReg.Card.Item[i].item == item)
            return TRUE;
    }

    return FALSE;
}

static void DLLCALLCONV PCI9056_IntHandler(PVOID pData)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
    PPCI9056_DEV_CTX pDevCtx = (PPCI9056_DEV_CTX)WDC_GetDevContext(pDev);


//    printf("device = %p\n", pDevCtx->device);
    /* Execute the diagnostics application's interrupt handler routine */
    pDevCtx->funcDiagIntHandler(pDevCtx->arg);
}

DWORD PCI9056_IntEnable(WDC_DEVICE_HANDLE hDev, PCI9056_INT_HANDLER funcIntHandler, void *arg)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    PPCI9056_DEV_CTX pDevCtx;

    DEBUG_PRINTF("PCI9056_IntEnable entered. Device handle: 0x%p\n", hDev);

    if (!IsValidDevice(pDev, "PCI9056_IntEnable"))
        return WD_INVALID_PARAMETER;

    if (!IsItemExists(pDev, ITEM_INTERRUPT))
        return WD_OPERATION_FAILED;

    pDevCtx = (PPCI9056_DEV_CTX)WDC_GetDevContext(pDev);

    /* Check if interrupts are already enabled */
    if (WDC_IntIsEnabled(hDev))
    {
		DEBUG_PRINTF("Interrupts are already enabled ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* Define the number of interrupt transfer commands to use */
    #define NUM_TRANS_CMDS 0

    /* NOTE: In order to correctly handle PCI interrupts, you need to
             ADD CODE HERE to set up transfer commands to read/write the
             relevant register(s) in order to correctly acknowledge the
             interrupts, as dictated by your hardware's specifications.
             When adding transfer commands, be sure to also modify the
             definition of NUM_TRANS_CMDS (above) accordingly. */

    /* Store the diag interrupt handler routine, which will be executed by
       PCI9056_IntHandler() when an interrupt is received */
    pDevCtx->funcDiagIntHandler = funcIntHandler;
    pDevCtx->arg = arg;
    
    /* Enable the interrupts */
    /* NOTE: When adding read transfer commands, set the INTERRUPT_CMD_COPY flag
             in the 4th argument (dwOptions) passed to WDC_IntEnable() */
    dwStatus = WDC_IntEnable(hDev, NULL, 0, 0,
        PCI9056_IntHandler, (PVOID)hDev, WDC_IS_KP(hDev));
        
    if (WD_STATUS_SUCCESS != dwStatus)
    {
		DEBUG_PRINTF("Failed enabling interrupts. Error 0x%lx\n",
            dwStatus);
        
        return dwStatus;
    }

    /* TODO: You can add code here to write to the device in order
             to physically enable the hardware interrupts */

    DEBUG_PRINTF("PCI9056_IntEnable: Interrupts enabled\n");

    return WD_STATUS_SUCCESS;
}

DWORD PCI9056_IntDisable(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    PPCI9056_DEV_CTX pDevCtx;

    DEBUG_PRINTF("PCI9056_IntDisable entered. Device handle: 0x%p\n", hDev);

    if (!IsValidDevice(pDev, "PCI9056_IntDisable"))
        return WD_INVALID_PARAMETER;

    pDevCtx = (PPCI9056_DEV_CTX)WDC_GetDevContext(pDev);
 
    if (!WDC_IntIsEnabled(hDev))
    {
		DEBUG_PRINTF("Interrupts are already disabled ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* TODO: You can add code here to write to the device in order
             to physically disable the hardware interrupts */

    /* Disable the interrupts */
    dwStatus = WDC_IntDisable(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
		DEBUG_PRINTF("Failed disabling interrupts. Error 0x%lx\n",
            dwStatus);
    }

    return dwStatus;
}

BOOL PCI9056_IntIsEnabled(WDC_DEVICE_HANDLE hDev)
{
    if (!IsValidDevice((PWDC_DEVICE)hDev, "PCI9056_IntIsEnabled"))
        return FALSE;

    return WDC_IntIsEnabled(hDev);
}

BOOL PCI9056_DeviceClose(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    PPCI9056_DEV_CTX pDevCtx;
    
    DEBUG_PRINTF("PCI9056_DeviceClose entered. Device handle: 0x%p\n", hDev);

    if (!hDev)
    {
        DEBUG_PRINTF("PCI9056_DeviceClose: Error - NULL device handle\n");
        return FALSE;
    }

    pDevCtx = (PPCI9056_DEV_CTX)WDC_GetDevContext(pDev);
    
    /* Disable interrupts */
    if (WDC_IntIsEnabled(hDev))
    {
        dwStatus = PCI9056_IntDisable(hDev);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            DEBUG_PRINTF("Failed disabling interrupts. Error 0x%lx \n",
                dwStatus);
        }
    }

    /* Close the device */
    dwStatus = WDC_PciDeviceClose(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        DEBUG_PRINTF("Failed closing a WDC device handle (0x%p). Error 0x%lx\n",
            hDev, dwStatus);
    }

    /* Free PCI9056 device context memory */
    if (pDevCtx)
        free (pDevCtx);
    
    return (WD_STATUS_SUCCESS == dwStatus);
}

WDC_DEVICE_HANDLE PCI9056_DeviceOpen(const WD_PCI_CARD_INFO *pDeviceInfo)
{
    DWORD dwStatus;
    PPCI9056_DEV_CTX pDevCtx = NULL;
    WDC_DEVICE_HANDLE hDev = NULL;

    /* Validate arguments */
    if (!pDeviceInfo)
    {
        DEBUG_PRINTF("PCI9056_DeviceOpen: Error - NULL device information struct pointer\n");
        return NULL;
    }

    /* Allocate memory for the PCI9056 device context */
    pDevCtx = (PPCI9056_DEV_CTX)malloc(sizeof (PCI9056_DEV_CTX));
    if (!pDevCtx)
    {
        DEBUG_PRINTF("Failed allocating memory for PCI9056 device context\n");
        return NULL;
    }

    BZERO(*pDevCtx);

    /* Open a WDC device handle */
    dwStatus = WDC_PciDeviceOpen(&hDev, pDeviceInfo, pDevCtx, NULL, NULL, NULL);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        DEBUG_PRINTF("Failed opening a WDC device handle. Error 0x%lx \n",
            dwStatus);
        goto Error;
    }

    /* Validate device information */
    if (!DeviceValidate((PWDC_DEVICE)hDev))
        goto Error;

    /* Return handle to the new device */
    DEBUG_PRINTF("PCI9056_DeviceOpen: Opened a PCI9056 device (handle 0x%p)\n", hDev);
    return hDev;

Error:    
    if (hDev)
        PCI9056_DeviceClose(hDev);
    else
        free(pDevCtx);
    
    return NULL;
}

/* Find a PCI9056 device */
static BOOL DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, DWORD dwDeviceNo, WD_PCI_SLOT *pSlot)
{
    DWORD dwStatus;
    DWORD i, dwNumDevices;
    WDC_PCI_SCAN_RESULT scanResult;

    BZERO(scanResult);
    dwStatus = WDC_PciScanDevices(dwVendorId, dwDeviceId, &scanResult);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        DEBUG_PRINTF("DeviceFind: Failed scanning the PCI bus.\n"
            "Error: 0x%lx\n", dwStatus);
        return FALSE;
    }

    dwNumDevices = scanResult.dwNumDevices;
    if (!dwNumDevices)
    {
        DEBUG_PRINTF("No matching device was found for search criteria "
            "(Vendor ID 0x%lX, Device ID 0x%lX)\n",
            dwVendorId, dwDeviceId);

        return FALSE;
    }
    
    printf("\n");
    printf("Found %ld matching device%s [ Vendor ID 0x%lX%s, Device ID 0x%lX%s ]:\n",
        dwNumDevices, dwNumDevices > 1 ? "s" : "",
        dwVendorId, dwVendorId ? "" : " (ALL)",
        dwDeviceId, dwDeviceId ? "" : " (ALL)");
    
    for (i = 0; i < dwNumDevices; i++)
    {
        printf("\n");
        printf("%2ld. Vendor ID: 0x%lX, Device ID: 0x%lX\n",
            i + 1,
            scanResult.deviceId[i].dwVendorId,
            scanResult.deviceId[i].dwDeviceId);
    }
    printf("\n");

    if (dwNumDevices <= dwDeviceNo)
    {
        return FALSE;
    }

    *pSlot = scanResult.deviceSlot[i - 1];

    return TRUE;
}

/* Open a handle to a PCI9056 device */
static WDC_DEVICE_HANDLE DeviceOpen(const WD_PCI_SLOT *pSlot)
{
    WDC_DEVICE_HANDLE hDev;
    DWORD dwStatus;
    WD_PCI_CARD_INFO deviceInfo;
    
    /* Retrieve the device's resources information */
    BZERO(deviceInfo);
    deviceInfo.pciSlot = *pSlot;
    dwStatus = WDC_PciGetDeviceInfo(&deviceInfo);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        DEBUG_PRINTF("DeviceOpen: Failed retrieving the device's resources information.\n"
            "Error 0x%lx\n", dwStatus);
        return NULL;
    }

    /* NOTE: You can modify the device's resources information here, if
       necessary (mainly the deviceInfo.Card.Items array or the items number -
       deviceInfo.Card.dwItems) in order to register only some of the resources
       or register only a portion of a specific address space, for example. */

    /* Open a handle to the device */
    hDev = PCI9056_DeviceOpen(&deviceInfo);
    if (!hDev)
    {
        DEBUG_PRINTF("DeviceOpen: Failed opening a handle to the device");
        return NULL;
    }

    return hDev;
}

static WDC_DEVICE_HANDLE DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId, DWORD dwDeviceNo)
{
    WD_PCI_SLOT slot;
    
    if (!DeviceFind(dwVendorId, dwDeviceId, dwDeviceNo, &slot))
        return NULL;

    return DeviceOpen(&slot);
}

/* Close handle to a PCI9056 device */
static void DeviceClose(WDC_DEVICE_HANDLE hDev)
{
    if (!hDev)
        return;

    if (!PCI9056_DeviceClose(hDev))
    {
        DEBUG_PRINTF("DeviceClose: Failed closing PCI9056 device");
    }
}

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

bht_L0_u32 
bht_L0_init(void)
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

bht_L0_u32 
bht_L0_device_scan(bht_L0_dtype_e dtype)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_itype_e itype;
    bht_L0_u16 device_id;
    WDC_PCI_SCAN_RESULT scan_result;

    if(dtype >= BHT_L0_DEVICE_TYPE_MAX)
		return 0;

    itype = bht_L0_dtypeinfo_items[dtype].itype;
    
    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:
            /* device id*/
            if((BHT_L0_DEVICE_TYPE_ARINC429 == dtype))
            {
                device_id = BHT_PCI_DEVICE_ID_PMC429;
            }
			else
			{
				DEBUG_PRINTF("interface type not support\n");
				return 0;
			}
            /* scan */
			if ((WD_STATUS_SUCCESS != WDC_PciScanDevices(BHT_PCI_VENDOR_ID, device_id, &scan_result))
				|| (scan_result.dwNumDevices < 0))
			{
				DEBUG_PRINTF("WDC_PciScanDevices err\n");
				return 0;
			}

            return scan_result.dwNumDevices;
        default:
			return 0;
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
				WD_PCI_CARD_INFO card_info;
                WDC_DEVICE_HANDLE hDev;
                WD_PCI_SLOT pSlot;

                if(NULL != device->lld_hand)
                    break;

                BZERO(card_info);

                /* device id*/
                if(BHT_L0_DEVICE_TYPE_ARINC429 == dtype)
                {
                    pci_device_id = BHT_PCI_DEVICE_ID_PMC429;
                }
                else
                    return BHT_ERR_UNSUPPORTED_DEVICE_TYPE;

                if(!DeviceFind(pci_vendor_id, pci_device_id, device_no, &pSlot))
                    return BHT_ERR_NO_DEVICE;

                hDev = DeviceOpen(&pSlot);
                if(NULL == hDev)
                    result = BHT_ERR_CANT_OPEN_DEV;
                else
                    device->lld_hand = (void*)hDev;
                    
#if 0
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
                
                if(WD_STATUS_SUCCESS != WDC_PciDeviceOpen((WDC_DEVICE_HANDLE*)&device->lld_hand, \
                    &card_info, NULL, NULL, NULL, NULL))
                    return BHT_ERR_CANT_OPEN_DEV;

                if (!DeviceValidate((PWDC_DEVICE)device->lld_hand))
                {
                    DEBUG_PRINTF("Device is not valid\n");
                    WDC_PciDeviceClose((WDC_DEVICE_HANDLE)device->lld_hand);
                    return BHT_ERR_CANT_OPEN_DEV;
                }
#endif
            }while(0); 
            break;
        default:
            result = BHT_ERR_UNSUPPORTED_DTYPE;
    }

    return result;
}

bht_L0_u32 bht_L0_unmap_memory(bht_L0_device_t *device)
{
    bht_L0_u32 result = BHT_SUCCESS;	

    switch(device->itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:            
            if(NULL != device->lld_hand)
            {
                if( TRUE != PCI9056_DeviceClose((WDC_DEVICE_HANDLE )device->lld_hand))
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

bht_L0_u32 
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

bht_L0_u32 bht_L0_read_mem32_dma(bht_L0_device_t *device, bht_L0_u32 offset, bht_L0_u32 *data, bht_L0_u32 count)
{
    return BHT_FAILURE;
}

bht_L0_u32 bht_L0_read_mem16(bht_L0_device_t *device, bht_L0_u32 offset, bht_L0_u16 *data, bht_L0_u32 count)
{
    return BHT_FAILURE;
}

bht_L0_u32 bht_L0_write_mem16(bht_L0_device_t *device, bht_L0_u32 offset, bht_L0_u16 *data, bht_L0_u32 count)
{
    return BHT_FAILURE;
}

bht_L0_u32 
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

bht_L0_u32 
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

bht_L0_u32 
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

bht_L0_u32 
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

bht_L0_u32 
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

bht_L0_u32 
bht_L0_attach_inthandler(bht_L0_device_t *device, 
        bht_L0_u32 chan_regoffset, 
        BHT_L0_USER_ISRFUNC isr, 
        void * arg)
{
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
#if 1
                DWORD dwStatus;
				WDC_DEVICE_HANDLE hDev = device->lld_hand;
                
                dwStatus = PCI9056_IntEnable(hDev, isr, device);

                if (WD_STATUS_SUCCESS == dwStatus)
                    printf("Interrupts enabled\n");
                else
                {
                    DEBUG_PRINTF("Failed enabling interrupts. Error 0x%lx \n",
                       dwStatus);
                }
#else           
				if(WD_STATUS_SUCCESS != (ret = WDC_IntEnable((WDC_DEVICE_HANDLE)device->lld_hand, \
                    NULL, 0, INTERRUPT_CMD_COPY, (INT_HANDLER)isr, (PVOID)arg, WDC_IS_KP((WDC_DEVICE_HANDLE)device->lld_hand))))
                {
                    printf("func[%s] line[%d] ret = %d\n", __FUNCTION__, __LINE__, ret);
                    result = BHT_ERR_DRIVER_INT_ATTACH_FAIL;
                }


#endif
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

bht_L0_u32 bht_L0_detach_inthandler(bht_L0_device_t *device)
{
    bht_L0_u32 result = BHT_SUCCESS;	
    bht_L0_itype_e itype = device->itype;
    PWDC_DEVICE pDev = (PWDC_DEVICE)device->lld_hand;
    
    switch(itype)
    {
        case BHT_L0_INTERFACE_TYPE_PCI:            
            if(NULL != device->lld_hand)
            {
                if (WD_STATUS_SUCCESS == PCI9056_IntDisable(device->lld_hand))
                    printf("Interrupts disabled\n");
                else
                    DEBUG_PRINTF("Failed disabling interrupts");
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
        return BHT_ERR_SEM_TAKE;
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
        return BHT_ERR_SEM_GIVE;
}

bht_L0_u32
bht_L0_sem_destroy(bht_L0_sem sem)
{
    DWORD ret;

    ret = CloseHandle((HANDLE)sem);
    if(BHT_TRUE == ret)
        return BHT_SUCCESS;
    else
        return BHT_ERR_SEM_DESTROY;
}

#endif

