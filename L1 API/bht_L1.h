/* bht_L1.h - Layer 1 API*/

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

#ifndef __BHT_L1_H__
#define __BHT_L1_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <bht_L0.h>

#define BHT_L1_API_VERSION          0x01000000      /* Version V 1.0.0.0 */

/********* Layer 1 Error Codes (1000 to 1999) *********/
#define BHT_ERR_BAD_INPUT			1000		/*!< \brief Bad input parameters. */
#define BHT_ERR_MEM_TEST_FAIL		1001		/*!< \brief Failed memory test. */
#define BHT_ERR_MEM_MGT_NO_INIT		1002		/*!< \brief Memory Management not initialized for the device ID */
#define BHT_ERR_MEM_MGT_INIT		1003		/*!< \brief Memory Management already initialized for the device ID */
#define BHT_ERR_MEM_MGT_NO_MEM		1004		/*!< \brief Not enough memory available */
#define BHT_ERR_BAD_DEV_TYPE		1005		/*!< \brief Bad device type in device ID */
#define BHT_ERR_RT_FT_UNDEF			1006		/*!< \brief RT Filter Table not defined */
#define BHT_ERR_RT_SA_UNDEF			1007		/*!< \brief RT Subaddress not defined */
#define BHT_ERR_RT_SA_CDP_UNDEF		1008		/*!< \brief RT SA CDP not defined */
#define BHT_ERR_IQ_NO_NEW_ENTRY		1009		/*!< \brief No new entry in interrupt queue */
#define BHT_ERR_NO_BCCB_TABLE		1010		/*!< \brief BCCB Table Pointer is zero */
#define BHT_ERR_BCCB_ALREADY_ALLOCATED 1011		/*!< \brief BCCB already allocated */
#define BHT_ERR_BCCB_NOT_ALLOCATED	1012		/*!< \brief BCCB has not been allocated */
#define BHT_ERR_BUFFER_FULL			1013		/*!< \brief 1553-ARINC PB (CDP/PCB or RXP/PXP) buffer is full */
#define BHT_ERR_TIMEOUT				1014		/*!< \brief Timeout error. */
#define BHT_ERR_BAD_CHAN_NUM		1015		/*!< \brief Bad channel number or channel does not exist on this board */
#define BHT_ERR_BITFAIL				1016		/*!< \brief Built-In Test failure */
#define BHT_ERR_DEVICEINUSE			1017		/*!< \brief Device in use already */
#define BHT_ERR_NO_TXCB_TABLE		1018		/*!< \brief TXCB Table Pointer is zero */
#define BHT_ERR_TXCB_ALREADY_ALLOCATED 1019		/*!< \brief TXCB already allocated */
#define BHT_ERR_TXCB_NOT_ALLOCATED	1020		/*!< \brief TXCB has not been allocated */
#define BHT_ERR_PBCB_TOOMANYPXPS	1021		/*!< \brief PBCB Too Many PXPs For PCBC Allocation */
#define BHT_ERR_NORXCHCVT_ALLOCATED 1022		/*!< \brief RX CH - No CVT Option Defined at Init */
#define BHT_ERR_NO_DATA_AVAILABLE   1023		/*!< \brief No Data Available */
#define BHT_ERR_LOAD_FPGA_FAIL      1024        /* load fpga fail */

typedef enum
{
    BHT_L1_WAIT_FOREVER = -1,
    BHT_L1_NOWAIT = 0
};

typedef enum
{
    BHT_L1_OPT_DISABLE = 0, 
    BHT_L1_OPT_ENABLE = 1
};

typedef enum
{
    BHT_L1_DISABLE = 0, 
    BHT_L1_ENABLE = 1
};

typedef enum
{
    BHT_L1_PARAM_OPT_GET = 0, 
    BHT_L1_PARAM_OPT_SET = 1
}bht_L1_param_opt_e;

typedef enum
{
    BHT_L1_CHAN_TYPE_RX = 0, 
    BHT_L1_CHAN_TYPE_TX = 1
}bht_L1_chan_type_e;

typedef enum
{
    BHT_L1_A429_SLOPE_10_US = 0, 
    BHT_L1_A429_SLOPE_1_5_US = 1
}bht_L1_a429_slope_e;

typedef enum
{
    BHT_L1_A429_FILTER_MODE_BLACKLIST = 0,
    BHT_L1_A429_FILTER_MODE_WHITELIST = 1
}bht_L1_a429_filter_mode_e;

typedef enum
{
    BHT_L1_A429_CHAN_WORK_MODE_ENABLE = 3,
    BHT_L1_A429_CHAN_WORK_MODE_STOP = 2,
    BHT_L1_A429_CHAN_WORK_MODE_STOP_AND_CLEAR = 1,
    BHT_L1_A429_CHAN_WORK_MODE_CLOSE_AND_CLEAR = 0,
}bht_L1_a429_chan_work_mode_e;

typedef enum {
    BHT_L1_A429_BAUD_5K = 5000, 
    BHT_L1_A429_BAUD_12_5K = 12500,
    BHT_L1_A429_BAUD_100K = 100000,
	BHT_L1_A429_BAUD_150K = 150000,
    BHT_L1_A429_BAUD_200K = 200000
}bht_L1_a429_baud_rate_e;

typedef enum
{
    BHT_L1_A429_PARITY_ODD  = 1,
    BHT_L1_A429_PARITY_EVEN = 0,
    BHT_L1_A429_PARITY_NONE = 2,
}bht_L1_a429_parity_e;

typedef enum
{
    BHT_L1_A429_WORD_BIT32  = 0,
    BHT_L1_A429_WORD_BIT31  = 1,
    BHT_L1_A429_WORD_BIT33  = 2,
}bht_L1_a429_word_bit_e;

typedef enum
{
    BHT_L1_A429_GAP_4BIT  = 0,
    BHT_L1_A429_GAP_2BIT  = 1,
}bht_L1_a429_gap_e;

typedef enum
{
    BHT_L1_A429_IRIGB_MODE_SLAVE = 0,
    BHT_L1_A429_IRIGB_MODE_MASTER = 1
}bht_L1_a429_irigb_mode_e;

typedef enum
{
    BHT_L1_A429_RECV_MODE_LIST = 0,
    BHT_L1_A429_RECV_MODE_SAMPLE = 1,
}bht_L1_a429_recv_mod_e;

typedef enum
{
    BHT_L1_A429_OPT_RANDOM_SEND,
    BHT_L1_A429_OPT_PERIOD_SEND_UPDATE,
    BHT_L1_A429_OPT_PERIOD_SEND_START,
    BHT_L1_A429_OPT_PERIOD_SEND_STOP
}bht_L1_a429_send_opt_e;

typedef union
{
    bht_L0_u32 time[2];
    
    struct
    {
        bht_L0_u32 tm_us : 10;
        bht_L0_u32 tm_ms : 10;
        bht_L0_u32 tm_sec : 6;
        bht_L0_u32 tm_min : 6;
        bht_L0_u32 tm_hour : 5;
        bht_L0_u32 tm_day : 9;
        bht_L0_u32 resv : 18;
    }tm;
}bht_L1_a429_irigb_time_t;

typedef struct
{
    bht_L1_a429_chan_work_mode_e work_mode; /* channel work mode */
    bht_L1_a429_baud_rate_e baud;           /* channel baud rate */
    bht_L1_a429_parity_e par;               /* channel parity */
}bht_L1_a429_chan_comm_param_t;

typedef struct
{
    bht_L1_a429_word_bit_e tb_bits;         /* bits count trouble */
    bht_L1_a429_gap_e tb_gap;               /* gap trouble */
    bht_L0_u32 tb_par_en;                   /* parity trouble : 0 - disable, 1 - enable */
}bht_L1_a429_tx_chan_inject_param_t;

typedef struct
{
    bht_L0_u32 gather_enable;               /* 0 - disable, 1 - enable */
    bht_L1_a429_recv_mod_e recv_mode;       /* select list mode or sample mode */
    bht_L0_u16 threshold_count;              /* 0-1024 */
    bht_L0_u16 threshold_time;               /* 单位100us */
}bht_L1_a429_rx_chan_gather_param_t;

typedef struct
{
    bht_L1_a429_filter_mode_e filter_mode;  /* black list mode or white list mode */
    union
    {
        bht_L0_u16 reg16;
        struct
        {
            bht_L0_u8 label;                /* label */ 
            bht_L0_u8 sdi  : 2;             /* Source/Destination Identifier */ 
            bht_L0_u8 ssm  : 2;             /* Sign/Status Matrix */
            bht_L0_u8 revs : 4;             /* reserve bits*/
        }flt_label;
    }flt;
}bht_L1_a429_rx_chan_filter_t;

typedef struct
{
    bht_L0_u32 cnt;                         /* mib - recv or send word count*/
    bht_L0_u32 err_cnt;                     /* mib - recv or send error word count*/
}bht_L1_a429_mib_data_t;

typedef struct
{
    bht_L0_u32 timestamp;                   /* the unit of timestamp is 0.1 millisecond*/
    bht_L0_u32 data;
}bht_L1_a429_rxp_t;


/**************************general*************************/

/* bht_L1_error_to_string converts error number to error 
 * message, the return value is the head of the error message 
 */
__declspec(dllexport) const char *
bht_L1_error_to_string(bht_L0_u32 err_num);

/* bht_L1_device_probe detect the device with the dev_id,
 * If the device is installed, it will be initialized.
 * @param dev_id, organization rules :
 *    - backplane Type (4 bits) -	(dev_id & 0xF0000000) 
 *    - board type (8 bits)     -   (dev_id & 0x0FF00000) 
 *    - board number (4 bits)   -	(dev_id & 0x000F0000) 
 * return BHT_SUCCESS or other error number.
 */
__declspec(dllexport) bht_L0_u32 
bht_L1_device_probe(bht_L0_u32 dev_id);

/* bht_L1_device_remove remove device from driver
 * @param dev_id
 * return BHT_SUCCESS or other error number.
 */
__declspec(dllexport) bht_L0_u32 
bht_L1_device_remove(bht_L0_u32 dev_id);

/**************************a429 general*************************/
/* bht_L1_a429_default_init ,the a429 device will be softreset,
 * and clear the mib info
 * @param dev_id
 * return BHT_SUCCESS or other error number.
 */
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_default_init(bht_L0_u32 dev_id);

/* bht_L1_a429_irigb_mode_cfg ,the a429 device irig-b mode config
 * @param dev_id
 * @param mode, witch can be config to master or slave
 * return BHT_SUCCESS or other error number.
 */
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_irigb_mode_cfg(bht_L0_u32 dev_id, 
        bht_L1_a429_irigb_mode_e mode);

/* bht_L1_a429_irigb_time ,the a429 device irig-b time config
 * @param dev_id
 * @param ti, this pointer store the irig-b time
 * @param opt, the opt can be BHT_L1_PARAM_OPT_GET or BHT_L1_PARAM_OPT_SET, 
    witch means to get or set the irig-b time
 * return BHT_SUCCESS or other error number.
 */
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_irigb_time(bht_L0_u32 dev_id, 
        bht_L1_a429_irigb_time_t *ti, 
        bht_L1_param_opt_e param_opt);

/**************************a429 tx channel*************************/
/* bht_L1_a429_tx_chan_comm_param ,the a429 transmit channel 
 * common param operate function
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param comm_param, this struct contain baud rate/ parity and work mode
 * @param param_opt, param_opt can be BHT_L1_PARAM_OPT_GET or BHT_L1_PARAM_OPT_SET
 * return BHT_SUCCESS or other error number.
 */
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_tx_chan_comm_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_a429_chan_comm_param_t *comm_param, 
        bht_L1_param_opt_e param_opt);

/* bht_L1_a429_tx_chan_inject_param ,the a429 transmit channel 
 * trouble inject param operate function
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param inject_param, this struct contain bits count trouble/
    gap trouble and parity trouble config
 * @param param_opt, param_opt can be BHT_L1_PARAM_OPT_GET or BHT_L1_PARAM_OPT_SET
 * return BHT_SUCCESS or other error number.
 */   
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_tx_chan_inject_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_tx_chan_inject_param_t *inject_param, 
        bht_L1_param_opt_e param_opt);

/* bht_L1_a429_tx_chan_period_param ,the a429 transmit channel 
 * send period config, if the period is > 0, when you use bht_L1_a429_tx_chan_send to send on the 
 * channel, you should use the option BHT_L1_A429_OPT_PERIOD_SEND_UPDATE to update the data which will
 * be send on the channel periodically, then you can use the option BHT_L1_A429_OPT_PERIOD_SEND_START
 * and BHT_L1_A429_OPT_PERIOD_SEND_STOP to start/stop the period send 
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param period, if you use this option BHT_L1_PARAM_OPT_SET, this pointer should store 
    the period param, if you use option  BHT_L1_PARAM_OPT_GET, the period param will be return in 
    this pointer
 * @param param_opt, param_opt can be BHT_L1_PARAM_OPT_GET or BHT_L1_PARAM_OPT_SET
 * return BHT_SUCCESS or other error number.
 */
__declspec(dllexport) bht_L0_u32
bht_L1_a429_tx_chan_period_param(bht_L0_u32 dev_id,
        bht_L0_u32 chan_num,
        bht_L0_u32 * period,
        bht_L1_param_opt_e param_opt);

/* bht_L1_a429_tx_chan_loop ,the a429 transmit channel 
 * loopback config function
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param opt, opt can be BHT_L1_OPT_ENABLE or BHT_L1_OPT_DISABLE
 * return BHT_SUCCESS or other error number.
 */          
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_tx_chan_loop(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L0_u32 opt);

/* bht_L1_a429_tx_chan_slope_cfg ,the a429 transmit channel 
 * slope config function
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param slope, you config the slope with 1.5us or 10us, the 
 *  default value is 1.5us
 * return BHT_SUCCESS or other error number.
 */         
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_tx_chan_slope_cfg(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_slope_e slope);
        
/* bht_L1_a429_tx_chan_mib_clear ,the a429 transmit channel 
 * statistics info clear function, contain total transmit word 
 * number and total transmit error word number 
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * return BHT_SUCCESS or other error number.
 */        
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_tx_chan_mib_clear(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num);
        
/* bht_L1_a429_tx_chan_mib_get ,the a429 transmit channel 
 * statistics info get function, contain total transmit word 
 * number and total transmit error word number 
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param mib_data, if success, the mib info will store in this poiter 
 * return BHT_SUCCESS or other error number.
 */        
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_tx_chan_mib_get(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_mib_data_t *mib_data); 
        
/* bht_L1_a429_tx_chan_send ,the a429 transmit channel 
 * data send function
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param opt
            BHT_L1_A429_OPT_RANDOM_SEND, if the tx channel's period param is zero, if you want to 
                                         send a word on the channel, you need to use this opt.
            BHT_L1_A429_OPT_PERIOD_SEND_UPDATE,if the tx channel's period param is not zero,if you 
                                         want to update the word which be sended repeatedly on the 
                                         channel, you need to use this opt.
            BHT_L1_A429_OPT_PERIOD_SEND_START,this option is means to start the period send
            BHT_L1_A429_OPT_PERIOD_SEND_STOP,this option is means to stop the period send
            
 * @param data, if send_opt is BHT_L1_A429_OPT_RANDOM_SEND or BHT_L1_A429_OPT_PERIOD_SEND_UPDATE,
          this param will be necessary, witch will be send on the assigned channel
 * return BHT_SUCCESS or other error number.
 */             
__declspec(dllexport) bht_L0_u32
bht_L1_a429_tx_chan_send(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_a429_send_opt_e opt,
        bht_L0_u32 data);

/**************************a429 rx channel*************************/
/* bht_L1_a429_rx_chan_comm_param ,the a429 receive channel 
 * common param operate function
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param comm_param, this struct contain baud rate/ parity and work mode
 * @param param_opt, param_opt can be BHT_L1_PARAM_OPT_GET or BHT_L1_PARAM_OPT_SET
 * return BHT_SUCCESS or other error number.
 */
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_rx_chan_comm_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_a429_chan_comm_param_t *comm_param, 
        bht_L1_param_opt_e param_opt);
        
/* bht_L1_a429_rx_chan_gather_param ,the a429 receive channel 
 * gather param operate function
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param gather_param, this struct contain gather mode/ receive mode and interrupt config
 * @param param_opt, param_opt can be BHT_L1_PARAM_OPT_GET or BHT_L1_PARAM_OPT_SET
 * return BHT_SUCCESS or other error number.
 */        
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_rx_chan_gather_param(bht_L0_u32 dev_id,
        bht_L0_u32 chan_num,
        bht_L1_a429_rx_chan_gather_param_t *gather_param,
        bht_L1_param_opt_e param_opt);

/* bht_L1_a429_rx_chan_filter_cfg ,the a429 receive channel 
 * label filter param config function
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param filter, this struct contain label filter config
 * return BHT_SUCCESS or other error number.
 */  
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_rx_chan_filter_cfg(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_rx_chan_filter_t *filter);
        
/* bht_L1_a429_rx_chan_mib_clear ,the a429 receive channel 
 * statistics info clear function, contain total receive word 
 * number and total receive error word number 
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * return BHT_SUCCESS or other error number.
 */        
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_rx_chan_mib_clear(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num);
        
/* bht_L1_a429_rx_chan_mib_get ,the a429 receive channel 
 * statistics info get function, contain total receive word 
 * number and total receive error word number 
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param mib_data, if success, the mib info will store in this poiter 
 * return BHT_SUCCESS or other error number.
 */        
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_rx_chan_mib_get(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_mib_data_t *mib_data); 

/* bht_L1_a429_rx_chan_recv ,the a429 receive channel 
 * receive data function 
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param rxp_buf, this pointer will store the receive data
 * @param max_rxp, the max number of data the rxp_buf can store
 * @param rxp_num, if receive success, this pointer will store 
    a number that how many data received
 * @param timeout_ms, opt can be BHT_L1_NOWAIT, BHT_L1_WAIT_FOREVER 
    or the miliseconds you can wait
 * return BHT_SUCCESS or other error number.
 */         
__declspec(dllexport) bht_L0_u32 
bht_L1_a429_rx_chan_recv(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_rxp_t *rxp_buf, 
        bht_L0_u32 max_rxp, 
        bht_L0_u32 *rxp_num, 
        bht_L0_s32 timeout_ms);

/* bht_L1_a429_chan_dump ,print chan param and mib data
 * @param dev_id
 * @param chan_num, 1 <= chan_num <= 16
 * @param type, channel type
 * return BHT_SUCCESS or other error number.
 */
__declspec(dllexport) bht_L0_u32
bht_L1_a429_chan_dump(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_chan_type_e type);

/* 1553B */

#ifdef __cplusplus
}
#endif

#endif
