#ifndef __BHT_PCI_H__
#define __BHT_PCI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <bht_type.h>


#define BHT_PCI_CARDS_MAX	100


typedef enum{
	BAR0 = 0,
	BAR1,
	BAR2,
	BAR3,
	BAR4,
	BAR5,
	BAR6,
	BARN
}bht_pci_bar_e;

typedef struct
{
	u32 vendor_id;
	u32 device_id;
}bht_pci_id_t;

typedef struct
{
	u32 bus;
	u32 slot;
	u32 function;
}bht_pci_slot_t;

typedef struct
{
    u32 num_devices;
	bht_pci_id_t device_id[BHT_PCI_CARDS_MAX];
	bht_pci_slot_t device_slot[BHT_PCI_CARDS_MAX];
}bht_pci_scan_result_t;

typedef void * bht_pci_device_handle_t;


extern status_t 
bht_pci_scan_device(u32 vid, u32did, bht_pci_scan_result_t* result);
extern bht_pci_device_handle_t 
bht_pci_open_device(bht_pci_slot_t *slot_info);
extern status_t 
bht_pci_close_device(bht_pci_device_handle_t device_handle);
extern status_t 
bht_pci_device_read32(bht_pci_device_handle_t device_handle, bht_pci_bar_e nbar, u32 offset, u32 * value);
extern status_t 
bht_pci_device_write32(bht_pci_device_handle_t device_handle, bht_pci_bar_e nbar, u32 offset, u32 value);


#ifdef __cplusplus
}
#endif

#endif