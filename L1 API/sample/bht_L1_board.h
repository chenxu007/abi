#ifndef __BHT_L1_BOARD_H__
#define __BHT_L1_BOARD_H__

bht_L0_u32
bht_L1_bd_fpga_eeprom_read(bht_L1_device_handle_t device,
        bht_L0_u16 addr,
        bht_L0_u8 *data);
        
bht_L0_u32
bht_L1_bd_fpga_eeprom_write(bht_L1_device_handle_t device,
        bht_L0_u16 addr,
        bht_L0_u8 data);

bht_L0_u32        
bht_L1_bd_fpga_eeprom_test(bht_L1_device_handle_t device);

bht_L0_u32 
bht_L1_bd_plx_eeprom_read(bht_L1_device_handle_t device,
        bht_L0_u32 addr,
        bht_L0_u16 *buf, 
        bht_L0_u32 count);

bht_L0_u32 
bht_L1_bd_plx_eeprom_reload(bht_L1_device_handle_t device);

bht_L0_u32 
bht_L1_bd_plx_eeprom_write(bht_L1_device_handle_t device,
        bht_L0_u32 addr,
        bht_L0_u16 *buf, 
        bht_L0_u32 count);
        
bht_L0_u32 
bht_L1_bd_plx_eeprom_write_all(bht_L1_device_handle_t device,
        bht_L0_u16 value);
        
bht_L0_u32
bht_L1_bd_plx_eeprom_rfsetting (bht_L1_device_handle_t device,
        const char* filename);

#endif