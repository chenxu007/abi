#ifndef __BHT_L1_EXP_H__
#define __BHT_L1_EXP_H__

typedef void * bht_L1_device_handle_t;

typedef enum
{
    BHT_L1_PARAM_OPT_GET = 0, 
    BHT_L1_PARAM_OPT_SET = 1
}bht_L1_param_opt_e;

typedef enum
{
    BHT_L1_DISABLE = 0, 
    BHT_L1_ENABLE = 1
}bht_L1_able_e;

typedef enum
{
    BHT_L1_CHAN_TYPE_RX = 0, 
    BHT_L1_CHAN_TYPE_TX = 1
}bht_L1_chan_type_e;

typedef enum
{
    BHT_L1_A429_PARITY_ODD  = 1,
    BHT_L1_A429_PARITY_EVEN = 0,
    BHT_L1_A429_PARITY_NONE = 2,
}bht_L1_a429_parity_e;

typedef enum
{
    BHT_L1_A429_RECV_MODE_LIST = 0,
    BHT_L1_A429_RECV_MODE_SAMPLE = 1,
}bht_L1_a429_recv_mod_e;

typedef enum
{
    BHT_L1_A429_LINK_STAT_CLOSED = 0,
    BHT_L1_A429_LINK_STAT_FINDED = 1,
    BHT_L1_A429_LINK_STAT_OPENED = 2,
}bht_L1_a429_link_stat_e;

typedef enum
{
    BHT_L1_A429_CHAN_STAT_OPEN = 3,
    BHT_L1_A429_CHAN_STAT_CLOSE = 2,
    BHT_L1_A429_CHAN_STAT_CLOSE_AND_CLEAR_FIFO = 1,
}bht_L1_a429_chan_stat_e;

typedef enum
{
    BHT_L1_A429_LIST_TYPE_BLACK = 0,
    BHT_L1_A429_LIST_TYPE_WHITE = 1
}bht_L1_a429_list_type_e;

typedef enum
{
    BHT_L1_A429_ERR_TYPE_NONE = 0,
    BHT_L1_A429_ERR_TYPE_31BIT,
    BHT_L1_A429_ERR_TYPE_33BIT,
    BHT_L1_A429_ERR_TYPE_2GAP,
    BHT_L1_A429_ERR_TYPE_PARITY
}bht_L1_a429_err_type_e;


typedef struct
{
    bht_L0_dtype_e dtype;         /** device type */
    bht_L0_u32 logic_version;           /** fpga logic version */
    bht_L0_u32 total_chans;             /** total channels on this device */
    
    bht_L0_sem mutex_sem;             /** device mutex semaphore */
    
    bht_L1_a429_link_stat_e link_stat;  /** link status */
    
    bht_L0_u32 int_count;               /** total interrupt number after reset */
    bht_L0_u32 int_rdwr_err_count;        /** read / write error count */
    bht_L0_u32 int_vecter_idle_err_count; /** vecter is idle in interrupt */
    bht_L0_u32 int_valid_count;         /** valid interrupt count after reset */
    
    bht_L0_u32 rdwr_err_count;            /** read / write error count on device */
    
}bht_L1_device_t;

/* bht_L1_error_to_string, get the error message with error number
 * @param err_num, the error number
 * return, one message pointer will be returned
 */
__declspec(dllexport) const char *
bht_L1_error_to_string(bht_L0_u32 err_num);

/* bht_L1_device_scan, device scan
 * @param void
 * return the number of deivce which have been finded
 */
__declspec(dllexport) bht_L0_s32
bht_L1_device_scan(bht_L0_dtype_e dtype);

/* bht_L1_device_info_get, device scan
// * @param device_array,the device infomation will store in this array_size
// * @param array_size, the size of device_array
// * return the number of deivce which have been finded
// */
//__declspec(dllexport) bht_L0_s32
//bht_L1_device_info_get(bht_L1_device_info_t (*device_array)[], 
//        bht_L0_u32 array_size);
        
/* bht_L1_device_open, device open
 * @param device_info, the device infomation that you want to open
 * return ,the device handle or error,(when 0 is greater than the return value, 
 * it means open failed)
 */
__declspec(dllexport) bht_L0_u32
bht_L1_device_open(bht_L0_dtype_e dtype,
        bht_L0_u32 device_no,
        bht_L1_device_handle_t *device,
        const char *filename);

/* bht_L1_device_open, device close
 * @param device_handle, the device handle
 * return BHT_SUCCESS or error
 */
/*
 * Function bht_L1_device_close
 * Description 
 * @param bht_L1_device_handle_t device_handle  
 * Output        : None
 * @Return 
 * Others        : 
 * Record
 * 1.Date        : 20170915
 *   Author      : chenxu
 *   Modification: Created function

*****************************************************************************/
__declspec(dllexport) bht_L0_u32
bht_L1_device_close(bht_L1_device_handle_t device);

/* bht_L1_device_reset, device reset
 * @param device_handle, the device handle
 * return BHT_SUCCESS or error
 */
__declspec(dllexport) bht_L0_u32
bht_L1_device_reset(bht_L1_device_handle_t device);

/* bht_L1_device_pciload, device fpga logic load from pci
// * @param device_handle, the device handle
// * @param filename, the name of file (fpga bin file with path) which you want to load
// * return BHT_SUCCESS or error
// */
//__declspec(dllexport) bht_L0_s32
//bht_L1_device_pciload(bht_L1_device_handle_t device,
//        const bht_L0_u8 *filename);
        
/* bht_L1_device_logic_version, get logic version
 * @param device_handle, the device handle
 * return logic version
 */
__declspec(dllexport) bht_L0_u32
bht_L1_device_logic_version(bht_L1_device_handle_t device);

/* bht_L1_device_hw_version, get hardware version
 * @param device_handle, the device handle
 * return hardware version
 */
__declspec(dllexport) bht_L0_u32
bht_L1_device_hw_version(bht_L1_device_handle_t device);

/* bht_L1_device_config_from_xml, config the device with a xml file, 
   driver will phase the xml, and config the device
 * @param device_handle, the device handle
 * @param filename, the xml config file name (with path)
 * return BHT_SUCCESS or error
 */
__declspec(dllexport) bht_L0_s32
bht_L1_device_config_from_xml(bht_L1_device_handle_t device,
        const char *filename);

/* bht_L1_device_default_param_save, this function will save the current param 
   as default param, means after your next reset, the device's param will be resumed to 
   the param once you save the default param.
 * @param device_handle, the device handle
 * return BHT_SUCCESS or error
 */
__declspec(dllexport) bht_L0_s32
bht_L1_device_default_param_save(bht_L1_device_handle_t device);

/*************************************A429 part**************************************/
/* bht_L1_a429_irigb_mode, the a429 device irig-b mode config
 * @param device_handle, the device handle
 * @param mode, witch can be config to master or slave
 * return BHT_SUCCESS or other error number.
 */
__declspec(dllexport) bht_L0_s32
bht_L1_a429_irigb_mode(bht_L1_device_handle_t device,
        bht_L1_a429_irigb_mode_e *mode,
        bht_L1_param_opt_e param_opt);

/* bht_L1_a429_irigb_time ,the a429 device irig-b time config
 * @param device_handle, the device handle
 * @param ti, this pointer store the irig-b time
 * @param opt, the opt can be BHT_L1_PARAM_OPT_GET or BHT_L1_PARAM_OPT_SET, 
    witch means to get or set the irig-b time
 * return BHT_SUCCESS or other error number.
 */
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_irigb_time(bht_L1_device_handle_t device,
        bht_L1_a429_irigb_time_t *ti, 
        bht_L1_param_opt_e param_opt);

/********************************A429 channel part**********************************/
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_chan_open(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_chan_close(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_chan_close_and_clear_fifo(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_chan_get_stat(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type,
        bht_L1_a429_chan_stat_e *chan_stat);        
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_chan_baud(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type,
        bht_L1_a429_baud_rate_e * baud,
        bht_L1_param_opt_e param_opt);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_chan_parity(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type,
        bht_L1_a429_parity_e * parity,
        bht_L1_param_opt_e param_opt);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_chan_mib_get(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type,
        bht_L1_a429_mib_data_t *mib_data);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_chan_mib_clear(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_chan_loop(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_able_e *able,
        bht_L1_param_opt_e param_opt);
        
/********************************A429 rx channel part**********************************/
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_rx_chan_recv_mode(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_a429_recv_mod_e *recv_mode,
        bht_L1_param_opt_e param_opt);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_rx_chan_int_threshold(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L0_u16 *threshold_count,
        bht_L0_u16 *threshold_time,
        bht_L1_param_opt_e param_opt);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_rx_chan_filter(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_able_e *able,
        bht_L1_param_opt_e param_opt);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_rx_chan_filter_label(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L0_u8 label,
        bht_L0_u8 sdi,
        bht_L0_u8 ssm,
        bht_L1_a429_list_type_e *list_type,
        bht_L1_param_opt_e param_opt);        
        
/********************************A429 tx channel part**********************************/
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_tx_chan_period(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L0_u32 *period,
        bht_L1_param_opt_e param_opt);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_tx_chan_send(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L0_u32 data);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_tx_chan_update_data(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L0_u32 data);

__declspec(dllexport) bht_L0_s32 
bht_L1_a429_tx_chan_start(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_tx_chan_stop(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num);
        
__declspec(dllexport) bht_L0_s32 
bht_L1_a429_tx_chan_err_inject(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_a429_err_type_e *err_type,
        bht_L1_param_opt_e param_opt);
        

        



#endif