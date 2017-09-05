#ifndef __BHT_L1_EXP_H__
#define __BHT_L1_EXP_H__


typedef enum
{
    BHT_L1_DEVICE_TYPE_1553 = 0,
    BHT_L1_DEVICE_TYPE_A429 = 1,
}bht_L1_device_type_e;

typedef enum
{
    BHT_L1_A429_LINK_STAT_CLOSED = 0,
    BHT_L1_A429_LINK_STAT_FINDED = 1,
    BHT_L1_A429_LINK_STAT_OPENED = 2,
}bht_L1_a429_link_stat_e;

typedef struct
{
    bht_L1_device_type_e dtype;         /** device type */
    bht_L0_u32 logic_version;           /** fpga logic version */
    bht_L0_u32 total_chans;             /** total channels on this device */
    
    bht_L0_m_sem mutex_sem;             /** device mutex semaphore */
    
    bht_L1_a429_link_stat_e link_stat;  /** link status */
    
    bht_L0_u32 int_count;               /** total interrupt number after reset */
    bht_L0_u32 int_rdwr_err_count;        /** read / write error count */
    bht_L0_u32 int_vecter_idle_err_count; /** vecter is idle in interrupt */
    bht_L0_u32 int_valid_count;         /** valid interrupt count after reset */
    
    bht_L0_u32 rdwr_err_count;            /** read / write error count on device */
    
    
}bht_L1_device_t;

#endif